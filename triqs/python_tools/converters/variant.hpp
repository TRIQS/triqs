#pragma once
#include "../wrapper_tools.hpp"
#include <boost/variant.hpp>

namespace triqs {
namespace py_tools {

// variant visitor: converts variant type to PyObject
struct index_visitor: public boost::static_visitor<PyObject *> {
  template<typename T>
  PyObject * operator()(T i) { return py_converter<T>::c2py(i); }
};

// boost::variant<U,V> converter
// note: this converter is restricted to the case of two possible types
template<typename U, typename V> struct py_converter<boost::variant<U,V>> {

 static PyObject *c2py(boost::variant<U,V> index) {
  index_visitor iv;
  return boost::apply_visitor(iv, index);
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (py_converter<V>::is_convertible(ob, false) ||
      py_converter<U>::is_convertible(ob, false)) return true;
  if (raise_exception) {
    PyErr_SetString(PyExc_TypeError, "Cannot convert to variant");
  }
  return false;
 }

 static boost::variant<U,V> py2c(PyObject *ob) {
  if (py_converter<V>::is_convertible(ob, false)) return py_converter<V>::py2c(ob);
  if (py_converter<U>::is_convertible(ob, false)) return py_converter<U>::py2c(ob);
 }

};

}}

