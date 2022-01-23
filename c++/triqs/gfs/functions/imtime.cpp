// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include "../../gfs.hpp"
#include <triqs/utility/legendre.hpp>

namespace triqs {
  namespace gfs {

    using nda::array;

    //-------------------------------------------------------
    // For Imaginary Time functions
    // ------------------------------------------------------
    gf<mesh::imtime> rebinning_tau(gf_const_view<mesh::imtime> const &g, size_t new_n_tau) {

      auto const &old_m = g.mesh();
      gf<mesh::imtime> new_gf{{old_m.domain().beta, old_m.domain().statistic, static_cast<long>(new_n_tau)}, g.target_shape()};
      auto const &new_m = new_gf.mesh();
      new_gf.data()()   = 0;
      long prev_index   = 0;
      long norm         = 0;
      for (auto const &tau : old_m) {
        long index = std::round((double(tau) - new_m.x_min()) / new_m.delta());
        if (index == prev_index) {
          norm++;
        } else {
          new_gf[index - 1] /= double(norm);
          prev_index = index;
          norm       = 1;
        }
        new_gf[index] += g[tau];
      }
      if (norm != 1) new_gf[new_m.size() - 1] /= norm;
      return new_gf;
    }
  } // namespace gfs
} // namespace triqs
