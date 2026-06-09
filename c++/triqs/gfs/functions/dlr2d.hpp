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
#include "dlr.hpp"
#include "../../mesh/dlr2d.hpp"
#include "../../mesh/dlr2d_imfreq.hpp"
#include "../../mesh/prod.hpp"
#include <nda/blas/gemm.hpp>
#include <nda/blas/gemv.hpp>
#include <nda/layout_transforms.hpp>

namespace triqs::gfs {

  using mesh::dlr2d;
  using mesh::dlr2d_imfreq;

  namespace detail {
    // Replace element N of a tuple with two new elements r1, r2.
    // E.g. tuple<A, B, C> with N=1 becomes tuple<A, R1, R2, C>.
    template <size_t N, typename... Ms, typename R1, typename R2> auto tuple_replace_expand(std::tuple<Ms...> const &t, R1 const &r1, R2 const &r2) {
      auto before = [&]<size_t... Is>(std::index_sequence<Is...>) { return std::make_tuple(std::get<Is>(t)...); }(std::make_index_sequence<N>{});
      auto after  = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(std::get<N + 1 + Is>(t)...);
      }(std::make_index_sequence<sizeof...(Ms) - N - 1>{});
      return std::tuple_cat(before, std::make_tuple(r1, r2), after);
    }
  } // namespace detail

  //-------------------------------------------------------
  // Transformation of 2D DLR Green's functions
  // ------------------------------------------------------

  /// Transform a 2D DLR Matsubara Green's function to its 2D DLR coefficient representation
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr2d(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr2d<N, Ns...>(gbl); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr2d(gfl); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
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

        // Unpack: f_reg_all(term, k, l, j) and f_sing_all(k, j) -> result.data()(i, ...)
        auto result_flat = nda::reshape(result.data(), n_rf, nrhs);
        for (long j = 0; j < nrhs; ++j) {
          for (long i = 0; i < n_rf; ++i) {
            int term_type     = dlr2d_rf(i, 0);
            int k_idx         = dlr2d_rf(i, 1);
            int l_idx         = dlr2d_rf(i, 2);
            result_flat(i, j) = (term_type < 3) ? f_reg_all(term_type, k_idx, l_idx, j) : f_sing_all(k_idx, j);
          }
        }
      }
      return result;
    }
  }

  /// Transform a 2D DLR coefficient Green's function to its 2D DLR Matsubara frequency representation
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr2d_imfreq(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr2d_imfreq<N, Ns...>(gbl); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr2d_imfreq(gfl); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, dlr2d>, "Input mesh must be dlr2d");
      auto result = gf{dlr2d_imfreq{g.mesh()}, g.target_shape()};

      auto const &cf2if = g.mesh().cf2if();
      long n_if         = result.mesh().size();
      long n_coefs      = g.mesh().size();

      if constexpr (G::target_rank == 0) {
        // Scalar-valued case: vals = cf2if * coefs
        auto coefs = nda::vector<dcomplex>(g.data());
        nda::blas::gemv(dcomplex{1.0}, cf2if, coefs, dcomplex{0.0}, result.data());
      } else {
        // Tensor-valued case: single gemm
        long nrhs = g.data().size() / n_coefs;

        auto coefs = nda::matrix<dcomplex, nda::F_layout>(n_coefs, nrhs);
        auto vals  = nda::matrix<dcomplex, nda::F_layout>(n_if, nrhs);

        auto data_flat = nda::reshape(g.data(), n_coefs, nrhs);
        for (long i = 0; i < n_coefs; ++i)
          for (long j = 0; j < nrhs; ++j) coefs(i, j) = data_flat(i, j);

        nda::blas::gemm(dcomplex{1.0}, cf2if, coefs, dcomplex{0.0}, vals);

        auto result_flat = nda::reshape(result.data(), n_if, nrhs);
        for (long i = 0; i < n_if; ++i)
          for (long j = 0; j < nrhs; ++j) result_flat(i, j) = vals(i, j);
      }
      return result;
    }
  }

  /// Evaluate a 2D DLR coefficient Green's function on a Matsubara frequency product mesh.
  // Note: Product mesh support for dlr2d requires special handling because the transformation
  // produces a 2D output (prod<imfreq, imfreq>) from a 1D input (dlr2d). The standard apply_to_mesh
  // helper doesn't support this 1D->2D transformation pattern.
  template <int N = 0, int... Ns, typename G>
    requires HasDlr2dMeshesAt<G, N, Ns...>
  auto make_gf_imfreq(G const &g, long n_iw = 0) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_imfreq<N, Ns...>(gbl, n_iw); }, g);
    } else if constexpr (mesh::is_product<M>) {
      static_assert(sizeof...(Ns) == 0, "Multiple DLR2D index expansion not supported");
      static_assert(std::is_same_v<detail::mesh_at_t<N, M>, dlr2d>, "Mesh at position N must be dlr2d");

      auto const &dlr2d_m = get_mesh<N>(g);
      long n_coefs        = dlr2d_m.size();

      if (n_iw == 0) n_iw = dlr2d_imfreq{dlr2d_m}.max_n() + 1;
      mesh::imfreq iw_mesh{dlr2d_m.beta(), dlr2d_m.statistic(), n_iw};
      long n_iw_size = iw_mesh.size();

      // Build output mesh: splice (imfreq, imfreq) at position N
      auto out_mesh = mesh::prod{detail::tuple_replace_expand<N>(g.mesh().components(), iw_mesh, iw_mesh)};
      auto result   = gf{out_mesh, g.target_shape()};

      // Reshape data: group dims before N, dim N (coefs), dims after N + target dims
      auto shape    = g.data().shape();
      long n_before = 1;
      for (int d = 0; d < N; ++d) n_before *= shape[d];
      long n_after = 1;
      for (size_t d = N + 1; d < shape.size(); ++d) n_after *= shape[d];
      long nrhs = n_before * n_after;

      // Pack: (n_before, n_coefs, n_after) -> (n_coefs, nrhs) transposed for cppdlr2d
      auto data_3d = nda::reshape(g.data(), n_before, n_coefs, n_after);
      auto gc      = nda::matrix<dcomplex>(n_coefs, nrhs);
      for (long ib = 0; ib < n_before; ++ib)
        for (long c = 0; c < n_coefs; ++c)
          for (long ia = 0; ia < n_after; ++ia) gc(c, ib * n_after + ia) = data_3d(ib, c, ia);

      // Evaluate on imfreq grid
      int idx_min = -static_cast<int>(n_iw);
      int idx_max = static_cast<int>(n_iw) - 1;
      auto vals   = ::cppdlr2d::coefs2eval_if_grid(dlr2d_m.beta(), dlr2d_m.dlr_rf(), gc, dlr2d_m.dlr2d_rf(), idx_min, idx_max, idx_min, idx_max,
                                                   dlr2d_m.channel());
      // vals: (nrhs, n_iw_size, n_iw_size)

      // Unpack: (nrhs, niw, niw) -> (n_before, niw, niw, n_after)
      auto result_4d = nda::reshape(result.data(), n_before, n_iw_size, n_iw_size, n_after);
      for (long ib = 0; ib < n_before; ++ib)
        for (long i1 = 0; i1 < n_iw_size; ++i1)
          for (long i2 = 0; i2 < n_iw_size; ++i2)
            for (long ia = 0; ia < n_after; ++ia) result_4d(ib, i1, i2, ia) = vals(ib * n_after + ia, i1, i2);

      return result;
    } else {
      static_assert(N == 0 and sizeof...(Ns) == 0);
      static_assert(std::is_same_v<M, dlr2d>, "Input mesh must be dlr2d");
      auto const &m = g.mesh();

      // Default n_iw: cover all DLR2D imfreq points
      if (n_iw == 0) n_iw = dlr2d_imfreq{m}.max_n() + 1;

      // Build target product mesh
      mesh::imfreq iw_mesh{m.beta(), m.statistic(), n_iw};
      mesh::prod<mesh::imfreq, mesh::imfreq> prod_mesh{iw_mesh, iw_mesh};
      auto result = gf{prod_mesh, g.target_shape()};

      long n_iw_size = iw_mesh.size();
      long n_coefs   = m.size();
      long nrhs      = g.data().size() / n_coefs;

      // Grid range: indices from -n_iw to n_iw-1
      int idx_min = -static_cast<int>(n_iw);
      int idx_max = static_cast<int>(n_iw) - 1;

      // Use flat coefficient version of coefs2eval_if_grid
      auto gc_flat = nda::reshape(g.data(), n_coefs, nrhs);
      auto vals    = ::cppdlr2d::coefs2eval_if_grid(m.beta(), m.dlr_rf(), gc_flat, m.dlr2d_rf(), idx_min, idx_max, idx_min, idx_max, m.channel());

      // Unpack: vals is (nrhs, nm, nn) -> result is (nm, nn, ...)
      auto result_flat = nda::reshape(result.data(), n_iw_size, n_iw_size, nrhs);
      for (long j = 0; j < nrhs; ++j) result_flat(nda::range::all, nda::range::all, j) = vals(j, nda::range::all, nda::range::all);

      return result;
    }
  }

} // namespace triqs::gfs
