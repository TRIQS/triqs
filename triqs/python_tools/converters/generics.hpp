#pragma once
#include "../wrapper_tools.hpp"

namespace triqs {
namespace py_tools {

 // A generic converter for type T, if
 // - T can be explicitly constructed from R
 // - T can be explicitly casted into R.
 // - R is convertible
 template <typename T, typename R> struct py_converter_generic_cast_construct {

  using conv_t = py_converter<R>;

  static PyObject *c2py(T const &x) { return conv_t::c2py(static_cast<R>(x)); }

  static bool is_convertible(PyObject *ob, bool raise_exception) {
   if (conv_t::is_convertible(ob, false)) return true;
   if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to triqs::hilbert_space::fundamental_operator_set"); }
   return false;
  }

  static T py2c(PyObject *ob) { return T{conv_t::py2c(ob)}; }
 };
}
}

