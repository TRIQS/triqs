import sys
import re
import os
from mako.template import Template
import importlib

# the correspondance c type -> py_type
c_to_py_type = {'void' : 'None', 'int' : 'int', 'long' : 'int', 'double' : "float", "std::string" : "str"}

# Translation for formatting
basic_types_formatting = {'double' : 'd', 'int' : 'i'}

def translate_c_type_to_py_type(t) :
    # mainly for doc signatures...
    if t in c_to_py_type : return c_to_py_type[t]
    m = re.match('std::vector<(.*)>',t)
    if m: return "list[%s]"%translate_c_type_to_py_type(m.group(1))
    # numpy, etc...
    return t

class cfunction :
    """
       Representation of one overload of a C++ function.
       Data :
        - c_name : the name of the function in C++
        - doc : the doc string.
        - is_constructor : boolean
        - rtype : the C++ type returned by the function. None for constructor
        - args : The list of arguments, as [ (c_type, variable_name, default_value)]
                 default_value is None when there is no default.
        - signature : (incompatible with rtype and args).
                rtype( arg1 name1, arg2 name2, ....)
        - calling_pattern : Pattern to rewrite the call of the c++ function,
              it is a string, using self_c, argument name and defining result at
              the end if rtype != void
              e.g., teh default pattern is : 
              auto result = self_c.method_name(a,b,c).
              INCOMPATIBLE with c_name.
              If c_name is given, the default calling_pattern is made.
    """
    def __init__(self, doc = '', is_method = False, no_self_c = False, **kw) :
      """ Use keywords to build, from the data. Cf doc of class"""
      self.c_name = kw.pop("c_name", None)
      self._calling_pattern = kw.pop("calling_pattern", None)
      self.is_constructor = kw.pop("is_constructor", False)
      self.no_self_c = no_self_c # do not generate self_c reference, in some rare calling_pattern. Avoid a warning.
      assert self.c_name or self._calling_pattern or self.is_constructor, "You must specify c_name or calling_pattern"
      assert not(self.c_name and self._calling_pattern), "You can not specify c_name and calling_pattern"
      self.doc = doc
      self.is_method = is_method
      self.args = []
      if 'signature' in kw :
        assert 'rtype' not in kw and 'args' not in kw, "signature and rtype/args are not compatible"
        signature = kw.pop("signature")
        m = re.match(r"\s*(.*?)\s*\((.*)\)",signature)
        self.rtype, args = m.group(1).strip() or None, m.group(2).strip()
        def f(): # analyse the argument, be careful that , can also be in type, like A<B,C>, so we count the < >
            acc = ''
            for s in args.split(',') :
                acc += (',' if acc else '') + s
                if acc.count('<') == acc.count('>') :
                    r, acc = acc,''
                    yield r
        args = [ re.sub('=',' ',x).split() for x in f() if x] # list of (type, name, default) or (type, name)
      else:
          self.rtype = kw.pop("rtype", None)
          args = kw.pop('args',())
      for a in args: # put back the default if there is none
        if a[0] == 'const' : a = [' '.join(a[:2])] + list(a[2:]) 
        if a[1] == '*' : a = [' '.join(a[:2])] + list(a[2:]) 
        if len(a) == 2 : (t,n),d = a,None
        elif len(a) == 3 : t,n,d = a
        else : raise RuntimeError, "Syntax error in overload: args = %s"%args
        self.args.append([t,n,d])
      #assert len(kw)==0, "unknown parameters %s"%kw.keys()
      if self.is_constructor :
        assert self.rtype == None,  "Constructor must not have a return type"
        self.is_method = False

    def calling_pattern(self) :
        if self._calling_pattern : return self._calling_pattern
        s= "%s result = "%self.rtype if self.rtype != "void" else ""
        self_c = "self_c." if self.is_method else ""
        # the wrapped types are called by pointer !
        # do we want to keep it this way ?
        return "%s %s%s(%s)"%(s,self_c, self.c_name , ",".join([ ('*' if t in module_.wrapped_types else '') + n for t,n,d in self.args]))

    def signature (self):
        """Signature for the python doc"""
        name = self.c_name
        rtype = translate_c_type_to_py_type(self.rtype) if self.rtype else ''
        args_rep = ", ".join(["%s %s%s"%(translate_c_type_to_py_type(t),n,' = ' + str(d) if d else '') for t,n,d in self.args])
        return "({args_rep}) -> {rtype}".format(**locals())

    def c_signature (self):
        """Signature for the C++ calling errors"""
        name = self.c_name if self.c_name else "(no C++ name)"
        rtype = self.rtype if self.rtype else ''
        args_rep = ", ".join(["%s %s"%(t,n) for t,n,d in self.args])
        return "{name}({args_rep}) -> {rtype}".format(**locals())

    def __repr__(self):
        return "C++ function of signature :  %s"%(self.signature())

    def format(self) : 
      def f(t) : 
          return basic_types_formatting[t] if t in basic_types_formatting else 'O&'
      l1 = [ f(t) for  t,n,d in self.args if d==None]
      l2 = [ f(t) for  t,n,d in self.args if d!=None]
      if l2 : l2.insert(0,'|')
      return ''.join(l1 + l2) 
        
    def generate_doc(self) :
        doc = "\n".join([ "   " + x.strip() for x in self.doc.split('\n')])
        return "Signature : %s\n%s"%( self.signature(),doc)

