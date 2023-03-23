// Copyright (c) 2023 Hugo U.R. Strand
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
// Authors: Hugo U.R. Strand

#include "../../gfs.hpp"

namespace triqs {
  namespace gfs {

    using nda::array;

    //-------------------------------------------------------
    // For DLR Green's functions
    // ------------------------------------------------------

    gf<mesh::dlr_coeffs> dlr_coeffs_from_dlr_imtime(gf_const_view<mesh::dlr_imtime> const &g_tau) {
      gf<mesh::dlr_coeffs> g_dlr{g_tau.mesh(), g_tau.target_shape()};
      g_dlr.data() = g_dlr.mesh().dlr_it().vals2coefs(g_tau.data());
      return g_dlr;
    }

    gf<mesh::dlr_coeffs, scalar_valued> dlr_coeffs_from_dlr_imtime(
      gf_const_view<mesh::dlr_imtime, scalar_valued> const &g_tau) {
      gf<mesh::dlr_coeffs, scalar_valued> g_dlr{g_tau.mesh()};
      g_dlr.data() = g_dlr.mesh().dlr_it().vals2coefs(g_tau.data());
      return g_dlr;
    }

    gf<mesh::dlr_imtime> dlr_imtime_from_dlr_coeffs(gf_const_view<mesh::dlr_coeffs> const &g_dlr) {
      auto g_tau = gf<triqs::mesh::dlr_imtime>{g_dlr.mesh()};
      g_tau.data() = g_dlr.mesh().dlr_it().coefs2vals(g_dlr.data());
      return g_tau;
    }
    
    gf<mesh::dlr_imtime, scalar_valued> dlr_imtime_from_dlr_coeffs(gf_const_view<mesh::dlr_coeffs, scalar_valued> const &g_dlr) {
      auto g_tau = gf<triqs::mesh::dlr_imtime, scalar_valued>{g_dlr.mesh()};
      g_tau.data() = g_dlr.mesh().dlr_it().coefs2vals(g_dlr.data());
      return g_tau;
    }
    

  } // namespace gfs
} // namespace triqs
