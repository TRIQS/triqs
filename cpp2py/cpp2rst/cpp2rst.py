import os, re, sys, itertools
from collections import OrderedDict

import cpp2py.clang_parser as CL
import synopsis
from processed_doc import ProcessedDoc
from renderers import render_fnt, render_cls

# --------------------------------

regex_space_end_of_line = re.compile(r"[ \t\r\f\v]+$",re.MULTILINE)

def safe_write(output_name, data):
    if '/' in output_name:
        print "Skipping ", output_name
        return
    with open("{output_name}.rst".format(output_name=output_name), "w") as f:
        f.write(re.sub(regex_space_end_of_line,'',data.strip()))

def mkchdir(*subdirs):
    for d in subdirs:
        try:
            os.mkdir(d)
        except OSError:
            pass
        os.chdir(d)

# -------------------------------------------------------

class Cpp2Rst: 
    """ """
    def __init__(self, filename, namespaces=(), compiler_options=None, includes= (), parse_all_comments = False, libclang_location = None):
        """
           Parse the file at construction
           
           Parameters
           -----------

           filename : string
                      Name of the file to parse
           
           namespaces : list of string 
                      Restrict the generation to the given namespaces.
           
           includes         : string, optional
                      Additional includes to add (-I xxx) for clang
           
           compiler_options : string, optional 
                      Additional option for clang compiler
           
           libclang_location : string, optional
                      Absolute path to libclang. By default, the detected one.
        """
        self.filename, self.namespaces = filename, namespaces
        self.root = CL.parse(filename, compiler_options, includes, libclang_location, parse_all_comments)

    # ------------------------
    
    def run(self, output_directory):

        print "Generating the documentation ..."
        mkchdir(output_directory)

        # Filters
        def keep_ns(n):
            ns = CL.fully_qualified(n)
            if 'std' in ns or 'boost' in ns or 'detail' in ns or 'impl' in ns : return False
            if len(self.namespaces) == 0 : return True
            return any((ns in x) for x in self.namespaces)
        
        def keep_cls(c):
            """Keeps the class if its namespace is EXACTLY in self.namespaces
               e.g. A::B::cls will be kept iif A::B is in self.namespaces, not it A is.
            """
            if not c.raw_comment : return False
            if len(self.namespaces)>0 :
                ns = CL.fully_qualified(c.referenced).rsplit('::',1)[0]
                return ns in self.namespaces
            return True
        
        def keep_fnt(f) :
            if not f.raw_comment : return False
            if  f.spelling.startswith('operator') or f.spelling in ['begin','end'] : return False
            return keep_cls(f)

        # A list of AST nodes for classes
        classes =CL.get_classes(self.root, keep_cls, traverse_namespaces = True, keep_ns = keep_ns)
        classes = list(classes) # to avoid exhaustion of the generator
   
        # A list of AST nodes for the methods and functions
        all_functions = CL.get_functions(self.root, keep_fnt, traverse_namespaces = True, keep_ns = keep_ns)
        all_functions = list(all_functions) # to avoid exhaustion of the generator

        # Build the doc for all functions
        for f in all_functions:
            f.processed_doc = ProcessedDoc(f)

        # c : AST node of name A::B::C::clsname makes and cd into A/B/C
        def mkchdir_for_one_node(node): mkchdir(* ( CL.fully_qualified_name(node).split('::')[:-1]))

        # regroup the functions into a list of overloads
        def regroup_func_by_names(fs):
            """ Given a list of functions, regroup them by names"""
            d = OrderedDict()
            def decay(name):
                if 'operator' not in name : return name
                a, ca, c, co = '+ - * /',  '+= -= \*= /=',  "== !=",  r" comparison"
                d = {'*=' : ca,'+=' : ca,'/=' : ca,'-=' : ca,'*' : a,'+' : a,'/' : a,'-' : a, '==': c, '!=' : c, '<': co, '>' : co, '<=' : co, '>=' : co}
                n = name.replace('operator','').strip()
                return 'operator' + d[n] if n in d else name
            for f in fs:
                d.setdefault(decay(f.spelling),[]).append(f)
            return d

        synopsis.class_list = classes
        synopsis.class_list_name = [n.spelling for n in classes]

        for c in classes:
            if not c.spelling.strip() : 
                print "Skipping a class with an empty name !"
                continue

            print " ... class :  " + c.spelling, CL.fully_qualified_name(c), c.kind, CL.get_name_with_template_specialization(c)

            # process the doc of the class and add it to the node
            c.processed_doc = ProcessedDoc(c)

            # all methods and constructors
            constructors = list(CL.get_constructors(c))
            for f in constructors : f.is_constructor = True # tag them for later use
            all_methods = OrderedDict()
            if constructors: all_methods['constructor'] = constructors
            all_methods.update(regroup_func_by_names(CL.get_methods(c, True))) # True : with inherited ?

            # all non member functions
            all_friend_functions = regroup_func_by_names(CL.get_friend_functions(c))

            # process the doc for all methods and functions, and store it in the node.
            for (n,f_list) in (all_methods.items() + all_friend_functions.items()):
                for f in f_list:
                    f.processed_doc = ProcessedDoc(f)

            # One directory for the class
            cur_dir = os.getcwd()
            mkchdir_for_one_node(c)

            # the file for the class
            r = render_cls(c, all_methods, all_friend_functions)
            safe_write(c.spelling, r)

            # create a directory with the class name
            mkchdir(c.spelling)

            # write a file for each function
            def render(mess, d) : 
                for f_name, f_overloads in d.items():
                    print " ...... %s %s"%(mess, f_name)               
                    r = render_fnt(parent_class = c, f_name = f_name, f_overloads = f_overloads)
                    safe_write(f_name, r)

            render('methods', all_methods)
            render('non member functions', all_friend_functions)

            # Change back to up directory
            os.chdir(cur_dir)

        all_functions = regroup_func_by_names(all_functions)

        docs = dict ( (n, [ProcessedDoc(f) for f in fs]) for (n,fs) in all_functions.items())

        for f_name, f_overloads in all_functions.items():
            print " ... function " + f_name, "      [", f_overloads[0].location.file.name, ']'
            cur_dir = os.getcwd()
            mkchdir_for_one_node(f_overloads[0])
            r = render_fnt(parent_class = None, f_name = f_name, f_overloads = f_overloads)
            safe_write(f_name, r)            
            os.chdir(cur_dir)

        print "... done"


