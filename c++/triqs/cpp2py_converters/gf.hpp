// Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2020-2023 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Alexander Hampel, Olivier Parcollet, Nils Wentzell

#pragma once
#include <triqs/gfs.hpp>
#include <triqs/mesh/utils.hpp>
#include <triqs/utility/typeid_name.hpp>
#include <triqs/cpp2py_converters/arrays.hpp>

#include <cpp2py/misc.hpp>
#include <cpp2py/converters/vector.hpp>
#include <cpp2py/converters/tuple.hpp>
#include <cpp2py/converters/std_array.hpp>
#include <cpp2py/converters/string.hpp>

/// Additional converters for gf
namespace cpp2py {

  // -----------------------------------
  //    is_view
  //  One MUST declare to cpp2py that a type is a view.
  // -----------------------------------

  template <typename M, typename T> struct is_view<triqs::gfs::gf_view<M, T>> : std::true_type {};
  template <typename M, typename T> struct is_view<triqs::gfs::gf_const_view<M, T>> : std::true_type {};

  template <typename M, typename T, int A, bool C> struct is_view<triqs::gfs::block_gf_view<M, T, A, C>> : std::true_type {};

  // -----------------------------------
  //   gf
  // -----------------------------------

  template <typename M, typename T> struct py_converter<triqs::gfs::gf<M, T>> {
    using conv_t = py_converter<triqs::gfs::gf_view<M, T>>;
    using c_type = triqs::gfs::gf<M, T>;

    static PyObject *c2py(triqs::gfs::gf_view<M, T> g) { return conv_t::c2py(g); }
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_type py2c(PyObject *ob) { return c_type{conv_t::py2c(ob)}; }
  };

  // -----------------------------------
  //   gf_const_view
  // -----------------------------------

  template <typename M, typename T> struct py_converter<triqs::gfs::gf_const_view<M, T>> {
    using conv_t = py_converter<triqs::gfs::gf_view<M, T>>;
    using c_type = triqs::gfs::gf_const_view<M, T>;

    static PyObject *c2py(c_type g) = delete; // You can not convert a C++ const_view to a Python Gf ! Violates const correctness.
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_type py2c(PyObject *ob) { return conv_t::py2c(ob); }
  };

  // -----------------------------------
  //    gf_view
  // -----------------------------------

