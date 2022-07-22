// Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet

#pragma once

namespace cpp2py {

  /// REAL
  template <> struct py_converter<triqs::operators::many_body_operator_real> {
    using c_t    = triqs::operators::many_body_operator_real;
    using conv_t = py_converter<triqs::operators::many_body_operator>;

    static PyObject *c2py(c_t const &x) { return conv_t::c2py(x); }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      bool ok = conv_t::is_convertible(ob, false /* we set the error ourselves */);
      if (ok) {
        // check the operator is real
        auto op = conv_t::py2c(ob);
        for (auto const &c_m : op) ok = ok && c_m.coef.is_real();
        if (ok) return true;
      }
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to many_body_operator_real"); }
      return false;
    }

    static c_t py2c(PyObject *ob) { return c_t{conv_t::py2c(ob)}; }
  };

  // ----------------------------------------------

  /// COMPLEX
  template <> struct py_converter<triqs::operators::many_body_operator_complex> {
    using c_t    = triqs::operators::many_body_operator_complex;
    using conv_t = py_converter<triqs::operators::many_body_operator>;

    static PyObject *c2py(c_t const &x) { return conv_t::c2py(x); }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      bool ok = conv_t::is_convertible(ob, false);
      if (ok) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to many_body_operator_complex"); }
      return false;
    }

    static c_t py2c(PyObject *ob) { return c_t{conv_t::py2c(ob)}; }
  };
} // namespace cpp2py
