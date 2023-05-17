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
#include "triqs/gfs/gf/gf.hpp"
#include "../../mesh/imtime.hpp"
#include "../../mesh/imfreq.hpp"
#include "../../mesh/dlr_imtime.hpp"
#include "../../mesh/dlr_imfreq.hpp"
#include "../..//mesh/dlr_coeffs.hpp"
namespace triqs::gfs {

  using mesh::dlr_coeffs;
  using mesh::dlr_imfreq;
  using mesh::dlr_imtime;

  //-------------------------------------------------------
  // Transformation of DLR Green's functions
  // ------------------------------------------------------

  auto make_gf_dlr_coeffs(GfMemory<dlr_imtime> auto const &g) {
    using target_t = decltype(auto{g})::target_t;
    //gf<dlr_coeffs, target_t> result{dlr_coeffs{g.mesh()}, g.target_shape()};
    auto result   = gf{dlr_coeffs{g.mesh()}, g.target_shape()};
    result.data() = result.mesh().dlr_it().vals2coefs(g.data());
    return result;
  }

  auto make_gf_dlr_coeffs(GfMemory<dlr_imfreq> auto const &g) {
    using target_t = decltype(auto{g})::target_t;
    gf<dlr_coeffs, target_t> result{dlr_coeffs{g.mesh()}, g.target_shape()};
    auto beta_inv = 1. / result.mesh().beta;
    result.data() = -beta_inv * result.mesh().dlr_if().vals2coefs(g.data());
    return result;
  }

  auto make_gf_dlr_imtime(GfMemory<dlr_coeffs> auto const &g) {
    using target_t = decltype(auto{g})::target_t;
    gf<dlr_imtime, target_t> result{dlr_imtime{g.mesh()}, g.target_shape()};
    result.data() = g.mesh().dlr_it().coefs2vals(g.data());
    return result;
  }

  auto make_gf_dlr_imfreq(GfMemory<dlr_coeffs> auto const &g) {
    using target_t = decltype(auto{g})::target_t;
    gf<dlr_imfreq, target_t> result{dlr_imfreq{g.mesh()}, g.target_shape()};
    auto beta     = result.mesh().beta;
    result.data() = -beta * g.mesh().dlr_if().coefs2vals(g.data());
    return result;
  }

  auto make_gf_imtime(GfMemory<dlr_coeffs> auto const &g) {}

} // namespace triqs::gfs
