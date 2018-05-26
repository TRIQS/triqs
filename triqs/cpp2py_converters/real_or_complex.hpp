#pragma once

namespace cpp2py {

  template <> struct py_converter<triqs::utility::real_or_complex> {

    using c_t      = triqs::utility::real_or_complex;
    using conv_d_t = py_converter<double>;
    using conv_c_t = py_converter<std::complex<double>>;

    static PyObject *c2py(c_t const &x) {
      if (x.is_real()) return conv_d_t::c2py(double(x));
      return conv_c_t::c2py(std::complex<double>(x));
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      bool ok = conv_d_t::is_convertible(ob, false) || conv_c_t::is_convertible(ob, false);
      if (ok) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to real_or_complex"); }
      return false;
    }

    static c_t py2c(PyObject *ob) {
      if (conv_d_t::is_convertible(ob, false)) return conv_d_t::py2c(ob);
      return conv_c_t::py2c(ob);
    }
  };
} // namespace cpp2py
