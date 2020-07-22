#pragma once
#include <map>
#include "../traits.hpp"

namespace cpp2py {

  template <typename K, typename V> struct py_converter<std::map<K, V>> {

    template <typename M> static PyObject *c2py(M &&m) {
      static_assert(is_instantiation_of_v<std::map, std::decay_t<M>>, "Logic Error");

      PyObject *d = PyDict_New();
      for (auto &[key, val] : m) {
        pyref k, v;
        if constexpr (std::is_reference_v<M>) {
          k = convert_to_python(key);
          v = convert_to_python(val);
        } else { // Map passed as rvalue
          k = convert_to_python(std::move(key));
          v = convert_to_python(std::move(val));
        }

        // if the K is a list, we transform into a tuple
        if (PyList_Check(k)) k = PyList_AsTuple(k);

        if (k.is_null() or v.is_null() or (PyDict_SetItem(d, k, v) == -1)) {
          Py_DECREF(d);
          return NULL;
        } // error
      }
      return d;
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (!PyDict_Check(ob)) goto _false;
      {
        pyref keys   = PyDict_Keys(ob);
        pyref values = PyDict_Values(ob);
        int len      = PyDict_Size(ob);
        for (int i = 0; i < len; i++) {
          if (!py_converter<K>::is_convertible(PyList_GET_ITEM((PyObject *)keys, i), raise_exception)) goto _false;   //borrowed ref
          if (!py_converter<V>::is_convertible(PyList_GET_ITEM((PyObject *)values, i), raise_exception)) goto _false; //borrowed ref
        }
        return true;
      }
    _false:
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to std::map"); }
      return false;
    }

    // ----------------------------------------------

    static std::map<K, V> py2c(PyObject *ob) {
      pyref keys   = PyDict_Keys(ob);
      pyref values = PyDict_Values(ob);
      std::map<K, V> res;
      int len = PyDict_Size(ob);
      for (int i = 0; i < len; i++)
        res.emplace(py_converter<K>::py2c(PyList_GET_ITEM((PyObject *)keys, i)),    //borrowed ref
                    py_converter<V>::py2c(PyList_GET_ITEM((PyObject *)values, i))); //borrowed ref
      return res;
    }
  };
} // namespace cpp2py
