#pragma once
#include <array>
#include <tuple>
#include <numeric>

#include "../traits.hpp"

namespace cpp2py {

  template <typename... Types> struct py_converter<std::tuple<Types...>> {

    private:
    using tuple_t = std::tuple<Types...>;

    // c2py implementation
    template <typename T, auto... Is> static PyObject *c2py_impl(T &&t, std::index_sequence<Is...>) {
      auto objs        = std::array<pyref, sizeof...(Is)>{convert_to_python(std::get<Is>(std::forward<T>(t)))...};
      bool one_is_null = std::accumulate(std::begin(objs), std::end(objs), false, [](bool x, PyObject *a) { return x or (a == NULL); });
      if (one_is_null) return NULL;
      return PyTuple_Pack(sizeof...(Types), (PyObject *)(objs[Is])...);
    }

    // Helper function needed due to clang v6 parameter pack issue
    static auto py_seq_fast_get_item(PyObject *seq, Py_ssize_t i) {
      return PySequence_Fast_GET_ITEM(seq, i);
    }

    // is_convertible implementation
    template <auto... Is> static bool is_convertible_impl(PyObject *seq, bool raise_exception, std::index_sequence<Is...>) {
      return (py_converter<std::decay_t<Types>>::is_convertible(py_seq_fast_get_item(seq, Is), raise_exception) and ...);
    }

    template <auto... Is> static auto py2c_impl(PyObject *seq, std::index_sequence<Is...>) {
      return std::make_tuple(py_converter<std::decay_t<Types>>::py2c(py_seq_fast_get_item(seq, Is))...);
    }

    public:
    // -----------------------------------------

    template <typename T> static PyObject *c2py(T &&t) {
      static_assert(is_instantiation_of_v<std::tuple, std::decay_t<T>>, "Logic Error");
      return c2py_impl(std::forward<T>(t), std::make_index_sequence<sizeof...(Types)>());
    }

    // -----------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (PySequence_Check(ob)) {
        pyref seq = PySequence_Fast(ob, "expected a sequence");
        // Sizes must match! Could we relax this condition to '<'?
        if (PySequence_Fast_GET_SIZE((PyObject *)seq) != std::tuple_size<tuple_t>::value) goto _false;
        if (!is_convertible_impl((PyObject *)seq, raise_exception, std::make_index_sequence<sizeof...(Types)>())) goto _false;
        return true;
      }
    _false:
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::tuple"); }
      return false;
    }

    // -----------------------------------------

    static tuple_t py2c(PyObject *ob) {
      pyref seq = PySequence_Fast(ob, "expected a sequence");
      return py2c_impl((PyObject *)seq, std::make_index_sequence<sizeof...(Types)>());
    }
  };
} // namespace cpp2py
