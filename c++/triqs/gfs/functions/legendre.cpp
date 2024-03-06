// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include "../../gfs.hpp"
#include <triqs/utility/legendre.hpp>

namespace triqs {
  namespace gfs {

    using nda::array;

    //-------------------------------------------------------
    // For Legendre functions
    // ------------------------------------------------------

    // compute a tail from the Legendre GF
    // this is Eq. 8 of our paper
    array<dcomplex, 3> get_tail(gf_const_view<legendre> gl, int order) {

      auto sh = gl.data().shape();
      sh[0]   = order;
      array<dcomplex, 3> t{sh};
      t() = 0.0;

      for (int p = 0; p < order; p++)
        for (auto l : gl.mesh()) t(p, ellipsis{}) += (triqs::utility::legendre_t(l.index(), p) / std::pow(gl.mesh().beta(), p)) * gl[l];

      return t;
    }

    // Impose a discontinuity G(\tau=0)-G(\tau=\beta)
    void enforce_discontinuity(gf_view<legendre> &gl, nda::array_view<double, 2> disc) {

      double norm = 0.0;
      nda::vector<double> t(gl.data().shape()[0]);
      for (int i = 0; i < t.size(); ++i) {
        t(i) = triqs::utility::legendre_t(i, 1) / gl.mesh().beta();
        norm += t(i) * t(i);
      }

      nda::array<dcomplex, 2> corr(disc.shape());
      corr() = 0;
      for (auto l : gl.mesh()) corr += t(l.index()) * gl[l];

      for (auto l : gl.mesh()) gl.data()(l.index(), range::all, range::all) += (disc - corr) * t(l.index()) / norm;
    }

  } // namespace gfs
} // namespace triqs
