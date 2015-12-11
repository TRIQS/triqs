// Specialization of py_converter to types wrapped by the wrap_generator.
// DO NOT EDIT. Generated automatically by wrap_generator
// We store here the list of the C++ converted types for use in another module
// WrappedTypeList
// ${list(set(module._wrapped_types))}
//
#pragma once
// in case it is included in the module ${module.full_name}.so, we do not want this specialization
#ifndef TRIQS_PYTHON_WRAPPER_MODULE_${module.name}

%for file in module.include_list :
%if file.startswith('<'):
#include ${file}
%else:
#include "${file}"
%endif
%endfor

#include <triqs/python_tools/wrapper_tools.hpp>

namespace triqs { namespace py_tools { 

%for n,c in enumerate(module.classes.values()) :

template<> struct py_converter<${c.c_type_absolute}> { 

  static void ** init() {
   PyObject * mod =  PyImport_ImportModule("${module.full_name}");
   if (mod ==NULL) return NULL;
   pyref capsule =  PyObject_GetAttrString(mod,  "_exported_wrapper_convert_fnt");
   if (capsule.is_null()) {
     PyErr_SetString(PyExc_RuntimeError, "TRIQS: can not find _exported_wrapper_convert_fnt in the module ${module.full_name}");
     return NULL;
   }
   void ** table = (void**) PyCapsule_GetPointer(capsule, "${module.full_name}._exported_wrapper_convert_fnt");
   //void ** table = (void **)PyCapsule_Import("${module.full_name}._exported_wrapper_convert_fnt", 0);
   return table;
 }
 
  static PyObject * c2py(${c.c_type_absolute} const & x){
   static void **wrapped_convert_fnt = init();
   if (wrapped_convert_fnt == NULL) return NULL;
   return ((PyObject * (*)(${c.c_type_absolute} const &)) wrapped_convert_fnt[3*${n}])(x);
 }
 
  static ${c.c_type_absolute}& py2c(PyObject * ob){
   static void **wrapped_convert_fnt = init();
   if (wrapped_convert_fnt == NULL) std::terminate(); // It should never happen since py2c is called only is is_convertible is true (py_converter specs) 
   return ((${c.c_type_absolute}& (*)(PyObject *)) wrapped_convert_fnt[3*${n}+1])(ob);
 }
 
  static bool is_convertible(PyObject *ob, bool raise_exception) {
   static void **wrapped_convert_fnt = init();
   if (wrapped_convert_fnt == NULL) {
    if (!raise_exception && PyErr_Occurred()) {PyErr_Print();PyErr_Clear();}
    return false;
   }
   return ((bool (*)(PyObject *,bool)) wrapped_convert_fnt[3*${n}+2])(ob,raise_exception);
 }
};

%endfor

//--------------------- Converters of regular types --------------------------

%for c in module.classes.values() :

 %if c.implement_regular_type_converter : 
 // ${c.py_type} is wrapping a view, we are also implementing the converter of the associated regular type
 template<> struct py_converter<${c.regular_type_absolute}> {
 using regular_type = ${c.regular_type_absolute};
 using conv = py_converter<${c.c_type_absolute}>;
 static PyObject *c2py(regular_type &g) { return conv::c2py(g); }
 static PyObject *c2py(regular_type &&g) { return conv::c2py(g); }
 static bool is_convertible(PyObject * ob, bool raise_exception) { return conv::is_convertible(ob, raise_exception); }
 static regular_type py2c(PyObject *ob) { return conv::py2c(ob); }
};
%endif
%endfor
//--------------------- Converters of enums --------------------------

%for en in module.enums :

template <> struct py_converter<${en.c_name_absolute}> {
 static PyObject * c2py(${en.c_name_absolute} x) {
   %for n,val in enumerate(en.values[:-1]) :
    if (x == ${en.c_namespace}${val}) return PyString_FromString("${val}");
   %endfor
   return PyString_FromString("${en.values[-1]}"); // last case separate to avoid no return warning of compiler
 }
 static ${en.c_name_absolute} py2c(PyObject * ob){
   std::string s=PyString_AsString(ob);
   %for n,val in enumerate(en.values[:-1]) :
    if (s == "${val}") return ${en.c_namespace}${val};
   %endfor
   return ${en.c_namespace}${en.values[-1]};
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
   if (!PyString_Check(ob))  {
     if (raise_exception) PyErr_SetString(PyExc_ValueError, "Convertion of C++ enum ${en.c_name_absolute} : the object is not a string");
     return false;
   }
   std::string s=PyString_AsString(ob);
   %for n,val in enumerate(en.values) :
    if (s == "${val}") return true;
   %endfor
   if (raise_exception) {
    auto err = "Convertion of C++ enum ${en.c_name_absolute} : \nThe string \"" + s +"\" is not in [${','.join([str(x) for x in en.values])}]";
    PyErr_SetString(PyExc_ValueError, err.c_str());
   }
   return false;
 }
};

%endfor

}}
#endif
