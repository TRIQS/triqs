import sys, imp
import re
import os
from mako.template import Template
import importlib

# the correspondance c type -> py_type
c_to_py_type = {'void' : 'None', 'int' : 'int', 'long' : 'int', 'double' : "float", "std::string" : "str"}

# Translation for formatting of parsing converter.
basic_types_formatting = {'double' : 'd', 'int' : 'i'}

# Translate the name of the c++ type to the python type.
# for doc signatures.
def translate_c_type_to_py_type(t) :
    if t in c_to_py_type : return c_to_py_type[t]
    m = re.match('std::vector<(.*)>',t)
    if m: return "list[%s]"%translate_c_type_to_py_type(m.group(1))
    # numpy, etc...
    return t

class cfunction :
    """
       Representation of one overload of a C++ function or method.
    """
    def __init__(self, signature, calling_pattern = None, no_self_c = False, is_constructor = False,
             is_method = False,  is_static = False, release_GIL_and_enable_signal = False, c_name = None, doc = '') :
      """
        Parameters
        ----------

        signature : string
             signature of the function, with types, parameter names and default value
             rtype( arg1 name1, arg2 name2 = default2, ....)
             it can be :
                - a string :
                     rtype (arg1 name1, arg2 name2 = default2, ....)
                - a string :
                     rtype c_name ( arg1 name1, arg2 name2 = default2, ....)
                - a dict [expert only] : rtype -> string , args -> list of tuples [ (c_type, variable_name, default_value)]
                           where rtype is the C++ type returned by the function.

        calling_pattern : string
              A string containing a piece of C++ code to call the C++ function.
              This code can use :
                - self_c  : a reference to the C++ object (except for constructor, and static method).
                - self_class : the name of the class of the C++ object (only for static method)
                - the name of the parameters.
              It should define a "result" variable :
                - unless for a constructor or if the C++ return type is void.
                - result shall be of any type from which the C++ return type is (move) constructible.
              If calling_pattern is None, a default one is synthesized by the generator,
              assuming the C++ function has exactly the signature given by the signature parameter of this function
              including the c_name in it (which is then mandatory).

        no_self_c : boolean. 
                    Do not generate self_c reference in C++ code, in some rare calling_pattern. Avoid a compiler warning.
        is_constructor : boolean
        is_method : boolean
        is_static : boolean. 
                    If True, it is a static method
        release_GIL_and_enable_signal : boolean, expert only
          - For long functions in pure C++.
          - If True, the GIL is released in the call of the C++ function and restored after the call.
          - It also saves the signal handler of python and restores it after the call,
            and enables the C++ signal_handler.
          - This allows e.g. to intercept Ctrl-C during the long C++ function.
          - **Requirement** :
             The function wrapped must be pure C++, i.e.  no call whatsoever to the python C API, directly or indirectly.
             otherwise the behaviour is undefined.
        doc : string
            the doc string.
        
      """
      #c_name : internal use only
      self._calling_pattern = calling_pattern
      self.is_constructor = is_constructor
      self.no_self_c = no_self_c
      self.doc = doc
      self.is_method = is_method
      self.is_static = is_static
      self._dict_call =  None
      if is_static : assert is_method, "is_static only works with method"
      self.release_GIL_and_enable_signal = release_GIL_and_enable_signal
      assert isinstance(signature, str) or isinstance(signature, dict), "Signature must be a string of a dict: cf doc"
      self.c_name = c_name # Not none for internal call only
      ## Analyse signature.
      self.args, self.namespace = [], ''
      if isinstance(signature, str) : # it is a string, we analyse it to get the rtype, and args
        signature = re.sub('operator\(\s*\)','__operator_call',signature) # temp. replacement, to make the regex easier
        m = re.match(r"\s*(.*?)\s*\((.*)\)",signature)
        self.rtype, args = m.group(1).strip() or None, m.group(2).strip()
        # extract the c_name if present
        if self.rtype :
            spl = self.rtype.strip().rsplit(' ',1)
            if not is_constructor and len(spl)> 1 and '>' not in spl[-1] :
                self.rtype, c_name_fully_qualified = spl
                self.namespace, self.c_name = c_name_fully_qualified.rsplit('::',1 ) if ':' in c_name_fully_qualified else ('', c_name_fully_qualified)
                if self.c_name == '__operator_call' : self.c_name = "operator()"
        if args.strip().startswith("**") : # special case : dict call
            assert calling_pattern is None, "When ** is given as argument, no calling pattern can be provided"
            self._dict_call =  args[2:]
            args, self.args = '','' # no argument
        def f(): # analyse the argument, be careful that , can also be in type, like A<B,C>, so we count the < >
            acc = ''
            for s in args.split(',') :
                acc += (',' if acc else '') + s.strip()
                if acc.count('<') == acc.count('>') :
                    r, acc = acc,''
                    yield r
        def g(a) :
            if '=' in a :
              l,r = a.split('=')
              return l.strip().rsplit(' ') + [r]
            else :
              return a.rsplit(' ',1)
        #args = [ re.sub('=',' ',x).split() for x in f() if x] # list of (type, name, default) or (type, name)
        args = [ g(x) for x in f() if x] # list of (type, name, default) or (type, name)
      else:
          # mostly internal use, give signature as a dict 
          self.rtype = signature.pop("rtype", None)
          args = signature.pop('args',())
          self.c_name = signature.pop("c_name", '')
      for a in args: # put back the default if there is none
        # treat the case when the type is const T *, or T* (e.g. const char *).
        # Need to regroup the first pieces.
        assert len(a)>1, 'Incorrect signature %s: did you forget the name of the parameter ?'%a
        if a[0] == 'const' : a = [' '.join(a[:2])] + list(a[2:])
        if a[1] == '*' : a = [' '.join(a[:2])] + list(a[2:])
        if len(a) == 2 : (t,n),d = a,None
        elif len(a) == 3 : t,n,d = a
        else : raise RuntimeError, "Syntax error in overload: args = %s"%args
        self.args.append([t.strip(),n.strip(),d])
      # end analyze signature

      # ensure no variable starts with __
      for t,n,d in self.args : 
          assert not n.startswith('__'), "Variables names starting with __ are reserved for internal use"
      #
      assert self.c_name or self._calling_pattern or self.is_constructor, "You must specify a calling_pattern or the signature must contain the name of the function"
      if self.is_constructor :
        assert self.rtype == None,  "Constructor must not have a return type"
        self.is_method = False

    def _get_calling_pattern1(self) :
        """Generation only: gets the calling_pattern or synthesize the default"""
        if not self._dict_call: return ''
        return """if (PySequence_Size(args)>0) {PyErr_SetString(PyExc_TypeError, "The function must be called only with named arguments"); goto error_return;}
           if (!convertible_from_python<%s>(keywds,true)) goto error_return;
           auto dict_transcript = convert_from_python<%s>(keywds);
        """%(self._dict_call, self._dict_call)

    def _get_calling_pattern(self) :
        """Generation only: gets the calling_pattern or synthesize the default"""
        if self._calling_pattern : return self._calling_pattern
        s = "%s result = "%self.rtype if self.rtype != "void" else ""
        self_c = ""
        if self.is_method:
            self_c = "self_c." if not self.is_static else "self_class::"
        # the wrapped types are called by pointer
        args = ",".join( n for t,n,d in self.args)
        args = args if self._dict_call is None else "dict_transcript" 
        return "%s %s%s(%s)"%(s,self_c, (self.namespace + '::' if self.namespace else '') + self.c_name, args)

    def _get_signature (self):
        """Signature for the python doc"""
        rtype = translate_c_type_to_py_type(self.rtype) if self.rtype else ''
        args_rep = ", ".join(["%s %s%s"%(translate_c_type_to_py_type(t),n,r' = ' + str(d).replace('"',"'") if d else '') for t,n,d in self.args])
        return "({args_rep}) -> {rtype}".format(**locals())

    def _get_c_signature (self):
        """Signature for the C++ calling errors"""
        name = self.c_name if self.c_name else "(no C++ name)"
        rtype = self.rtype if self.rtype else ''
        args_rep = ", ".join(["%s %s"%(t,n) for t,n,d in self.args])
        return "{name}({args_rep}) -> {rtype}".format(**locals())

    def __repr__(self):
        return "C++ function of signature :  %s"%(self._get_signature())

    def _parsing_format(self) :
      """Generation only: the formatting for the PyParse_xxx calls"""
      def f(t) :
          return basic_types_formatting[t] if t in basic_types_formatting else 'O&'
      l1 = [ f(t) for  t,n,d in self.args if d==None]
      l2 = [ f(t) for  t,n,d in self.args if d!=None]
      if l2 : l2.insert(0,'|') # starts the default arguments, cf python doc
      return ''.join(l1 + l2)

    def _generate_doc(self) :
        doc = "\n".join([ "   " + x.rstrip() for x in self.doc.split('\n')])
        doc = doc.replace('"',"'") # the " are replaced by \"r.
        #doc = doc.replace('"',r'\"') # the " are replaced by \"r. Does not work, makes \\"
        if self._dict_call is not None : return doc
        return "Signature : %s\n%s"%( self._get_signature(),doc)

