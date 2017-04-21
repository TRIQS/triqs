#pragma once
#include "../wrapper_tools.hpp"
#include "./tuple.hpp"
#include <triqs/gfs.hpp>

// converter for MeshProduct
namespace triqs {
 namespace py_tools {

  template <typename... Ms> struct py_converter<triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<Ms...>>> {

   using c_type      = triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<Ms...>>;
   using mtuple_conv = py_converter<typename c_type::m_tuple_t>; // the tuple of meshes

   static PyObject* c2py(c_type m) {
    pyref cls     = pyref::get_class("pytriqs.gf", "MeshProduct", true);
    if (cls.is_null()) return NULL;
    pyref m_tuple = mtuple_conv::c2py(m.components()); // take the C++ tuple of meshes and make the corresponding Python tuple
    if (m_tuple.is_null()) return NULL;
    return PyObject_Call(cls, m_tuple, NULL);
   }

   static bool is_convertible(PyObject* ob, bool raise_exception) {
    pyref x  = borrowed(ob);
    pyref ml = x.attr("_mlist");
    return mtuple_conv::is_convertible(ml, raise_exception);
   }

   static c_type py2c(PyObject* ob) {
    pyref x  = borrowed(ob);
    pyref ml = x.attr("_mlist");
    return triqs::tuple::apply_construct<c_type>(mtuple_conv::py2c(ml));
   }
  };
 }
}
