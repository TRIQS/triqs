#pragma once

namespace cpp2py {

  // --- complex

  template <> struct py_converter<std::complex<double>> {
    static PyObject *c2py(std::complex<double> x) { return PyComplex_FromDoubles(x.real(), x.imag()); }
    static std::complex<double> py2c(PyObject *ob) {
      if (PyComplex_Check(ob)) {
        auto r = PyComplex_AsCComplex(ob);
        return {r.real, r.imag};
      }
      return PyFloat_AsDouble(ob);
    }
    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PyComplex_Check(ob) || PyFloat_Check(ob) || PyInt_Check(ob)) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to complex"); }
      return false;
    }
  };

} // namespace cpp2py