  // Converter for gf one var
  template <typename M, typename T> struct py_converter<triqs::gfs::gf_view<M, T>> {

    using c_type = triqs::gfs::gf_view<M, T>;
    using mesh_t = typename c_type::mesh_t;
    using data_t = typename c_type::data_t;

    static PyObject *c2py(c_type g) {

      static pyref cls = pyref::get_class("triqs.gf", "Gf", true);
      if (cls.is_null()) return NULL;
      pyref m = convert_to_python(g.mesh());
      if (m.is_null()) return NULL;
      pyref d = convert_to_python(g.data());
      if (d.is_null()) return NULL;

      pyref kw = PyDict_New();
      PyDict_SetItemString(kw, "mesh", m);
      PyDict_SetItemString(kw, "data", d);

      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    // ----------------------------------------------

    static void _set_err(PyObject *p, const char *X, std::string const &C) {
      using namespace std::string_literals;
      std::string err = "Cpp2py converter: Python to C++ :\n"s + "  ... Conversion of a Gf from Python to C++ "
         + triqs::utility::typeid_name<triqs::gfs::gf_view<M, T>>() + "\n  ... Cannot convert the "s + X
         + " of Gf from Python type :  " + p->ob_type->tp_name + " to the C++ type " + C;
      PyErr_SetString(PyExc_TypeError, err.c_str());
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      static pyref cls = pyref::get_class("triqs.gf", "Gf", true);

      // first check it is a Gf
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      pyref x = borrowed(ob);

      // check the mesh, data, indices
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

      return true;
    }

    // ----------------------------------------------

    static c_type py2c(PyObject *ob) {
      pyref x = borrowed(ob);
      pyref m = x.attr("_mesh");
      pyref d = x.attr("_data");
      return c_type{convert_from_python<mesh_t>(m), convert_from_python<data_t>(d)};
    }
  };

  // -----------------------------------
  //   block_gf
  // -----------------------------------

  template <typename M, typename T, int A> struct py_converter<triqs::gfs::block_gf<M, T, A>> {
    using conv_t = py_converter<triqs::gfs::block_gf_view<M, T, A>>;
    using c_type = triqs::gfs::block_gf<M, T, A>;

    static PyObject *c2py(triqs::gfs::block_gf_view<M, T, A> g) { return conv_t::c2py(g); }
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_type py2c(PyObject *ob) { return c_type{conv_t::py2c(ob)}; }
  };

  // -----------------------------------
  //   block_gf_const_view
  // -----------------------------------

  template <typename M, typename T, int A> struct py_converter<triqs::gfs::block_gf_const_view<M, T, A>> {
    using conv_t = py_converter<triqs::gfs::block_gf_view<M, T, A>>;
    using c_type = triqs::gfs::block_gf_const_view<M, T, A>;

    static PyObject *c2py(c_type g) = delete; // You can not convert a C++ const_view to a Python Gf ! Violates const correctness.
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_type py2c(PyObject *ob) { return conv_t::py2c(ob); }
  };

  // -----------------------------------
  //    block_gf_view
  // -----------------------------------

  template <typename M, typename T> struct py_converter<triqs::gfs::block_gf_view<M, T>> {

    using gf_type      = triqs::gfs::gf<M, T>;
    using gf_view_type = triqs::gfs::gf_view<M, T>;
    using c_type       = triqs::gfs::block_gf_view<M, T>;

    // ----------------------------------------------

    static PyObject *c2py(c_type g) {
      // rm the view_proxy
      std::vector<gf_view_type> vg;
      vg.reserve(g.data().size());
      for (auto const &x : g.data()) vg.push_back(x);
      pyref v_gf    = convert_to_python(vg);
      pyref v_names = convert_to_python(g.block_names());
      if (v_gf.is_null() or v_names.is_null()) return NULL;
      pyref cls = pyref::module("triqs.gf").attr("BlockGf");
      if (cls.is_null()) return NULL;
      pyref kw = PyDict_New();
      PyDict_SetItemString(kw, "name_list", v_names);
      PyDict_SetItemString(kw, "block_list", v_gf);
      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    // ----------------------------------------------

    static void _set_err(PyObject *p, const char *X, std::string const &C) {
      using namespace std::string_literals;
      std::string err = "Cpp2py converter: Python to C++ :\n"s + "  ... Conversion of a BlockGf from Python to C++ "
         + triqs::utility::typeid_name<c_type>() + "\n  ... Cannot convert the "s + X + " of BlockGf from Python type :  " + p->ob_type->tp_name
         + " to the C++ type " + C;
      PyErr_SetString(PyExc_TypeError, err.c_str());
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      static pyref cls = pyref::get_class("triqs.gf", "BlockGf", true);

      // first check it is a BlockGf
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      pyref x = borrowed(ob);

      // check the gfs and indicies
      pyref gfs = x.attr("_BlockGf__GFlist");
      if (!py_converter<std::vector<gf_view_type>>::is_convertible(gfs, false)) {
        if (raise_exception) _set_err(gfs, "_BlockGf__GFlist", triqs::utility::typeid_name<std::vector<gf_view_type>>());
        return false;
      }

      pyref names = x.attr("_BlockGf__indices");
      if (!py_converter<std::vector<std::string>>::is_convertible(names, false)) {
        if (raise_exception) _set_err(names, "_BlockGf__indices", triqs::utility::typeid_name<std::vector<std::string>>());
        return false;
      }

      return true;
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

  template <typename M, typename T> struct py_converter<triqs::gfs::block2_gf_view<M, T>> {

    using gf_type      = triqs::gfs::gf<M, T>;
    using gf_view_type = triqs::gfs::gf_view<M, T>;
    using c_type       = triqs::gfs::block2_gf_view<M, T>;

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
      pyref cls = pyref::module("triqs.gf").attr("Block2Gf");
      if (cls.is_null()) return NULL;
      pyref args = PyTuple_Pack(3, (PyObject *)v_names1, (PyObject *)v_names2, (PyObject *)v_gf);
      return PyObject_Call(cls, args, NULL);
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      static pyref cls = pyref::get_class("triqs.gf", "Block2Gf", true);
      if (cls.is_null()) throw std::runtime_error("Cannot find the triqs.gf.Block2Gf");

      // first check it is a Block2Gf
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;

      pyref x   = borrowed(ob);
      pyref gfs = x.attr("_Block2Gf__GFlist");
      return py_converter<std::vector<std::vector<gf_view_type>>>::is_convertible(gfs, raise_exception);
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
