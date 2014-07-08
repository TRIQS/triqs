//--------------------- includes and using  -------------------------------------
#define TRIQS_PYTHON_WRAPPER_MODULE_${module.name}

%for file in module.include_list :
%if file.startswith('<'):
#include ${file}
%else:
#include "${file}"
%endif
%endfor

using dcomplex = std::complex<double>;

%for ns in module.using:
using ${ns};
%endfor

#include <triqs/python_tools/wrapper_tools.hpp>
#include <triqs/utility/signal_handler.hpp>
using namespace triqs::py_tools;

${module._preamble}

//--------------------- a dict of python function used in the module but not exposed to user (cf init function) ----------------

%if len(module.python_functions) + len(module.hidden_python_functions) > 0 :
static PyObject * _module_hidden_python_function = NULL;
%endif

// We use the order, in the following order (which is necessary for compilation : we need the converters in the implementations)
// - function/method declaration
// - implement type, and all tables
// - implement the converter of types, then of associated regular_type if any.
// - implement functions/method.
//
//----------------------------------------------------------------
//---------------------  function declarations -------------------
//----------------------------------------------------------------

//--------------------- all functions/methods with args, kwds, including constructors -----------------------------

%for py_meth, module_or_class_name, self_c_type in module._all_args_kw_functions() :
 static ${'PyObject*' if not py_meth.is_constructor else 'int'} ${module_or_class_name}_${py_meth.py_name}(PyObject *self, PyObject *args, PyObject *keywds);
%endfor

// ------------------------------- Loop on all classes ----------------------------------------------------

%for c in module.classes.values() :

// ----------------------------
// start class : ${c.py_type}
// ----------------------------
//--------------------- all pure python methods  -----------------------------

%for f_name, (f,ty,doc) in c.pure_python_methods.items():
static PyObject* ${c.py_type}_${f_name} (PyObject *self, PyObject *args, PyObject *keywds);
%endfor

//--------------------- all members  -----------------------------

%for m in c.members:
static PyObject * ${c.py_type}__get_member_${m.py_name} (PyObject *self, void *closure);
%if not m.read_only:
static int ${c.py_type}__set_member_${m.py_name} (PyObject *self, PyObject *value, void *closure);
%endif
%endfor

//--------------------- all properties  -----------------------------

%for p in c.properties :
static PyObject * ${c.py_type}__get_prop_${p.name} (PyObject *self, void *closure);
%if p.setter :
static int ${c.py_type}__set_prop_${p.name} (PyObject *self, PyObject *value, void *closure);
%endif
%endfor

//--------------------- [] -----------------------------

%if "__len__impl" in c.methods :
static Py_ssize_t ${c.py_type}___len__(PyObject *self);
%endif

%if "__getitem__impl" in c.methods :
static PyObject* ${c.py_type}___getitem__(PyObject *self, PyObject *key);
%endif

%if "__setitem__impl" in c.methods :
static int ${c.py_type}___setitem__(PyObject *self, PyObject *key, PyObject *v);
%endif


//--------------------- reduce  -----------------------------

%if c.serializable == "via_string" :
 static PyObject* ${c.py_type}___reduce__ (PyObject *self, PyObject *args, PyObject *keywds);
%endif

//--------------------- reduce version 2 -----------------------------

%if c.serializable == "tuple" :
 static PyObject* ${c.py_type}___reduce__ (PyObject *self, PyObject *args, PyObject *keywds);
 static PyObject* ${c.py_type}___reduce_reconstructor__ (PyObject *self, PyObject *args, PyObject *keywds);
%endif

//--------------------- hdf5 : write part -----------------------------

static PyObject* ${c.py_type}___write_hdf5__ (PyObject *self, PyObject *args);

//--------------------- repr  -----------------------------

%if c.is_printable :
static PyObject* ${c.py_type}___repr__ (PyObject *self);
static PyObject* ${c.py_type}___str__ (PyObject *self);
%endif

//--------------------- Arithmetic declaration only -----------------------------

% for op_name, op in c.number_protocol.items():
%if op.arity == 2:
static PyObject * ${c.py_type}_${op_name} (PyObject* v, PyObject *w);
%elif op.arity ==1:
static PyObject * ${c.py_type}_${op_name} (PyObject *v);
%endif
%endfor

// ----------------------------
// stop class : ${c.py_type}
// ----------------------------
%endfor // cc classes

//----------------------------------------------------------------
//---------------------  implementation --------------------------
//----------------------------------------------------------------

// ------------------------------- Loop on all classes ----------------------------------------------------

%for c in module.classes.values() :
// ----------------------------
// start class : ${c.py_type}
// ----------------------------

//--------------------- define all the types and the converter functions -----------------------------


 // the python type
typedef struct {
    PyObject_HEAD
    ${c.c_type} * _c;
} ${c.py_type};

// the new
static PyObject* ${c.py_type}_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  ${c.py_type} *self;
  self = (${c.py_type} *)type->tp_alloc(type, 0);
  if (self != NULL) {
   try {
   self->_c = NULL;
   ##//%if not c.c_type_is_view :
   ##// self->_c = new ${c.c_type}{};
   ##//%else :
   ##// self->_c = new ${c.c_type}{typename ${c.c_type}::regular_type{}}; // no default constructor for views
   ##//%endif
   }
   catch (std::exception const & e) {
    std::cout  << e.what()<<std::endl;
    PyErr_SetString(PyExc_RuntimeError, "Default constructor of class ${c.py_type} is throwing an exception !");
    return NULL;
   }
  }
  return (PyObject *)self;
}

// dealloc
static void ${c.py_type}_dealloc(${c.py_type}* self) {
  if (self->_c != NULL) delete self->_c; // should never be null, but I protect it anyway
  self->ob_type->tp_free((PyObject*)self);
}

