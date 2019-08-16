#pragma once
#include <triqs/utility/typeid_name.hpp>
#include <triqs/cpp2py_converters/arrays.hpp>

#include <cpp2py/converters/vector.hpp>
#include <cpp2py/converters/tuple.hpp>

/// Additional converters for gf
namespace cpp2py {

  // -----------------------------------
  //    is_view
  //  One MUST declare to cpp2py that a type is a view.
  // -----------------------------------

  template <typename M, typename T> struct is_view<triqs::gfs::gf_view<M, T>> : std::true_type {};
  template <typename... T> struct is_view<triqs::gfs::block_gf_view<T...>> : std::true_type {};
  template <typename... T> struct is_view<triqs::gfs::block2_gf_view<T...>> : std::true_type {};

  template <typename M, typename T> struct is_view<triqs::gfs::gf_const_view<M, T>> : std::true_type {};
  template <typename... T> struct is_view<triqs::gfs::block_gf_const_view<T...>> : std::true_type {};
  template <typename... T> struct is_view<triqs::gfs::block2_gf_const_view<T...>> : std::true_type {};

  // -----------------------------------
  // domains
  // -----------------------------------

  template <bool B> struct py_converter<triqs::mesh::matsubara_domain<B>> : py_converter_from_reductor<triqs::mesh::matsubara_domain<B>> {};
  template <> struct py_converter<triqs::mesh::R_domain> : py_converter_from_reductor<triqs::mesh::R_domain> {};

  // -----------------------------------
  // all_t mapped to all
  // -----------------------------------

  template <> struct py_converter<triqs::mesh::all_t> {

    static PyObject *c2py(triqs::mesh::all_t m) {
      static pyref all = pyref::get_class("__builtin__", "all", true);
      if (all.is_null()) return NULL;
      return all.new_ref();
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      static pyref all = pyref::get_class("__builtin__", "all", true);
      return (all == ob);
    }

    static triqs::mesh::all_t py2c(PyObject *ob) { return {}; }
  };

  // -----------------------------------
  //     Mesh Product
  // -----------------------------------

  template <typename... Ms> struct py_converter<triqs::mesh::cartesian_product<Ms...>> {

    using c_type      = triqs::mesh::cartesian_product<Ms...>;
    using mtuple_conv = py_converter<typename c_type::m_tuple_t>; // the tuple of meshes

    static PyObject *c2py(c_type m) {
      static pyref cls = pyref::get_class("pytriqs.gf", "MeshProduct", true);
      if (cls.is_null()) return NULL;
      pyref m_tuple = mtuple_conv::c2py(m.components()); // take the C++ tuple of meshes and make the corresponding Python tuple
      if (m_tuple.is_null()) return NULL;
      return PyObject_Call(cls, m_tuple, NULL);
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref x  = borrowed(ob);
      pyref ml = x.attr("_mlist");
      return mtuple_conv::is_convertible(ml, raise_exception);
    }

    static c_type py2c(PyObject *ob) {
      pyref x  = borrowed(ob);
      pyref ml = x.attr("_mlist");
      return triqs::tuple::apply_construct<c_type>(mtuple_conv::py2c(ml));
    }
  };

  // Converter of mesh_point
  template <typename M> struct py_converter<triqs::mesh::mesh_point<M>> {

    using c_type = triqs::mesh::mesh_point<M>;

    static PyObject *c2py(c_type const &p) {

      static pyref cls = pyref::get_class("pytriqs.gf", "MeshPoint", /* raise_exception */ true);
      if (cls.is_null()) return NULL;

      pyref val = convert_to_python(static_cast<typename c_type::cast_t>(p));
      if (val.is_null()) return NULL;

      //pyref idx   = convert_to_python(p.index());
      //if (idx.is_null()) return NULL;

      pyref lidx = convert_to_python(p.linear_index());
      if (lidx.is_null()) return NULL;

      return PyObject_Call(cls, pyref::make_tuple(lidx, val), NULL);
    }
  };

  // -----------------------------------
  //    regular types
  // -----------------------------------

  // when converting gf Py->C, we return a gf_view. That is ok, we can construct a gf from it ...
  /*
 template <typename M, typename T> struct py_converter<triqs::gfs::gf<M, T>> {
    using conv_t = py_converter<triqs::gfs::gf_view<M, T>>;
    using c_type = triqs::gfs::gf<M, T>;

    static PyObject *c2py(triqs::gfs::gf_view<M, T>> g) { return conv_t::c2py(g);}
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob,raise_exception);}
    static c_type py2c(PyObject *ob) { return conv_t::py2c(ob);}
   };
*/

  template <typename M, typename T> struct py_converter<triqs::gfs::gf<M, T>> : py_converter<triqs::gfs::gf_view<M, T>> {};
  template <typename... T> struct py_converter<triqs::gfs::block_gf<T...>> : py_converter<triqs::gfs::block_gf_view<T...>> {};
  template <typename... T> struct py_converter<triqs::gfs::block2_gf<T...>> : py_converter<triqs::gfs::block2_gf_view<T...>> {};