class pyfunction :
    """
       Representation of one python function of the extension
       It is basically :
         - a python name
         - a list of overload
         - possibly some preprocessing/postprocessing python code.
    """
    def __init__(self, name, arity = None, is_method = False, is_static = False, doc = ''):
      """
        - name : name given in Python
        - arity : arity of the function
        - is_method : boolean
        - is_static : boolean. Is is a static method
        - doc : the doc string.
        - overloads : a list of cfunction objects representing the various C++ overloads of the function
      """
      self.py_name =name       # name given in python
      self.arity = arity
      self.is_method = is_method  # can be a method, a function...
      self.is_static = is_static  #
      self.doc = doc
      self.overloads = [] # List of all C++ overloads
      self.do_implement = True # in some cases, we do not want to implement it automatically, (special methods).
      self.is_constructor = False

    def add_overload(self, **kw) :
        self.overloads.append(cfunction(**kw))
        return self

    def _generate_doc(self) :
        if len(self.overloads) == 1 : #only one overload
            s = "\n".join([f._generate_doc() for f in self.overloads])
        else :
            s = "\n".join([self.doc, "\n"] + [f._generate_doc() for f in self.overloads])
        s=s.replace('@{','').replace('@}','')
        return repr(s)[1:-1] # remove the ' ' made by repr


