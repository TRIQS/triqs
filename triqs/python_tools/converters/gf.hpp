#pragma once
#include "../wrapper_tools.hpp"
#include <triqs/gfs.hpp>

/// Additional converters for gf : block gf, and nothing
//
namespace triqs {
namespace py_tools {

 // nothing
template<> struct py_converter<triqs::gfs::nothing> {
 static PyObject *c2py(triqs::gfs::nothing g) { Py_RETURN_NONE;}
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (ob ==Py_None) return true;
  if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to triqs::gfs::nothing : can only convert None");}
  return false;
 }
 static triqs::gfs::nothing py2c(PyObject *ob) { return {}; }
};

// indices
template<> struct py_converter<triqs::gfs::indices_2> : py_converter_from_reductor<triqs::gfs::indices_2>{};
template<bool B> struct py_converter<triqs::gfs::matsubara_domain<B>> : py_converter_from_reductor<triqs::gfs::matsubara_domain<B>>{};
template<> struct py_converter<triqs::gfs::R_domain> : py_converter_from_reductor<triqs::gfs::R_domain>{};

// Converter for Block gf
template <typename... T> struct py_converter<triqs::gfs::gf_view<triqs::gfs::block_index, triqs::gfs::gf<T...>>> {

 using gf_type = triqs::gfs::gf<T...>;
 using gf_view_type = triqs::gfs::gf_view<T...>;
 using c_type = triqs::gfs::gf_view<triqs::gfs::block_index, gf_type>;

 static PyObject *c2py(c_type g) {
  // rm the view_proxy
  std::vector<gf_view_type> vg;
  vg.reserve(g.data().size());
  for (auto const & x : g.data()) vg.push_back(x);
  pyref v_gf = convert_to_python(vg);
  pyref v_names = convert_to_python(g.mesh().domain().names());
  pyref cls = pyref::module("pytriqs.gf.local").attr("BlockGf");
  if (cls.is_null()) TRIQS_RUNTIME_ERROR <<"Cannot find the pytriqs.gf.local.BlockGf";
  pyref kw = PyDict_New();
  PyDict_SetItemString(kw, "name_list", v_names);
  PyDict_SetItemString(kw, "block_list", v_gf);
  pyref empty_tuple = PyTuple_New(0);
  return PyObject_Call(cls, empty_tuple, kw);
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  pyref cls = pyref::module("pytriqs.gf.local").attr("BlockGf");
  if (cls.is_null()) TRIQS_RUNTIME_ERROR <<"Cannot find the pytriqs.gf.local.BlockGf";
  int i = PyObject_IsInstance(ob, cls);
  if (i == -1) { // an error has occurred
   i = 0;
   if (!raise_exception) PyErr_Clear();
  }
  if ((i == 0) && (raise_exception)) PyErr_SetString(PyExc_TypeError, "The object is not a BlockGf");
  return i;
 }

 static c_type py2c(PyObject *ob) {
  pyref x = borrowed(ob);
  pyref names = x.attr("_BlockGf__indices");
  pyref gfs = x.attr("_BlockGf__GFlist");
  return make_block_gf_view_from_vector(convert_from_python<std::vector<std::string>>(names),
                                        convert_from_python<std::vector<gf_view_type>>(gfs));
 }
};

// Treat the block_gf as view
template <typename... T> struct py_converter<triqs::gfs::gf<triqs::gfs::block_index, triqs::gfs::gf<T...>>> :
py_converter<triqs::gfs::gf_view<triqs::gfs::block_index, triqs::gfs::gf<T...>>> {};

// Converter for scalar_valued gf : reinterpreted as 1x1 matrix
template <typename Variable, typename Opt> struct py_converter<triqs::gfs::gf_view<Variable, triqs::gfs::scalar_valued, Opt>>{

 using conv = py_converter<triqs::gfs::gf_view<Variable, triqs::gfs::matrix_valued, Opt>>;
 using c_t = triqs::gfs::gf_view<Variable, triqs::gfs::scalar_valued, Opt>;

 static PyObject *c2py(c_t g) {
  return conv::c2py(reinterpret_scalar_valued_gf_as_matrix_valued(g));
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (!conv::is_convertible(ob,raise_exception)) return false;
  auto g = conv::py2c(ob); // matrix view
  if (get_target_shape(g) == triqs::arrays::make_shape(1,1)) return true;
  if (raise_exception) PyErr_SetString(PyExc_RuntimeError,"The green function is not of dimension 1x1 : cannot be reinterpreted as a scalar_valued Green function");
  return false;
 }

 static c_t py2c(PyObject *ob) {
  return slice_target_to_scalar(conv::py2c(ob),0,0);
 }
};

}}

