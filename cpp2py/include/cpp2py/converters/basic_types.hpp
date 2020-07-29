#pragma once
#include "./complex.hpp"

namespace cpp2py {

  // PyObject *
  template <> struct py_converter<PyObject *> {
    static PyObject *c2py(PyObject *ob) { return ob; }
    static PyObject *py2c(PyObject *ob) { return ob; }
    static bool is_convertible(PyObject *ob, bool raise_exception) { return true; }
  };

  // --- bool
  template <> struct py_converter<bool> {
    static PyObject *c2py(bool b) {
      if (b)
        Py_RETURN_TRUE;
      else
        Py_RETURN_FALSE;
    }
    static bool py2c(PyObject *ob) { return ob == Py_True; }
    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PyBool_Check(ob)) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to bool"); }
      return false;
    }
  };

  // --- long

  namespace details{
    template <typename I> struct py_converter_impl {
      static PyObject *c2py(I i) { return PyInt_FromLong(long(i)); }
      static I py2c(PyObject *ob) { return I(PyInt_AsLong(ob)); }
      static bool is_convertible(PyObject *ob, bool raise_exception) {
        if (PyInt_Check(ob)) return true;
        if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to integer type"); }
        return false;
      }
    };
  }

  template <> struct py_converter<long> : details::py_converter_impl<long> {};
  template <> struct py_converter<int> : details::py_converter_impl<int> {};
  template <> struct py_converter<unsigned int> : details::py_converter_impl<unsigned int> {};
  template <> struct py_converter<unsigned long> : details::py_converter_impl<unsigned long> {};
  template <> struct py_converter<unsigned long long> : details::py_converter_impl<unsigned long long> {};

  // --- double

  template <> struct py_converter<double> {
    static PyObject *c2py(double x) { return PyFloat_FromDouble(x); }
    static double py2c(PyObject *ob) { return PyFloat_AsDouble(ob); }
    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PyFloat_Check(ob) || PyInt_Check(ob)) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to double"); }
      return false;
    }
  };

} // namespace cpp2py