class converter_:
    """
    Representation of a simple converter for a struct
    """
    def __init__(self, c_type, doc=''):
        """
        Parameters
        ----------

        c_type : string
                C++ type to be converted.
        """
        self.c_type = c_type
        self.doc = doc
        self.members = []

    class member_:
        pass

    def add_member(self, c_name, c_type, initializer = '', doc = ''):
        """
        Add a class member
        
        Parameters
        ----------
        c_name : string
                 name of the variable in C++
        c_type : string
                 type of the C++ variable
        initializer : string
                  Default value
        doc : string
              the doc string.
        """
        m = self.member_()
        m.c_name, m.c_type, m.initializer, m.doc = (c_name, c_type, initializer.strip(), doc) # the strip is crucial for empty string
        self.members.append(m)

    def generate(self):
        """ Generate the C code"""
        # generate the code for the converters
        script_path = os.path.dirname(os.path.abspath( __file__ ))
        tpl = Template(filename=script_path + '/mako/converters.cxx', strict_undefined=True)
        rendered = tpl.render(c=self)
        return rendered

class class_ :
    """
       Representation of a wrapped type
    """
    def __init__(self, py_type, c_type, c_type_absolute = None, hdf5 = False, arithmetic = None, serializable = None, 
            export = True, is_printable = False, doc = '', comparisons ='') :
      """
        Parameters
        ----------

        py_type : string
                Name given in Python
        c_type : string
                C++ type to be wrapped.
        c_type_absolute : string
                full path of c_type, no using, no alias (need for the py_converter hpp file)
        hdf5 : boolean
                generate the hdf5 write/read function from C++ triqs hdf5 protocol and register them in the hdf_archive
        arithmetic : tuple of strings

          Determines the operations to be implemented.
          - The idea is to give an abstract description of the mathematical structure to be implemented :
            an algebra, a group, a vector space, and so on.
            The generator will then implement all necessary functions, by calling their C++ counterparts.
          - Possible values :
               - ("abelian_group") : implements + and -
               - ("vector_space", Scalar) : implements a vector_space, with scalar Scalar
               - ("algebra", Scalar) : implements an algebra, with scalar Scalar
               - ("algebra_with_unit", with_options..., Scalars...) :
                  implements an algebra, with:
                    - scalars Scalar... : the scalars
                    - with_options is (possibly empty) list of options :
                        - with_unit :  +/- of an element with a scalar (injection of the scalar with the unit)
                        - with_unary_minus  : implement unary minus
               - "add_only" : implements only +
               - with_inplace_operators  : option to deduce the +=, -=, ...
                 operators from +,-, .. It deduces the possibles terms to put at the rhs, looking at the
                 case of the +,- operators where the lhs is of the type of self.
                 NB : The operator is mapped to the corresponding C++ operators (for some objects, this may be faster)
                 so it has to be defined in C++ as well....
               - .... more to be defined.
        serializable : boolean
           Whether and how the object is to be serialized.  Possible values are :
           - "tuple" : reduce it to a tuple of smaller objects, using the
              boost serialization compatible template in C++, and the converters of the smaller objects.
           - "repr" : serialize via a string produced by python repr, reconstructed by eval.

        is_printable : boolean
             If true, generate the str, repr from the C++ << stream operator
        comparisons : string
             a chain with all operators separated by space, e.g. "== != < >"
        export : boolean [True]
             if True, the class converter are exported to modules that import this module.
        doc : string
             the doc string.
      """
      self.c_type = c_type
      self.c_type_absolute = c_type_absolute or c_type
      self.implement_regular_type_converter = False # Default. Overrule with add_regular_type_converter
      self.py_type = py_type
      c_to_py_type[self.c_type] = self.py_type # register the name translation for the doc generation
      self.hdf5 = hdf5
      assert serializable in [None, "tuple", "repr"]
      self.serializable = serializable
      self.is_printable = is_printable
      self.comparisons = comparisons
      self.iterator = None
      self.doc = doc
      self.methods = {} #  a dict : string -> pyfunction for each method name
      self.constructor = None # a pyfunction  for the constructors.
      self.members= [] # a list of _member
      self.properties= [] # a list of _property
      self.export = export 

      # If hdf5 is True, wrap the C++.
      # We cannot generate a default implementation with error message as triqs::h5::group might not be available.
      # FIXME Remove triqs dependence
      if hdf5:
          self.add_method("void __write_hdf5__(triqs::h5::group gr, std::string key)", calling_pattern = "h5_write(gr, key, self_c);", doc = "hdf5 writing")
    
      # Init arithmetic
      # expect a tuple : "algebra", "scalar1", "scalar2", etc...
      self.number_protocol = {}
      if arithmetic :
          if not isinstance(arithmetic, tuple) : arithmetic = (arithmetic,)
          # read the with_... option and clean them for the list
          with_unary_minus = 'with_unary_minus' in arithmetic
          with_unit = 'with_unit' in arithmetic
          with_inplace_operators = 'with_inplace_operators' in arithmetic
          arithmetic = [x for x in arithmetic if not x.startswith("with_")]
          add =  arithmetic[0] in ("algebra", "abelian_group", "vector_space", "add_only")
          abelian_group = arithmetic[0] in ("algebra",  "abelian_group", "vector_space")
          vector_space = arithmetic[0] in ("algebra", "vector_space")
          algebra = arithmetic[0] in ("algebra",)

          if add :
            # add
            add = pyfunction(name ="__add__",arity = 2)
            add.add_overload (calling_pattern = "+", signature = {'args' : [(self.c_type,'x'), (self.c_type,'y')], 'rtype' :self.c_type})
            self.number_protocol['add'] = add

          if abelian_group :
            #sub
            sub = pyfunction(name ="__sub__",arity = 2)
            sub.add_overload (calling_pattern = "-", signature = {'args' :[(self.c_type,'x'), (self.c_type,'y')], 'rtype' : self.c_type})
            self.number_protocol['subtract'] = sub

          if vector_space :
            # mul
            mul = pyfunction(name ="__mul__", arity = 2)
            for scalar in arithmetic[1:] :
               mul.add_overload (calling_pattern = "*", signature = {'args' :[(self.c_type,'x'), (scalar,'y')], 'rtype' : self.c_type})
               mul.add_overload (calling_pattern = "*", signature = {'args' :[(scalar,'x'), (self.c_type,'y')], 'rtype' : self.c_type})
            self.number_protocol['multiply'] = mul
            # div
            div = pyfunction(name ="__div__", arity = 2)
            for scalar in arithmetic[1:] :
               div.add_overload (calling_pattern = "/", signature = {'args' :[(self.c_type,'x'), (scalar,'y')], 'rtype' : self.c_type})
            self.number_protocol['divide'] = div

          if algebra :
            mul.add_overload (calling_pattern = "*", signature = {'args' :[(self.c_type,'x'), (self.c_type,'y')], 'rtype' : self.c_type})

          if with_unit: # Allow + and - between scalar and operator
            assert algebra, "The with_unit option only makes sense for algebra"
            for scal in arithmetic[1:] :
                add = self.number_protocol['add']
                add.add_overload (calling_pattern = "+", signature = {'args' :[(self.c_type,'x'), (scal,'y')], 'rtype' : self.c_type})
                add.add_overload (calling_pattern = "+", signature = {'args' :[(scal,'x'), (self.c_type,'y')], 'rtype' : self.c_type})
                sub = self.number_protocol['subtract']
                sub.add_overload (calling_pattern = "-", signature = {'args' :[(self.c_type,'x'), (scal,'y')], 'rtype' : self.c_type})
                sub.add_overload (calling_pattern = "-", signature = {'args' :[(scal,'x'), (self.c_type,'y')], 'rtype' : self.c_type})

          if with_unary_minus :
                # Allow unary - on an operator
                neg = pyfunction(name = "__neg__", arity = 1)
                neg.add_overload (calling_pattern = "-", signature = {'args' :[(self.c_type,'x')], 'rtype' : self.c_type})
                self.number_protocol['negative'] = neg

          if with_inplace_operators : self.deduce_inplace_arithmetic()

    def add_regular_type_converter(self): 
        self.implement_regular_type_converter = True

    def deduce_inplace_arithmetic(self) :
        """Deduce all the +=, -=, *=, /= operators from the +, -, *, / operators"""
        def one_op(op, name, iname) :
            if name not in self.number_protocol : return
            impl = pyfunction(name = iname, arity = 2)
            for overload in self.number_protocol[name].overloads :
                x_t,y_t = overload.args[0][0], overload.args[1][0]
                if x_t == self.c_type : # only when first the object
                  impl.add_overload (calling_pattern = op+"=", signature = {'args' : [(x_t,'x'), (y_t,'y')], 'rtype' :overload.rtype})
            self.number_protocol['inplace_'+name] = impl
        one_op('+',"add","__iadd__")
        one_op('-',"subtract","__isub__")
        one_op('*',"multiply","__imul__")
        one_op('/',"divide","__idiv__")

    def add_constructor(self, signature, calling_pattern = None, intermediate_type = None, doc = ''):
        """
        Parameters
        ----------

        signature : string
            signature of the function, with types, parameter names and defaut value
            rtype( arg1 name1, arg2 name2 = default2, ....)
            signature can be :
               - a string of 2 possible forms (i.e. c_name can be omitted) :
                    - rtype (arg1 name1, arg2 name2 = default2, ....)
                    - rtype c_name ( arg1 name1, arg2 name2 = default2, ....)
               - a dict : rtype -> string , args -> list of tuples [ (c_type, variable_name, default_value)]
               - rtype : the C++ type returned by the function. None for constructor
            default_value is None when there is no default.
        
        calling_pattern : string, expert only
            - Pattern to rewrite the call of the c++ constructor.
            - It is a string, argument name and defining a result of the c_type
              e.g., the default pattern is ::
              auto result = c_type (a,b,c)
        
        intermediate_type : string
          - Name of a C++ type to be used for constructing the object   
            which is then constructed as c_type { intermediate_type {....}}
            E.g. Put a regular_type here when wrapping a view.

        doc : string
            the doc string.
        """
        f = cfunction(signature, calling_pattern = calling_pattern, is_constructor = True, is_method = True,  doc = doc)
        all_args = ",".join(n  for t,n,d in f.args)
        all_args = all_args if f._dict_call is None else "convert_from_python<%s>(keywds)"%f._dict_call # call with the keywds argument
        f._calling_pattern = '' if f._dict_call is None else "if (!convertible_from_python<%s>(keywds,true)) goto error_return;\n"%f._dict_call
        if calling_pattern is not None :
          f._calling_pattern, all_args = calling_pattern + ';\n', "std::move(result)"
        if intermediate_type: 
          f._calling_pattern += "((%s *)self)->_c = new %s(%s (%s));"%(self.py_type, self.c_type, intermediate_type, all_args)
        else :
          f._calling_pattern += "((%s *)self)->_c = new %s (%s);"%(self.py_type, self.c_type,all_args)

        if not self.constructor :
            self.constructor =  pyfunction(name = "__init__", is_method = True, doc = doc)
            self.constructor.is_constructor = True
        self.constructor.overloads.append(f)

    def add_method(self, signature, name =None, calling_pattern = None, no_self_c = False, is_method = False,  is_static = False,
                   doc = '', release_GIL_and_enable_signal = False, c_name = None):
        """
        Add a C++ overload to a method of name name.

        Parameters
        ----------

        signature : string
            signature of the function, with types, parameter names and defaut value
            rtype( arg1 name1, arg2 name2 = default2, ....)
            signature can be :
              - a string of 2 possible forms (i.e. c_name can be omitted) :
                  - rtype (arg1 name1, arg2 name2 = default2, ....)
                  - rtype c_name ( arg1 name1, arg2 name2 = default2, ....)
              - a dict : rtype -> string , args -> list of tuples [ (c_type, variable_name, default_value)]
              - rtype : the C++ type returned by the function. None for constructor
            default_value is None when there is no default.
        
        name : string
               name given in Python

        c_name : string
               name given in C++
               If None, the C++ name extracted from the signature is used.
        
        calling_pattern : string
          - Pattern to rewrite the call of the c++ function,
          - It is a string, using self_c, argument name and defining result at the end if rtype != void
            e.g., the default pattern is :
            auto result = self_c.method_name(a,b,c).
          - If None, the signature must contain c_name
        no_self_c : boolean. 
                    do not generate self_c reference in C++ code, in
                    some rare calling_pattern. Avoid a compiler warning.
        is_method : boolean
        is_static : boolean
                    Is is a static method
        doc : string
              the doc string.
        
        release_GIL_and_enable_signal : boolean, expert only
         - For long functions in pure C++.
         - If True, the GIL is released in the call of the C++ function and restored after the call.
         - It also saves the signal handler of python and restores it after the call,
           and enables the C++ signal_handler.
         - This allows e.g. to intercept Ctrl-C during the long C++ function.
         - **Requirement** :
            The function wrapped must be pure C++, i.e.  no call whatsoever to the python C API, directly or indirectly.
            otherwise the behaviour is undefined.
       """
        f = cfunction(signature, calling_pattern = calling_pattern, no_self_c = no_self_c, is_constructor = False,
             is_method = True,  is_static = is_static, release_GIL_and_enable_signal = release_GIL_and_enable_signal, doc = doc, c_name = c_name or name)
        name = name or f.c_name
        if name not in self.methods :
            self.methods[name] = pyfunction(name = name, is_method = True, is_static = is_static, doc = doc)
        self.methods[name].overloads.append(f)

    def add_call(self, **kw) :
        """
        Add the __call__ operator.

        It just calls add_method, for the operator(), with name = "__call__"

        Cf add_method documentation.
        """
        if 'c_name' not in kw and 'calling_pattern' not in kw : kw['c_name']= "operator()"
        self.add_method(name = "__call__", **kw)

    class _iterator :
        def __init__(self,c_type, c_cast_type, begin, end) :
          self.c_type, self.c_cast_type, self.begin, self.end = c_type, c_cast_type, begin, end

    def add_iterator(self, c_type = "const_iterator", c_cast_type = None, begin = "std::begin", end = "std::end") :
        """
        Add an iterator, wrapping a C++ iterator.
        
        Parameters
        ----------

        c_type : string
                 type of the C++ variable
        c_cast_type : string
                 If not None, the result of the C++ iterator dereference if converted to the cast_type.
        begin, end : string
                 Functions to find begin and end.
        """
        self.iterator = self._iterator(c_type, c_cast_type, begin, end)

    class _member :
        def __init__(self, c_name, c_type, py_name, read_only, doc):
            """
            Parameters
            ----------

            c_name : string
                     name in C
            c_type : string
                     type in C
            py_name : string
                     name in Python

            """
            self.c_name, self.c_type, self.py_name, self.doc, self.read_only = c_name, c_type, py_name or c_name, doc, read_only

        def _generate_doc(self) :
          doc = "\n".join([ "   " + x.strip() for x in self.doc.split('\n')])
          doc = doc.replace('@{','').replace('@}','')
          return repr(doc)[1:-1] # remove the ' ' made by repr

    def add_member(self, c_name, c_type, py_name = None, read_only = False, doc = ''):
        """
        Add a class member
        
        Parameters
        ----------
        c_name : string
                 name of the variable in C++
        c_type : string
                 type of the C++ variable
        py_name : string
                  name of the variable in python. If None, use c_name.
        read_only : boolean
                    is a read only parameter

        doc : string
              the doc string.
        """
        self.members.append( self._member(c_name, c_type, py_name, read_only, doc))

    class _property :
        def __init__(self, name, getter, setter, doc) :
          self.name, self.getter, self.setter, self.doc = name, getter, setter, doc

        def _generate_doc(self) :
          doc = "\n".join([ "   " + x.strip() for x in self.doc.split('\n')])
          doc = doc.replace('@{','').replace('@}','')
          return repr(doc)[1:-1] # remove the ' ' made by repr

    def add_property(self,  getter, setter = None, name = None, doc = ''):
        """
        Add a property
        
        Parameters
        ----------
         - getter : the cfunction representing the get part
         - setter : the cfunction representing the set part or None if the property if read only
         - name : name in python. If None, try to use the C++ name of the getter.
         - doc : the doc string.
        """
        if not isinstance(getter, str) : getter.is_method = True
        self.properties.append( self._property(name or getter.c_name, getter, setter, doc) )

    def add_len(self, c_name = None, calling_pattern = None, doc = "Length") :
        """
        Add the len operator
        """
        if not c_name and not calling_pattern : c_name = "size"
        self.add_method(name = "__len__impl", calling_pattern = calling_pattern, signature="int %s()"%c_name, doc= doc)
        self.methods['__len__impl'].do_implement = False # do not implement automatically, the signature is special

    def add_getitem(self, signature, calling_pattern = None, doc = "operator[]" ) :
        """
        Add a the __getitem__ operator
        """
        self.add_method(name = "__getitem__impl", calling_pattern = calling_pattern, doc = doc, signature = signature, c_name = "operator[]")

    def add_setitem(self, signature, calling_pattern = None, doc = "operator[]", **d ) :
        """
        Add a the __setitem__ operator
        """
        self.add_method(name = "__setitem__impl", calling_pattern = calling_pattern or "self_c[i] = v", doc = doc, signature = signature, **d)

    def add_method_copy(self, clone_function = "cpp2py::make_clone") :
        """Add a method copy, that make a **deep** copy, using the clone function"""
        self.add_method(name = "copy", calling_pattern = self.c_type + " result = %s(self_c)"%clone_function, 
                        signature = self.c_type +"()", doc = "Make a copy (clone) of self")

    def add_method_cpp_copy(self) :
        """Add a method sh_copy, that make an ordinary copy in C++"""
        self.add_method(name = "sh_copy", calling_pattern = self.c_type + " result = self_c", signature = self.c_type +"()", doc = "Make a copy of self")

    def add_method_copy_from(self) :
        """Add a copy_from, using C++ assignment"""
        # other by pointer, it is necessarly a wrapped type
        self.add_method(name = "copy_from", calling_pattern = " self_c = other", signature = 'void(' + self.c_type +" other)", doc = "Assignment")

    def _prepare_for_generation(self) :
        """Internal :  Called just before the code generation"""
        self.has_mapping_protocol = '__getitem__impl' in self.methods or '__len__impl' in self.methods
        if '__setitem__impl' in self.methods and not  '__getitem__impl' in self.methods : raise RuntimeError, "Cannot generate a class with a setter and no getter"

