#pragma once
#include <Python.h>
#include "structmember.h"
#include <string>
#include <complex>
#include <vector>
#include <triqs/utility/exceptions.hpp>
#include "./pyref.hpp"
#include <time.h>

#pragma clang diagnostic ignored "-Wdeprecated-writable-strings"
#pragma GCC diagnostic ignored "-Wwrite-strings"

inline char *get_current_time() { // helper function to print the time in the CATCH_AND_RETURN macro
 time_t rawtime;
 time(&rawtime);
 return ctime(&rawtime);
}

// I can use the trace in triqs::exception
#define CATCH_AND_RETURN(MESS,RET)\
 catch(triqs::keyboard_interrupt const & e) {\
 PyErr_SetString(PyExc_KeyboardInterrupt, e.what());\
 return RET; }\
 catch(triqs::exception const & e) {\
 auto err = std::string("Error occurred at ") + get_current_time() + "\nError " + MESS + "\nC++ error was : \n"  + e.what();\
 PyErr_SetString(PyExc_RuntimeError, err.c_str());\
 return RET; }\
 catch(std::exception const & e) {\
 auto err = std::string("Error occurred at ") + get_current_time() + "\nError " + MESS + "\nC++ error was : \n"  + e.what();\
 PyErr_SetString(PyExc_RuntimeError, err.c_str());\
 return RET; }\
 catch(...) { PyErr_SetString(PyExc_RuntimeError,"Unknown error " MESS ); return RET; }\

namespace triqs { namespace py_tools {

//---------------------  py_converters -----------------------------

// default version for a wrapped type. To be specialized later.
// py2c behaviour is undefined is is_convertible return false
template<typename T> struct py_converter;
 //{
 //  static PyObject * c2py(T const & x);
 //  static T & py2c(PyObject * ob);
 //  static bool is_convertible(PyObject * ob, bool raise_exception);
 //}

// We only use these functions in the code, not converter
// TODO : Does c2py return NULL in failure ? Or is it undefined...
template <typename T> static PyObject *convert_to_python(T &&x) {
 return py_converter<typename std::decay<T>::type>::c2py(std::forward<T>(x));
}
template<typename T> static auto convert_from_python(PyObject * ob) -> decltype(py_converter<T>::py2c(ob)) { return py_converter<T>::py2c(ob);}
template <typename T> static bool convertible_from_python(PyObject *ob, bool raise_exception) {
 return py_converter<T>::is_convertible(ob, raise_exception);
}

// details
template <bool B> struct _bool {};
template <typename T> struct _is_pointer : _bool<false> {};
template <typename T> struct _is_pointer<T *> : _bool<true> {};
template <> struct _is_pointer<PyObject  *> : _bool<false> {}; // yes, false, it is a special case...

// adapter needed for parsing with PyArg_ParseTupleAndKeywords later in the functions
template <typename T> static int converter_for_parser_(PyObject *ob, T *p, _bool<false>) {
 if (!py_converter<T>::is_convertible(ob, true)) return 0;
 *p = std::move(convert_from_python<T>(ob)); // non wrapped types are converted to values, they can be moved !
 return 1;
}
template <typename T> static int converter_for_parser_(PyObject *ob, T **p, _bool<true>) {
 if (!convertible_from_python<T>(ob)) return 0;
 *p = &(convert_from_python<T>(ob));
 return 1;
}
template <typename T> static int converter_for_parser(PyObject *ob, T *p) {
 return converter_for_parser_(ob, p, _is_pointer<T>());
}

// -----------------------------------
//    Tools for the implementation of reduce (V2)
// -----------------------------------

 // auxiliary object to reduce the object into a tuple
 class reductor {
  std::vector<PyObject *> elem;
  PyObject *as_tuple() {
   int l = elem.size();
   PyObject *tup = PyTuple_New(l);
   for (int pos = 0; pos < l; ++pos) PyTuple_SetItem(tup, pos, elem[pos]);
   return tup;
  }
  public:
  template <typename T> reductor & operator&(T &x) { elem.push_back(convert_to_python(x)); return *this;}
  template<typename T>
  PyObject * apply_to(T & x) { x.serialize(*this,0); return as_tuple();}
 };

 // inverse : auxiliary object to reconstruct the object from the tuple ...
 class reconstructor {
  PyObject * tup; // borrowed ref
  int pos=0, pos_max = 0;
  public:
  reconstructor(PyObject *borrowed_ref) : tup(borrowed_ref) { pos_max = PyTuple_Size(tup)-1;}
  template <typename T> reconstructor &operator&(T &x) {
   if (pos > pos_max) TRIQS_RUNTIME_ERROR << " Tuple too short in reconstruction";
   x = convert_from_python<T>(PyTuple_GetItem(tup, pos++));
   return *this;
  }
 };

 // no protection for convertion !
 template <typename T> struct py_converter_from_reductor {
 template<typename U> static PyObject *c2py(U && x) { return reductor{}.apply_to(x); }
 static T py2c(PyObject *ob) {
  T res;
  auto r = reconstructor{ob};
  res.serialize(r, 0);
  return res;
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) { return true;}
};

// -----------------------------------
//       basic types
// -----------------------------------

// PyObject *
template <> struct py_converter<PyObject *> {
 static PyObject *c2py(PyObject *ob) { return ob; }
 static PyObject *py2c(PyObject *ob) { return ob; }
 static bool is_convertible(PyObject *ob, bool raise_exception) { return true;}
};

// --- bool
template <> struct py_converter<bool> {
 static PyObject *c2py(bool b) {
  if (b)
   Py_RETURN_TRUE;
  else
   Py_RETURN_FALSE;
 }
 static bool py2c(PyObject *ob) { return ob == Py_True; }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyBool_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to bool");}
  return false;
 }
};

// --- long

template <> struct py_converter<long> {
 static PyObject *c2py(long i) { return PyInt_FromLong(i); }
 static long py2c(PyObject *ob) { return PyInt_AsLong(ob); }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyInt_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to long");}
  return false;
 }
};

template <> struct py_converter<int> : py_converter<long> {};
template <> struct py_converter<size_t> : py_converter<long> {};

// --- double

template <> struct py_converter<double> {
 static PyObject *c2py(double x) { return PyFloat_FromDouble(x); }
 static double py2c(PyObject *ob) { return PyFloat_AsDouble(ob); }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyFloat_Check(ob) || PyInt_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to double");}
  return false;
 }
};

// --- complex

template <> struct py_converter<std::complex<double>> {
 static PyObject *c2py(std::complex<double> x) { return PyComplex_FromDoubles(x.real(), x.imag()); }
 static std::complex<double> py2c(PyObject *ob) {
  if (PyComplex_Check(ob)) {
    auto r = PyComplex_AsCComplex(ob);
    return {r.real, r.imag};
  }
  return PyFloat_AsDouble(ob);
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (PyComplex_Check(ob) || PyFloat_Check(ob) || PyInt_Check(ob)) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to complex");}
  return false;
 }
};

}}
