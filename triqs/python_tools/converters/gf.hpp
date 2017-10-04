#pragma once
#include "../wrapper_tools.hpp"
#include <triqs/gfs.hpp>
#include <triqs/utility/typeid_name.hpp>
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

    static pyref cls = pyref::get_class("pytriqs.gf", "Gf", true);
    if (cls.is_null()) return NULL;
    pyref m   = convert_to_python(g.mesh());
    if (m.is_null()) return NULL;
    pyref d   = convert_to_python(g.data());
    if (d.is_null()) return NULL;
    pyref s   = convert_to_python(g.singularity());
    if (s.is_null()) return NULL;
    pyref i   = convert_to_python(g.indices());
    if (i.is_null()) return NULL;

    pyref kw = PyDict_New();
    PyDict_SetItemString(kw, "mesh", m);
    PyDict_SetItemString(kw, "data", d);
    PyDict_SetItemString(kw, "singularity", s);
    PyDict_SetItemString(kw, "indices", i);

    PyDict_SetItemString(kw, "tail_valued", (triqs::gfs::is_tail_valued<T>::value ? Py_True: Py_False));
    pyref empty_tuple = PyTuple_New(0);
    return PyObject_Call(cls, empty_tuple, kw);
   }

   static void _set_err(PyObject * p, const char * X, std::string const & C) { 
     using namespace std::string_literals; 
     std::string err= "Cpp2py converter: Python to C++ :\n"s + 
                      "  ... Conversion of a gf from Python to C++ " + utility::typeid_name<triqs::gfs::gf_view<M, T>>() + 
    		       "\n  ... Cannot convert the "s + X + " of gf from Python type :  " + p->ob_type->tp_name + " to the C++ type " + C;
     PyErr_SetString(PyExc_TypeError, err.c_str() ); 
   }

   static bool is_convertible(PyObject* ob, bool raise_exception) {
    // first check it is a Gf
    if (not pyref::check_is_instance(ob, "pytriqs.gf", "Gf", raise_exception)) return false;
    pyref x = borrowed(ob);
    
    // check the mesh, data, sing
    pyref m = x.attr("_mesh");
   
    if (!py_converter<mesh_t>::is_convertible(m, false)) {
    	if (raise_exception) _set_err(m, "mesh", utility::typeid_name<mesh_t>()); 
        return false;
    }

    pyref d = x.attr("_data");
    if (!py_converter<data_t>::is_convertible(d, raise_exception)) {
    	if (raise_exception) _set_err(d, "data", utility::typeid_name<data_t>());
        return false;
    }
    
    pyref s = x.attr("_singularity");
    if (!py_converter<sing_t>::is_convertible(s, raise_exception)) {
    	if (raise_exception) _set_err(s, "singularity", utility::typeid_name<sing_t>());
        return false;
    }
    
    pyref i = x.attr("_indices");
    if (!py_converter<indices_t>::is_convertible(i, raise_exception)) {
    	if (raise_exception) _set_err(i, "indices", utility::typeid_name<indices_t>());
        return false;
    }

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
  
  // Converter of mesh_point
  template <typename M> struct py_converter<triqs::gfs::mesh_point<M>> {

   using c_type = triqs::gfs::mesh_point<M>;

   static PyObject* c2py(c_type const & p) {

    static pyref cls = pyref::get_class("pytriqs.gf", "MeshPoint", /* raise_exception */ true);
    if (cls.is_null()) return NULL;

    pyref val = convert_to_python(static_cast<typename c_type::cast_t>(p));
    if (val.is_null()) return NULL;

    //pyref idx   = convert_to_python(p.index());
    //if (idx.is_null()) return NULL;

    pyref lidx   = convert_to_python(p.linear_index());
    if (lidx.is_null()) return NULL;

    return PyObject_Call(cls, pyref::make_tuple(lidx, val), NULL);
   }
  };
 }
}
