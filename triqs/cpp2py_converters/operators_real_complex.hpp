#pragma once

namespace cpp2py {

  /// REAL
  template <> struct py_converter<triqs::operators::many_body_operator_real> {
    using c_t = triqs::operators::many_body_operator_real;
    using conv_t = py_converter<triqs::operators::many_body_operator>;

    static PyObject *c2py(c_t const &x) { return conv_t::c2py(x); }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      bool ok = conv_t::is_convertible(ob, false);
      // check the operator is real
      auto op = conv_t::py2c(ob);
      for (auto const &c_m : op) ok= ok && c_m.coef.is_real();
      if (ok) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to many_body_operator_real"); }
      return false;
    }

    static c_t py2c(PyObject *ob) { return c_t{conv_t::py2c(ob)}; }
  };

  // ----------------------------------------------

  /// COMPLEX
  template <> struct py_converter<triqs::operators::many_body_operator_complex> {
    using c_t = triqs::operators::many_body_operator_complex;
    using conv_t = py_converter<triqs::operators::many_body_operator>;

    static PyObject *c2py(c_t const &x) { return conv_t::c2py(x); }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      bool ok = conv_t::is_convertible(ob, false);
      if (ok) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to many_body_operator_complex"); }
      return false;
    }

    static c_t py2c(PyObject *ob) { return c_t{conv_t::py2c(ob)}; }
  };
} // namespace cpp2py
