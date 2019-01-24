#pragma once
//#include <string>

namespace cpp2py {

  template <> struct py_converter<std::string> {

    static PyObject *c2py(std::string const &x) { return PyString_FromString(x.c_str()); }

    static std::string py2c(PyObject *ob) { return PyString_AsString(ob); }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PyString_Check(ob) or PyUnicode_Check(ob)) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to string"); }
      return false;
    }
  };

  template <> struct py_converter<const char *> {

    static PyObject *c2py(const char *x) { return PyString_FromString(x); }

    static const char *py2c(PyObject *ob) { return PyString_AsString(ob); }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PyString_Check(ob) or PyUnicode_Check(ob)) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to string"); }
      return false;
    }
  };

} // namespace cpp2py
