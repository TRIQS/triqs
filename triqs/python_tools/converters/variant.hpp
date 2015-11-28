#pragma once
#include "../wrapper_tools.hpp"
#include <triqs/utility/variant.hpp>

namespace triqs {
namespace py_tools {

// triqs::utility::variant<Types...> converter
// converts in the first possible type
template<typename... Types>
struct py_converter<triqs::utility::variant<Types...>> {

private:

  using variant_t = triqs::utility::variant<Types...>;
  template<int N> using types_t = typename variant_t::template bounded_type<N>;
  constexpr static int n_types = variant_t::n_bounded_types;

  // c2py_visitor
  struct c2py_visitor {
    template<typename T> PyObject * operator()(T const &x) { return py_converter<T>::c2py(x); }
  };

  // is_convertible_impl
  template<int N> static std14::enable_if_t<(N<n_types),bool>
  is_convertible_impl(PyObject *ob) {
    return py_converter<types_t<N>>::is_convertible(ob,false) ||
           is_convertible_impl<N+1>(ob);
  }
  template<int N> static std14::enable_if_t<N==n_types,bool>
  is_convertible_impl(PyObject *ob) { return false; }

  // py2c_impl
  template<int N> static std14::enable_if_t<(N<n_types),variant_t>
  py2c_impl(PyObject *ob) {
    if(py_converter<types_t<N>>::is_convertible(ob,false))
      return py_converter<types_t<N>>::py2c(ob);
    else
      return py2c_impl<N+1>(ob);
  }
  template<int N> static std14::enable_if_t<N==n_types,variant_t>
  py2c_impl(PyObject *ob) {
    TRIQS_RUNTIME_ERROR <<
    "Internal error: py2c called for a Python object incompatible with triqs::utility::variant";
  }

public:

  static PyObject* c2py(variant_t const& v) {
    return apply_visitor(c2py_visitor(), v);
  }

  static bool is_convertible(PyObject *ob, bool raise_exception) {
    if (is_convertible_impl<0>(ob)) return true;
    if (raise_exception) {
      PyErr_SetString(PyExc_TypeError, "Cannot convert to triqs::utility::variant");
    }
    return false;
  }

  static variant_t py2c(PyObject *ob) {
    return py2c_impl<0>(ob);
  }
};

}}