//--------------------- Iterator by wrapping the C++ -----------------------------

%if c.iterator :
 // I used the doc and http://stackoverflow.com/questions/1815812/how-to-create-a-generator-iterator-with-the-python-c-api
 // to write this
 // the iterator type
 typedef struct {
  PyObject_HEAD
  PyObject * container;
  ${c.c_type}::${c.iterator.c_type} iter, end;
 } ${c.py_type}__iterator;

 // dealloc
 static void  ${c.py_type}__iterator_dealloc(${c.py_type}__iterator * self) {
   Py_XDECREF(self->container);
   self->ob_type->tp_free((PyObject*)self);
 }

 // the __iter__ of the iterator type : returns itself
 PyObject* ${c.py_type}__iterator__iter__(PyObject *self) {
  Py_INCREF(self);
  return self;
 }

 // the next() method of the iterator
 PyObject* ${c.py_type}__iterator__iternext__(PyObject *self);

 static PyTypeObject ${c.py_type}__iteratorType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "${module.name}.${c.py_type}__iterator",            /*tp_name*/
    sizeof(${c.py_type}__iterator),       /*tp_basicsize*/
    0,     /*tp_itemsize*/
    (destructor)${c.py_type}__iterator_dealloc,     /*tp_dealloc*/
    0,     /*tp_print*/
    0,     /*tp_getattr*/
    0,     /*tp_setattr*/
    0,     /*tp_compare*/
    0,     /*tp_repr*/
    0,     /*tp_as_number*/
    0,     /*tp_as_sequence*/
    0,     /*tp_as_mapping*/
    0,     /*tp_hash */
    0,     /*tp_call*/
    0,     /*tp_str*/
    0,     /*tp_getattro*/
    0,     /*tp_setattro*/
    0,     /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER, /* tp_flags: Py_TPFLAGS_HAVE_ITER tells python to use tp_iter and tp_iternext fields. */
    "Internal ${c.py_type} iterator object.",           /* tp_doc */
    0,  /* tp_traverse */
    0,  /* tp_clear */
    0,  /* tp_richcompare */
    0,  /* tp_weaklistoffset */
    ${c.py_type}__iterator__iter__,  /* tp_iter: __iter__() method */
    ${c.py_type}__iterator__iternext__  /* tp_iternext: next() method */
};

// the __iter__ of the main type: return a new iterator. Need to allocate and init it by hand (no init method).
PyObject* ${c.py_type}___iter__(PyObject *self);

%endif

//--------------------- Register as_number  -----------------------------

%if c.number_protocol :
static PyNumberMethods ${c.py_type}_as_number = {

%for op_name in ["add", "subtract", "multiply", "divide", "remainder", "divmod", "power", "negative", "positive", "absolute", "nonzero", "invert", "lshift", "rshift", "and", "xor", "or", "coerce", "int", "long", "float", "oct", "hex", "inplace_add", "inplace_subtract", "inplace_multiply", "inplace_divide", "inplace_remainder", "inplace_power", "inplace_lshift", "inplace_rshift", "inplace_and", "inplace_xor", "inplace_or", "floor_divide ", "true_divide ", "inplace_floor_divide ", "inplace_true_divide ", "index "] :
% if op_name in c.number_protocol and c.number_protocol[op_name].arity==2 :
 (binaryfunc)${c.py_type}_${op_name}, /*nb_${op_name}*/
%elif op_name in c.number_protocol and c.number_protocol[op_name].arity==1 :
 (unaryfunc)${c.py_type}_${op_name}, /*nb_${op_name}*/
%else :
 0, /*nb_${op_name}*/
%endif
%endfor
};
%endif

//--------------------- Register as_mapping  -----------------------------

%if c.has_mapping_protocol :
static PyMappingMethods ${c.py_type}_mapping {
%if "__len__impl" in c.methods :
 (lenfunc)${c.py_type}___len__, /* mp_length */
%else:
NULL, /* mp_length */
%endif
%if "__getitem__impl" in c.methods :
 (binaryfunc)${c.py_type}___getitem__,     /* mp_subscript*/
%else:
NULL,     /* mp_subscript*/
%endif
%if "__setitem__impl" in c.methods :
 (objobjargproc)${c.py_type}___setitem__     /* mp_ass_subscript*/
%else:
NULL     /* mp_ass_subscript*/
%endif
};
%endif

//--------------------- Register members and properties -----------------------------

static PyGetSetDef ${c.py_type}_getseters[] = {

%for mb in [m for m in c.members if not m.read_only] :
{"${mb.py_name}", (getter)${c.py_type}__get_member_${mb.py_name}, ${c.py_type}__set_member_${mb.py_name}, "${mb.doc}", NULL},
%endfor

%for mb in [m for m in c.members if m.read_only] :
{"${mb.py_name}", (getter)${c.py_type}__get_member_${mb.py_name}, NULL, "${mb.doc}", NULL},
%endfor

%for p in [p for p in c.properties if p.setter ] :
{"${p.name}", (getter)${c.py_type}__get_prop_${p.name}, ${c.py_type}__set_prop_${p.name}, "${p.doc}", NULL},
%endfor

%for p in [p for p in c.properties if not p.setter ] :
{"${p.name}", (getter)${c.py_type}__get_prop_${p.name}, NULL, "${p.doc}", NULL},
%endfor

{NULL}  /* Sentinel */
};

//--------------------- Register the methods for the types  -----------------------------

