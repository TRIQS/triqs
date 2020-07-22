#pragma once
#include <Python.h>

namespace cpp2py {

  /** 
   * A class to own a reference PyObject *, with proper reference counting.
   */
  class pyref {
    PyObject *ob = NULL;

    public:
   
    /// Null
    pyref() = default;

    /// Takes ownership of the reference
    pyref(PyObject *new_ref) : ob(new_ref) {}

    /// Release the ref
    ~pyref() { Py_XDECREF(ob); }

    /// Copy constructor
    pyref(pyref const &p) {
      ob = p.ob;
      Py_XINCREF(ob);
    }

    /// Move constructor
    pyref(pyref &&p) {
      ob   = p.ob;
      p.ob = NULL;
    }

    /// No copy assign.
    pyref &operator=(pyref const &) = delete;

    /// Move assign
    pyref &operator=(pyref &&p) {
      Py_XDECREF(ob);
      ob   = p.ob;
      p.ob = NULL;
      return *this;
    }

    /// Returns a borrowed reference
    operator PyObject *() const { return ob; }

    /// Returns a new reference to the object
    PyObject *new_ref() const {
      Py_XINCREF(ob);
      return ob;
    }

    /// ref counting
    int refcnt() const { return (ob !=NULL ? ob->ob_refcnt : -100);}

    /// True iif the object is not NULL
    explicit operator bool() const { return (ob != NULL); }

    /// Is object NULL
    bool is_null() const { return ob == NULL; }

    /// Is it Py_None
    bool is_None() const { return ob == Py_None; }

    /// Returns the attribute of this. Null if error, or if is_null.
    pyref attr(const char *s) { return (ob ? PyObject_GetAttrString(ob, s) : NULL); } // NULL : pass the error in chain call x.attr().attr()....

    /// Call
    pyref operator()(PyObject *a1) {
      return (ob ? PyObject_CallFunctionObjArgs(ob, a1, NULL) : NULL);
    } // NULL : pass the error in chain call x.attr().attr()....

    ///  Call
    pyref operator()(PyObject *a1, PyObject *a2) {
      return (ob ? PyObject_CallFunctionObjArgs(ob, a1, a2, NULL) : NULL);
    } // NULL : pass the error in chain call x.attr().attr()....

    /// Import the module and returns a pyref to it
    static pyref module(std::string const &module_name) { return PyImport_ImportModule(module_name.c_str()); }

    /// Make a Python string from the C++ string
    static pyref string(std::string const &s) { return PyString_FromString(s.c_str()); }

    /// Make a Python Tuple from the C++ objects
    template<typename ... T>
    static pyref make_tuple(T const & ...x) { return PyTuple_Pack(sizeof...(T), static_cast<PyObject*>(x)...);}

    /// gets a reference to the class cls_name in module_name
    static pyref get_class(const char *module_name, const char *cls_name, bool raise_exception) {
      pyref cls = pyref::module(module_name).attr(cls_name);
      if (cls.is_null() && raise_exception) {
        std::string s = std::string{"Cannot find the class "} + module_name + "." + cls_name;
        PyErr_SetString(PyExc_TypeError, s.c_str());
      }
      return cls;
    }

    /// checks that ob is of type module_name.cls_name
    static bool check_is_instance(PyObject *ob, const char *module_name, const char *cls_name, bool raise_exception) {
      pyref cls = pyref::get_class(module_name, cls_name, raise_exception);
      int i     = PyObject_IsInstance(ob, cls);
      if (i == -1) { // an error has occurred
        i = 0;
        if (!raise_exception) PyErr_Clear();
      }
      if ((i == 0) && (raise_exception)) PyErr_SetString(PyExc_TypeError, "Type error : a Gf is expected in Python -> C++ convertion");
      return i;
    }
  };
  static_assert(sizeof(pyref) == sizeof(PyObject *), "pyref must contain only a PyObject *");

  // FIXME : put static or the other functions inline ?
  
  /// Returns a pyref from a borrowed ref
  inline pyref borrowed(PyObject *ob) {
    Py_XINCREF(ob);
    return {ob};
  }
} // namespace cpp2py