class pyfunction :
    """
       Representation of one python function of the extension
       Data :
        - py_name : name given in Python
        - doc : the doc string.
        - overloads : a list of cfunction objects representing the various C++ overloads of the function
        - python_precall : a python function_ to be called before the call of the C++ function
                           The function must take  F(*args, **kw) and return (args, kw)
        - python_postcall : a python function_ to be called after the call of the C++ function
                           The function must take a python object, and return one...
        - module : module path to the function [pure python only]
    """
    def __init__(self, py_name, is_method = False, doc = '', python_precall = None, python_postcall = None, arity = None, **unused) : 
      """ Use keywords to build, from the data. Cf doc of class"""
      self.py_name =py_name       # name given in python 
      self.doc = doc        
      self.arity = arity
      self.is_method = is_method  # can be a method, a function...
      self.python_precall, self.python_postcall  = python_precall, python_postcall
      self.overloads = [] # List of all C++ overloads
      self.do_implement = True # in some cases, we do not want to implement it automatically, (special methods).
      self.is_constructor = False

    def add_overload(self, **kw) :
        self.overloads.append(cfunction(**kw))

    #def __repr__(self):
    #    return ""

    def has_pre_post_call(self) : return self.python_precall or self.python_postcall

    def generate_doc(self) :
        s = "\n".join([self.doc, "\n"] + [f.generate_doc() for f in self.overloads])
        return repr(s)[1:-1] # remove the ' ' made by repr

class pure_pyfunction_from_module :
    """
       Representation of one python function defined in Python code in an external module.
       Will be use to make a pure python method of an object, or or a module.
       Data :
        - py_name : name given in Python
        - doc : the doc string.
        - module : module path to the function [pure python only]
    """
    def __init__(self, py_name, module, doc = '') :
      """ """
      self.py_name, self.module, self.doc = py_name, module, doc 
      try : 
          m = __import__(module.rsplit('.')[-1])
          f = getattr(m,py_name)
          self.doc = f.__doc__ # get the doc and check the function can be loaded.
      except :
          print " I can not import the function %s from the module %s"%(py_name,module)
          raise

    #def __repr__(self):
    #return ""

    def generate_doc(self) :
        return self.doc

class python_function: 
    """
    A python function, given as a function.
    Its code gets analysed and will be put into the C++ wrapper, to avoid import.
    """
    def __init__(self, name, f) : 
        """ """
        self.name, self.f = name,f
        import inspect as ins
        self.code = "\n".join(['"%s\\n"'%line.rstrip().replace('"', '\\"') for line in ins.getsourcelines(self.f)[0]])
        self.doc = f.__doc__ # UNUSED AT THE MOMENT     

class property_ :
    """
      Representation of a property of a class
      Data :
       - name : Name, in python
       - getter : the cfunction representing the get part
       - setter : the cfunction representing the set part or None if the property if read only
       - doc : the doc string.
    """
    def __init__(self, name, getter, setter = None, doc = '') :
      self.name, self.getter, self.setter, self.doc = name, getter, setter, doc 
      
class member_ :
    """
      Representation of a member of a class
      Data :
       - c_name : name of the variable in C++
       - py_name : name of the variable in python
       - c_type : type of the C++ variable
       - read_only : bool
       - doc : the doc string.
    """
    def __init__(self, c_name, c_type, py_name = None, read_only = False, doc = '') :
      self.c_name, self.c_type, self.py_name, self.doc, self.read_only = c_name, c_type, py_name or c_name, doc, read_only

