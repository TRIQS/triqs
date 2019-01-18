#pragma once
#include <array>
#include <tuple>
#include <numeric>

namespace cpp2py {

  template <typename... Types> struct py_converter<std::tuple<Types...>> {

    private:
    using tuple_t = std::tuple<Types...>;

    // c2py implementation
    template <std::size_t... Is> static PyObject *c2py_impl(tuple_t const &t, std::index_sequence<Is...>) {
      auto objs        = std::array<pyref, sizeof...(Is)>{pyref(py_converter<Types>::c2py(std::get<Is>(t)))...};
      bool one_is_null = std::accumulate(std::begin(objs), std::end(objs), false, [](bool x, PyObject *a) { return x or (a == NULL); });
      if (one_is_null) return NULL;
      return PyTuple_Pack(sizeof...(Types), (PyObject *)(objs[Is])...);
    }

    // is_convertible implementation
    template <int N, typename T, typename... Tail> static bool is_convertible_impl(PyObject *seq, bool raise_exception) {
      return py_converter<T>::is_convertible(PySequence_Fast_GET_ITEM(seq, N), raise_exception)
         && is_convertible_impl<N + 1, Tail...>(seq, raise_exception);
    }
    template <int> static bool is_convertible_impl(PyObject *seq, bool raise_exception) { return true; }

    template <size_t... Is> static auto py2c_impl(std::index_sequence<Is...>, PyObject *seq) {
      return std::make_tuple(py_converter<Types>::py2c(PySequence_Fast_GET_ITEM(seq, Is))...);
    }

    public:
    // -----------------------------------------

    static PyObject *c2py(tuple_t const &t) { return c2py_impl(t, std::make_index_sequence<sizeof...(Types)>()); }

    // -----------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PySequence_Check(ob)) {
        pyref seq = PySequence_Fast(ob, "expected a sequence");
        // Sizes must match! Could we relax this condition to '<'?
        if (PySequence_Fast_GET_SIZE((PyObject *)seq) != std::tuple_size<tuple_t>::value) goto _false;
        if (!is_convertible_impl<0, Types...>((PyObject *)seq, raise_exception)) goto _false;
        return true;
      }
    _false:
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::tuple"); }
      return false;
    }

    // -----------------------------------------

    static tuple_t py2c(PyObject *ob) {
      pyref seq = PySequence_Fast(ob, "expected a sequence");
      return py2c_impl(std::make_index_sequence<sizeof...(Types)>(), (PyObject *)seq);
    }
  };
} // namespace cpp2py
