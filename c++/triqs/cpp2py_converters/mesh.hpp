// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once

namespace cpp2py {

  // -----------------------------------
  // all_t mapped to all
  // -----------------------------------

  template <> struct py_converter<triqs::mesh::all_t> {

    static PyObject *c2py(triqs::mesh::all_t m) {
      static pyref all = pyref::get_class("builtins", "all", true);
      if (all.is_null()) return NULL;
      return all.new_ref();
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      static pyref all = pyref::get_class("builtins", "all", true);
      return (all == ob);
    }

    static triqs::mesh::all_t py2c(PyObject *ob) { return {}; }
  };

  // -----------------------------------
  //   statistic_enum
  // -----------------------------------

  template <> struct py_converter<triqs::mesh::statistic_enum> {
   static PyObject * c2py(triqs::mesh::statistic_enum x) {
      if (x == triqs::mesh::Fermion) return PyUnicode_FromString("Fermion");
     return PyUnicode_FromString("Boson"); // last case separate to avoid no return warning of compiler
   }
   static triqs::mesh::statistic_enum py2c(PyObject * ob){
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
      auto err = "Convertion of C++ enum statistic_enum : \nThe string \"" + s +"\" is not in [Fermion,Boson]";
      PyErr_SetString(PyExc_ValueError, err.c_str());
     }
     return false;
   }
  };

  // -----------------------------------
  //   matsubara_freq
  // -----------------------------------

  template <> struct py_converter<triqs::mesh::matsubara_freq> {

    using c_type = triqs::mesh::matsubara_freq;

    static PyObject *c2py(c_type const &x) {
      static pyref cls = pyref::get_class("triqs.gf", "MatsubaraFreq", true);
      if (cls.is_null()) return NULL;

      pyref kw = PyDict_New();

      pyref n = convert_to_python(x.n);
      if (n.is_null()) return NULL;
      pyref beta = convert_to_python(x.beta);
      if (beta.is_null()) return NULL;
      pyref statistic = convert_to_python(x.statistic);
      if (n.is_null()) return NULL;
      PyDict_SetItemString(kw, "n", n);
      PyDict_SetItemString(kw, "beta", beta);
      PyDict_SetItemString(kw, "statistic", statistic);

      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      static pyref cls = pyref::get_class("triqs.gf", "MatsubaraFreq", true);
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      return true;
    }

    // ----------------------------------------------

    static c_type py2c(PyObject *ob) {
      pyref x         = borrowed(ob);
      pyref n         = x.attr("n");
      pyref beta      = x.attr("beta");
      pyref statistic = x.attr("statistic");
      return c_type{convert_from_python<long>(n), convert_from_python<double>(beta),
                    convert_from_python<triqs::mesh::statistic_enum>(statistic)};
    }
  };

  // -----------------------------------
  //     Mesh Product
  // -----------------------------------

  template <triqs::mesh::Mesh... Ms> struct py_converter<triqs::mesh::prod<Ms...>> {

    using c_type      = triqs::mesh::prod<Ms...>;
    using mtuple_conv = py_converter<typename c_type::m_tuple_t>; // the tuple of meshes

    static PyObject *c2py(c_type m) {
      static pyref cls = pyref::get_class("triqs.gf", "MeshProduct", true);
      if (cls.is_null()) return NULL;
      pyref m_tuple = mtuple_conv::c2py(m.components()); // take the C++ tuple of meshes and make the corresponding Python tuple
      if (m_tuple.is_null()) return NULL;
      return PyObject_Call(cls, m_tuple, NULL);
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      static pyref cls = pyref::get_class("triqs.gf", "MeshProduct", true);

      // first check it is a MeshProduct
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      pyref x = borrowed(ob);

      // check conversion of the mesh list
      pyref ml = x.attr("_mlist");
      return mtuple_conv::is_convertible(ml, raise_exception);
    }

    static c_type py2c(PyObject *ob) {
      pyref x  = borrowed(ob);
      pyref ml = x.attr("_mlist");
      return triqs::tuple::apply_construct<c_type>(mtuple_conv::py2c(ml));
    }
  };

  // -----------------------------------
  //   mesh_point
  // -----------------------------------

  template <triqs::mesh::MeshPoint MP> struct py_converter<MP> {

    using c_type = MP;

    static PyObject *c2py(c_type const &p) {

      static pyref cls = pyref::get_class("triqs.gf", "MeshPoint", /* raise_exception */ true);
      if (cls.is_null()) return NULL;

      pyref idx = convert_to_python(p.idx);
      if (idx.is_null()) return NULL;

      pyref datidx = convert_to_python(p.datidx);
      if (datidx.is_null()) return NULL;

      if constexpr (requires { p.value(); }) {
      pyref val = convert_to_python(p.value());
      if (val.is_null()) return NULL;

      if constexpr (requires { p.weight(); }) {
        pyref weight = convert_to_python(p.weight());
        if (weight.is_null()) return NULL;
        return PyObject_Call(cls, pyref::make_tuple(idx, datidx, val, weight), NULL);
      }
      return PyObject_Call(cls, pyref::make_tuple(idx, datidx, val), NULL);
      }

      return PyObject_Call(cls, pyref::make_tuple(idx, datidx), NULL);
    }
  };

} // namespace cpp2py
