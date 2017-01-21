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
   Py_XDECREF(ob);
   ob = p.ob;
   p.ob = NULL;
   return *this;
  }
  static pyref module(std::string const &module_name) { return PyImport_ImportModule(module_name.c_str()); }
  static pyref string(std::string const &s) { return PyString_FromString(s.c_str()); }

  // gets a reference to the class cls_name in module_name
  static pyref get_class(const char * module_name, const char* cls_name, bool raise_exception) {
    pyref cls = pyref::module(module_name).attr(cls_name);
    if (cls.is_null() && raise_exception) {
       std::string s= std::string {"Cannot find the class "} + module_name + "."+ cls_name;
       PyErr_SetString(PyExc_TypeError, s.c_str());
    }
    return cls;
  }

  // checks that ob is of type module_name.cls_name
  static bool check_is_instance(PyObject* ob, const char* module_name, const char* cls_name, bool raise_exception) {
   pyref cls = pyref::get_class(module_name, cls_name, raise_exception);
   int i = PyObject_IsInstance(ob, cls);
   if (i == -1) { // an error has occurred
    i = 0;
    if (!raise_exception) PyErr_Clear();
   }
   if ((i == 0) && (raise_exception))
    PyErr_SetString(PyExc_TypeError, "Type error : a Gf is expected in Python -> C++ convertion");
   return i;
  }
 };

 inline pyref borrowed(PyObject *ob) {
  Py_XINCREF(ob);
  return {ob};
 }
}
}

