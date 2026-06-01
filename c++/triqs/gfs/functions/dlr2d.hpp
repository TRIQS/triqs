// Copyright (c) 2025 Simons Foundation
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
// Authors: Nils Wentzell

#pragma once
#include "triqs/gfs/gf/gf.hpp"
#include "triqs/gfs/block/block_gf.hpp"
#include "../../mesh/dlr2d.hpp"
#include "../../mesh/dlr2d_imfreq.hpp"
#include "../../mesh/imfreq.hpp"
#include "../../mesh/prod.hpp"
#include <nda/blas/gemm.hpp>
#include <nda/blas/gemv.hpp>
#include <nda/layout_transforms.hpp>

namespace triqs::gfs {

  using mesh::dlr2d;
  using mesh::dlr2d_imfreq;

  //-------------------------------------------------------
  // Transformation of 2D DLR Green's functions
  // ------------------------------------------------------

  /// Transform a 2D DLR Matsubara Green's function to its 2D DLR coefficient representation
  template <int N = 0, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr2d(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr2d<0>(gbl); }, g);
    } else {
      static_assert(std::is_same_v<M, dlr2d_imfreq>, "Input mesh must be dlr2d_imfreq");
      auto result = gf{dlr2d{g.mesh()}, g.target_shape()};

      auto const &cf2if    = g.mesh().cf2if();
      auto const &dlr2d_rf = g.mesh().dlr2d_rf();
      long rank            = g.mesh().rank();

      if constexpr (G::target_rank == 0) {
        // Scalar-valued case
        auto [f_reg, f_sing] = ::cppdlr2d::vals2coefs(static_cast<int>(rank), cf2if, g.data(), dlr2d_rf);
        result.data()        = mesh::detail::reg_sing_to_flat(dlr2d_rf, f_reg, f_sing);
      } else {
        // Tensor-valued case: batch all target elements with a single LAPACK solve
        long n_if = g.mesh().size();
        long n_rf = result.mesh().size();
        long nrhs = g.data().size() / n_if;

        // Pack into F-layout (n_if, nrhs) array for vals2coefs_many
        auto vals_flat = nda::array<dcomplex, 2, nda::F_layout>(n_if, nrhs);
        auto data_flat = nda::reshape(g.data(), n_if, nrhs);
        for (long i = 0; i < n_if; ++i)
          for (long j = 0; j < nrhs; ++j) vals_flat(i, j) = data_flat(i, j);

        auto [f_reg_all, f_sing_all] = ::cppdlr2d::vals2coefs_many(static_cast<int>(rank), cf2if, vals_flat, dlr2d_rf);

        // Unpack: f_reg_all(j, term, k, l) and f_sing_all(j, k) -> result.data()(i, ...)
        auto result_flat = nda::reshape(result.data(), n_rf, nrhs);
        for (long j = 0; j < nrhs; ++j) {
          for (long i = 0; i < n_rf; ++i) {
            int term_type  = dlr2d_rf(i, 0);
            int k_idx      = dlr2d_rf(i, 1);
            int l_idx      = dlr2d_rf(i, 2);
            result_flat(i, j) = (term_type < 3) ? f_reg_all(j, term_type, k_idx, l_idx) : f_sing_all(j, k_idx);
          }
        }
      }
      return result;
    }
  }

  /// Transform a 2D DLR coefficient Green's function to its 2D DLR Matsubara frequency representation
  template <int N = 0, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr2d_imfreq(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr2d_imfreq<0>(gbl); }, g);
    } else {
      static_assert(std::is_same_v<M, dlr2d>, "Input mesh must be dlr2d");
      auto result = gf{dlr2d_imfreq{g.mesh()}, g.target_shape()};

      auto const &cf2if = g.mesh().cf2if();

      if constexpr (G::target_rank == 0) {
        // Scalar-valued case: vals = cf2if * coefs
        auto coefs = nda::vector<dcomplex>(g.data());
        nda::blas::gemv(dcomplex{1.0}, cf2if, coefs, dcomplex{0.0}, result.data());
      } else {
        // Tensor-valued case: process element by element
        nda::for_each(g.target_shape(), [&](auto... is) {
          auto coefs = nda::vector<dcomplex>(g.data()(nda::range::all, is...));
          auto vals  = nda::vector<dcomplex>(cf2if.shape(0));
          nda::blas::gemv(dcomplex{1.0}, cf2if, coefs, dcomplex{0.0}, vals);
          result.data()(nda::range::all, is...) = vals;
        });
      }
      return result;
    }
  }

  /// Evaluate a 2D DLR coefficient Green's function on a Matsubara frequency product mesh.
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> and std::is_same_v<typename G::mesh_t, dlr2d>)
  auto make_gf_imfreq(G const &g, long n_iw = 0) {
    static_assert(N == 0 and sizeof...(Ns) == 0);
    auto const &m = g.mesh();

    // Default n_iw: cover all DLR2D imfreq points
    if (n_iw == 0) n_iw = dlr2d_imfreq{m}.max_n() + 1;

    // Build target product mesh
    mesh::imfreq iw_mesh{m.beta(), Fermion, n_iw};
    long n_iw_size = iw_mesh.size(); // = 2*n_iw
    long n_target  = n_iw_size * n_iw_size;

    // Build target_if in PP convention (what build_cf2if expects)
    auto target_if = nda::array<int, 2>(n_target, 2);
    long idx       = 0;
    for (auto iw1 : iw_mesh) {
      int n1_pp = (m.channel() == mesh::PH) ? static_cast<int>(-iw1.index() - 1) : static_cast<int>(iw1.index());
      for (auto iw2 : iw_mesh) {
        target_if(idx, 0) = n1_pp;
        target_if(idx, 1) = static_cast<int>(iw2.index());
        ++idx;
      }
    }

    // Build evaluation matrix: (n_target, n_coefs), F_layout
    auto cf2if_target = ::cppdlr2d::build_cf2if(m.beta(), m.dlr_rf(), target_if, m.dlr2d_rf());

    // Evaluate
    mesh::prod<mesh::imfreq, mesh::imfreq> prod_mesh{iw_mesh, iw_mesh};
    auto result = gf{prod_mesh, g.target_shape()};

    if constexpr (G::target_rank == 0) {
      auto coefs = nda::vector<dcomplex>(g.data());
      auto vals  = nda::vector<dcomplex>(n_target);
      nda::blas::gemv(dcomplex{1.0}, cf2if_target, coefs, dcomplex{0.0}, vals);
      result.data()() = nda::reshape(vals, n_iw_size, n_iw_size);
    } else {
      nda::for_each(g.target_shape(), [&](auto... is) {
        auto coefs = nda::vector<dcomplex>(g.data()(nda::range::all, is...));
        auto vals  = nda::vector<dcomplex>(n_target);
        nda::blas::gemv(dcomplex{1.0}, cf2if_target, coefs, dcomplex{0.0}, vals);
        result.data()(nda::range::all, nda::range::all, is...) = nda::reshape(vals, n_iw_size, n_iw_size);
      });
    }
    return result;
  }

  /// Evaluate a 2D DLR coefficient block Green's function on a Matsubara frequency product mesh.
  template <int N = 0, int... Ns, typename G>
    requires(is_block_gf_v<G> and std::is_same_v<typename G::g_t::mesh_t, dlr2d>)
  auto make_gf_imfreq(G const &g, long n_iw = 0) {
    static_assert(N == 0 and sizeof...(Ns) == 0);
    return map_block_gf([&](auto const &gbl) { return make_gf_imfreq(gbl, n_iw); }, g);
  }

} // namespace triqs::gfs
