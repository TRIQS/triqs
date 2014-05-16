// Specialization of py_converter to types wrapped by the wrap_generator.
// DO NOT EDIT. Generated automatically by wrap_generator

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

//#include <triqs/arrays.hpp>
#include <triqs/python_tools/wrapper_tools.hpp>

namespace triqs { namespace py_tools { 

%for n,t in enumerate(module.wrapped_types_by_me) :

template<> struct py_converter<${t}> {
 static void ** init();
 static PyObject * c2py(${t} const & x);
 static ${t}& py2c(PyObject * ob);
 static bool is_convertible(PyObject *ob, bool raise_exception);
};

 void ** py_converter<${t}>::init() {
   PyObject * mod =  PyImport_ImportModule("${module.full_name}");
   if (mod ==NULL) return NULL;
   void ** table = (void **)PyCapsule_Import("${module.full_name}._exported_wrapper_convert_fnt", 0);
   return table;
 }
 
 PyObject * py_converter<${t}>::c2py(${t} const & x){
   static void **wrapped_convert_fnt = init();
   if (wrapped_convert_fnt == NULL) return NULL;
   return ((PyObject * (*)(${t} const &)) wrapped_convert_fnt[3*${n}])(x);
 }
 
 ${t}& py_converter<${t}>::py2c(PyObject * ob){
   static void **wrapped_convert_fnt = init();
   if (wrapped_convert_fnt == NULL) std::terminate(); // It should never happen since py2c is called only is is_convertible is true (py_converter specs) 
   return ((${t}& (*)(PyObject *)) wrapped_convert_fnt[3*${n}+1])(ob);
 }
 
 bool py_converter<${t}>::is_convertible(PyObject *ob, bool raise_exception) {
   static void **wrapped_convert_fnt = init();
   if (wrapped_convert_fnt == NULL) {
    if (!raise_exception && PyErr_Occurred()) {PyErr_Print();PyErr_Clear();}
    return false;
   }
   return ((bool (*)(PyObject *,bool)) wrapped_convert_fnt[3*${n}+2])(ob,raise_exception);
 }
%endfor
}}

