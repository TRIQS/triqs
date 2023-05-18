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
    
    // ------------------------------------------------------
    // dlr_imtime <-> dlr_coeffs
    // ------------------------------------------------------

    template<typename target_t>
    gf<dlr_coeffs, target_t> dlr_coeffs_from_dlr_imtime_impl(gf_const_view<dlr_imtime, target_t> g_tau) {
      auto g_dlr = make_gf<dlr_coeffs>(dlr_coeffs(g_tau.mesh()), g_tau.target());
      g_dlr.data() = g_dlr.mesh().dlr_it().vals2coefs(g_tau.data());
      return g_dlr;
    }
    
    gf<dlr_coeffs, scalar_valued> dlr_coeffs_from_dlr_imtime(gf_const_view<dlr_imtime, scalar_valued> g_tau) {
      return dlr_coeffs_from_dlr_imtime_impl<scalar_valued>(g_tau);
    }

    gf<dlr_coeffs, matrix_valued> dlr_coeffs_from_dlr_imtime(gf_const_view<dlr_imtime, matrix_valued> g_tau) {
      return dlr_coeffs_from_dlr_imtime_impl<matrix_valued>(g_tau);
    }

    gf<dlr_coeffs, tensor_valued<4>> dlr_coeffs_from_dlr_imtime(gf_const_view<dlr_imtime, tensor_valued<4>> g_tau) {
      return dlr_coeffs_from_dlr_imtime_impl<tensor_valued<4>>(g_tau);
    }
    
    // --

    template<typename target_t>
    gf<dlr_imtime, target_t> dlr_imtime_from_dlr_coeffs_impl(gf_const_view<dlr_coeffs, target_t> g_dlr) {
      auto g_tau = make_gf<dlr_imtime>(dlr_imtime(g_dlr.mesh()), g_dlr.target());
      g_tau.data() = g_dlr.mesh().dlr_it().coefs2vals(g_dlr.data());
      return g_tau;
    }

    gf<dlr_imtime, scalar_valued> dlr_imtime_from_dlr_coeffs(gf_const_view<dlr_coeffs, scalar_valued> g_dlr) {
      return dlr_imtime_from_dlr_coeffs_impl<scalar_valued>(g_dlr);      
    }

    gf<dlr_imtime, matrix_valued> dlr_imtime_from_dlr_coeffs(gf_const_view<dlr_coeffs, matrix_valued> g_dlr) {
      return dlr_imtime_from_dlr_coeffs_impl<matrix_valued>(g_dlr);
    }

    gf<dlr_imtime, tensor_valued<4>> dlr_imtime_from_dlr_coeffs(gf_const_view<dlr_coeffs, tensor_valued<4>> g_dlr) {
      return dlr_imtime_from_dlr_coeffs_impl<tensor_valued<4>>(g_dlr);
    }
    
    // ------------------------------------------------------
    // dlr_imfreq <-> dlr_coeffs
    // ------------------------------------------------------

    template<typename target_t>
    gf<dlr_coeffs, target_t> dlr_coeffs_from_dlr_imfreq_impl(
      gf_const_view<dlr_imfreq, target_t> g_iw) {
      auto g_dlr = make_gf<dlr_coeffs>(dlr_coeffs(g_iw.mesh()), g_iw.target());
      auto beta_inv = 1. / g_dlr.mesh().domain().beta;
      g_dlr.data() = beta_inv * g_dlr.mesh().dlr_if().vals2coefs(g_iw.data());
      return g_dlr;
    }

    gf<dlr_coeffs, scalar_valued> dlr_coeffs_from_dlr_imfreq(gf_const_view<dlr_imfreq, scalar_valued> g_iw) {
      return dlr_coeffs_from_dlr_imfreq_impl<scalar_valued>(g_iw);
    }

    gf<dlr_coeffs, matrix_valued> dlr_coeffs_from_dlr_imfreq(gf_const_view<dlr_imfreq, matrix_valued> g_iw) {
      return dlr_coeffs_from_dlr_imfreq_impl<matrix_valued>(g_iw);
    }

    gf<dlr_coeffs, tensor_valued<4>> dlr_coeffs_from_dlr_imfreq(gf_const_view<dlr_imfreq, tensor_valued<4>> g_iw) {
      return dlr_coeffs_from_dlr_imfreq_impl<tensor_valued<4>>(g_iw);
    }
    
    template<typename target_t>
    gf<dlr_imfreq, target_t> dlr_imfreq_from_dlr_coeffs_impl(gf_const_view<dlr_coeffs, target_t> g_dlr) {
      auto g_iw = make_gf<dlr_imfreq>(dlr_imfreq(g_dlr.mesh()), g_dlr.target());
      auto beta = g_dlr.mesh().domain().beta;
      g_iw.data() = beta * g_dlr.mesh().dlr_if().coefs2vals(g_dlr.data());
      return g_iw;
    }

    gf<dlr_imfreq, scalar_valued> dlr_imfreq_from_dlr_coeffs(gf_const_view<dlr_coeffs, scalar_valued> g_dlr) {
      return dlr_imfreq_from_dlr_coeffs_impl<scalar_valued>(g_dlr);
    }

    gf<dlr_imfreq, matrix_valued> dlr_imfreq_from_dlr_coeffs(gf_const_view<dlr_coeffs, matrix_valued> g_dlr) {
      return dlr_imfreq_from_dlr_coeffs_impl<matrix_valued>(g_dlr);
    }
    
    gf<dlr_imfreq, tensor_valued<4>> dlr_imfreq_from_dlr_coeffs(gf_const_view<dlr_coeffs, tensor_valued<4>> g_dlr) {
      return dlr_imfreq_from_dlr_coeffs_impl<tensor_valued<4>>(g_dlr);
    }    

  } // namespace gfs
} // namespace triqs
