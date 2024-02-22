// Copyright (c) 2023 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#pragma once
#include "triqs/gfs/gf/gf.hpp"
#include "triqs/gfs/block/block_gf.hpp"
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

  template <int N, int... Ns, typename F, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto apply_to_mesh(F const &f, G const &g) {
    using M = typename G::mesh_t;
    if constexpr (sizeof...(Ns) > 0) {
      return apply_to_mesh<Ns...>(f, apply_to_mesh<N>(f, g));
    } else if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return apply_to_mesh<N>(f, gbl); }, g);
    } else {
      static_assert(mesh::is_product<M>, "requires product mesh");
      auto gfl      = f(flatten_gf_2d<N>(g));
      auto out_mesh = mesh::prod{triqs::tuple::replace<N>(g.mesh(), gfl.mesh())};
      auto g_out    = gf{out_mesh, g.target_shape()};
      unflatten_gf_2d<N>(g_out, gfl);
      return g_out;
    }
  }

  /// Transform a DLR imaginary time or Matsubara Green's function to it's DLR coefficient representation
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr<N, Ns...>(gbl); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr(gfl); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(any_of<M, dlr_imtime, dlr_imfreq>, "Mesh must be dlr_imtime or dlr_imfreq");
      auto result = gf{dlr{g.mesh()}, g.target_shape()};
      if constexpr (std::is_same_v<M, dlr_imtime>)
        result.data() = result.mesh().dlr_it().vals2coefs(g.data());
      else { // dlr_imfreq
        auto beta_inv = 1. / result.mesh().beta();
        result.data() = beta_inv * result.mesh().dlr_if().vals2coefs(g.data());
      }
      return result;
    }
  }

  /// Perform a least square fit of a imaginary time Green's function to obtain a DLR coefficient representation
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto fit_gf_dlr(G const &g, double w_max, double eps) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return fit_gf_dlr<N, Ns...>(gbl, w_max, eps); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return fit_gf_dlr(gfl, w_max, eps); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, mesh::imtime>, "Input mesh must be imtime");
      auto tvals    = nda::array_adapter(std::array{g.mesh().size()}, [&](auto i) { return g.mesh()[i].value() / g.mesh().beta(); });
      auto mesh     = dlr{g.mesh().beta(), g.mesh().statistic(), w_max, eps};
      auto result   = gf{mesh, g.target_shape()};
      result.data() = result.mesh().dlr_it().fitvals2coefs(make_regular(tvals), g.data());
      return result;
    }
  }

  /// Transform a DLR coefficient Green's function to it's DLR imaginary time representation
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr_imtime(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr_imtime<N, Ns...>(gbl); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr_imtime(gfl); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, mesh::dlr>, "Input mesh must be dlr");
      auto result   = gf{dlr_imtime{g.mesh()}, g.target_shape()};
      result.data() = g.mesh().dlr_it().coefs2vals(g.data());
      return result;
    }
  }

  /// Transform a DLR coefficient Green's function to it's DLR Matsubara frequency representation
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr_imfreq(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr_imfreq<N, Ns...>(gbl); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr_imfreq(gfl); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, mesh::dlr>, "Input mesh must be dlr");
      auto result   = gf{dlr_imfreq{g.mesh()}, g.target_shape()};
      auto beta     = result.mesh().beta();
      result.data() = beta * g.mesh().dlr_if().coefs2vals(g.data());
      return result;
    }
  }

  /// Transform a DLR coefficient Green's function to a imaginary time Green's function
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_imtime(G const &g, long n_tau) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_imtime<N, Ns...>(gbl, n_tau); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_imtime(gfl, n_tau); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, dlr>, "Input mesh must be dlr");
      auto result = gf{mesh::imtime{g.mesh().beta(), g.mesh().statistic(), n_tau}, g.target_shape()};
      for (auto tau : result.mesh()) result[tau] = g(tau.value());
      return result;
    }
  }

  /// Transform a DLR coefficient Green's function to a Matsubara frequency Green's function
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_imfreq(G const &g, long n_iw) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_imfreq<N, Ns...>(gbl, n_iw); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_imfreq(gfl, n_iw); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, dlr>, "Input mesh must be dlr");
      auto result = gf{mesh::imfreq{g.mesh().beta(), g.mesh().statistic(), n_iw}, g.target_shape()};
      for (auto w : result.mesh()) result[w] = g(w.value());
      return result;
    }
  }

} // namespace triqs::gfs
