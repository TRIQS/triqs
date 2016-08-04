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
template<> struct py_converter<triqs::gfs::gf_indices> {
 using v_t = std::vector<std::string>;
 using vv_t = std::vector<v_t>;
 using vvi_t = std::vector<std::vector<int>>;

 static PyObject *c2py(triqs::gfs::gf_indices const & ind) {
  return py_converter<vv_t>::c2py(ind.data());
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (py_converter<vv_t>::is_convertible(ob, false) ||
      py_converter<vvi_t>::is_convertible(ob, false)) return true;
  if (raise_exception) {
    PyErr_SetString(PyExc_TypeError, "Cannot convert to triqs::gfs::gf_indices_one");
  }
  return false;
 }
 static triqs::gfs::gf_indices py2c(PyObject *ob) {
  if (py_converter<vv_t>::is_convertible(ob, false)) {
   return py_converter<vv_t>::py2c(ob);
  }
  if (py_converter<vvi_t>::is_convertible(ob, false)) {
   auto vec_vec_int = py_converter<vvi_t>::py2c(ob);
   vv_t res;
   for (auto const & x: vec_vec_int) {
     v_t vs;
     for (int i: x) vs.push_back(std::to_string(i));
     res.push_back(vs);
   }
   return {res};
  }
  TRIQS_RUNTIME_ERROR << "Internal error: py2c called for a Python object incompatible with gf_indices";
  return {};
 }
};

/*// indices
template<> struct py_converter<triqs::gfs::gf_indices_one> {
 static PyObject *c2py(triqs::gfs::gf_indices_one indices) {
  return py_converter<std::vector<std::string>>::c2py(indices.ind);
 }
 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (py_converter<std::vector<std::string>>::is_convertible(ob, false) ||
      py_converter<std::vector<int>>::is_convertible(ob, false)) return true;
  if (raise_exception) {
    PyErr_SetString(PyExc_TypeError, "Cannot convert to triqs::gfs::gf_indices_one");
  }
  return false;
 }
 static triqs::gfs::gf_indices_one py2c(PyObject *ob) {
  if (py_converter<std::vector<std::string>>::is_convertible(ob, false)) {
   return py_converter<std::vector<std::string>>::py2c(ob);
  }
  if (py_converter<std::vector<int>>::is_convertible(ob, false)) {
   auto vec_int = py_converter<std::vector<int>>::py2c(ob);
   std::vector<std::string> vec_string;
   for (int i: vec_int) vec_string.push_back(std::to_string(i));
   return vec_string;
  }
  TRIQS_RUNTIME_ERROR << "Internal error: py2c called for a Python object incompatible with gf_indices_one";
  return {};
 }
};

template<int N> struct py_converter<triqs::gfs::gf_indices_tuple<N>> : py_converter_from_reductor<triqs::gfs::gf_indices_tuple<N>>{};
*/

// domains
template<bool B> struct py_converter<triqs::gfs::matsubara_domain<B>> : py_converter_from_reductor<triqs::gfs::matsubara_domain<B>>{};
template<> struct py_converter<triqs::gfs::R_domain> : py_converter_from_reductor<triqs::gfs::R_domain>{};

// Converter for Block gf
template <typename... T> struct py_converter<triqs::gfs::block_gf_view<T...>> {

 using gf_type = triqs::gfs::gf<T...>;
 using gf_view_type = triqs::gfs::gf_view<T...>;
 using c_type = triqs::gfs::block_gf_view<T...>;

 static PyObject *c2py(c_type g) {
  // rm the view_proxy
  std::vector<gf_view_type> vg;
  vg.reserve(g.data().size());
  for (auto const & x : g.data()) vg.push_back(x);
  pyref v_gf = convert_to_python(vg);
  pyref v_names = convert_to_python(g.block_names());
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

  pyref x = borrowed(ob);
  pyref gfs = x.attr("_BlockGf__GFlist");
  return (i && py_converter<std::vector<gf_view_type>>::is_convertible(gfs, raise_exception));

 }

 static c_type py2c(PyObject *ob) {
  pyref x = borrowed(ob);
  pyref names = x.attr("_BlockGf__indices");
  pyref gfs = x.attr("_BlockGf__GFlist");
  return make_block_gf_view(convert_from_python<std::vector<std::string>>(names),
                            convert_from_python<std::vector<gf_view_type>>(gfs));
 }
};