  // -----------------------------------
  //   Const view
  // -----------------------------------
  template <typename M, typename T> struct py_converter<triqs::gfs::gf_const_view<M, T>> {
    using conv_t = py_converter<triqs::gfs::gf_view<M, T>>;
    using c_type = triqs::gfs::gf_const_view<M, T>;

    static PyObject *c2py(c_type g) = delete; // You can not convert a C++ const_view to a Python Gf ! Violates const correctness.
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_type py2c(PyObject *ob) { return conv_t::py2c(ob); }
  };

  template <typename M, typename T> struct py_converter<triqs::gfs::block_gf_const_view<M, T>> {
    using conv_t = py_converter<triqs::gfs::block_gf_view<M, T>>;
    using c_type = triqs::gfs::block_gf_const_view<M, T>;

    static PyObject *c2py(c_type g) = delete; // You can not convert a C++ const_view to a Python Gf ! Violates const correctness.
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_type py2c(PyObject *ob) { return conv_t::py2c(ob); }
  };

  template <typename M, typename T> struct py_converter<triqs::gfs::block2_gf_const_view<M, T>> {
    using conv_t = py_converter<triqs::gfs::block2_gf_view<M, T>>;
    using c_type = triqs::gfs::block2_gf_const_view<M, T>;

    static PyObject *c2py(c_type g) = delete; // You can not convert a C++ const_view to a Python Gf ! Violates const correctness.
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_type py2c(PyObject *ob) { return conv_t::py2c(ob); }
  };

  // -----------------------------------
  //    gf_view
  // -----------------------------------

