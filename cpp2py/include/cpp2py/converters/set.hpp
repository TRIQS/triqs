#pragma once
#include <set>
#include "../traits.hpp"

namespace cpp2py {

  template <typename K> struct py_converter<std::set<K>> {

    template <typename S> static PyObject *c2py(S &&s) {
      static_assert(is_instantiation_of_v<std::set, std::decay_t<S>>, "Logic error");
      PyObject *set = PySet_New(NULL);
      for (auto &x : s) {
        pyref y;
        if constexpr (std::is_reference_v<S>) {
          y = convert_to_python(x);
        } else { // s passed as rvalue
          y = convert_to_python(std::move(x));
        }
        if (y.is_null() or (PySet_Add(set, y) == -1)) {
          Py_DECREF(set);
          return NULL;
        } // error
      }
      return set;
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (!PySet_Check(ob)) goto _false;
      {
        pyref keys_it = PyObject_GetIter(ob);
        pyref key;
        while (bool(key = PyIter_Next(keys_it))) {
          if (!py_converter<K>::is_convertible(key, raise_exception)) goto _false; //borrowed ref
        }
        return true;
      }
    _false:
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::set"); }
      return false;
    }

    static std::set<K> py2c(PyObject *ob) {
      std::set<K> res;
      pyref keys_it = PyObject_GetIter(ob);
      pyref key;
      while (bool(key = PyIter_Next(keys_it))) {
        res.insert(py_converter<K>::py2c(key)); //borrowed ref
      }
      return res;
    }
  };
} // namespace cpp2py
