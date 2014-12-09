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

// boost::variant<Types...> converter
template<typename... Types> struct py_converter<boost::variant<Types...>> {

 using variant_t = boost::variant<Types...>;
 using types_t = typename variant_t::types;
 using begin_t = typename boost::mpl::begin<types_t>::type;
 using end_t = typename boost::mpl::end<types_t>::type;

private:

// Implementation details
template<typename Iter, typename IterEnd> struct is_convertible_impl {
 static bool apply(PyObject *ob, bool raise_exception) {
  return py_converter<typename boost::mpl::deref<Iter>::type>::is_convertible(ob, raise_exception) ||
         is_convertible_impl<typename boost::mpl::next<Iter>::type,IterEnd>::apply(ob, raise_exception);
 }
};

template<typename Iter> struct is_convertible_impl<Iter,Iter> {
 static bool apply(PyObject *ob, bool raise_exception) { return false; }
};

template<typename Iter, typename IterEnd> struct py2c_impl {
 static variant_t apply(PyObject *ob) {
  if(py_converter<typename boost::mpl::deref<Iter>::type>::is_convertible(ob, false))
   return py_converter<typename boost::mpl::deref<Iter>::type>::py2c(ob);
  else
   return py2c_impl<typename boost::mpl::next<Iter>::type,IterEnd>::apply(ob);
 }
};

template<typename Iter> struct py2c_impl<Iter,Iter> {
 static variant_t apply(PyObject *ob) {
  TRIQS_RUNTIME_ERROR << "Internal error: py2c called for a Python object incompatible with boost::variant";
 }
};

public:

 static PyObject *c2py(variant_t index) {
  index_visitor iv;
  return boost::apply_visitor(iv, index);
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (is_convertible_impl<begin_t,end_t>::apply(ob, false)) return true;
  if (raise_exception) {
    PyErr_SetString(PyExc_TypeError, "Cannot convert to variant");
  }
  return false;
 }

 static variant_t py2c(PyObject *ob) {
  return py2c_impl<begin_t,end_t>::apply(ob);
 }

};

}}

