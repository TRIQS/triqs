import os, re, sys, itertools
from collections import OrderedDict

import cpp2py.clang_parser as CL
import synopsis, renderers, global_vars
from processed_doc import ProcessedDoc

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
    def __init__(self, filename, namespaces=(), compiler_options=None, includes=None, system_includes=None, libclang_location = None, parse_all_comments = False, target_file_only = False):
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

           parse_all_comments : bool
                      Grab all comments, including non doxygen like [default = False]

           target_file_only : bool
                      Neglect any included files during desc generation [default = False]
        """
        self.filename, self.namespaces, self.target_file_only = filename, namespaces, target_file_only
        self.root = CL.parse(filename, compiler_options, includes, system_includes, libclang_location, parse_all_comments)
        # Root of examples
        global_vars.examples_root = os.path.dirname(filename) + '/examples' 

    # ------------------------
    
    def run(self, output_directory):

        print "Generating the documentation ..."
        mkchdir(output_directory)

        # The namespace are going to be cleaned in the parameters in the synopsis
        global_vars.process_type_name_ns_to_clean = [x + '::' for x in self.namespaces]
       
        # Gather and preprocess all classes and functions
        d = {}
        for ns in self.namespaces:
            d[ns] = self.analyse_one_ns(ns)
       
        # Cross linking
        # synopsis will be called by the renderers, and they need to know the class we are documenting
        global_vars.synopsis_class_list = sum((cls_list for (cls_list, fnt_list, using_list) in d.values()), list())
       
        for ns in self.namespaces:
            self.run_one_ns(ns, *d[ns])

    # ----------------------

    # regroup the functions into a list of overloads
    def regroup_func_by_names(self, fs):
        """ Given a list of functions, regroup them by names in an OrderedDict. The Name will serve a filename """
        d = OrderedDict()
        def decay(name):
            """Replace operator X by better names for"""
            return name
            # if 'operator' not in name : return name
            # a, ca, c, co = '+ - * /',  '+= -= \*= /=',  "== !=",  r" comparison"
            # d = {'*=' : ca,'+=' : ca,'/=' : ca,'-=' : ca,'*' : a,'+' : a,'/' : a,'-' : a, '==': c, '!=' : c, '<': co, '>' : co, '<=' : co, '>=' : co}
            # n = name.replace('operator','').strip()
            # return 'operator' + d[n] if n in d else name
        for f in fs:
            d.setdefault(decay(f.spelling),[]).append(f)
        return d
   
    # ------------------------
    
    def analyse_one_ns(self, namespace):

        print "*** Namespace %s ***"%namespace

        # ----------------------
        #      Filters
        # ----------------------
        def keep_ns(n):
            """Given a namespace node n, shall we keep it ?"""
            ns = CL.fully_qualified(n) 
            return ns in namespace
        
        def keep_cls(c):
            """ Given a class node, shall we keep it ? 

               Reject if no raw_comment.
               Keeps if its namespace is EXACTLY in self.namespaces
               e.g. A::B::cls will be kept iif A::B is in self.namespaces, not it A is.

               The filter to keep a class/struct or an enum :
                 it must have a raw comment
                 if we a namespace list, it must be in it.
                 if target_file_only it has to be in the file given to c++2py
            """
            if c.spelling.startswith('_') : return False
            # FIXME : Commented for debug
            if not c.raw_comment : return False
            if namespace:
                qualified_ns = CL.get_namespace(c) 
                if qualified_ns != namespace : return False
            return (c.location.file.name == self.filename) if self.target_file_only else True
        
        def keep_fnt(f):
            #print "RAW", f.spelling, f.raw_comment
            if not f.raw_comment : return False
            if  f.spelling.startswith('operator') or f.spelling in ['begin','end'] : return False
            return keep_cls(f) 

        def keep_using(c):
            #if not c.raw_comment : return False
            if namespace:
                qualified_ns = CL.get_namespace(c) 
                if qualified_ns != namespace : return False
            return (c.location.file.name == self.filename) if self.target_file_only else True

        # ----------------------

        # A list of AST nodes for classes
        classes = CL.get_classes(self.root, keep_cls, traverse_namespaces = True, keep_ns = keep_ns)
        classes = list(classes) # make a list to avoid exhaustion of the generator
  
        D = OrderedDict()
        for cls in classes: 
            cls.namespace = CL.get_namespace(cls)
            cls.name = CL.get_name_with_template_specialization(cls) or cls.spelling 
            cls.fully_qualified_name = '::'.join([cls.namespace, cls.name])
            cls.name_for_label = synopsis.make_label(cls.fully_qualified_name)
            D[cls.fully_qualified_name] = cls
            # print "CLASS", cls.name
            # print "CLASS", cls.fully_qualified_name 
            # print "CLASS", cls.name_for_label
            # print "CLASS", "----------------------"
       
            print " ... class :  %s"%cls.fully_qualified_name, cls.location
            #assert ',' not in cls.fully_qualified_name, "Not implemented"

            # process the doc of the class and add it to the node
            cls.processed_doc = ProcessedDoc(cls)

            # all methods and constructors
            # we build a OrderedDict of the constructors (first) and the methods, in order of declaration 
            constructors = list(CL.get_constructors(cls))
            for f in constructors : f.is_constructor = True # tag them for later use
            methods = OrderedDict()
            if constructors: methods['constructor'] = constructors
            methods.update(self.regroup_func_by_names(CL.get_methods(cls, True))) # True : with inherited 
            #methods.update(self.regroup_func_by_names(CL.get_methods(cls, True, keep = keep_fnt))) # True : with inherited 

            # all non member functions
            friend_functions = self.regroup_func_by_names(CL.get_friend_functions(cls))
            #friend_functions = self.regroup_func_by_names(CL.get_friend_functions(cls, keep = keep_fnt))

            # Analyse the doc string for all methods and functions, and store the result in the node itself
            for (n,f_list) in (methods.items() + friend_functions.items()):
                for f in f_list:
                    f.processed_doc = ProcessedDoc(f)

            # attach to the node
            cls.methods, cls.friend_functions = methods, friend_functions

            # members 
            cls.members = list(CL.get_members(cls, True)) 
            
            # using 
            cls.usings = list(CL.get_usings(cls)) # , keep_using)) 
             
        # Eliminate doublons, like forward declarations
        classes = D.values()

        # A list of AST nodes for the methods and functions
        functions = CL.get_functions(self.root, keep_fnt, traverse_namespaces = True, keep_ns = keep_ns)
        functions = list(functions) # make a to avoid exhaustion of the generator

        # Analyse the doc strings 
        for f in functions:
            f.processed_doc = ProcessedDoc(f)
        
        # Find the using of this namespace, and make the list unique based on the fully_qualified_name 
        usings = list(CL.get_usings(self.root, keep_using, traverse_namespaces = True, keep_ns = keep_ns)) 
        D = OrderedDict()
        for c in usings:
            c.namespace = CL.get_namespace(c)
            c.fully_qualified_name = '::'.join([c.namespace, c.spelling])
            D[c.fully_qualified_name] = c
        usings = D.values()

        return classes, functions, usings

    # ------------------------
    
    def run_one_ns(self, namespace, classes, functions, usings):

        # c : AST node of name A::B::C::clsname makes and cd into A/B/C
        def mkchdir_for_one_node(node): 
            mkchdir(* ( CL.fully_qualified_name(node).split('::')[:-1]))

        # First treat the class
        for c in classes:
            if not c.spelling.strip() : 
                print "Skipping a class with an empty name !"
                continue

            # One directory for the class : make it and cd into it
            cur_dir = os.getcwd()
            mkchdir_for_one_node(c)

            # the file for the class
            r = renderers.render_cls(c, c.methods, c.friend_functions)
            safe_write(synopsis.replace_ltgt(c.name), r)
             
            # create a directory with the class name and cd into it
            mkchdir(synopsis.replace_ltgt(c.name))

            # write a file for each function
            def render(message, d) : 
                for f_name, f_overloads in d.items():
                    print " ...... %s [%s]"%(f_name, message)
                    r = renderers.render_fnt(parent_class = c, f_name = f_name, f_overloads = f_overloads)
                    safe_write(f_name, r)

            render('method', c.methods)
            render('non member function', c.friend_functions)

            # Change back to up directory
            os.chdir(cur_dir)

        # Now treat the functions
        functions_by_name = self.regroup_func_by_names(functions)

        docs = dict ( (n, [ProcessedDoc(f) for f in fs]) for (n,fs) in functions_by_name.items())

        for f_name, f_overloads in functions_by_name.items():
            print " ... function " + f_name, "      [", f_overloads[0].location.file.name, ']'
            cur_dir = os.getcwd()
            mkchdir_for_one_node(f_overloads[0])
            r = renderers.render_fnt(parent_class = None, f_name = f_name, f_overloads = f_overloads)
            safe_write(f_name, r)            
            os.chdir(cur_dir)

        # namespace resume function
        cur_dir = os.getcwd()
        mkchdir(*namespace.split('::')[:-1])

        r = renderers.render_ns(namespace, functions_by_name, classes, usings)
        ns = namespace.split('::',1)[-1]
        safe_write(ns, r)

        os.chdir(cur_dir)
        


