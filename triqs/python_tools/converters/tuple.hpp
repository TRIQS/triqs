#pragma once
#include "../wrapper_tools.hpp"
#include <tuple>

namespace triqs {
namespace py_tools {

 template <typename... Types> struct py_converter<std::tuple<Types...>> {

 private:
  using tuple_t = std::tuple<Types...>;

  // c2py implementation
  template<int N, typename T, typename... Tail> static void c2py_impl(PyObject *list, tuple_t const& t, bool& ok) {
   pyref e = py_converter<T>::c2py(std::get<N>(t));
   int res = PyList_Append(list,e);
   ok = (res != -1);
   if(ok) c2py_impl<N+1,Tail...>(list,t,ok);
  }
  template<int> static void c2py_impl(PyObject *list, tuple_t const& t, bool& ok) {}

  // is_convertible implementation
  template<int N, typename T, typename... Tail> static bool is_convertible_impl(PyObject *seq, bool raise_exception) {
   return py_converter<T>::is_convertible(PySequence_Fast_GET_ITEM(seq, N), raise_exception) &&
          is_convertible_impl<N+1,Tail...>(seq, raise_exception);
  }
  template<int> static bool is_convertible_impl(PyObject *seq, bool raise_exception){ return true; }

  // py2c implementation
  template<int N, typename T, typename... Tail> static std::tuple<T,Tail...> py2c_impl(PyObject *seq) {
   return std::tuple_cat(std::make_tuple(py_converter<T>::py2c(PySequence_Fast_GET_ITEM(seq, N))),
                         py2c_impl<N+1,Tail...>(seq));
  }
  template<int> static std::tuple<> py2c_impl(PyObject *seq) { return std::make_tuple(); }

 public:
  static PyObject *c2py(tuple_t const &t) {
   PyObject *list = PyList_New(0);
   bool ok = true;
   c2py_impl<0,Types...>(list,t,ok);
   if(!ok) {
     Py_DECREF(list);
     return NULL;
   }
   return list;
  }

  static bool is_convertible(PyObject *ob, bool raise_exception) {
   if (PySequence_Check(ob)) {
    pyref seq = PySequence_Fast(ob, "expected a sequence");
    // Sizes must match! Could we relax this condition to '<'?
    if (PySequence_Fast_GET_SIZE((PyObject *)seq) != std::tuple_size<tuple_t>::value) goto _false;
    if (!is_convertible_impl<0,Types...>((PyObject *)seq, raise_exception)) goto _false;
    return true;
   }
  _false:
   if (raise_exception) {
    PyErr_SetString(PyExc_TypeError, "Cannot convert to std::tuple");
   }
   return false;
  }

  static tuple_t py2c(PyObject *ob) {
   pyref seq = PySequence_Fast(ob, "expected a sequence");
   return py2c_impl<0,Types...>((PyObject *)seq);
  }
 };
}
}