static PyMethodDef ${c.py_type}_methods[] = {
   %for meth_name, meth in c.methods.items():
    %if not meth_name.startswith('__') :
    {"${meth_name}", (PyCFunction)${c.py_type}_${meth_name}, METH_VARARGS| METH_KEYWORDS ${"|METH_STATIC" if meth.is_static else ""}, "${c.methods[meth_name]._generate_doc()}" },
    %endif
   %endfor
%if c.serializable :
    {"__reduce__", (PyCFunction)${c.py_type}___reduce__, METH_VARARGS, "Internal  " },
%endif
##%if c.serializable == "tuple" :
##    {"__reduce_reconstructor__", (PyCFunction)${c.py_type}___reduce_reconstructor__, METH_VARARGS|METH_STATIC, "Internal " },
##%endif
##%if c.hdf5:
    {"__write_hdf5__", (PyCFunction)${c.py_type}___write_hdf5__, METH_VARARGS, "Internal : hdf5 writing via C++ " },
##%endif
 %for meth_name, (meth,ty,doc) in c.pure_python_methods.items():
    {"${meth_name}", (PyCFunction)${c.py_type}_${meth_name}, METH_VARARGS| METH_KEYWORDS, "${doc}" },
 %endfor

{NULL}  /* Sentinel */
};

//--------------------- The xxxType table  -----------------------------

static PyTypeObject ${c.py_type}Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "${module.full_name}.${c.py_type}",             /*tp_name*/
    sizeof(${c.py_type}),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)${c.py_type}_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    ${"&%s___repr__"%c.py_type if c.is_printable else 0},           /*tp_repr*/
    ${"&%s_as_number"%c.py_type if c.number_protocol else 0},       /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    ${"&%s_mapping"%c.py_type if c.has_mapping_protocol else 0},    /*tp_as_mapping*/
    0,                         /*tp_hash */
    ${c.py_type+"___call__" if "__call__" in c.methods else 0},                         /*tp_call*/
    ${"&%s___str__"%c.py_type if c.is_printable else 0},                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE |Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
    "${c.doc}",                /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    ${c.py_type+"___iter__" if c.iterator else 0},		               /* tp_iter */
    0,		               /* tp_iternext */
    ${c.py_type}_methods,             /* tp_methods */
    0,              /* tp_members */
    ${c.py_type}_getseters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    ${"(initproc)%s___init__"%c.py_type if c.constructor else 0},      /* tp_init */
    0,                         /* tp_alloc */
    ${c.py_type}_new,                 /* tp_new */
};

//--------------------- converters for the class c -----------------------------

namespace triqs { namespace py_tools {

template <> struct py_converter<${c.c_type}> {

 template<typename U> static PyObject * c2py(U&& x){
  ${c.py_type} *self;
  self = (${c.py_type} *)${c.py_type}Type.tp_alloc(&${c.py_type}Type, 0);
  if (self != NULL) {
   self->_c = new ${c.c_type}{std::forward<U>(x)};
  }
  return (PyObject *)self;
 }

 static ${c.c_type} & py2c(PyObject * ob){
  auto *_c = ((${c.py_type} *)ob)->_c;
  if (_c == NULL) TRIQS_RUNTIME_ERROR << "Severe internal error : _c is null in py2c for type ${c.c_type} !";
  return *_c;
 }

 static bool is_convertible(PyObject *ob, bool raise_exception){
  if (PyObject_TypeCheck(ob, & ${c.py_type}Type)) {
   if (((${c.py_type} *)ob)->_c != NULL) return true;
   if (raise_exception) PyErr_SetString(PyExc_TypeError, "Severe internal error : Python object of ${c.py_type} has a _c NULL pointer !!");
   return false;
  }
  if (raise_exception) PyErr_SetString(PyExc_TypeError, "Python object is not a ${c.py_type}");
  return false;
 }
};

// TO BE MOVED IN GENERAL HPP
%if c.implement_regular_type_converter :
 // ${c.py_type} is wrapping a view, we are also implementing the converter of the associated regular type
 template<> struct py_converter<${c.regular_type}> {
 using regular_type = ${c.regular_type};
 using conv = py_converter<${c.c_type}>;
 static PyObject *c2py(regular_type &g) { return conv::c2py(g); }
 static PyObject *c2py(regular_type &&g) { return conv::c2py(g); }
 static bool is_convertible(PyObject * ob, bool raise_exception) { return conv::is_convertible(ob, raise_exception); }
 static regular_type py2c(PyObject *ob) { return conv::py2c(ob); }
};
%endif

}} // namespace py_tools

// ----------------------------
// stop class : ${c.py_type}
// ----------------------------

%endfor ## loop on classes

//--------------------- Converters of enums --------------------------

%for en in module.enums :

namespace triqs { namespace py_tools {

template <> struct py_converter<${en.c_name}> {
 static PyObject * c2py(${en.c_name} x) {
   %for n,val in enumerate(en.values[:-1]) :
    if (x == ${val}) return PyString_FromString("${val}");
   %endfor
   return PyString_FromString("${en.values[-1]}"); // last case separate to avoid no return warning of compiler
 }
 static ${en.c_name} py2c(PyObject * ob){
   std::string s=PyString_AsString(ob);
   %for n,val in enumerate(en.values[:-1]) :
    if (s == "${val}") return ${val};
   %endfor
   return ${en.values[-1]};
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
   if (!PyString_Check(ob))  {
     if (raise_exception) PyErr_SetString(PyExc_ValueError, "Convertion of C++ enum ${en.c_name} : the object is not a string");
     return false;
   }
   std::string s=PyString_AsString(ob);
   %for n,val in enumerate(en.values) :
    if (s == "${val}") return true;
   %endfor
   if (raise_exception) {
    auto err = "Convertion of C++ enum ${en.c_name} : \nThe string \"" + s +"\" is not in [${','.join([str(x) for x in en.values])}]";
    PyErr_SetString(PyExc_ValueError, err.c_str());
   }
   return false;
 }
};

}}
%endfor

