#pragma once
#include <Python.h>

namespace triqs {
namespace py_tools {

 //--------------------- pyref -----------------------------

 // a little class to hold an owned ref, make sure it is decref at destruction
 class pyref {
  PyObject *ob = NULL;

  public:
  pyref() = default;
  pyref(PyObject *new_ref) : ob(new_ref) {}
  ~pyref() { Py_XDECREF(ob); }
  operator PyObject *() const { return ob; }
  PyObject *new_ref() const {
   Py_XINCREF(ob);
   return ob;
  }
  explicit operator bool() const { return (ob != NULL); }
  bool is_null() const { return ob == NULL; }
  bool is_None() const { return ob == Py_None; }

  pyref attr(const char *s) {
   return (ob ? PyObject_GetAttrString(ob, s) : NULL);
  } // NULL : pass the error in chain call x.attr().attr()....

  pyref operator()(PyObject *a1) {
   return (ob ? PyObject_CallFunctionObjArgs(ob, a1, NULL) : NULL);
  } // NULL : pass the error in chain call x.attr().attr()....

  pyref operator()(PyObject *a1, PyObject *a2) {
   return (ob ? PyObject_CallFunctionObjArgs(ob, a1, a2, NULL) : NULL);
  } // NULL : pass the error in chain call x.attr().attr()....

  pyref(pyref const &p) {
   ob = p.ob;
   Py_XINCREF(ob);
  }
  pyref(pyref &&p) {
   ob = p.ob;
   p.ob = NULL;
  }
  pyref &operator=(pyref const &) = delete;
  pyref &operator=(pyref &&p) {
   ob = p.ob;
   p.ob = NULL;
   return *this;
  }
  static pyref module(std::string const &module_name) { return PyImport_ImportModule(module_name.c_str()); }
  static pyref string(std::string const &s) { return PyString_FromString(s.c_str()); }
 };

 inline pyref borrowed(PyObject *ob) {
  Py_XINCREF(ob);
  return {ob};
 }
}
}

