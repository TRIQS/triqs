#pragma once
#include "../wrapper_tools.hpp"
#include "../../operators/many_body_operator.hpp"
#include <utility>

namespace triqs {
namespace py_tools {

 /// REAL
 template <> struct py_converter<operators::many_body_operator_real> {
  using conv_t = py_converter<operators::many_body_operator>;

  static PyObject *c2py(operators::many_body_operator_real const &x) {
   return conv_t::c2py(x); 
  }

  static bool is_convertible(PyObject *ob, bool raise_exception) {
   bool ok = conv_t::is_convertible(ob, false); 
   // check the operator is real
   auto op = conv_t::py2c(ob);
   for (auto const &c_m : op) ok = ok && c_m.coef.is_real();
   if (ok) return true;
   if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to many_body_operator_real"); }
   return false;
  }

  static operators::many_body_operator_real py2c(PyObject *ob) {
   return operators::many_body_operator_real{conv_t::py2c(ob)};
  }
 };

 /// COMPLEX 
 template <> struct py_converter<operators::many_body_operator_complex> {
  using conv_t = py_converter<operators::many_body_operator>;

  static PyObject *c2py(operators::many_body_operator_complex const &x) {
   return conv_t::c2py(x); 
  }

  static bool is_convertible(PyObject *ob, bool raise_exception) {
   bool ok = conv_t::is_convertible(ob, false); 
   if (ok) return true;
   if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to many_body_operator_complex"); }
   return false;
  }

  static operators::many_body_operator_complex py2c(PyObject *ob) {
   return operators::many_body_operator_complex{conv_t::py2c(ob)};
  }
 };
}
}

