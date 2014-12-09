#pragma once
#include "../wrapper_tools.hpp"
#include <map>

namespace triqs { namespace py_tools {

template <typename K, typename V> struct py_converter<std::map<K,V>> {
 static PyObject * c2py(std::map<K,V> const &m) {
  PyObject * d = PyDict_New();
  for (auto & x : m) if (PyDict_SetItem(d, py_converter<K>::c2py(x.first), py_converter<V>::c2py(x.second)) == -1) { Py_DECREF(d); return NULL;} // error
  return d;
 }
 static PyObject * c2py(std::map<K,V> &m) {
  PyObject * d = PyDict_New();
  for (auto & x : m) if (PyDict_SetItem(d, py_converter<K>::c2py(x.first), py_converter<V>::c2py(x.second)) == -1) { Py_DECREF(d); return NULL;} // error
  return d;
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (!PyDict_Check(ob)) goto _false;
  {
   pyref keys = PyDict_Keys(ob);
   pyref values = PyDict_Values(ob);
   int len = PyDict_Size(ob);
   for (int i = 0; i < len; i++) {
     if (!py_converter<K>::is_convertible(PyList_GET_ITEM((PyObject*)keys, i),raise_exception)) goto _false; //borrowed ref
     if (!py_converter<V>::is_convertible(PyList_GET_ITEM((PyObject*)values, i),raise_exception)) goto _false; //borrowed ref
   }
   return true;
  }
  _false:
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::map");}
  return false;
 }
 static std::map<K,V> py2c(PyObject * ob) {
   pyref keys = PyDict_Keys(ob);
   pyref values = PyDict_Values(ob);
   std::map<K,V> res;
   int len = PyDict_Size(ob);
   for (int i = 0; i < len; i++)
     res.emplace(py_converter<K>::py2c(PyList_GET_ITEM((PyObject*)keys, i)),  //borrowed ref
                 py_converter<V>::py2c(PyList_GET_ITEM((PyObject*)values, i))); //borrowed ref
   return res;
 }
};
}}
