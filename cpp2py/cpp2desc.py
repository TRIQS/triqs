import os, re, sys, itertools
from mako.template import Template

import cpp2py.clang_parser as CL
import util, doc, dependency_analyzer

class Cpp2Desc: 
    """ """
    def __init__(self, filename, namespaces=(), classes= (), namespace_to_factor= (), appname= '', 
                 modulename = '', moduledoc ='', use_properties = False, members_read_only = True,  converters = (),
                 compiler_options=None, includes= None, libclang_location = None, shell_command = '', parse_all_comments = True):
        """
           Parse the file at construction
           
           Parameters
           -----------

           filename : string
                      Name of the file to parse
           
           namespaces : list of string 
                      Restrict the generation to the given namespaces.
           
           classes : list of string
                      Restrict the generation to the given classes.
           
           appname  : string
                      Name of the application
           
           modulename : string
                      Name of the module
           
           moduledoc : string
                       Documentation of the module

           shell_command : string
                           script command that was called (to regenerate the desc file)

           use_properties : Boolean
                            Transform method with no args into property

           members_read_only : Boolean
                               The members are wrapped "read only"

           includes         : string, optional
                      Additional includes to add (-I xxx) for clang
           
           compiler_options : string, optional 
                      Additional option for clang compiler
           
           libclang_location : string, optional
                      Absolute path to libclang. By default, the detected one.
        """
        for x in ['filename', 'namespaces', 'classes', 'namespace_to_factor', 'appname', 'modulename', 'moduledoc', 
                  'use_properties', 'members_read_only', 'shell_command']:
            setattr(self, x, locals()[x])
        self.DE = dependency_analyzer.DependencyAnalyzer(converters)
        # parse the file
        self.root = CL.parse(filename, compiler_options, includes, libclang_location, parse_all_comments)

    # ---------- Generate the AST nodes for every classes, functions, functions and methods
   
    namespaces_to_skip = ['std', 'boost', 'detail', 'impl']

    def keep_ns(self, n):
        ns = CL.fully_qualified(n) 
        if any((x in ns) for x in self.namespaces_to_skip) : return False
        return len(self.namespaces) == 0 or any((ns in x) for x in self.namespaces)
    
    def keep_cls(self, c):
        """ 
           The filter to keep a class/struct or an enum : 
            if we a namespace list, it must be in it. 
            if we have an explicit self.classes : c must be into it
            else it has to be in the file given to c++2py 
        """
        if CL.is_template(c) or ("ignore_in_python" in CL.get_annotations(c)): return False
        if self.namespaces:
            ns = CL.get_namespace(c) 
            if not any((x in ns) for x in self.namespaces) : return False
        if self.classes: 
            return c.spelling in self.classes or CL.fully_qualified(c) in self.classes
        return c.location.file.name == self.filename
        
    def keep_fnt(self, f):
        # Same as class, but eliminate operator, begin, end.
        if f.spelling.startswith('operator') or f.spelling in ['begin','end'] : return False
        return self.keep_cls(f)
    
    def all_functions_gen(self):
        """Generates all the AST nodes of functions"""
        return CL.get_functions(self.root, self.keep_fnt, traverse_namespaces = True, keep_ns = self.keep_ns)

    def all_classes_gen(self):
        """Generates all the AST nodes of classes"""
        return CL.get_classes(self.root, self.keep_cls, traverse_namespaces = True, keep_ns = self.keep_ns)

    def all_enums_gen(self):
        """Generates all the AST nodes of enums"""
        return CL.get_enums(self.root, self.keep_cls, traverse_namespaces = True, keep_ns = self.keep_ns)

    def get_all_functions_and_methods(self):
        """ AST nodes for every function, class methods and constructors"""
        for f in self.all_functions_gen(): 
            yield f
        for c in self.all_classes_gen(): 
            for m in CL.get_methods(c):
                yield m
            for m in CL.get_constructors(c):
                yield m

    def get_all_param_classes(self):
        """ yields all param classes """
        for f in self.get_all_functions_and_methods():
            r = util.get_decl_param_class(f)
            if r : 
                yield r

    def get_all_params_ret_type(self, param_cls_list):
        """Yields every parameters and return type of every methods and functions"""
       
        for f in self.all_functions_gen():
            yield getattr(f, 'result_type', None)
            for p in CL.get_params(f) : 
                yield p.type

        for x in itertools.chain(param_cls_list, self.all_classes_gen()): 
            for m in CL.get_members(x, False): # False : no inherited
                yield m.type
            for m in itertools.chain(CL.get_constructors(x), CL.get_methods(x)):
                # A minimal filter, do not reuse self.keep_fnt here, because the namespace is N1::N2:...::ClassName
                if CL.is_template(m) or ("ignore_in_python" in CL.get_annotations(m)): continue
                yield getattr(m, 'result_type', None)
                for p in CL.get_params(m) : 
                    yield p.type

    def get_public_methods(self, c):
        """
        Parameters
        -----------

        c:  AST node
            a cursor to a class
        
        Returns
        --------
        A list of cursors to the methods
        return : a tuple (proplist, methodlist) where proplist : a list of property_  and methodlist : the others methods
        """
        keep = lambda m : CL.is_public(m) and not CL.is_template(m) and not ("ignore_in_python" in CL.get_annotations(m)) and not m.spelling.startswith('operator')
        return CL.get_methods(c, True, keep)

    def has_hdf5_scheme(self, c):
        """True iif the class c has a static method std::string hdf5_scheme()"""
        keep = lambda m : CL.is_public(m)
        for m in CL.get_methods(c, True, keep):
            if m.spelling == "hdf5_scheme" and m.is_static_method() and ('string' in m.result_type.spelling) and len(list(CL.get_params(m))) == 0: 
               return True
        return False

    def separate_method_and_properties(self, c):
        """
        Treatment of properties

        Parameters
        -----------

        method_list : a generator of the methods to treat
        
        Returns
        --------
          Tuple (proplist, methodlist) where
                  proplist : a list of property_
                  methodlist : the others methods
        """ 

        method_list = list(self.get_public_methods(c)) # MUST be a list, or the generator will be exhausted later in mlist = ...
        if not self.use_properties : return method_list, ()

        class property_:
            def __init__ (self, **kw) :
                self.__dict__.update(kw)

        def maybe_prop(m):
            return len(list(CL.get_params(m))) == 0 and not m.is_static_method()

        plist1 = [m for m in method_list if maybe_prop(m)]
        mlist =  [m for m in method_list if not maybe_prop(m)]
        plist = []
    
        OUT, SEP = '', '        '   
        for m in plist1:
            n, set_m = m.spelling, None
            if n.startswith('set_') : continue # do nothing, will be treated with the get_
            if n.startswith('get_') : 
                # treat the corresponding setter 
                n = n[4:] 
                set_m = next( (m for m in plist1 if m.spelling == 'set_' + n), None)
                if set_m : 
                    p = list(CL.get_params(set_m)) 
                    if set_m.result_type.spelling == "void" and len(p) ==1 :
                        if not util.decay(p[0].spelling) == m.result_type.spelling :
                            OUT += SEP + "Warning :\n"
                            OUT += SEP + "    in get_%s/set_%s\n" %(X,X)
                            OUT += SEP + "    The type taken from set_%s is not the return type of get_%s\n"%(X,X)
                            OUT += SEP + "    Expected %s\n"%m.result_type.spelling
                            OUT += SEP + "    Got %s\n"% decay(p[0].spelling)
                            OUT += SEP + "    I am not adding the setter to the property\n"
                            set_m = None
            OUT += SEP + "%s %s\n" %(m.spelling, set_m.spelling if set_m else '')
            plist.append(property_(name= n, doc = doc.make_doc(m), getter = m, setter = set_m))

        if OUT: 
            print "   Class %s : transforming to property : \n%s"%(c.spelling, OUT)

        return mlist, plist
  
    def make_signature_for_desc(self, f, is_constructor = False, is_free_function = False):
        """Given a node of a function/methods, it makes the signature for desc file"""
        # first format the arguments
        def cls(t) :
            tname = util.decay(t)
            tname = tname.replace(' ','')
            for ns in self.namespace_to_factor : 
                tname = re.sub(ns + '::','',tname)
            return tname
        
        if util.use_parameter_class(f) : 
            r = '**%s'%cls(CL.get_params(f).next().type.spelling)
        else:
            plist = [ (cls(p.type.spelling), p.spelling, CL.get_param_default_value(p)) for p in CL.get_params(f)]
            r = ', '.join("%s %s"%(t, n) + (" = %s"%d.replace('"','\\"') if d else "") for t, n, d  in plist ) 

        if is_constructor:
            return "(%s)"%r
        else :
            f_name = f.spelling if not is_free_function else CL.fully_qualified(f)
            return ("%s %s (%s)"%(cls(f.result_type.spelling), f_name, r)).strip()

    def generate_desc_file(self, output_filename, verbose  = True):
        """ Makes the desc file"""
         
        # First treat the parameter class if any (classes passed by dictionnary that MUST be converted)
        param_cls_list = list(self.get_all_param_classes())
        for c in param_cls_list : 
            open('parameters_%s.rst'%c.spelling, 'w').write(doc.doc_param_dict_format(CL.get_members(c, True)))

        # Precompute
        self.all_enums     = list(self.all_enums_gen())
        self.all_classes   = list(self.all_classes_gen())
        self.all_functions = list(self.all_functions_gen())
        self.param_cls_list = param_cls_list
       
        # checks
        for c in param_cls_list:
            for m in CL.get_members(c, True):
                assert CL.is_public(m), "Parameter class : all members must be public. %s::%s is not"%(c.spelling, m.spelling) 

        # analyse the modules and converters that need to be added
        print "Analysing dependencies"
        types_being_wrapped_or_converted = param_cls_list + self.all_classes + self.all_enums 
        import_list, converters_list = self.DE(self.get_all_params_ret_type(param_cls_list), types_being_wrapped_or_converted)
        if any(map(self.has_hdf5_scheme, self.all_classes)):
            converters_list.append("triqs/cpp2py_converters/h5.hpp")
   
        # Reporting 
        if self.all_classes:
            print "Wrapping classes:"
            for c in self.all_classes: 
                print "   ", c.spelling
        if self.all_enums:
            print "Wrapping enums:"
            for c in self.all_enums: 
                print "   ", c.spelling
        if self.all_functions:
            print "Wrapping functions:"
            for c in self.all_functions: 
                print "   ", c.spelling
        if param_cls_list:
            print "Generating converters for :"
            for c in param_cls_list: 
                print "   ", c.spelling

        # Render mako
        print "Generating " + output_filename
        tpl = Template(filename= util.script_path() + '/mako/desc.py', strict_undefined = True)
        rendered = tpl.render(W = self, CL = CL, doc = doc, util = util,  
                              import_list = import_list, converters_list = converters_list, using_list = list(self.namespaces) + list(self.namespace_to_factor))
        open(output_filename, "w").write(util.clean_end_and_while_char(rendered))

