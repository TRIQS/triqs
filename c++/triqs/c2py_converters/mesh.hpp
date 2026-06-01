// Copyright (c) 2022-2023 Simons Foundation
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
// Authors: Nils Wentzell

#pragma once

#include "../mesh/concepts.hpp"
#include "../mesh/matsubara_freq.hpp"
#include "../mesh/prod.hpp"
#include "../mesh/utils.hpp"
#include "../utility/tuple_tools.hpp"

#include <c2py/converters/basic_types.hpp>
#include <c2py/converters/stl/array.hpp>
#include <c2py/converters/wrapped.hpp>
#include <c2py/py_converter.hpp>
#include <c2py/pyref.hpp>
#include <Python.h>

#include <array>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

namespace c2py {

  // -----------------------------------
  // all_t mapped to all
  // -----------------------------------

  template <> struct py_converter<triqs::mesh::all_t> {

    static constexpr const char *tp_name = "all";

    static PyObject *c2py(triqs::mesh::all_t m) {
      pyref all = pyref::get_class("builtins", "all", true);
      if (all.is_null()) return NULL;
      return all.new_ref();
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref all = pyref::get_class("builtins", "all", true);
      return (all == ob);
    }

    static triqs::mesh::all_t py2c(PyObject *ob) { return {}; }
  };

  // -----------------------------------
  //   statistic_enum
  // -----------------------------------