class module_ :
    """
       Representation of a module
    """
    def __init__(self, full_name,  doc = '', app_name = None) :
        """
        Parameters
        ----------

        full_name : string
                    complete name of the module (after install, e.g.  pytriqs.gf.local.gf)

        doc : string 
              doc string

        """
        self.full_name = full_name if app_name is None or app_name=="triqs" else app_name+"."+full_name
        self.name = full_name.rsplit('.',1)[-1]
        self.doc = doc
        self.classes = {}    # dict : string -> class_. Key is the Python type
        self.converters = {} # dict : string -> converter
        self.functions = {}  # functions : dict : string -> function_. Modules functions. Key is the python name.
        self.include_list = []
        self.enums = []
        self.using =[]
        self.imports =[]
        self._preamble = ''

    def add_class(self, cls):
        """
          Add a class into the module.
          It should not exist in the module already.
        """
        if cls.py_type in self.classes : raise IndexError, "The class %s already exists"%cls.py_type
        self.classes[cls.py_type] = cls

    def add_converter(self, conv):
        """
          Add a converter into the module.
          It should not exist in the module already.
        """
        if conv.c_type in self.converters : raise IndexError, "The class %s already exists"%conv.c_type
        self.converters[conv.c_type] = conv

    def add_function(self, signature, name =None, calling_pattern = None,  doc = '', release_GIL_and_enable_signal = False, c_name = None):
        """
        Add a C++ overload to function of the module

        Parameters
        ----------

        signature : string
           signature of the function, with types, parameter names and defaut value
           rtype( arg1 name1, arg2 name2 = default2, ....)
           signature can be :
              - a string of 2 possible forms (i.e. c_name can be omitted) :
                  - rtype (arg1 name1, arg2 name2 = default2, ....)
                  - rtype c_name ( arg1 name1, arg2 name2 = default2, ....)
              - a dict : rtype -> string , args -> list of tuples [ (c_type, variable_name, default_value)]
              - rtype : the C++ type returned by the function. None for constructor
           default_value is None when there is no default.
        
        name : string 
               name given in Python

        c_name : string
               name given in C++
               If None, the C++ name extracted from the signature is used.

        calling_pattern : string
          - Pattern to rewrite the call of the c++ function,
          - It is a string, using self_c, argument name and defining result at the end if rtype != void
            e.g., the default pattern is :
            auto result = self_c.method_name(a,b,c).
          - If None, the signature must contain c_name
        
        doc : string
              the doc string.
        
        release_GIL_and_enable_signal : boolean, expert only
         - For long functions in pure C++.
         - If True, the GIL is released in the call of the C++ function and restored after the call.
         - It also saves the signal handler of python and restores it after the call,
           and enables the C++ signal_handler.
         - This allows e.g. to intercept Ctrl-C during the long C++ function.
         - **Requirement** :
            The function wrapped MUST be pure C++, i.e.  no call whatsoever to the python C API, directly or indirectly.
            otherwise the behaviour is undefined.
       """
        f = cfunction(signature, calling_pattern = calling_pattern, release_GIL_and_enable_signal = release_GIL_and_enable_signal, doc = doc,c_name = c_name or name)
        name = name or f.c_name
        if name not in self.functions :
            self.functions[name] = pyfunction(name = name, doc = doc)
        self.functions[name].overloads.append(f)

    def add_include(self, *filenames) :
        """
        Add the filenames as C++ include in the generated wrapper and header.
        """
        self.include_list.extend(filenames)

    def add_using(self,ns) :
        """
        Add the using statement into the generated wrapper (and NOT the header).
        """
        self.using.append(ns)

    def add_imports(self, *lst):
        """
          Add a dependent import to the module.
        """
        self.imports += lst

    def add_preamble(self, preamble) :
        """
        Add the using statement into the generated wrapper (and NOT the header).
        """
        self._preamble += preamble + '\n'

    class _enum :
        def __init__(self, c_name, values, c_namespace, doc) :
            self.c_name, self.c_namespace, self.values, self.doc = c_name, c_namespace + "::", values, doc
            self.c_name_absolute = self.c_namespace + self.c_name

    def add_enum(self, c_name, values, c_namespace ="", doc = '') :
        """
          Add an enum into the module.
    
          Parameters
          ----------

          c_name : string
                  name in C++
          c_namespace: string
                     namespace of the enum
          values : list of string 
                  represents the C++ enum values
          doc : string
                the doc string.
        """
        self.enums.append( self._enum(c_name, values, c_namespace, doc))

    def _all_args_kw_functions(self) :
        l = [ (f, self.name, None) for f in self.functions.values()]
        for c in self.classes.values() :
            l += [(m,c.py_type, c.c_type) for m in c.methods.values() if m.do_implement]
            if c.constructor :
                l.append( (c.constructor,c.py_type, c.c_type))
        # Check before generation
        for f, name, x in l:
            n_dict_call = sum( 1 if overload._dict_call else 0 for overload in f.overloads)
            assert n_dict_call <= 1, "At most one possible overload with ** call"
            assert n_dict_call ==0  or len(f.overloads) == 1, "The function %s.%s has a ** call and overloads, which is meaningless !"%(name,f.py_name)
        return l

    def generate_code(self) :
        """
         Generate the wrapper and the header.
         The filenames are given in the sys.argv
         if self.app_name is set, generate a copy of the py_converter
         file with includes consistent with the installation (as opposed to the build):
         e.g. #include "a.hpp" in py_converter.hpp becomes
              #include <app_name/a.hpp> in py_converter.hpp.app_name.install
        """
        script_path = os.path.dirname(os.path.abspath( __file__ ))
        mako_template = script_path + '/mako/wrap.cxx'
        wrap_file = sys.argv[1]
 
        # prepare generation
        for c in self.classes.values() : c._prepare_for_generation()
           
        # call mako
        tpl = Template(filename=mako_template, strict_undefined=True)
        rendered = tpl.render(module=self, 
                   sys_modules = sys.modules)
       
        with open(wrap_file,'w') as f:
           f.write(rendered)



