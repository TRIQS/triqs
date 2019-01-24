#pragma once
#include <tuple>
#include <triqs/utility/variant.hpp>

namespace cpp2py {

  // std::variant<T...> converter
  // converts in the first possible type
  template <typename... T> struct py_converter<std::variant<T...>> {

    private:
    // using variant_t                = std::variant<T...>;
    template <int N> using types_t = std::tuple_element_t<N, std::tuple<T...>>;
    //constexpr static int n_types   = sizeof...(T);

    // c2py_visitor
    //struct c2py_visitor {
    //template <typename T> PyObject *operator()(T const &x) { return py_converter<T>::c2py(x); }
    //};

    // is_convertible_impl
    /*  template <int N> static std::enable_if_t<(N < n_types), bool> is_convertible_impl(PyObject *ob) {*/
    //return py_converter<types_t<N>>::is_convertible(ob, false) || is_convertible_impl<N + 1>(ob);
    //}
    //template <int N> static std::enable_if_t<N == n_types, bool> is_convertible_impl(PyObject *ob) { return false; }

    // py2c_impl
    //template <int N> static std::enable_if_t<(N < n_types), variant_t> py2c_impl(PyObject *ob) {
    //if (py_converter<types_t<N>>::is_convertible(ob, false))
    //return py_converter<types_t<N>>::py2c(ob);
    //else
    //return py2c_impl<N + 1>(ob);
    //}
    //template <int N> static std::enable_if_t<N == n_types, variant_t> py2c_impl(PyObject *ob) {
    //CPP2PY_RUNTIME_ERROR << "Internal error: py2c called for a Python object incompatible with std::variant";
    //}

    template <int N> static std::variant<T...> py2c_impl(PyObject *ob) {
      using conv = py_converter<std::tuple_element_t<N, std::tuple<T...>>>;
      if (conv::is_convertible(ob, false)) return conv::py2c(ob);
      if constexpr (N < sizeof...(T) - 1)
        return py2c_impl<N + 1>(ob);
      else  
        CPP2PY_RUNTIME_ERROR << "Internal error: py2c called for a Python object incompatible with std::variant";
    }

    struct _visitor {
      template <typename U> PyObject *operator()(U const &x) { return py_converter<U>::c2py(x); }
    };

    public:
    static PyObject *c2py(std::variant<T...> const &v) {
      //auto l = [](auto const &x) -> PyObject * { return py_converter<decltype(x)>::c2py(x); };
      return visit(_visitor{}, v);
      //return visit(_visitor{}, v);
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if ((...  or py_converter<T>::is_convertible(ob, false))) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::variant"); }
      return false;
    }

    static std::variant<T...> py2c(PyObject *ob) { return py2c_impl<0>(ob); }
  };

} // namespace cpp2py