//--------------------- Parser auxiliary functions for PyArg_ParseTupleAndKeywords  --------------------------

// adapter needed for parsing with PyArg_ParseTupleAndKeywords later in the functions
// We need several slighly different version for
//  - wrapped types : we handle them by pointer
//  - non wrapped_type : by value
//  - view : needs to be rebinded, not assigned.
// Selection of the converter for each type is done later,  Cf _get_proper_converter.
// take the object a pointer to the place into which convert the object.
// return 1 (success), 0 (failure). If fails, also set the python exception which will be analyzed later.
template<typename T>
 static int converter_for_parser_non_wrapped_type(PyObject * ob, T * p) {
  if (!convertible_from_python<T>(ob,true)) return 0;
  *p = convert_from_python<T>(ob);
  return 1;
 }
template<typename T>
 static int converter_for_parser_wrapped_type(PyObject * ob, T ** p) {
  if (!convertible_from_python<T>(ob,true)) return 0;
  *p = &(convert_from_python<T>(ob)); // wrapped types are manipulated by pointers.
  return 1;
 }
template<typename T>
 static int converter_for_parser_view_type(PyObject * ob, T * p) {
  if (!convertible_from_python<T>(ob,true)) return 0;
  p->rebind(convert_from_python<T>(ob));
  return 1;
 }

//--------------------- define all functions/methods with args, kwds, including constructors -----------------------------

%for py_meth, module_or_class_name, self_c_type in module._all_args_kw_functions() :

 static ${'PyObject*' if not py_meth.is_constructor else 'int'} ${module_or_class_name}_${py_meth.py_name}(PyObject *self, PyObject *args, PyObject *keywds) {

  <%
     n_overload = len(py_meth.overloads)
     has_overloads = (n_overload>1)
  %>
  %if not py_meth.is_constructor :
  PyObject * py_result; //final result
  %endif
  %if has_overloads :
  PyObject * errors[${n_overload}] = {${",".join(n_overload*['NULL'])}}; //errors of the parsing attempts...
  %endif

  %if py_meth.python_precall :
    pyref res_py; // the intermediate result of the pretreatment

    %if isinstance(py_meth.python_precall,python_function) :

     /// A precall function where the code is given
     static const char * _precall_code = ${py_meth.python_precall.code};
     static pyref _precall_py_fnt, _precall_py_dict; // keep the function, contained in a specific local dict
     if (_precall_py_fnt.is_null()) { // first init
       PyObject* main_module = PyImport_AddModule("__main__"); //borrowed
       PyObject* global_dict = PyModule_GetDict(main_module); //borrowed
       _precall_py_dict = PyDict_New(); // new ref
       if (!PyRun_String(_precall_code, Py_file_input, global_dict, _precall_py_dict)) goto error_return;
       _precall_py_fnt = borrowed(PyDict_GetItemString(_precall_py_dict,"${py_meth.python_precall.name}"));
     }

    %else:

      // get the module where the function is
      static pyref module = pyref::module("${py_meth.python_precall.rsplit('.',1)[0]}");
      if (module.is_null()) goto error_return;
      // get the function (only once)
      static pyref _precall_py_fnt = module.attr("${py_meth.python_precall.rsplit('.',1)[1]}");
      // call the function : return must be a tuple args, kw to replace the current args, kw

    %endif

    res_py = PyObject_Call(_precall_py_fnt, args, keywds);
    if (PyErr_Occurred()) goto error_return;
    if (!PyTuple_Check(res_py) || (PyTuple_Size(res_py)!=2))
        { PyErr_SetString(PyExc_RuntimeError,"${py_meth.python_precall} must return a tuple (args,kw) "); goto error_return; }
    args = PyTuple_GetItem(res_py,0);
    keywds = PyTuple_GetItem(res_py,1); // Borrowed ref, Cf res_py comment above.
    if (!PyTuple_Check(args)) { PyErr_SetString(PyExc_RuntimeError,"${py_meth.python_precall}: return error : first element must be a tuple of arguments"); goto error_return; }
    if (!PyDict_Check(keywds)) { PyErr_SetString(PyExc_RuntimeError,"${py_meth.python_precall}: return error : second element must be a dict"); goto error_return; }
  %endif

  // If no overload, we avoid the err_list and let the error go through (to save some code).
  %for n_overload, overload in enumerate(py_meth.overloads) :
    {// overload ${overload._get_c_signature()}
     // define the variable to be filled by the parsing method
     // wrapped types are converted to a pointer, other converted types to a value or a view
     %for t,n,d in overload.args :
       %if t in module._wrapped_types :
       ${t}* ${n} = NULL; // ${t} is a wrapped type
       %elif is_type_a_view(t):
       ${t} ${n}  = typename ${t}::regular_type{}; // ${t} is a view, but not wrapped
       %else:
       ${t} ${n} ${'=%s'%d if d else ''}; //  ${t} is a regular type
       %endif
     %endfor
     static char *kwlist[] = {${",".join([ '"%s"'%n for t,n,d in overload.args] + ["NULL"])}};
     static const char * format = "${overload._parsing_format()}";
     if (PyArg_ParseTupleAndKeywords(args, keywds, format, kwlist ${"".join([ module._get_proper_converter(t) + ' ,&%s'%n for t,n,d in overload.args])})) {
      %if overload.is_method and not overload.is_constructor and not overload.no_self_c and not overload.is_static :
      auto & self_c = convert_from_python<${self_c_type}>(self);
      %endif
      %if overload.is_static :
       using self_class = ${self_c_type};
      %endif
      try {
        %if overload.release_GIL_and_enable_signal :
         PyOS_sighandler_t sig = PyOS_getsig(SIGINT);
         triqs::signal_handler::start(); // start the C++ signal handler
         Py_BEGIN_ALLOW_THREADS;
	 try {
        %endif
        ${overload._get_calling_pattern()}; // the call is here. It sets up "result" : sets up in the python layer.
        %if overload.release_GIL_and_enable_signal :
	 }
	 catch(...) {
	   // an error has occurred : clean GIL, handler and rethrow
           Py_BLOCK_THREADS; // cf python include, ceval.h, line 72 comments and below.
           triqs::signal_handler::stop(); // stop the C++ signal handler
           PyOS_setsig(SIGINT, sig);
           throw; //
	 }
         Py_END_ALLOW_THREADS;
         triqs::signal_handler::stop();
         PyOS_setsig(SIGINT, sig);
        %endif
        %if not overload.is_constructor :
 	 %if overload.rtype != "void" :
         py_result = convert_to_python(std::move(result));
         %else:
         Py_INCREF(Py_None);
         py_result = Py_None;
         %endif
        %endif
        goto post_treatment; // eject, computation is done
      }
      %if not py_meth.is_constructor:
      CATCH_AND_RETURN("calling C++ overload \n ${overload._get_c_signature()} \nin implementation of ${'method' if py_meth.is_method else 'function'} ${module_or_class_name}.${py_meth.py_name}", NULL);
      %else:
      CATCH_AND_RETURN ("in calling C++ overload of constructor :\n${overload._get_c_signature()}",-1);
      %endif
     }
  %if has_overloads :
     else { // the overload does not parse the arguments. Keep the error set by python, for later use, and clear it.
      PyObject * ptype,  *ptraceback; // unused.
      PyErr_Fetch(&ptype, &errors[${n_overload}], &ptraceback);
      Py_XDECREF(ptype); Py_XDECREF(ptraceback);
     }
  %endif
    } // end overload ${overload._get_c_signature()}
  %endfor # overload

  %if has_overloads :
   // finally, no overload was successful. Composing a detailed error message, with the reason of failure of all overload !
   {
    std::string err_list = "Error: no suitable C++ overload found in implementation of ${'method' if py_meth.is_method else 'function'} ${module_or_class_name}.${py_meth.py_name}\n";
    %for n_overload, overload in enumerate(py_meth.overloads) :
      err_list += "\n ${overload._get_c_signature()} \n failed with the error : \n  ";
      if (errors[${n_overload}])err_list += PyString_AsString(errors[${n_overload}]);
      err_list +="\n";
      Py_XDECREF(errors[${n_overload}]);
    %endfor
    PyErr_SetString(PyExc_TypeError,err_list.c_str());
   }
  %endif
   goto error_return;

  post_treatment:
   %if py_meth.python_postcall :
    if (py_result) { // should always be true

    %if isinstance(py_meth.python_postcall,python_function) :

     /// A postcall function where the code is given
     static const char * _postcall_code = ${py_meth.python_postcall.code};
     static pyref _postcall_py_fnt, _postcall_py_dict; // keep the function, contained in a specific local dict
     if (_postcall_py_fnt.is_null()) { // first init
       PyObject* main_module = PyImport_AddModule("__main__"); //borrowed
       PyObject* global_dict = PyModule_GetDict(main_module); //borrowed
       _postcall_py_dict = PyDict_New(); // new ref
       if (!PyRun_String(_postcall_code, Py_file_input, global_dict, _postcall_py_dict)) goto error_return;
       _postcall_py_fnt = borrowed(PyDict_GetItemString(_postcall_py_dict,"${py_meth.python_postcall.name}"));
     }

    %else:

     static pyref module2 = pyref::module("${py_meth.python_postcall.rsplit('.',1)[0]}");
     static pyref _postcall_py_fnt = module2.attr("${py_meth.python_postcall.rsplit('.',1)[1]}");

    %endif

     PyObject * res_final =  PyObject_CallFunctionObjArgs(_postcall_py_fnt, py_result, NULL);
     Py_XDECREF(py_result);
     py_result = res_final; // stealing the ref
   }
   %endif

   return ${'py_result' if not py_meth.is_constructor else '0'};

  error_return :
   return ${'NULL' if not py_meth.is_constructor else '-1'};
 }