class iterator_ :
    """
      Representation of an iterator
      Data :
       - c_type : type of the C++ variable
       - c_cast_type : 
       - begin, end : 
    """
    def __init__(self,c_type = "const_iterator", c_cast_type = None, begin = "std::begin", end = "std::end") : 
      self.c_type, self.c_cast_type, self.begin, self.end = c_type, c_cast_type, begin, end

def is_type_a_view(c_type) : 
    return c_type.split('<', 1)[0].endswith("_view") # A bit basic ?

def regular_type_if_view_else_type(c_type) : 
        return "typename %s::regular_type"%c_type if is_type_a_view(c_type) else c_type

class class_ :
    """
       Representation of a wrapped type
       Data :
        - c_type : C++ type to be wrapped.
        - c_type_absolute : full path of c_type, no using, no alias (need for
          the py_converter hpp file)
        - py_type : Name given in Python
        - doc : the doc string.
        - c_type_is_view : boolean
        - methods : a dict : string -> pyfunction for each method name
        - constructor : a pyfunction  for the constructors.
        - properties : a dict : string -> property_
        - members : a dict : string -> member_
    """
    hidden_python_function = {} # global dict of the python function to add to the module, hidden for the user, for precompute and so on
    def __init__(self, c_type, py_type, c_type_absolute = None, hdf5 = False, arithmetic = None, serializable = None, is_printable = False, doc = '' ) :
      self.c_type = c_type
      self.c_type_absolute = c_type_absolute or c_type
      self.c_type_is_view = is_type_a_view(c_type)
      self.implement_regular_type_converter = self.c_type_is_view # by default, it will also make the converter of the associated regular type
      if self.c_type_is_view : 
          self.regular_type = 'typename ' +  self.c_type + '::regular_type'
          self.regular_type_absolute = 'typename ' +  self.c_type_absolute  + '::regular_type'
      self.py_type = py_type
      c_to_py_type[self.c_type] = self.py_type # register the name translation for the doc generation
      self.hdf5 = hdf5
      assert serializable in [None, "boost", "tuple"]
      self.serializable = serializable
      self.is_printable = is_printable
      self.iterator = None
      self.doc = doc 
      self.methods = {}
      self.pure_python_methods= {}
      self.constructor = None
      self.members= []
      self.properties= []

      # Init arithmetic
      # expect a tuple : "algebra", "scalar1", "scalar2", etc...
      self.number_protocol = {}
      if arithmetic :
          add =  arithmetic[0] in ("algebra", "abelian_group", "vector_space", "only_add")
          abelian_group = arithmetic[0] in ("algebra", "abelian_group", "vector_space")
          vector_space = arithmetic[0] in ("algebra", "vector_space")
          algebra = arithmetic[0] in ("algebra")
          if add :
            # add
            add = pyfunction(py_name ="__add__")
            add.arity = 2
            add.add_overload (calling_pattern = "+", args = [(self.c_type,'x'), (self.c_type,'y')], rtype = self.c_type)
            self.number_protocol['add'] = add
          if abelian_group :
            #sub
            sub = pyfunction(py_name ="__sub__")
            sub.arity = 2
            sub.add_overload (calling_pattern = "-", args = [(self.c_type,'x'), (self.c_type,'y')], rtype = self.c_type)
            self.number_protocol['subtract'] = sub
          if vector_space :
            # mul
            mul = pyfunction(py_name ="__mul__")
            mul.arity = 2
            for scalar in arithmetic[1:] :
               mul.add_overload (calling_pattern = "*", args = [(self.c_type,'x'), (scalar,'y')], rtype = self.c_type)
               mul.add_overload (calling_pattern = "*", args = [(scalar,'x'), (self.c_type,'y')], rtype = self.c_type)
            self.number_protocol['multiply'] = mul
            # div
            div = pyfunction(py_name ="__div__")
            div.arity = 2
            for scalar in arithmetic[1:] :
               div.add_overload (calling_pattern = "/", args = [(self.c_type,'x'), (scalar,'y')], rtype = self.c_type)
            self.number_protocol['divide'] = div
          if algebra :
            mul.add_overload (calling_pattern = "*", args = [(self.c_type,'x'), (self.c_type,'y')], rtype = self.c_type)

    def prepare_for_generation(self) :
        # Called just before the code generation
        self.has_mapping_protocol = '__getitem__impl' in self.methods or '__len__impl' in self.methods
        if '__setitem__impl' in self.methods and not  '__getitem__impl' in self.methods : raise RuntimeError, "Can not generate a class with a setter and no getter"
        
    def add_method(self, py_name, **kw):
        """
        Add a method name (or an overload of method name).
        All arguments passed by keywords to cfunction construction
        """
        f = cfunction(is_method = True, **kw)
        if py_name not in self.methods : self.methods[py_name] = pyfunction(py_name = py_name, is_method = True, **kw)
        self.methods[py_name].overloads.append(f)

    def add_iterator(self, **kw) : 
        self.iterator = iterator_(**kw)

    def add_pure_python_method(self, f, py_name = None):
        """
        Add a method name (or an overload of method name).
        f can be : 
           - a string module1.module2.fnt_name
           - a function in python...
        """
        def process_doc(doc) : 
            return doc.replace('\n','\\n') if doc else ''
        if type(f) ==type('') : 
           module, name = f.rsplit('.',1)
           try : 
               m = __import__(module.rsplit('.')[-1])
               doc = m.__dict__[name].__doc__
           except : 
               raise
           self.pure_python_methods[py_name or name] = pure_pyfunction_from_module(py_name = name, module = module), 'module', process_doc(doc)
        elif callable(f) :
           assert py_name == None
           self.hidden_python_function[f.__name__] = f
           self.pure_python_methods[f.__name__] = f.__name__, 'inline', process_doc(f.__doc__)
        else : raise ValueError, "argument f must be callable or a string"

    def add_constructor(self, build_from_regular_type = True, **kw):
        """
        Add an overload of a constructor
        All arguments passed by keywords to function_ construction
        """
        assert 'c_name' not in kw, "No c_name here"
        assert 'calling_pattern' not in kw, "No calling_pattern here"
        f = cfunction(c_name = "__init__", is_constructor = True, **kw)
        build_type = regular_type_if_view_else_type(self.c_type) if self.c_type_is_view and build_from_regular_type else self.c_type
        all_args = ",".join([ ('*' if t in module_.wrapped_types else '') + n  for t,n,d in f.args])
        #all_args = ",".join([ ('*' if t in module_.wrapped_types else '') + n + (' = ' + d if d else '') for t,n,d in f.args])
        f._calling_pattern = "((%s *)self)->_c ->"%self.py_type + ('operator =' if not self.c_type_is_view else 'rebind') + " (%s (%s));"%(build_type,all_args)
        if not self.constructor : 
            self.constructor = pyfunction(py_name = "__init__", **kw)
            self.constructor.is_constructor = True
        self.constructor.overloads.append(f)

    def add_member(self, **kw):
        """
        Add a class member
        All arguments passed by keywords to function_ construction
        """
        m = member_(**kw)
        self.members.append(m)

    def add_property(self,  getter, setter = None, name = None, doc = ''):
        """
        Add a property
        """
        if not isinstance(getter, str) : getter.is_method = True
        self.properties.append( property_(name = name or getter.c_name, getter = getter, setter = setter, doc = doc) )

    def add_call(self, **kw) :
        """
        Add a the __call__ operator
        """
        if 'c_name' not in kw and 'calling_pattern' not in kw : kw['c_name']= "operator()"
        self.add_method(py_name = "__call__", **kw)

    def add_len(self, c_name = None, calling_pattern = None, doc = "Length") :
        """
        Add the len operator
        """
        if not c_name and not calling_pattern : c_name = "size"
        self.add_method(py_name = "__len__impl", c_name = c_name, calling_pattern = calling_pattern, signature="int()", doc= doc)
        self.methods['__len__impl'].do_implement = False # do not implement automatically, the signature is special

    def add_getitem(self, signature, c_name = None, calling_pattern = None, doc = "operator[]" ) :
        """
        Add a the __getitem__ operator
        """
        assert not(c_name and calling_pattern)
        if calling_pattern :
           self.add_method(py_name = "__getitem__impl", calling_pattern = calling_pattern, doc = doc, signature = signature)
        else :
           self.add_method(py_name = "__getitem__impl", c_name = c_name or "operator[]", doc = doc, signature = signature)

    def add_setitem(self, signature, calling_pattern = None, doc = "operator[]", **d ) :
        """
        Add a the __setitem__ operator
        """
        self.add_method(py_name = "__setitem__impl", calling_pattern = calling_pattern or "self_c[i] = v", doc = doc, signature = signature, **d)

    def add_method_copy(self) :
        """Add a method copy, that make a DEEP copy, using triqs make_clone"""
        self.add_method(py_name = "copy", calling_pattern = self.c_type + " result = make_clone(self_c)", signature = self.c_type +"()", doc = "Make a copy (clone) of self")

    def add_method_copy_from(self) :
        """Add a copy_from, using C++ assignment"""
        # other by pointer, it is necessarly a wrapped type
        self.add_method(py_name = "copy_from", calling_pattern = " self_c = *other", signature = 'void(' + self.c_type +" other)", doc = "Assignment")

