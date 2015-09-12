#pragma once
#include "../wrapper_tools.hpp"
#include <utility>

namespace triqs {
namespace py_tools {

 template <> struct py_converter<utility::real_or_complex> {
  using d_t = py_converter<double>;
  using c_t = py_converter<std::complex<double>>;

  static PyObject *c2py(utility::real_or_complex const &x) {
   if (x.is_real()) return d_t::c2py(double(x));
   return c_t::c2py(std::complex<double>(x));
  }

  static bool is_convertible(PyObject *ob, bool raise_exception) {
   bool ok = d_t::is_convertible(ob, false) || c_t::is_convertible(ob, false);
   if (ok) return true;
   if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to real_or_complex"); }
   return false;
  }

  static utility::real_or_complex py2c(PyObject *ob) {
   if (d_t::is_convertible(ob, false)) return d_t::py2c(ob);
   return c_t::py2c(ob);
  }
 };
}
}