%endfor

// ------------------------------- Loop on all classes ----------------------------------------------------

%for c in module.classes.values() :

//--------------------- define all pure python methods  -----------------------------

// The methods called from an external module
%for f_name, (f,ty,doc) in c.pure_python_methods.items():
 %if ty=='module':
static PyObject* ${c.py_type}_${f_name} (PyObject *self, PyObject *args, PyObject *keywds) {
  static pyref module = pyref::module("${f.module}");
  if (module.is_null()) {
     PyErr_SetString(PyExc_ImportError,"Cannot import module ${f.module}");
     return NULL;
  }
  static pyref py_fnt = module.attr("${f.py_name}");
  if (py_fnt.is_null()) {
     PyErr_SetString(PyExc_ImportError,"Cannot import function ${f.py_name} in module ${f.module}");
     return NULL;
  }
  pyref args2 = PySequence_Concat(PyTuple_Pack(1,self),args);
  PyObject * ret = PyObject_Call(py_fnt, args2,keywds);
  return ret;
}
 %else :
  // The methods with inline code in the module
static PyObject* ${c.py_type}_${f_name} (PyObject *self, PyObject *args, PyObject *keywds) {
  pyref args2 = PySequence_Concat(PyTuple_Pack(1,self),args);
  PyObject * ret = PyObject_Call(PyDict_GetItemString(PyModule_GetDict(_module_hidden_python_function),"${f}"), args2, keywds);
  return ret;
}
%endif
%endfor

//--------------------- define all members  -----------------------------

%for m in c.members:

