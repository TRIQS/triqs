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

#include "../lattice/bravais_lattice.hpp"

#include <c2py/converters/basic_types.hpp>
#include <c2py/converters/stl/array.hpp>
#include <c2py/converters/wrapped.hpp>
#include <c2py/py_converter.hpp>
#include <c2py/pyref.hpp>
#include <Python.h>

#include <array>
#include <iostream>
#include <string>

namespace c2py {

  // -----------------------------------
  //   bravais_lattice::point_t
  // -----------------------------------

  template <> struct py_converter<triqs::lattice::bravais_lattice::point_t> {
    using c_t = triqs::lattice::bravais_lattice::point_t;

    static constexpr const char *tp_name = "LatticePoint";

    static PyObject *c2py(c_t const &x) {
      pyref cls = pyref::get_class("triqs.lattice", "LatticePoint", true);
      if (cls.is_null()) return NULL;

      pyref kw = PyDict_New();

      pyref index = cxx2py(x.index());
      if (index.is_null()) return NULL;
      pyref lattice = cxx2py(x.lattice());
      if (lattice.is_null()) return NULL;
      PyDict_SetItemString(kw, "index", index);
      PyDict_SetItemString(kw, "lattice", lattice);

      pyref empty_tuple = PyTuple_New(0);
      return PyObject_Call(cls, empty_tuple, kw);
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      pyref cls = pyref::get_class("triqs.lattice", "LatticePoint", true);
      if (not pyref::check_is_instance(ob, cls, raise_exception)) return false;
      return true;
    }

    // ----------------------------------------------

    using lattice_py_type = struct {
      PyObject_HEAD;
      triqs::lattice::bravais_lattice *_c;
    };

    static c_t py2c(PyObject *ob) {

      pyref x             = pyref::borrowed(ob);
      pyref index         = x.attr("index");
      pyref lattice       = x.attr("lattice");
      auto *lattice_c_ptr = reinterpret_cast<lattice_py_type *>(static_cast<PyObject *>(lattice))->_c;
      if (lattice_c_ptr == NULL) {
        std::cerr << "Severe internal error : lattice_ptr is null in py2c\n";
        std::terminate();
      }

      return c_t{py2cxx<std::array<long, 3>>(index), lattice_c_ptr};
    }
  };

} // namespace c2py
