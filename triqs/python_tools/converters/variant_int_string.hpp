#pragma once
#include "../wrapper_tools.hpp"
#include "../../utility/variant_int_string.hpp"
#include "./string.hpp"

namespace triqs {
namespace py_tools {

 template <> struct py_converter<utility::variant_int_string> {

#ifndef TRIQS_CPP11
  static PyObject *c2py(utility::variant_int_string const &v) {
   return apply_visitor([](auto const &x) { return triqs::py_tools::convert_to_python(x); }, v);
  }
#else
  struct visitor {
   template <typename T> PyObject *operator()(T const &i) { return convert_to_python(i); }
  };

  static PyObject *c2py(utility::variant_int_string const &v) {
   return apply_visitor(visitor{}, v);
  }
#endif

  static bool is_convertible(PyObject *ob, bool raise_exception) {
   bool b = py_converter<int>::is_convertible(ob, false) || py_converter<std::string>::is_convertible(ob, false);
   if (!b && raise_exception) {
    PyErr_SetString(PyExc_TypeError, "Cannot convert to a variant int|string");
   }
   return b;
  }

  static utility::variant_int_string py2c(PyObject *ob) {
   if (py_converter<int>::is_convertible(ob, false)) return {int(py_converter<int>::py2c(ob))};
   return {py_converter<std::string>::py2c(ob)};
  }
 };
}
}