  // Converter for gf one var
  template <typename M, typename T> struct py_converter<triqs::gfs::gf_view<M, T>> {

    using c_type    = triqs::gfs::gf_view<M, T>;
    using mesh_t    = typename c_type::mesh_t;
    using data_t    = typename c_type::data_t;
    using indices_t = typename c_type::indices_t;

    static PyObject *c2py(c_type g) {

      static pyref cls = pyref::get_class("pytriqs.gf", "Gf", true);
      if (cls.is_null()) return NULL;
      pyref m = convert_to_python(g.mesh());
      if (m.is_null()) return NULL;
      pyref d = convert_to_python(g.data());
      if (d.is_null()) return NULL;
      pyref i = convert_to_python(g.indices());
      if (i.is_null()) return NULL;

      pyref kw = PyDict_New();
      PyDict_SetItemString(kw, "mesh", m);
      PyDict_SetItemString(kw, "data", d);
      PyDict_SetItemString(kw, "indices", i);

      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    // ----------------------------------------------

    static void _set_err(PyObject *p, const char *X, std::string const &C) {
      using namespace std::string_literals;
      std::string err = "Cpp2py converter: Python to C++ :\n"s + "  ... Conversion of a gf from Python to C++ "
         + triqs::utility::typeid_name<triqs::gfs::gf_view<M, T>>() + "\n  ... Cannot convert the "s + X
         + " of gf from Python type :  " + p->ob_type->tp_name + " to the C++ type " + C;
      PyErr_SetString(PyExc_TypeError, err.c_str());
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      // first check it is a Gf
      if (not pyref::check_is_instance(ob, "pytriqs.gf", "Gf", raise_exception)) return false;
      pyref x = borrowed(ob);

      // check the mesh, data, sing
      pyref m = x.attr("_mesh");

      if (!py_converter<mesh_t>::is_convertible(m, false)) {
        if (raise_exception) _set_err(m, "mesh", triqs::utility::typeid_name<mesh_t>());
        return false;
      }

      pyref d = x.attr("_data");
      if (!py_converter<data_t>::is_convertible(d, raise_exception)) {
        if (raise_exception) _set_err(d, "data", triqs::utility::typeid_name<data_t>());
        return false;
      }

      pyref i = x.attr("_indices");
      if (!py_converter<indices_t>::is_convertible(i, raise_exception)) {
        if (raise_exception) _set_err(i, "indices", triqs::utility::typeid_name<indices_t>());
        return false;
      }

      return true;
    }

    // ----------------------------------------------

    static c_type py2c(PyObject *ob) {
      pyref x = borrowed(ob);
      pyref m = x.attr("_mesh");
      pyref d = x.attr("_data");
      pyref i = x.attr("_indices");
      return c_type{convert_from_python<mesh_t>(m), convert_from_python<data_t>(d), convert_from_python<indices_t>(i)};
    }
  };

  // -----------------------------------
  //    block_gf_view
  // -----------------------------------

  template <typename... T> struct py_converter<triqs::gfs::block_gf_view<T...>> {

    using gf_type      = triqs::gfs::gf<T...>;
    using gf_view_type = triqs::gfs::gf_view<T...>;
    using c_type       = triqs::gfs::block_gf_view<T...>;

    // ----------------------------------------------

    static PyObject *c2py(c_type g) {
      // rm the view_proxy
      std::vector<gf_view_type> vg;
      vg.reserve(g.data().size());
      for (auto const &x : g.data()) vg.push_back(x);
      pyref v_gf    = convert_to_python(vg);
      pyref v_names = convert_to_python(g.block_names());
      if (v_gf.is_null() or v_names.is_null()) return NULL;
      pyref cls = pyref::module("pytriqs.gf").attr("BlockGf");
      if (cls.is_null()) return NULL;
      pyref kw = PyDict_New();
      PyDict_SetItemString(kw, "name_list", v_names);
      PyDict_SetItemString(kw, "block_list", v_gf);
      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref cls = pyref::module("pytriqs.gf").attr("BlockGf");
      if (cls.is_null()) CPP2PY_RUNTIME_ERROR << "Cannot find the pytriqs.gf.BlockGf";
      int i = PyObject_IsInstance(ob, cls);
      if (i == -1) { // an error has occurred
        i = 0;
        if (!raise_exception) PyErr_Clear();
      }
      if ((i == 0) && (raise_exception)) PyErr_SetString(PyExc_TypeError, "The object is not a BlockGf");

      pyref x     = borrowed(ob);
      pyref names = x.attr("_BlockGf__indices");
      pyref gfs   = x.attr("_BlockGf__GFlist");
      return (i && py_converter<std::vector<gf_view_type>>::is_convertible(gfs, raise_exception) && py_converter<std::vector<std::string>>::is_convertible(names, raise_exception));
    }

    // ----------------------------------------------

    static c_type py2c(PyObject *ob) {
      pyref x     = borrowed(ob);
      pyref names = x.attr("_BlockGf__indices");
      pyref gfs   = x.attr("_BlockGf__GFlist");
      return make_block_gf_view(convert_from_python<std::vector<std::string>>(names), convert_from_python<std::vector<gf_view_type>>(gfs));
    }
  };

  // -----------------------------------
  //    block2_gf_view
  // -----------------------------------

  template <typename... T> struct py_converter<triqs::gfs::block2_gf_view<T...>> {

    // using gf_type = triqs::gfs::gf<T...>;
    using gf_view_type = triqs::gfs::gf_view<T...>;
    using c_type       = triqs::gfs::block2_gf_view<T...>;

    // ----------------------------------------------

    static PyObject *c2py(c_type g) {
      std::vector<std::vector<gf_view_type>> vvg;
      vvg.reserve(g.data().size());
      for (auto const &x : g.data()) {
        std::vector<gf_view_type> vg;
        vg.reserve(x.size());
        for (auto const &y : x) vg.push_back(y);
        vvg.push_back(vg);
      }
      pyref v_gf     = convert_to_python(vvg);
      pyref v_names1 = convert_to_python(g.block_names()[0]);
      pyref v_names2 = convert_to_python(g.block_names()[1]);
      if (v_gf.is_null() or v_names1.is_null() or v_names2.is_null()) return NULL;
      pyref cls = pyref::module("pytriqs.gf").attr("Block2Gf");
      if (cls.is_null()) return NULL;
      pyref args = PyTuple_Pack(3, (PyObject *)v_names1, (PyObject *)v_names2, (PyObject *)v_gf);
      return PyObject_Call(cls, args, NULL);
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref cls = pyref::module("pytriqs.gf").attr("Block2Gf");
      if (cls.is_null()) CPP2PY_RUNTIME_ERROR << "Cannot find the pytriqs.gf.Block2Gf";
      int i = PyObject_IsInstance(ob, cls);
      if (i == -1) { // an error has occurred
        i = 0;
        if (!raise_exception) PyErr_Clear();
      }
      if ((i == 0) && (raise_exception)) PyErr_SetString(PyExc_TypeError, "The object is not a Block2Gf");

      pyref x   = borrowed(ob);
      pyref gfs = x.attr("_Block2Gf__GFlist");
      return (i && py_converter<std::vector<std::vector<gf_view_type>>>::is_convertible(gfs, raise_exception));
    }

    // ----------------------------------------------

    static c_type py2c(PyObject *ob) {
      pyref x      = borrowed(ob);
      pyref names1 = x.attr("_Block2Gf__indices1");
      pyref names2 = x.attr("_Block2Gf__indices2");
      pyref gfs    = x.attr("_Block2Gf__GFlist");
      return make_block2_gf_view(convert_from_python<std::vector<std::string>>(names1), convert_from_python<std::vector<std::string>>(names2),
                                 convert_from_python<std::vector<std::vector<gf_view_type>>>(gfs));
    }
  };

} // namespace cpp2py
