#pragma once
#include "../wrapper_tools.hpp"
#include <vector>

namespace triqs { namespace py_tools {

template <typename T> struct py_converter<std::vector<T>> {
 static PyObject * c2py(std::vector<T> const &v) {
  PyObject * list = PyList_New(0);
  for (auto & x : v) if (PyList_Append(list, py_converter<T>::c2py(x)) == -1) { Py_DECREF(list); return NULL;} // error
  return list;
 }
  static PyObject * c2py(std::vector<T> &v) {
  PyObject * list = PyList_New(0);
  for (auto & x : v) if (PyList_Append(list, py_converter<T>::c2py(x)) == -1) { Py_DECREF(list); return NULL;} // error
  return list;
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (!PySequence_Check(ob)) goto _false;
  {
   pyref seq = PySequence_Fast(ob, "expected a sequence");
   int len = PySequence_Size(ob);
   for (int i = 0; i < len; i++) if (!py_converter<T>::is_convertible(PySequence_Fast_GET_ITEM((PyObject*)seq, i),raise_exception)) goto _false; //borrowed ref
   return true;
  }
  _false:
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::vector");}
  return false;
 }
 static std::vector<T> py2c(PyObject * ob) {
  pyref seq = PySequence_Fast(ob, "expected a sequence");
  std::vector<T> res;
  int len = PySequence_Size(ob);
  for (int i = 0; i < len; i++) res.push_back(py_converter<T>::py2c(PySequence_Fast_GET_ITEM((PyObject*)seq, i))); //borrowed ref
  return res;
 }
};

}}
