import os, re, sys, itertools
from collections import OrderedDict

import cpp2py.clang_parser as CL
import synopsis
from doc import ProcessDoc
from render_fnt import render_fnt, render_cls

def clean_end_white_char(s):
    """ remove the space at the end of the lines (for clean git diff...)"""
    reg = re.compile(r"[ \t\r\f\v]+$",re.MULTILINE)
    return re.sub(reg,'',s.strip())

def safe_write(output_name, data):
    if '/' in output_name:
        print "Skipping ", output_name
        return
    with open("{output_name}.rst".format(output_name=output_name), "w") as f:
        f.write((clean_end_white_char(data)))

def mkchdir(*subdirs):
    for d in subdirs:
        try:
            os.mkdir(d)
        except OSError:
            pass
        os.chdir(d)

def mkchdir_for_one_class(node):
    """c : AST node for a class  A::B::C::clsname
       makes and cd into A/B/C
    """
    mkchdir(* ( CL.fully_qualified(node.referenced).split('::')[:-1]))
 
class Cpp2Rst: 
    """ """
    def __init__(self, filename, namespaces=(), compiler_options=None, includes=None, system_includes=None, libclang_location = None, parse_all_comments = False, target_file_only = False):
        """
           Parse the file at construction
           
           Parameters
           -----------

           filename : string
                      Name of the file to parse
           
           namespaces : list of string 
                      Restrict the generation to the given namespaces.
           
           includes : string, optional
                      Additional includes to add (-I xxx) for clang

           system_includes : string, optional
                      Additional System includes to add (-isystem xxx) for clang
           
           compiler_options : string, optional 
                      Additional option for clang compiler
           
           libclang_location : string, optional
                      Absolute path to libclang. By default, the detected one.

           parse_all_comments : bool
                      Grab all comments, including non doxygen like [default = False]

           target_file_only : bool
                      Neglect any included files during desc generation [default = False]
        """
        self.filename, self.namespaces, self.target_file_only = filename, namespaces, target_file_only
        self.root = CL.parse(filename, compiler_options, includes, system_includes, libclang_location, parse_all_comments)

    def keep_ns(self, n):
        ns = CL.fully_qualified(n) 
        if 'std' in ns or 'boost' in ns or 'detail' in ns or 'impl' in ns : return False
        if len(self.namespaces) == 0 : return True
        return any((ns in x) for x in self.namespaces)
    
    def keep_cls(self, c):
        """
           The filter to keep a class/struct or an enum :
             it must have a raw comment
             if we a namespace list, it must be in it.
             if target_file_only it has to be in the file given to c++2py
        """
        if not c.raw_comment : return False
        if self.namespaces:
            qualified_ns = CL.get_namespace(c)
            if not any((x in qualified_ns) for x in self.namespaces) : return False
        return (c.location.file.name == self.filename) if self.target_file_only else True
    
    def keep_fnt(self, f) :
        if not f.raw_comment : return False
        if  f.spelling.startswith('operator') or f.spelling in ['begin','end'] : return False
        return self.keep_cls(f) 
    
    def all_functions_gen(self):
        """Generates all the AST nodes of classes (mode = 'C') or functions (mode = 'F')"""
        return CL.get_functions(self.root, self.keep_fnt, traverse_namespaces = True, keep_ns = self.keep_ns)

    def all_classes_gen(self):
        """Generates all the AST nodes of classes (mode = 'C') or functions (mode = 'F')"""
        return CL.get_classes(self.root, self.keep_cls, traverse_namespaces = True, keep_ns = self.keep_ns)

    def regroup_func_by_names(self, fs):
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

    def process_doc_function(self, f):
        """Process the doc of the function f"""
        return ProcessDoc(f, ['include', 'return', 'synopsis', 'warning','figure', 'note', 'example'],['param', 'tparam'])

    def process_doc_class(self, c):
        """Process the doc of the class c"""
        return ProcessDoc(c, ['include', 'manual_methods', 'manual_friends','warning','figure', 'note', 'example'],['tparam'])

    def run(self, output_directory):

        print "Generating the documentation ..."
        mkchdir(output_directory)
        classes = list(self.all_classes_gen())


        # FIXME 
        synopsis.class_list = classes
        synopsis.class_list_name = [n.spelling for n in classes]

        for c in classes:
            if not c.spelling.strip() : 
                print "Skipping a class with an empty name !"
                continue

            print " ... class :  " + c.spelling, CL.fully_qualified(c.referenced)

            # process the doc of the class
            cls_doc = self.process_doc_class(c)

            # all methods and constructors
            all_m = self.regroup_func_by_names(CL.get_methods(c, True)) # True : with inherited ?
            constructors = list(CL.get_constructors(c))
            if constructors: all_m['constructor'] = constructors

            # all non member functions
            all_friend_functions = self.regroup_func_by_names(CL.get_friend_functions(c))

            # process the doc
            doc_methods = dict ( (n, [self.process_doc_function(f) for f in fs]) for (n,fs) in (all_m.items() + all_friend_functions.items()))

            # One directory for the class
            cur_dir = os.getcwd()
            mkchdir_for_one_class(c)

            # the file for the class
            r = render_cls(doc_class = self.process_doc_class(c), doc_methods = doc_methods, cls = c, all_m = all_m, all_friend_functions = all_friend_functions, cls_doc = cls_doc)
            safe_write(c.spelling, r)
                
            mkchdir(c.spelling)

            def render(mess, d) : 
                for f_name, f_overloads in all_m.items():
                    print " ...... %s %s"%(mess, f_name)               
                    r = render_fnt(doc_methods = doc_methods[f_name], parent_class = c, f_name = f_name, f_overloads = f_overloads)
                    safe_write(f_name, r)

            render('method', all_m)
            render('non member function', all_friend_functions)

            os.chdir(cur_dir)

        all_functions = self.regroup_func_by_names(self.all_functions_gen())

        docs = dict ( (n, [self.process_doc_function(f) for f in fs]) for (n,fs) in all_functions.items())

        for f_name, f_overloads in all_functions.items():
            print " ... function " + f_name
            cur_dir = os.getcwd()
            mkchdir_for_one_class(f_overloads[0])
            print " ..... located: ", f_overloads[0].location.file.name
            r = render_fnt( doc_methods = docs[f_name], parent_class = None, f_name = f_name, f_overloads = f_overloads)
            safe_write(f_name, r)            
            os.chdir(cur_dir)

        print "... done"


