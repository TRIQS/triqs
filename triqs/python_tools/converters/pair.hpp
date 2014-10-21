#pragma once
#include "../wrapper_tools.hpp"
#include <utility>

namespace triqs {
namespace py_tools {

 template <typename T1, typename T2> struct py_converter<std::pair<T1, T2>> {
  static PyObject *c2py(std::pair<T1, T2> const &p) {
   PyObject *list = PyList_New(0);
   if (PyList_Append(list, py_converter<T1>::c2py(std::get<0>(p))) == -1) {
    Py_DECREF(list);
    return NULL;
   } // error
   if (PyList_Append(list, py_converter<T2>::c2py(std::get<1>(p))) == -1) {
    Py_DECREF(list);
    return NULL;
   } // error
   return list;
  }

  static bool is_convertible(PyObject *ob, bool raise_exception) {
   if (!PySequence_Check(ob)) goto _false;
   {
    pyref seq = PySequence_Fast(ob, "expected a sequence");
    if (!py_converter<T1>::is_convertible(PySequence_Fast_GET_ITEM((PyObject *)seq, 0), raise_exception))
     goto _false; // borrowed ref
    if (!py_converter<T2>::is_convertible(PySequence_Fast_GET_ITEM((PyObject *)seq, 1), raise_exception))
     goto _false; // borrowed ref
    return true;
   }
  _false:
   if (raise_exception) {
    PyErr_SetString(PyExc_TypeError, "Cannot convert to std::pair");
   }
   return false;
  }

  static std::pair<T1, T2> py2c(PyObject *ob) {
   pyref seq = PySequence_Fast(ob, "expected a sequence");
   return std::make_pair(py_converter<T1>::py2c(PySequence_Fast_GET_ITEM((PyObject *)seq, 0)),
                         py_converter<T2>::py2c(PySequence_Fast_GET_ITEM((PyObject *)seq, 1)));
  }
 };
}
}

