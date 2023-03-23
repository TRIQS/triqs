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

#pragma once

namespace triqs {
  namespace gfs {

    //-------------------------------------------------------
    // For DLR Green's functions
    // ------------------------------------------------------

    // dlr_imtime <-> dlr_coeffs

    gf<mesh::dlr_coeffs> dlr_coeffs_from_dlr_imtime(gf_const_view<mesh::dlr_imtime> const &g_tau);
    gf<mesh::dlr_coeffs, scalar_valued> dlr_coeffs_from_dlr_imtime(gf_const_view<mesh::dlr_imtime, scalar_valued> const &g_tau);

    gf<mesh::dlr_imtime> dlr_imtime_from_dlr_coeffs(gf_const_view<mesh::dlr_coeffs> const &g_dlr);
    gf<mesh::dlr_imtime, scalar_valued> dlr_imtime_from_dlr_coeffs(gf_const_view<mesh::dlr_coeffs, scalar_valued> const &g_dlr);

    // dlr_imfreq <-> dlr_coeffs

    gf<mesh::dlr_coeffs> dlr_coeffs_from_dlr_imfreq(gf_const_view<mesh::dlr_imfreq> const &g_iw);
    gf<mesh::dlr_coeffs, scalar_valued> dlr_coeffs_from_dlr_imfreq(gf_const_view<mesh::dlr_imfreq, scalar_valued> const &g_iw);

    gf<mesh::dlr_imfreq> dlr_imfreq_from_dlr_coeffs(gf_const_view<mesh::dlr_coeffs> const &g_dlr);
    gf<mesh::dlr_imfreq, scalar_valued> dlr_imfreq_from_dlr_coeffs(gf_const_view<mesh::dlr_coeffs, scalar_valued> const &g_dlr);
    
  } // namespace gfs
} // namespace triqs
