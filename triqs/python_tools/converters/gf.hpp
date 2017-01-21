#pragma once
#include "../wrapper_tools.hpp"
#include <triqs/gfs.hpp>
#include "./gf_details.hpp"
#include "./mesh_product.hpp"

// IS IS Necessary ?
#include <triqs/py_converters/meshes.hpp>
#include <triqs/py_converters/singularities.hpp>

/// Additional converters for gf
//
namespace triqs {
 namespace py_tools {

  // Treat the gf as view
  template <typename M, typename T> struct py_converter<triqs::gfs::gf<M, T>> : py_converter<triqs::gfs::gf_view<M, T>> {};

  // Converter for gf one var
  template <typename M, typename T> struct py_converter<triqs::gfs::gf_view<M, T>> {

   using c_type = triqs::gfs::gf_view<M, T>;
   using mesh_t = typename c_type::mesh_t;
   using data_t = typename c_type::data_t;
   using sing_t = typename c_type::singularity_t;
   using indices_t  = typename c_type::indices_t;

   static PyObject* c2py(c_type g) {
    pyref cls = pyref::get_class("pytriqs.gf", "Gf", true);
    pyref m   = convert_to_python(g.mesh());
    pyref d   = convert_to_python(g.data());
    pyref s   = convert_to_python(g.singularity());
    pyref i   = convert_to_python(g.indices());

    pyref kw = PyDict_New();
    PyDict_SetItemString(kw, "mesh", m);
    PyDict_SetItemString(kw, "data", d);
    PyDict_SetItemString(kw, "singularity", s);
    PyDict_SetItemString(kw, "indices", i);
    pyref empty_tuple = PyTuple_New(0);
    return PyObject_Call(cls, empty_tuple, kw);
   }

   static bool is_convertible(PyObject* ob, bool raise_exception) {
    // first check it is a Gf
    if (not pyref::check_is_instance(ob, "pytriqs.gf", "Gf", raise_exception)) return false;
    pyref x = borrowed(ob);
    
    // check the mesh, data, sing
    pyref m = x.attr("_mesh");
    if (!py_converter<mesh_t>::is_convertible(m, raise_exception)) return false;
    
    pyref d = x.attr("_data");
    if (!py_converter<data_t>::is_convertible(d, raise_exception)) return false;
    
    pyref s = x.attr("_singularity");
    if (!py_converter<sing_t>::is_convertible(s, raise_exception)) return false;
    
    pyref i = x.attr("_indices");
    if (!py_converter<indices_t>::is_convertible(i, raise_exception)) return false;

    /*  TRIQS_DEBUG(py_converter<mesh_t>::is_convertible(m, raise_exception) );*/
    // TRIQS_DEBUG(py_converter<data_t>::is_convertible(m, raise_exception) );
    // TRIQS_DEBUG(py_converter<sing_t>::is_convertible(m, raise_exception) );

    return true;
   }

   static c_type py2c(PyObject* ob) {
    pyref x = borrowed(ob);
    pyref m = x.attr("_mesh");
    pyref d = x.attr("_data");
    pyref s = x.attr("_singularity");
    pyref i = x.attr("_indices");
    return c_type{convert_from_python<mesh_t>(m), convert_from_python<data_t>(d), convert_from_python<sing_t>(s),
                  convert_from_python<indices_t>(i)};
   }
  };
 }
}