  template <> struct py_converter<triqs::mesh::statistic_enum> {

    static constexpr const char *tp_name = R"(Statistic ("Fermion" | "Boson"))";

    static PyObject *c2py(triqs::mesh::statistic_enum x) {
      if (x == triqs::mesh::Fermion) return PyUnicode_FromString("Fermion");
      return PyUnicode_FromString("Boson"); // last case separate to avoid no return warning of compiler
    }
    static triqs::mesh::statistic_enum py2c(PyObject *ob) {
      std::string s = PyUnicode_AsUTF8(ob);
      if (s == "Fermion") return triqs::mesh::Fermion;
      return triqs::mesh::Boson;
    }
    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (!PyUnicode_Check(ob)) {
        if (raise_exception) PyErr_SetString(PyExc_ValueError, "Convertion of C++ enum statistic_enum : the object is not a string");
        return false;
      }
      std::string s = PyUnicode_AsUTF8(ob);
      if (s == "Fermion") return true;
      if (s == "Boson") return true;
      if (raise_exception) {
        auto err = "Convertion of C++ enum statistic_enum : \nThe string \"" + s + "\" is not in [Fermion,Boson]";
        PyErr_SetString(PyExc_ValueError, err.c_str());
      }
      return false;
    }
  };

  // -----------------------------------
  //   channel_enum
  // -----------------------------------

  template <> struct py_converter<triqs::mesh::channel_enum> {

    static constexpr const char *tp_name = R"(Channel ("PH" | "PP"))";

    static PyObject *c2py(triqs::mesh::channel_enum x) {
      if (x == triqs::mesh::PP) return PyUnicode_FromString("PP");
      return PyUnicode_FromString("PH");
    }
    static triqs::mesh::channel_enum py2c(PyObject *ob) {
      std::string s = PyUnicode_AsUTF8(ob);
      if (s == "PP") return triqs::mesh::PP;
      return triqs::mesh::PH;
    }
    static bool is_convertible(PyObject *ob, bool raise_exception) {
      if (!PyUnicode_Check(ob)) {
        if (raise_exception) PyErr_SetString(PyExc_ValueError, "Convertion of C++ enum channel_enum : the object is not a string");
        return false;
      }
      std::string s = PyUnicode_AsUTF8(ob);
      if (s == "PH") return true;
      if (s == "PP") return true;
      if (raise_exception) {
        auto err = "Convertion of C++ enum channel_enum : \nThe string \"" + s + "\" is not in [PH,PP]";
        PyErr_SetString(PyExc_ValueError, err.c_str());
      }
      return false;
    }
  };

  // -----------------------------------
  //   matsubara_freq
  // -----------------------------------

  template <> struct py_converter<triqs::mesh::matsubara_freq> {
    using c_t = triqs::mesh::matsubara_freq;

    static constexpr const char *tp_name = "MatsubaraFreq";

    static PyObject *c2py(c_t const &x) {
      pyref cls = pyref::get_class("triqs.mesh", "MatsubaraFreq", true);
      if (cls.is_null()) return NULL;

      pyref kw = PyDict_New();

      pyref n = cxx2py(x.n);
      if (n.is_null()) return NULL;
      pyref beta = cxx2py(x.beta);
      if (beta.is_null()) return NULL;
      pyref statistic = cxx2py(x.statistic);
      if (statistic.is_null()) return NULL;
      PyDict_SetItemString(kw, "n", n);
      PyDict_SetItemString(kw, "beta", beta);
      PyDict_SetItemString(kw, "statistic", statistic);

      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref cls = pyref::get_class("triqs.mesh", "MatsubaraFreq", true);
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      return true;
    }

    // ----------------------------------------------

    static c_t py2c(PyObject *ob) {
      pyref x         = pyref::borrowed(ob);
      pyref n         = x.attr("n");
      pyref beta      = x.attr("beta");
      pyref statistic = x.attr("statistic");
      return c_t{py2cxx<long>(n), py2cxx<double>(beta), py2cxx<triqs::mesh::statistic_enum>(statistic)};
    }
  };

  // -----------------------------------
  //     Mesh Product
  // -----------------------------------

  template <triqs::mesh::Mesh... Ms> struct py_converter<triqs::mesh::prod<Ms...>> {
    using c_t         = triqs::mesh::prod<Ms...>;
    using mtuple_conv = py_converter<typename c_t::m_tuple_t>; // the tuple of meshes

    static std::string tp_name() {
      std::ostringstream out;
      std::string sep;
      out << "MeshProduct[";
      ((out << sep << ::c2py::python_typename<Ms>(), sep = ", "), ...);
      out << "]";
      return out.str();
    }

    static PyObject *c2py(c_t m) {
      pyref cls = pyref::get_class("triqs.mesh", "MeshProduct", true);
      if (cls.is_null()) return NULL;
      pyref m_tuple = mtuple_conv::c2py(m.components()); // take the C++ tuple of meshes and make the corresponding Python tuple
      if (m_tuple.is_null()) return NULL;
      return PyObject_Call(cls, m_tuple, NULL);
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref cls = pyref::get_class("triqs.mesh", "MeshProduct", true);

      // first check it is a MeshProduct
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      pyref x = pyref::borrowed(ob);

      // check conversion of the mesh list
      pyref ml = x.attr("_mlist");
      return mtuple_conv::is_convertible(ml, raise_exception);
    }

    static c_t py2c(PyObject *ob) {
      pyref x  = pyref::borrowed(ob);
      pyref ml = x.attr("_mlist");
      return triqs::tuple::apply_construct<c_t>(mtuple_conv::py2c(ml));
    }
  };

  // -----------------------------------
  //   mesh_point
  // -----------------------------------

  template <triqs::mesh::MeshPoint MP> struct py_converter<MP> {
    using c_t = MP;

    static constexpr const char *tp_name = "MeshPoint";

    static PyObject *c2py(c_t const &p) {

      pyref cls = pyref::get_class("triqs.mesh", "MeshPoint", /* raise_exception */ true);
      if (cls.is_null()) return NULL;

      pyref index = cxx2py(p.index());
      if (index.is_null()) return NULL;

      pyref data_index = cxx2py(p.data_index());
      if (data_index.is_null()) return NULL;

      pyref mesh_hash = cxx2py(p.mesh_hash());
      if (mesh_hash.is_null()) return NULL;

      if constexpr (requires { p.value(); }) {
        pyref val = cxx2py(p.value());
        if (val.is_null()) return NULL;

        if constexpr (requires { p.weight(); }) {
          pyref weight = cxx2py(p.weight());
          if (weight.is_null()) return NULL;
          return PyObject_Call(cls, pyref::make_tuple(index, data_index, mesh_hash, val, weight), NULL);
        }
        return PyObject_Call(cls, pyref::make_tuple(index, data_index, mesh_hash, val), NULL);
      }

      return PyObject_Call(cls, pyref::make_tuple(index, data_index, mesh_hash), NULL);
    }
  };

} // namespace c2py