static PyObject * ${c.py_type}__get_member_${m.py_name} (PyObject *self, void *closure) {
  auto & self_c = convert_from_python<${c.c_type}>(self);
  return convert_to_python(self_c.${m.c_name});
}

%if not m.read_only:
static int ${c.py_type}__set_member_${m.py_name} (PyObject *self, PyObject *value, void *closure) {
  if (value == NULL) { PyErr_SetString(PyExc_TypeError, "Cannot delete the attribute ${m.py_name}"); return -1; }
  if (!convertible_from_python<${m.c_type}>(value, true)) return -1;// exception is set by the converter
  auto & self_c = convert_from_python<${c.c_type}>(self);
  try {
   self_c.${m.c_name} = convert_from_python<${m.c_type}>(value);
  }
  CATCH_AND_RETURN("in setting the attribute '${m.py_name}'",-1);
  return 0;
}
%endif
%endfor ## c.members

//--------------------- define all properties  -----------------------------

%for p in c.properties :

static PyObject * ${c.py_type}__get_prop_${p.name} (PyObject *self, void *closure) {
  %if isinstance(p.getter, str):
    // pure python call
    static pyref py_fnt = pyref::module("${p.getter.rsplit('.',1)[0]}").attr("${p.getter.rsplit('.',1)[1]}");
    return py_fnt(self).new_ref();
  %else:
    auto & self_c = convert_from_python<${c.c_type}>(self);
    ${p.getter._get_calling_pattern()}; // defines result, which cannot be void (property would return None ??)
    return convert_to_python(result);
  %endif
}

%if p.setter :
static int ${c.py_type}__set_prop_${p.name} (PyObject *self, PyObject *value, void *closure) {
  if (value == NULL) { PyErr_SetString(PyExc_AttributeError, "Cannot delete the attribute ${p.name}"); return -1; }
  if (!convertible_from_python<${p.setter.args[0][0]}>(value, true)) return -1;
  auto & self_c = convert_from_python<${c.c_type}>(self);
  try {
   self_c.${p.setter.c_name} (convert_from_python<${p.setter.args[0][0]}>(value));
  }
  CATCH_AND_RETURN("in setting the property '${p.name}'",-1);
  return 0;
}
%endif
%endfor ## c.properties


//--------------------- [] implementation  -----------------------------

%if "__len__impl" in c.methods :
static Py_ssize_t ${c.py_type}___len__(PyObject *self) {
 auto & self_c = convert_from_python<${c.c_type}>(self);
 try {
  ${c.methods['__len__impl'].overloads[0]._get_calling_pattern()};
  return result;
 }
 CATCH_AND_RETURN("in calling C++ function for __len__  :\n${overload._get_c_signature()}", -1);
}
%endif

// The __getitem__impl has been implemented with other methods. We redirect the call to the general for (args, kw)
// after rebuilding args.
%if "__getitem__impl" in c.methods :
static PyObject* ${c.py_type}___getitem__(PyObject *self, PyObject *key) {
 pyref args =  (PyTuple_Check(key) ? borrowed(key) : pyref(PyTuple_Pack(1,key)));
 return ${c.py_type}___getitem__impl(self,args,NULL);
}
%endif

%if "__setitem__impl" in c.methods :
static int ${c.py_type}___setitem__(PyObject *self, PyObject *key, PyObject *v) {
 pyref args =  (PyTuple_Check(key) ? borrowed(key) : pyref(PyTuple_Pack(1,key)));
 pyref args2 = PySequence_Concat(args,   PyTuple_Pack(1,v));
 pyref res =  ${c.py_type}___setitem__impl(self,args2,NULL);
 return (res.is_null() ? -1 : 0); // the function will return None, we want to change to an int
}
%endif

//--------------------- reduce  -----------------------------

%if c.serializable == "via_string" :
 // we make the unserialize function using the convertion of a C++ lambda !
 static PyObject* ${c.py_type}___reduce__ (PyObject *self, PyObject *args, PyObject *keywds) {
  static PyObject* pyfoo = NULL; // never need to decref it, it is static
  if (!pyfoo) {
   auto lambda = [](std::string const &s) -> PyObject *{
    try {
     %if not c.c_type_is_view :
      return convert_to_python( triqs::deserialize<${c.c_type}>(s));
     %else:
      return convert_to_python( ${c.c_type} ( triqs::deserialize<typename ${c.c_type}::regular_type>(s)));
     %endif
    }
    CATCH_AND_RETURN("in boost unserialization of object ${c.py_type}",NULL);
   };
   pyfoo = convert_to_python(std::function<PyObject *(std::string)>{lambda}); // new ref
  }
  auto & self_c = convert_from_python<${c.c_type}>(self);
  return Py_BuildValue("(Os)",pyfoo, triqs::serialize(self_c).c_str()); // pyfoo ref ++ by Py_BuildValue
 }
%endif

//--------------------- reduce version 2 -----------------------------

%if c.serializable == "tuple" :

 // we make the unserialize function using the convertion of a C++ lambda !
 static PyObject* ${c.py_type}___reduce__ (PyObject *self, PyObject *args, PyObject *keywds) {
  auto & self_c = convert_from_python<${c.c_type}>(self);
  pyref r = pyref::module("${module.full_name}").attr("__reduce_reconstructor__${c.py_type}");
  if (r.is_null()) {
   PyErr_SetString(PyExc_ImportError,
                   "Cannot find the reconstruction function ${module.full_name}.__reduce_reconstructor__${c.py_type}");
   return NULL;
  }
  return Py_BuildValue("(NN)", r.new_ref() , reductor{}.apply_to(self_c)); // pyfoo ref ++ by Py_BuildValue
  //return Py_BuildValue("(NN)", borrowed(self).attr("__class__").attr("__module__").attr("__reduce_reconstructor__").new_ref(), reductor{}.apply_to(self_c)); // pyfoo ref ++ by Py_BuildValue
 }

