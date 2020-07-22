#pragma once
#include "../traits.hpp"

namespace cpp2py {

 template <typename T> struct py_converter<std::optional<T>> {

   using conv = py_converter<std::decay_t<T>>;

   template <typename O> static PyObject *c2py(O &&op) {
     static_assert(is_instantiation_of_v<std::optional, std::decay_t<O>>, "Logic Error");
     if (!bool(op)) Py_RETURN_NONE;
     return conv::c2py(*(std::forward<O>(op)));
   }

  static bool is_convertible(PyObject *ob, bool raise_exception) {
   return ((ob == Py_None) or conv::is_convertible(ob, raise_exception));
  }

  static std::optional<T> py2c(PyObject *ob) {
   if (ob == Py_None) return {};
   return std::optional<T>{conv::py2c(ob)};
  }
 };
}

