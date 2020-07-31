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

  template <> struct py_converter<triqs::utility::real_or_complex> {

    using c_t      = triqs::utility::real_or_complex;
    using conv_d_t = py_converter<double>;
    using conv_c_t = py_converter<std::complex<double>>;

    static PyObject *c2py(c_t const &x) {
      if (x.is_real()) return conv_d_t::c2py(double(x));
      return conv_c_t::c2py(std::complex<double>(x));
    }

    static bool is_convertible(PyObject *ob, bool raise_exception) {
      bool ok = conv_d_t::is_convertible(ob, false) || conv_c_t::is_convertible(ob, false);
      if (ok) return true;
      if (raise_exception) { PyErr_SetString(PyExc_TypeError, "Cannot convert to real_or_complex"); }
      return false;
    }

    static c_t py2c(PyObject *ob) {
      if (conv_d_t::is_convertible(ob, false)) return conv_d_t::py2c(ob);
      return conv_c_t::py2c(ob);
    }
  };
} // namespace cpp2py
