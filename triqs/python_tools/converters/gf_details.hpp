#pragma once
#include "../wrapper_tools.hpp"
#include "./vector.hpp"
#include <triqs/gfs.hpp>

// converters of small objects needed for gf
namespace triqs {
 namespace py_tools {

  // domains
  template <bool B>
  struct py_converter<triqs::gfs::matsubara_domain<B>> : py_converter_from_reductor<triqs::gfs::matsubara_domain<B>> {};
  template <> struct py_converter<triqs::gfs::R_domain> : py_converter_from_reductor<triqs::gfs::R_domain> {};

  // nothing
  template <> struct py_converter<triqs::gfs::nothing> {
   static PyObject* c2py(triqs::gfs::nothing g) { Py_RETURN_NONE; }
   static bool is_convertible(PyObject* ob, bool raise_exception) {
    if (ob == Py_None) return true;
    if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to triqs::gfs::nothing : can only convert None"); }
    return false;
   }
   static triqs::gfs::nothing py2c(PyObject* ob) { return {}; }
  };

 }
}