//
 static PyObject* ${c.py_type}___reduce_reconstructor__ (PyObject *self, PyObject *args, PyObject *keywds) {
  try {
    %if not c.c_type_is_view :
      ${c.c_type} result;
    %else:
      typename ${c.c_type}::regular_type result;
    %endif
    auto r = reconstructor{args};
    result.serialize(r,0);// make sure reconstructor is a friend as boost::serialization::access
    return convert_to_python(std::move(result));
   }
   CATCH_AND_RETURN("in boost unserialization of object ${c.py_type}",NULL);
 }
%endif

//--------------------- repr  -----------------------------

%if c.is_printable :

static PyObject* ${c.py_type}___repr__ (PyObject *self) {
  auto & self_c = convert_from_python<${c.c_type}>(self);
  std::stringstream fs; fs << self_c;
  return PyString_FromString(fs.str().c_str());
}

static PyObject* ${c.py_type}___str__ (PyObject *self) {
  auto & self_c = convert_from_python<${c.c_type}>(self);
  std::stringstream fs; fs << self_c;
  return PyString_FromString(fs.str().c_str());
}

%endif

//--------------------- hdf5 : write part -----------------------------
%if not c.hdf5 :

/// hdf5 is not defined for this object, we still but a function + exception for a clear and early error message.
static PyObject* ${c.py_type}___write_hdf5__ (PyObject *self, PyObject *args) {
  PyErr_SetString(PyExc_NotImplementedError, "No hdf5 support for the object ${c.py_type}");
  return NULL;
 }

%else:

static PyObject* ${c.py_type}___write_hdf5__ (PyObject *self, PyObject *args) {
  triqs::h5::group gr;
  const char * key;
  if (!PyArg_ParseTuple(args, "O&s", converter_for_parser_non_wrapped_type<triqs::h5::group>, &gr, &key)) return NULL;
  auto & self_c = convert_from_python<${c.c_type}>(self);
  try {
   h5_write(gr, key, self_c);
  }
  CATCH_AND_RETURN("in h5 writing of object ${c.py_type}",NULL);
  Py_RETURN_NONE;
 }

// Make the reader function for the type and register it in the hdf archive module
// This function is called once in the module init function
static void register_h5_reader_for_${c.py_type} () {

  auto reader = [] (PyObject * h5_gr, std::string const & name) -> PyObject *{
   auto gr = convert_from_python<triqs::h5::group>(h5_gr);
   // declare the target C++ object, with special case if it is a view...
   %if not c.c_type_is_view :
     ${c.c_type} ret;
   %else:
     typename ${c.c_type}::regular_type ret;
   %endif
   try { // now read
     h5_read (gr, name, ret);
   }
   CATCH_AND_RETURN("in h5 reading of object ${c.py_type}", NULL);
   return convert_to_python(${c.c_type}(std::move(ret))); // cover the view and value case
  }; // end reader lambda

  pyref h5_reader = convert_to_python(std::function<PyObject*(PyObject *, std::string)> (reader));
  pyref module = pyref::module("pytriqs.archive.hdf_archive_schemes");
  auto register_class = module.attr("register_class");
  pyref res = PyObject_CallFunctionObjArgs(register_class, (PyObject*)(&${c.py_type}Type), Py_None, (PyObject*)h5_reader, NULL);
  //pyref res = PyObject_CallFunction(register_class, "OOO", (PyObject*)(&${c.py_type}Type), Py_None, (PyObject*)h5_reader);
}

%endif

//--------------------- Arithmetic implementation -----------------------------
  // IMPROVE ERROR TREATEMENT LIKE OTHER ORDINARY METHODS

% for op_name, op in c.number_protocol.items():

%if op.arity == 2:
static PyObject * ${c.py_type}_${op_name} (PyObject* v, PyObject *w){

 %if op.python_precall :
    // get the function to call
    static pyref py_fnt = pyref::module("${op.python_precall.rsplit('.',1)[0]}").attr("${op.python_precall.rsplit('.',1)[1]}");
    // call the function : return must be a tuple args, kw to replace the current args, kw
    pyref res = py_fnt(v,w);
    if (!res.is_None()) return res.new_ref(); // new ref or NULL
    // otherwise the precall has failed to make the operation, proceed with the overloads
 %endif

  %for overload in op.overloads :
  if (convertible_from_python<${overload.args[0][0]}>(v,false) && convertible_from_python<${overload.args[1][0]}>(w,false)) {
   try {
    ${regular_type_if_view_else_type(overload.rtype)} r = convert_from_python<${overload.args[0][0]}>(v) ${overload._get_calling_pattern()} convert_from_python<${overload.args[1][0]}>(w);
    return convert_to_python(std::move(r)); // in two steps to force type for expression templates in C++
   }
   CATCH_AND_RETURN("in calling C++ overload \n  ${overload._get_c_signature()} \nin implementation of operator ${overload._get_calling_pattern()} ", NULL)
  }
  %endfor
  PyErr_SetString(PyExc_RuntimeError,"Error: no C++ overload found in implementation of operator ${overload._get_calling_pattern()} ");
  return NULL;
}

%elif op.arity == 1:
static PyObject * ${c.py_type}_${op_name} (PyObject *v){

  %for overload in op.overloads :
  if (py_converter<${overload.args[0][0]}>::is_convertible(v,false)) {
   try {
    ${regular_type_if_view_else_type(overload.rtype)} r = ${overload._get_calling_pattern()}(convert_from_python<${overload.args[0][0]}>(v));
    return convert_to_python(std::move(r)); // in two steps to force type for expression templates in C++
   }
   CATCH_AND_RETURN("in calling C++ overload \n  ${overload._get_c_signature()} \nin implementation of operator ${overload._get_calling_pattern()} ", NULL)
  }
  %endfor
  PyErr_SetString(PyExc_RuntimeError,"Error: no C++ overload found in implementation of operator ${overload._get_calling_pattern()} ");
  return NULL;
}

