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

#include "./mesh.hpp"
#include "../gfs/gf/gf.hpp"
#include "../gfs/gf/gf_view.hpp"
#include "../gfs/gf/gf_const_view.hpp"
#include "../gfs/gf/targets.hpp"
#include "../gfs/block/block_gf.hpp"
#include "../gfs/block/block_gf_view.hpp"
#include "../gfs/block/factories.hpp"
#include "../utility/typeid_name.hpp"

#include <c2py/converters/stl/vector.hpp>
#include <c2py/py_converter.hpp>
#include <c2py/pyref.hpp>
#include <Python.h>

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/// Additional converters for gf
namespace c2py {

  namespace detail {

    // Non-template core: builds and raises the TypeError. Called by the thin template wrappers below.
    inline void set_err_impl(PyObject *p, std::string_view kind, const char *X, std::string const &self_type, std::string const &C) {
      std::ostringstream err;
      err << "C2py converter: Python to C++ :\n"
          << "  ... Conversion of a " << kind << " from Python to C++ " << self_type << "\n"
          << "  ... Cannot convert the " << X << " of " << kind << " from Python type :  " << p->ob_type->tp_name << " to the C++ type " << C;
      PyErr_SetString(PyExc_TypeError, err.str().c_str());
    }

  } // namespace detail

  // -----------------------------------
  //   gf
  // -----------------------------------

  template <typename M, typename T> struct py_converter<triqs::gfs::gf<M, T>> {
    using c_t    = triqs::gfs::gf<M, T>;
    using conv_t = py_converter<triqs::gfs::gf_view<M, T>>;

    static std::string tp_name() { return conv_t::tp_name(); }

    static PyObject *c2py(triqs::gfs::gf_view<M, T> g) { return conv_t::c2py(g); }
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_t py2c(PyObject *ob) { return c_t{conv_t::py2c(ob)}; }
  };

  // -----------------------------------
  //   gf_const_view
  // -----------------------------------

  template <typename M, typename T> struct py_converter<triqs::gfs::gf_const_view<M, T>> {
    using c_t    = triqs::gfs::gf_const_view<M, T>;
    using conv_t = py_converter<triqs::gfs::gf_view<M, T>>;

    static std::string tp_name() { return conv_t::tp_name(); }

    static PyObject *c2py(c_t g) = delete; // You can not convert a C++ const_view to a Python Gf ! Violates const correctness.
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_t py2c(PyObject *ob) { return conv_t::py2c(ob); }
  };

  // -----------------------------------
  //    gf_view
  // -----------------------------------