// Treat the block_gf as view
template <typename... T> struct py_converter<triqs::gfs::block_gf<T...>> :
py_converter<triqs::gfs::block_gf_view<T...>> {};

// Converter for Block gf
template <typename... T> struct py_converter<triqs::gfs::block2_gf_view<T...>> {

 //using gf_type = triqs::gfs::gf<T...>;
 using gf_view_type = triqs::gfs::gf_view<T...>;
 using c_type = triqs::gfs::block2_gf_view<T...>;

 static PyObject *c2py(c_type g) {
  std::vector<std::vector<gf_view_type>> vvg;
  vvg.reserve(g.data().size());
  for (auto const & x : g.data()) {
   std::vector<gf_view_type> vg;
   vg.reserve(x.size());
   for(auto const & y : x) vg.push_back(y);
   vvg.push_back(vg);
  }
  pyref v_gf = convert_to_python(vvg);
  pyref v_names1 = convert_to_python(g.block_names()[0]);
  pyref v_names2 = convert_to_python(g.block_names()[1]);
  pyref cls = pyref::module("pytriqs.gf.local").attr("Block2Gf");
  if (cls.is_null()) TRIQS_RUNTIME_ERROR << "Cannot find the pytriqs.gf.local.Block2Gf";
  pyref args = PyTuple_Pack(3, (PyObject *)v_names1, (PyObject *)v_names2, (PyObject *)v_gf);
  return PyObject_Call(cls, args, NULL);
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  pyref cls = pyref::module("pytriqs.gf.local").attr("Block2Gf");
  if (cls.is_null()) TRIQS_RUNTIME_ERROR << "Cannot find the pytriqs.gf.local.Block2Gf";
  int i = PyObject_IsInstance(ob, cls);
  if (i == -1) { // an error has occurred
   i = 0;
   if (!raise_exception) PyErr_Clear();
  }
  if ((i == 0) && (raise_exception)) PyErr_SetString(PyExc_TypeError, "The object is not a Block2Gf");

  pyref x = borrowed(ob);
  pyref gfs = x.attr("_Block2Gf__GFlist");
  return (i && py_converter<std::vector<std::vector<gf_view_type>>>::is_convertible(gfs, raise_exception));

 }

 static c_type py2c(PyObject *ob) {
  pyref x = borrowed(ob);
  pyref names1 = x.attr("_Block2Gf__indices1");
  pyref names2 = x.attr("_Block2Gf__indices2");
  pyref gfs = x.attr("_Block2Gf__GFlist");
  return make_block2_gf_view(convert_from_python<std::vector<std::string>>(names1),
                             convert_from_python<std::vector<std::string>>(names2),
                             convert_from_python<std::vector<std::vector<gf_view_type>>>(gfs));
 }
};

// Treat the block2_gf as view
template <typename... T> struct py_converter<triqs::gfs::block2_gf<T...>> : py_converter<triqs::gfs::block2_gf_view<T...>> {};

// Converter for scalar_valued gf : reinterpreted as 1x1 matrix
template <typename Variable> struct py_converter<triqs::gfs::gf_view<Variable, triqs::gfs::scalar_valued>>{

 using conv = py_converter<triqs::gfs::gf_view<Variable, triqs::gfs::matrix_valued>>;
 using c_t = triqs::gfs::gf_view<Variable, triqs::gfs::scalar_valued>;

 static PyObject *c2py(c_t g) {
  return conv::c2py(reinterpret_scalar_valued_gf_as_matrix_valued(g));
 }

 static bool is_convertible(PyObject *ob, bool raise_exception) {
  if (!conv::is_convertible(ob,raise_exception)) return false;
  auto g = conv::py2c(ob); // matrix view
  if (g.target_shape() == triqs::arrays::make_shape(1,1)) return true;
  if (raise_exception) PyErr_SetString(PyExc_RuntimeError,"The green function is not of dimension 1x1 : cannot be reinterpreted as a scalar_valued Green function");
  return false;
 }

 static c_t py2c(PyObject *ob) {
  return gfs::slice_target_to_scalar(conv::py2c(ob),0,0); // qualif gfs:: necessary for gcc 5
 }
};

}}