%endif

%endfor

//--------------------- Implementation of iterator by wrapping the C++ -----------------------------

%if c.iterator :

 // the next() method of the iterator
 PyObject* ${c.py_type}__iterator__iternext__(PyObject *self) {
  auto * self_c = (${c.py_type}__iterator *)self;
  if (self_c->iter != self_c->end) {
    %if c.iterator.c_cast_type:
    PyObject *res = convert_to_python((${c.iterator.c_cast_type})(*(self_c->iter)));// make a COPY, I cannot wrap a ref in python !
    %else:
    PyObject *res = convert_to_python(*(self_c->iter));// make a COPY, I cannot wrap a ref in python !
    %endif
    ++self_c->iter;
    return res;
  } else {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
 }

// the __iter__ of the main type: return a new iterator. Need to allocate and init it by hand (no init method).
PyObject* ${c.py_type}___iter__(PyObject *self) {
  auto & self_c = convert_from_python<${c.c_type}>(self);
  auto * p = PyObject_New(${c.py_type}__iterator, &${c.py_type}__iteratorType);
  if (!p) return NULL;
  p->container = self;
  Py_INCREF(self); // We keep an owned reference to the container object (e.g. [x for x in g.mesh], if the container is a temporary and the iterator lives longer than its python reference).
  p->iter = ${c.iterator.begin}(self_c);
  p->end = ${c.iterator.end}(self_c);
  return (PyObject *)p;
}

%endif

%endfor  ## Big loop on classes c

//------------------------------------------------------------------------
//---------------------    MODULE  --------- -----------------------------
//------------------------------------------------------------------------

//--------------------- module function table  -----------------------------

// the table of the function of the module...
static PyMethodDef module_methods[] = {
%for pyf in module.functions.values():
    {"${pyf.py_name}", (PyCFunction)${module.name}_${pyf.py_name}, METH_VARARGS| METH_KEYWORDS, "${pyf._generate_doc()}"},
%endfor

%for c in module.classes.values() :
%if c.serializable == "tuple" :
    {"__reduce_reconstructor__${c.py_type}", (PyCFunction)${c.py_type}___reduce_reconstructor__, METH_VARARGS, "Internal " },
%endif
%endfor

{NULL}  /* Sentinel */
};

//--------------------- module init function -----------------------------

// The code of all module python functions
%for f in module.python_functions.values() :
 static const char * _module_python_function_code_${f.name} =
${f.code};
%endfor

%for f in module.hidden_python_functions.values() :
 static const char * _module_hidden_python_function_code_${f.name} =
${f.code};
%endfor

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
init${module.name}(void)
{
    // import numpy
    import_array();

    PyObject* m;

%for c in module.classes.values() :

    if (PyType_Ready(&${c.py_type}Type) < 0) return;

    %if c.iterator :
    // initializing the ${c.py_type}__iterator
    ${c.py_type}__iteratorType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&${c.py_type}__iteratorType) < 0)  return;
    Py_INCREF(&${c.py_type}__iteratorType);
    %endif
%endfor

    m = Py_InitModule3("${module.name}", module_methods, "${module.doc}");
    if (m == NULL) return;

%for c in module.classes.values() :
    Py_INCREF(&${c.py_type}Type);
    PyModule_AddObject(m, "${c.py_type}", (PyObject *)&${c.py_type}Type);
%endfor

    // hdf5 registration
%for c in [c for c in module.classes.values() if c.hdf5]:
    register_h5_reader_for_${c.py_type}();
%endfor

%if len(module.classes) >0 :
     // declare the exported wrapper functions
     static void * _exported_wrapped_convert_fnt[3*${len(module.classes)}];

     // init the array with the function pointers
     %for n,c in enumerate(module.classes.values()) :
       _exported_wrapped_convert_fnt[3*${n}] = (void *)convert_to_python<${c.c_type_absolute}>;
       _exported_wrapped_convert_fnt[3*${n}+1] = (void *)convert_from_python<${c.c_type_absolute}>;
       _exported_wrapped_convert_fnt[3*${n}+2] = (void *)convertible_from_python<${c.c_type_absolute}>;
     %endfor

    /* Create a Capsule containing the API pointer array's address */
    PyObject *c_api_object = PyCapsule_New((void *)_exported_wrapped_convert_fnt, "${module.full_name}._exported_wrapper_convert_fnt", NULL);
    if (c_api_object != NULL) PyModule_AddObject(m, "_exported_wrapper_convert_fnt", c_api_object);
%endif

   %if len(module.python_functions) + len(module.hidden_python_functions) > 0 :

    PyObject* main_module = PyImport_AddModule("__main__"); //borrowed
    PyObject* global_dict = PyModule_GetDict(main_module); //borrowed

    // load and compile the module function defined in pure python
   %for f in module.python_functions.values() :
    if (!PyRun_String( _module_python_function_code_${f.name},Py_file_input, global_dict, PyModule_GetDict(m) )) return;
   %endfor

    // now the hidden python function ...
    _module_hidden_python_function = PyModule_New("hidden_functions");
    // if we wish to still see the functions...
    PyModule_AddObject(m, "__hidden_fnt", _module_hidden_python_function);

    PyObject * d = PyModule_GetDict(_module_hidden_python_function); //borrowed
    %for f in module.hidden_python_functions.values() :
     if (!PyRun_String( _module_hidden_python_function_code_${f.name}, Py_file_input, global_dict ,d)) return;
    %endfor
   %endif
}