class enum_ : 
    """
       Representation of an enum
       Data :
        - c_name : name in C
        - values : list of string representing the enumerated
        - doc : the doc string.
    """
    def __init__(self, c_name, values, c_name_absolute = None, doc = '') : 
      self.c_name, self.c_name_absolute, self.values, self.doc = c_name, c_name_absolute or c_name, values, doc
 
class module_ :
    """
       Representation of a module
       Data :
        - name : name of the module
        - doc : the doc string.
        - classes : dict : string -> class_. Key is the Python type
        - c_types : dict : string -> string. Correspondance Python type -> C++ type
        - functions : dict : string -> function_. Modules functions. Key is the python name.
        - include_list : a list of files to include for compilation
    """
    wrapped_types = {}

    def __init__(self, full_name,  doc = '') : 
      self.full_name = full_name
      self.name = full_name.rsplit('.',1)[-1]
      self.doc = doc 
      self.classes = {}
      self.functions = {}
      self.include_list = []
      self.enums = []
      self.using =[]
      self.python_functions = {}
      self.hidden_python_functions = {}
     
    def add_class(self, cls):
        if cls.py_type in self.classes : raise IndexError, "The class %s already exists"%cls.py_type
        self.classes[cls.py_type] = cls
        self.wrapped_types[cls.c_type] = cls
        self.wrapped_types[cls.c_type_absolute] = cls # we can call is by its name or its absolute name

    def add_function(self, **kw):
        if "name" in kw :
            assert "py_name" not in kw and "c_name" not in kw, "name or c_name,py_name"
            name = kw.pop("name")
            kw["py_name"] = name
            kw["c_name"] = name
        py_name = kw["py_name"]
        f = cfunction(**kw)
        if py_name not in self.functions : self.functions[py_name] = pyfunction(**kw)
        self.functions[py_name].overloads.append(f)

    def add_python_function(self, f, name = None, hidden = False) : 
        assert callable(f)
        if not hidden : 
            self.python_functions[name or f.__name__] = python_function(name or f.__name__, f)
        else : 
            self.hidden_python_functions[name or f.__name__] = python_function(name or f.__name__, f)

    def add_include(self, *path) :
        self.include_list.extend(path)

    def add_using(self,ns) : 
        self.using.append(ns)

    def add_enum(self,**kw) : 
        self.enums.append( enum_(**kw))

    def get_proper_converter(self, t) :
        if t in basic_types_formatting : return ''
        if t in self.wrapped_types : return ',converter_for_parser_wrapped_type<'+t+'>'
        if t.split('<',1)[0].endswith("_view") : return ',converter_for_parser_view_type<'+t+'>'
        return ',converter_for_parser_non_wrapped_type<'+t+'>'

    def all_args_kw_functions(self) : 
        l = [ (f, self.name, None) for f in self.functions.values()] 
        for c in self.classes.values() : 
            l += [(m,c.py_type, c.c_type) for m in c.methods.values() if m.do_implement]
            if c.constructor : 
                l.append( (c.constructor,c.py_type, c.c_type))
        return l

    def prepare_for_generation(self) : 
        for c in self.classes.values() : c.prepare_for_generation()
        for n,f in class_.hidden_python_function.items() : 
            self.add_python_function(f,name = n, hidden=True)

    def generate_code(self, mako_template, wrap_file) :
        self.prepare_for_generation()
        tpl = Template(filename=mako_template)
        rendered = tpl.render(module=self, regular_type_if_view_else_type= regular_type_if_view_else_type, is_type_a_view = is_type_a_view)
        with open(wrap_file,'w') as f:
           f.write(rendered)

    def generate_py_converter_header(self, mako_template, wrap_file) :
        self.prepare_for_generation()
        tpl = Template(filename=mako_template)
        rendered = tpl.render(module=self)
        with open(wrap_file,'w') as f:
           f.write(rendered)

    #def generate_code_and_header(self, arglist) : 
    #    self.generate_code(mako_template = arglist.argv[0], wrap_file = arglist.argv[1])
    #    self.generate_py_converter_header(mako_template = arglist.argv[2], wrap_file = arglist.argv[3])
 
