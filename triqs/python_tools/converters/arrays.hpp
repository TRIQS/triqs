#pragma once
#include "../wrapper_tools.hpp"
#include <triqs/arrays.hpp>

// in the generated wrapper, we only "import_array" if this macro is defined.
#define TRIQS_IMPORTED_CONVERTERS_ARRAYS

namespace triqs {
namespace py_tools {

 // --- mini_vector<T,N>---
 // via std::vector
 template <typename T, int N> struct py_converter<triqs::utility::mini_vector<T, N>> {
  using conv = py_converter<std::vector<T>>;
  static PyObject *c2py(triqs::utility::mini_vector<T, N> const &v) { return conv::c2py(v.to_vector()); }
  static bool is_convertible(PyObject *ob, bool raise_exception) { return conv::is_convertible(ob, raise_exception); }
  static triqs::utility::mini_vector<T, N> py2c(PyObject *ob) { return conv::py2c(ob); }
 };

 // --- array

 inline static void import_numpy() {
  static bool init = false;
  if (!init) {
   _import_array();
   // std::cerr << "importing array"<<std::endl;
   init = true;
  }
 }

 template <typename ArrayType> struct py_converter_array {
  static PyObject *c2py(ArrayType const &x) {
   import_numpy();
   return x.to_python();
  }
  static ArrayType py2c(PyObject *ob) {
   import_numpy();
   return ArrayType(ob);
  }
  static bool is_convertible(PyObject *ob, bool raise_exception) {
   import_numpy();
   try {
    py2c(ob);
    return true;
   }
   catch (...) {
    if (raise_exception) {
     PyErr_SetString(PyExc_TypeError, "Cannot convert to array/matrix/vector");
    }
    return false;
   }
  }
 };

 template <typename T, int R>
 struct py_converter<triqs::arrays::array_view<T, R>> : py_converter_array<triqs::arrays::array_view<T, R>> {};
 template <typename T> struct py_converter<triqs::arrays::matrix_view<T>> : py_converter_array<triqs::arrays::matrix_view<T>> {};
 template <typename T> struct py_converter<triqs::arrays::vector_view<T>> : py_converter_array<triqs::arrays::vector_view<T>> {};

 template <typename T, int R>
 struct py_converter<triqs::arrays::array_const_view<T, R>> : py_converter_array<triqs::arrays::array_const_view<T, R>> {};
 template <typename T>
 struct py_converter<triqs::arrays::matrix_const_view<T>> : py_converter_array<triqs::arrays::matrix_const_view<T>> {};
 template <typename T>
 struct py_converter<triqs::arrays::vector_const_view<T>> : py_converter_array<triqs::arrays::vector_const_view<T>> {};

 template <typename T, int R> struct py_converter<triqs::arrays::array<T, R>> : py_converter_array<triqs::arrays::array<T, R>> {};
 template <typename T> struct py_converter<triqs::arrays::matrix<T>> : py_converter_array<triqs::arrays::matrix<T>> {};
 template <typename T> struct py_converter<triqs::arrays::vector<T>> : py_converter_array<triqs::arrays::vector<T>> {};

 // --- range

 // convert from python slice and int (interpreted are slice(i,i+1,1))
 template <> struct py_converter<triqs::arrays::range> {
  static PyObject *c2py(triqs::arrays::range const &r) {
   return PySlice_New(convert_to_python(r.first()), convert_to_python(r.last()), convert_to_python(r.step()));
  }
  static triqs::arrays::range py2c(PyObject *ob) {
   if (PyInt_Check(ob)) {
    int i = PyInt_AsLong(ob);
    return {i, i + 1, 1};
   }
   int len = 4; // no clue what this len is ??
   Py_ssize_t start, stop, step, slicelength;
   if (PySlice_GetIndicesEx((PySliceObject *)ob, len, &start, &stop, &step, &slicelength) < 0) return {};
   return {start, stop, step};
  }
  static bool is_convertible(PyObject *ob, bool raise_exception) {
   if (PySlice_Check(ob) || PyInt_Check(ob)) return true;
   if (raise_exception) {
    PyErr_SetString(PyExc_TypeError, "Cannot convert to python slice");
   }
   return false;
  }
 };
}
}