  // Converter for gf one var
  template <typename M, typename T> struct py_converter<triqs::gfs::gf_view<M, T>> {
    using c_t    = triqs::gfs::gf_view<M, T>;
    using mesh_t = typename c_t::mesh_t;
    using data_t = typename c_t::data_t;

    static std::string tp_name() {
      std::ostringstream out;
      out << "Gf[" << ::c2py::python_typename<M>() << ", " << T::rank << "]";
      return out.str();
    }

    static PyObject *c2py(c_t g) {

      pyref cls = pyref::get_class("triqs.gfs", "Gf", true);
      if (cls.is_null()) return NULL;
      pyref m = cxx2py(g.mesh());
      if (m.is_null()) return NULL;
      pyref d = cxx2py(g.data());
      if (d.is_null()) return NULL;

      pyref kw = PyDict_New();
      PyDict_SetItemString(kw, "mesh", m);
      PyDict_SetItemString(kw, "data", d);

      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    // ----------------------------------------------

    static void _set_err(PyObject *p, const char *X, std::string const &C) {
      detail::set_err_impl(p, "Gf", X, triqs::utility::typeid_name<triqs::gfs::gf_view<M, T>>(), C);
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref cls = pyref::get_class("triqs.gfs", "Gf", true);

      // first check it is a Gf
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      pyref x = pyref::borrowed(ob);

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

    static c_t py2c(PyObject *ob) {
      pyref x = pyref::borrowed(ob);
      pyref m = x.attr("_mesh");
      pyref d = x.attr("_data");
      return c_t{py2cxx<mesh_t>(m), py2cxx<data_t>(d)};
    }
  };

  // -----------------------------------
  //   block_gf
  // -----------------------------------

  template <typename M, typename T, int A> struct py_converter<triqs::gfs::block_gf<M, T, nda::C_layout, A>> {
    using conv_t = py_converter<triqs::gfs::block_gf_view<M, T, nda::C_stride_layout, A>>;
    using c_t    = triqs::gfs::block_gf<M, T, nda::C_layout, A>;

    // ------------ tp_name ---------------

    static std::string tp_name() { return conv_t::tp_name(); }

    static PyObject *c2py(triqs::gfs::block_gf_view<M, T, nda::C_stride_layout, A> g) { return conv_t::c2py(g); }
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_t py2c(PyObject *ob) { return c_t{conv_t::py2c(ob)}; }
  };

  // -----------------------------------
  //   block_gf_const_view
  // -----------------------------------

  template <typename M, typename T, int A> struct py_converter<triqs::gfs::block_gf_const_view<M, T, nda::C_stride_layout, A>> {
    using conv_t = py_converter<triqs::gfs::block_gf_view<M, T, nda::C_stride_layout, A>>;
    using c_t    = triqs::gfs::block_gf_const_view<M, T, nda::C_stride_layout, A>;

    // ------------ tp_name ---------------

    static std::string tp_name() { return conv_t::tp_name(); }

    static PyObject *c2py(c_t g) = delete; // You can not convert a C++ const_view to a Python Gf ! Violates const correctness.
    static bool is_convertible(PyObject *ob, bool raise_exception) { return conv_t::is_convertible(ob, raise_exception); }
    static c_t py2c(PyObject *ob) { return conv_t::py2c(ob); }
  };

  // -----------------------------------
  //    block_gf_view
  // -----------------------------------

  template <typename M, typename T> struct py_converter<triqs::gfs::block_gf_view<M, T>> {
    using gf_type      = triqs::gfs::gf<M, T>;
    using gf_view_type = triqs::gfs::gf_view<M, T>;
    using c_t          = triqs::gfs::block_gf_view<M, T>;

    static std::string tp_name() {
      std::ostringstream out;
      out << "BlockGf[" << ::c2py::python_typename<M>() << ", " << T::rank << "]";
      return out.str();
    }

    // ----------------------------------------------

    static PyObject *c2py(c_t g) {
      // rm the view_proxy
      std::vector<gf_view_type> vg;
      vg.reserve(g.data().size());
      for (auto const &x : g.data()) vg.push_back(x);
      pyref v_gf    = cxx2py(vg);
      pyref v_names = cxx2py(g.block_names());
      if (v_gf.is_null() or v_names.is_null()) return NULL;
      pyref cls = pyref::module("triqs.gfs").attr("BlockGf");
      if (cls.is_null()) return NULL;
      pyref kw = PyDict_New();
      PyDict_SetItemString(kw, "name_list", v_names);
      PyDict_SetItemString(kw, "block_list", v_gf);
      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    // ----------------------------------------------

    static void _set_err(PyObject *p, const char *X, std::string const &C) {
      detail::set_err_impl(p, "BlockGf", X, triqs::utility::typeid_name<c_t>(), C);
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref cls = pyref::get_class("triqs.gfs", "BlockGf", true);

      // first check it is a BlockGf
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      pyref x = pyref::borrowed(ob);

      // check the gfs and indices
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

    static c_t py2c(PyObject *ob) {
      pyref x     = pyref::borrowed(ob);
      pyref names = x.attr("_BlockGf__indices");
      pyref gfs   = x.attr("_BlockGf__GFlist");
      return make_block_gf_view(py2cxx<std::vector<std::string>>(names), py2cxx<std::vector<gf_view_type>>(gfs));
    }
  };

  // -----------------------------------
  //    block2_gf_view
  // -----------------------------------

  template <typename M, typename T> struct py_converter<triqs::gfs::block2_gf_view<M, T>> {
    using gf_type      = triqs::gfs::gf<M, T>;
    using gf_view_type = triqs::gfs::gf_view<M, T>;
    using c_t          = triqs::gfs::block2_gf_view<M, T>;

    static std::string tp_name() {
      std::ostringstream out;
      out << "Block2Gf[" << ::c2py::python_typename<M>() << ", " << T::rank << "]";
      return out.str();
    }

    // ----------------------------------------------

    static PyObject *c2py(c_t g) {
      std::vector<std::vector<gf_view_type>> vvg;
      vvg.reserve(g.data().size());
      for (auto const &x : g.data()) {
        std::vector<gf_view_type> vg;
        vg.reserve(x.size());
        for (auto const &y : x) vg.push_back(y);
        vvg.push_back(vg);
      }
      pyref v_gf     = cxx2py(vvg);
      pyref v_names1 = cxx2py(g.block_names()[0]);
      pyref v_names2 = cxx2py(g.block_names()[1]);
      if (v_gf.is_null() or v_names1.is_null() or v_names2.is_null()) return NULL;
      pyref cls = pyref::module("triqs.gfs").attr("Block2Gf");
      if (cls.is_null()) return NULL;
      pyref args = PyTuple_Pack(3, (PyObject *)v_names1, (PyObject *)v_names2, (PyObject *)v_gf);
      return PyObject_Call(cls, args, NULL);
    }

    // ----------------------------------------------

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref cls = pyref::get_class("triqs.gfs", "Block2Gf", true);
      if (cls.is_null()) throw std::runtime_error("Cannot find the triqs.gfs.Block2Gf");

      // first check it is a Block2Gf
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;

      pyref x   = pyref::borrowed(ob);
      pyref gfs = x.attr("_Block2Gf__GFlist");
      return py_converter<std::vector<std::vector<gf_view_type>>>::is_convertible(gfs, raise_exception);
    }

    // ----------------------------------------------

    static c_t py2c(PyObject *ob) {
      pyref x      = pyref::borrowed(ob);
      pyref names1 = x.attr("_Block2Gf__indices1");
      pyref names2 = x.attr("_Block2Gf__indices2");
      pyref gfs    = x.attr("_Block2Gf__GFlist");
      return make_block2_gf_view(py2cxx<std::vector<std::string>>(names1), py2cxx<std::vector<std::string>>(names2),
                                 py2cxx<std::vector<std::vector<gf_view_type>>>(gfs));
    }
  };

} // namespace c2py
