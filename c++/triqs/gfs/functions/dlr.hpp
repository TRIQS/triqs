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
//#include "triqs/mesh.hpp"
#include "triqs/gfs.hpp"

namespace triqs::gfs {

  //-------------------------------------------------------
  // For DLR Green's functions
  // ------------------------------------------------------

  // dlr_imtime <-> dlr_coeffs
  // OPFIXME : naming ?? too long. Why twice dlr_ ?
  // Why not a cross construction ???
  // Why scalar_valued only ???
  // Move into transform/

  //  gf<dlr_coeffs> {g_it};
  // cf gf.hpp line 247 : we have a TOO general gf cross constructor !
  // closed by making the dlr_xxx mesh cross construction EXPLICIT

  // No need to separate the scalar_valued case Cf cpp l68, generic code ok.

  // NAMING
  // to_dlr_coeffs(g)  or to_dlr_imtime(g)   (like std::to_string)...
  // to_ or make_gf_ ...

  gf<dlr_coeffs> dlr_coeffs_from_dlr_imtime(gf_const_view<dlr_imtime> g_tau);
  gf<dlr_coeffs, scalar_valued> dlr_coeffs_from_dlr_imtime(gf_const_view<dlr_imtime, scalar_valued> g_tau);

  gf<dlr_imtime> dlr_imtime_from_dlr_coeffs(gf_const_view<dlr_coeffs> g_dlr);
  gf<dlr_imtime, scalar_valued> dlr_imtime_from_dlr_coeffs(gf_const_view<dlr_coeffs, scalar_valued> g_dlr);

  // dlr_imfreq <-> dlr_coeffs

  gf<dlr_coeffs> dlr_coeffs_from_dlr_imfreq(gf_const_view<dlr_imfreq> g_iw);
  gf<dlr_coeffs, scalar_valued> dlr_coeffs_from_dlr_imfreq(gf_const_view<dlr_imfreq, scalar_valued> g_iw);

  gf<dlr_imfreq> dlr_imfreq_from_dlr_coeffs(gf_const_view<dlr_coeffs> g_dlr);
  gf<dlr_imfreq, scalar_valued> dlr_imfreq_from_dlr_coeffs(gf_const_view<dlr_coeffs, scalar_valued> g_dlr);

} // namespace triqs::gfs
