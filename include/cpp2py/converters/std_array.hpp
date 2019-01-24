#pragma once
#include <array>

namespace cpp2py {

  template <typename T, size_t R> struct py_converter<std::array<T, R>> {
    // --------------------------------------

    static PyObject *c2py(std::array<T, R> const &v) {
      PyObject *list = PyList_New(0);
      for (auto const &x : v) {
        pyref y = py_converter<T>::c2py(x);
        if (y.is_null() or (PyList_Append(list, y) == -1)) {
          Py_DECREF(list);
          return NULL;
        } // error
      }
      return list;
    }

    // --------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (!PySequence_Check(ob)) goto _false;
      {
        pyref seq = PySequence_Fast(ob, "expected a sequence");
        int len   = PySequence_Size(ob);
        if (len != R) {
          if (raise_exception) {
            auto s = std::string{"Convertion to std::array<T, R> failed : the length of the sequence ( = "} + std::to_string(len)
               + " does not match R = " + std::to_string(R);
            PyErr_SetString(PyExc_TypeError, s.c_str());
          }
          return false;
        }
        for (int i = 0; i < len; i++)
          if (!py_converter<T>::is_convertible(PySequence_Fast_GET_ITEM((PyObject *)seq, i), raise_exception)) goto _false; // borrowed ref

        return true;
      }
    _false:
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::array"); }
      return false;
    }

    // --------------------------------------

    static std::array<T, R> py2c(PyObject *ob) {
      pyref seq = PySequence_Fast(ob, "expected a sequence");
      std::array<T, R> res;
      for (int i = 0; i < R; i++) res[i] = py_converter<T>::py2c(PySequence_Fast_GET_ITEM((PyObject *)seq, i)); // borrowed ref
      return res;
    }
  };

} // namespace cpp2py
