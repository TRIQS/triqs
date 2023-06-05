// Copyright (c) 2023 Simons Foundation
// Copyright (c) 2023 Hugo Strand
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
// Authors: Michel Ferrero, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#pragma once
#include "triqs/gfs/gf/gf.hpp"
#include "../../mesh/imtime.hpp"
#include "../../mesh/imfreq.hpp"
#include "../../mesh/dlr_imtime.hpp"
#include "../../mesh/dlr_imfreq.hpp"
#include "../..//mesh/dlr.hpp"
namespace triqs::gfs {

  using mesh::dlr;
  using mesh::dlr_imfreq;
  using mesh::dlr_imtime;

  //-------------------------------------------------------
  // Transformation of DLR Green's functions
  // ------------------------------------------------------

  /// Transform a DLR imaginary time Green's function to it's DLR coefficient representation
  auto make_gf_dlr(MemoryGf<dlr_imtime> auto const &g) {
    auto result   = gf{dlr{g.mesh()}, g.target_shape()};
    result.data() = result.mesh().dlr_it().vals2coefs(g.data());
    return result;
  }

  /// Transform a DLR Matsubara Green's function to it's DLR coefficient representation
  auto make_gf_dlr(MemoryGf<dlr_imfreq> auto const &g) {
    auto result   = gf{dlr{g.mesh()}, g.target_shape()};
    auto beta_inv = 1. / result.mesh().beta();
    result.data() = beta_inv * result.mesh().dlr_if().vals2coefs(g.data());
    return result;
  }

  /// Perform a least square fit of a imaginary time Green's function to obtain a DLR coefficient representation
  auto make_gf_dlr(MemoryGf<mesh::imtime> auto const &g, double w_max, double eps) {
    auto tvals       = nda::array_adapter(std::array{g.mesh().size()}, [&](auto i){ return g.mesh()[i].value() / g.mesh().beta(); });
    auto mesh        = dlr{g.mesh().beta(), g.mesh().statistic(), w_max, eps};
    auto result      = gf{mesh, g.target_shape()};
    result.data()    = result.mesh().dlr_it().fitvals2coefs(make_regular(tvals), g.data());
    return result;
  }

  /// Transform a DLR coefficient Green's function to it's DLR imaginary time representation
  auto make_gf_dlr_imtime(MemoryGf<dlr> auto const &g) {
    auto result   = gf{dlr_imtime{g.mesh()}, g.target_shape()};
    result.data() = g.mesh().dlr_it().coefs2vals(g.data());
    return result;
  }

  /// Transform a DLR coefficient Green's function to it's DLR Matsubara frequency representation
  auto make_gf_dlr_imfreq(MemoryGf<dlr> auto const &g) {
    auto result   = gf{dlr_imfreq{g.mesh()}, g.target_shape()};
    auto beta     = result.mesh().beta();
    result.data() = beta * g.mesh().dlr_if().coefs2vals(g.data());
    return result;
  }

  /// Transform a DLR coefficient Green's function to a imaginary time Green's function
  auto make_gf_imtime(MemoryGf<dlr> auto const &g, long n_tau) {
    auto result = gf{mesh::imtime{g.mesh().beta(), g.mesh().statistic(), n_tau}, g.target_shape()};
    for (auto const &tau : result.mesh()) result[tau] = g(tau.value());
    return result;
  }

  /// Transform a DLR coefficient Green's  to a Matsubara frequency Green's function
  auto make_gf_imfreq(MemoryGf<dlr> auto const &g, long n_iw) {
    auto result = gf{mesh::imfreq{g.mesh().beta(), g.mesh().statistic(), n_iw}, g.target_shape()};
    for (auto const &w : result.mesh()) result[w] = g(w.value());
    return result;
  }

} // namespace triqs::gfs
