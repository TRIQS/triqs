#pragma once
#include <Python.h>
#include "structmember.h"
#include <string>
#include <complex>

// silence warning on intel
#ifndef __INTEL_COMPILER
#pragma clang diagnostic ignored "-Wdeprecated-writable-strings"
#endif
#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace cpp2py {

  // Should T be treated like a view, i.e. use rebind.
  // Must be specialized for each view type
  template <typename T> struct is_view : std::false_type {};

  //---------------------  Copied from TRIQS, basic utilities -----------------------------
  // to remove TRIQS dependence for this tool 
  namespace std17 {
    template <typename... Ts> struct _make_void { typedef void type; };
    template <typename... Ts> using void_t = typename _make_void<Ts...>::type;
  } // namespace std17

  // Makes a clone
  template<typename T, typename = std17::void_t<>> struct _make_clone { static T invoke(T const &x) { return T{x};} };
  template<typename T> struct _make_clone<T, std17::void_t<typename T::regular_type>> {
    static auto invoke(T const &x) { return typename T::regular_type{x};}
  };
  template<typename T> auto make_clone(T const & x) { return _make_clone<T>::invoke(x);}

  // regular type traits
  template<typename T> void _nop(T ...){};
  template<typename T, typename Enable=void> struct has_regular : std::false_type {};
  template <typename T> struct has_regular<T, decltype(_nop(std::declval<typename T::regular_type>()))> : std::true_type {};
  template<typename T, bool HasRegular = has_regular<T>::value> struct _regular_type_if_view_else_type_t;
  template<typename T> struct _regular_type_if_view_else_type_t<T,false> {using type=T;};
  template<typename T> struct _regular_type_if_view_else_type_t<T,true > {using type=typename T::regular_type;};
  template<typename A> using regular_type_if_view_else_type_t = typename _regular_type_if_view_else_type_t<std::decay_t<A>>::type;

  //---------------------  py_converters -----------------------------

  // default version for a wrapped type. To be specialized later.
  // py2c behaviour is undefined is is_convertible return false
  // c2py should return NULL on failure
  template <typename T> struct py_converter;
  //{
  //  static PyObject * c2py(T const & x);
  //  static T & py2c(PyObject * ob);
  //  static bool is_convertible(PyObject * ob, bool raise_exception);
  //}

  // helpers for better error message
  // some class (e.g. range !) only have ONE conversion, i.e. C -> Py, but not both
  // we need to distinguish
  template <class, class = std17::void_t<>> struct does_have_a_converterPy2C : std::false_type {};
  template <class T> struct does_have_a_converterPy2C<T, std17::void_t<decltype(py_converter<std::decay_t<T>>::py2c(nullptr))>> : std::true_type {};

  template <class, class = std17::void_t<>> struct does_have_a_converterC2Py : std::false_type {};
  template <class T>
  struct does_have_a_converterC2Py<T, std17::void_t<decltype(py_converter<std::decay_t<T>>::c2py(std::declval<T>()))>> : std::true_type {};

  // We only use these functions in the code, not directly the converter
  template <typename T> static PyObject *convert_to_python(T &&x) {
    static_assert(does_have_a_converterC2Py<T>::value, "The type does not have a converter from C++ to Python");
    return py_converter<std::decay_t<T>>::c2py(std::forward<T>(x));
  }
  template <typename T> static bool convertible_from_python(PyObject *ob, bool raise_exception) {
    return py_converter<T>::is_convertible(ob, raise_exception);
  }

  /*
 * type          T            py_converter<T>::p2yc     convert_from_python<T>   converter_for_parser type of p      impl
 *                                                 
 * regular       R            R or R&& or R const&      R  or R&& or R const&    R*                                *p = py_converter<T>::p2yc(ob))
 * view          V            V                         V                        V*                                p->rebind(py_converter<T>::p2yc(ob))
 * wrapped       W            W *                       W                        W**                               *p = py_converter<T>::p2yc(ob))  
 * wrapped view  WV           WV *                      WV                       WV**                              p->rebind(py_converter<T>::p2yc(ob))
 * PyObejct *    PyObject *   PyObject *                PyObject *               PyObject **                       *p = py_converter<T>::p2yc(ob))  
 * U*            U*           U*                        U*                       U**                               *p = py_converter<T>::p2yc(ob)) 
 *
 */

  // is_wrapped<T>  if py_converter has been reimplemented.
  template <typename T, class = void> struct is_wrapped : std::false_type {};
  template <typename T> struct is_wrapped<T, typename py_converter<T>::is_wrapped_type> : std::true_type {};

  template <typename T> constexpr bool is_wrapped_v = is_wrapped<T>::value;

  template <typename T> static auto convert_from_python(PyObject *ob) -> decltype(py_converter<T>::py2c(ob)) {
    static_assert(does_have_a_converterPy2C<T>::value, "The type does not have a converter from Python to C++");
    return py_converter<T>::py2c(ob);
  }

  // TODO C17 : if constexpr
  // used by PyParse_xxx : U is a pointer iif we have a wrapped object.
  template <typename T> static void converter_for_parser_dispatch(PyObject *ob, T *p, std::false_type, std::false_type) {
    *p = py_converter<T>::py2c(ob);
  }
  template <typename T> static void converter_for_parser_dispatch(PyObject *ob, T *p, std::false_type, std::true_type) {
    p->rebind(py_converter<T>::py2c(ob));
  }
  template <typename T> static void converter_for_parser_dispatch(PyObject *ob, T **p, std::true_type, std::false_type) {
    *p = &py_converter<T>::py2c(ob);
  }
  template <typename T> static void converter_for_parser_dispatch(PyObject *ob, T **p, std::true_type, std::true_type) {
    *p = &py_converter<T>::py2c(ob);
  }

  template <typename T> static int converter_for_parser(PyObject *ob, std::conditional_t<is_wrapped_v<T>, T *, T> *p) {
    if (!convertible_from_python<T>(ob, true)) return 0;
    converter_for_parser_dispatch(ob, p, is_wrapped<T>{}, cpp2py::is_view<T>{});
    return 1;
  }

  // pointer -> ref except PyObject *. We assume here that there is no
  // converter py_converter<U*>. The dereference should be only for wrapped type. Check by static_assert
  // Generalize if needed.
  inline PyObject *deref_is_wrapped(PyObject *x) { return x; }
  template <typename T> auto &deref_is_wrapped(T *x) {
    static_assert(is_wrapped<T>::value, "Internal assumption invalid");
    return *x;
  }
  template <typename T> auto &deref_is_wrapped(T &x) { return x; }

  // A generic converter for type T, if
  // - T can be explicitly constructed from R
  // - T can be explicitly casted into R.
  // - R is convertible
  template <typename T, typename R> struct py_converter_generic_cast_construct {

    using conv_t = py_converter<R>;

    static PyObject *c2py(T const &x) { return conv_t::c2py(static_cast<R>(x)); }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (conv_t::is_convertible(ob, false)) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to ... failed"); }
      return false;
    }

    static T py2c(PyObject *ob) { return T{conv_t::py2c(ob)}; }
  };

  // A generic converter for type T, if
  // - T can be explicitly constructed from R
  // - R can be explicitly constructed from T
  // - R is convertible
  // e.g. a view
  template <typename T, typename R> struct py_converter_generic_cross_construction {

    using conv_t = py_converter<R>;

    static PyObject *c2py(T const &x) { return conv_t::c2py(R{x}); }
    static PyObject *c2py(T &&x) { return conv_t::c2py(R{std::move(x)}); }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (conv_t::is_convertible(ob, false)) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to ... failed"); }
      return false;
    }

    static T py2c(PyObject *ob) { return T{conv_t::py2c(ob)}; }
  };

} // namespace cpp2py
