// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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

#pragma once

#include <triqs/tb/fourier_polynomial.hpp>
#include <ranges>
#include <tuple>

namespace triqs::gfs {

  // evaluator by default forwards everything to evaluate
  // specialize e.g. for tail where gf data is used

  /*----------------------------------------------------------
  *  Default
  *--------------------------------------------------------*/

  template <Mesh M> struct gf_evaluator {

    template <typename G, typename... XS>
      requires(is_gf_v<G>)
    auto operator()(G const &g, XS &&...xs) const {
      auto l    = [&g](auto &&...ys) -> decltype(auto) { return g.operator[](ys...); };
      using r_t = std::decay_t<decltype(make_regular(evaluate(g.mesh(), l, std::forward<XS>(xs)...)))>;
      if constexpr (nda::Array<r_t> or nda::is_scalar_v<r_t>) {
        // Return zero if any mesh evaluates to zero
        if (detail::eval_to_zero(g.mesh(), xs...)) { return r_t{nda::zeros<typename G::target_t::scalar_t>(g.target_shape())}; }
      }
      return make_regular(evaluate(g.mesh(), l, std::forward<XS>(xs)...));
    }
  };

  /*----------------------------------------------------------
   *  mesh::imfreq
   *--------------------------------------------------------*/

  template <> struct gf_evaluator<mesh::imfreq> {

    template <typename G> auto operator()(G const &g, matsubara_freq const &f) const {

      using r_t = std::decay_t<decltype(make_regular(g[0]))>;

      if (g.mesh().is_index_valid(f.n)) return r_t{g[f.n]};
      if (g.mesh().positive_only()) {
        int sh = (g.mesh().statistic() == Fermion ? 1 : 0);
        if (g.mesh().is_index_valid(-f.n - sh)) return r_t{conj(g[-f.n - sh])};
        TRIQS_RUNTIME_ERROR << " ERROR: Cannot evaluate Green function with positive only mesh outside grid ";
      }

      auto [tail, err] = fit_tail_no_normalize(g);
      dcomplex x       = std::abs(g.mesh().w_max()) / f;
      auto res         = r_t{nda::zeros<dcomplex>(g.target_shape())}; // a new array

      dcomplex z = 1.0;
      for (int n : range(tail.extent(0))) {
        res += tail(n, ellipsis()) * z;
        z = z * x;
      }

      return res;
    }

    // int -> replace by matsubara_freq
    template <typename G> decltype(auto) operator()(G const &g, int n) const { return g(matsubara_freq(n, g.mesh().beta(), g.mesh().statistic())); }
  };

  /*----------------------------------------------------------
   *  mesh::fourier_poly — Evaluator for Fourier polynomial GFs.
   *  Dispatches tuple/array k-arguments directly to triqs::tb::fourier_eval,
   *  using the mesh's R_mat and the GF's data array.
   *--------------------------------------------------------*/

  template <> struct gf_evaluator<mesh::fourier_poly> {

    template <typename G, typename X>
      requires(is_gf_v<G>)
    auto operator()(G const &g, X &&x) const {
      using Xd                       = std::decay_t<X>;
      static constexpr int coeff_dim = G::target_t::rank;

      // k-point inputs dispatch to tb::fourier_eval regardless of target rank;
      // index / mesh_point / range::all inputs go through the evaluate() fallback.
      if constexpr (requires { std::get<2>(x); }) {
        std::array<double, 3> k = {std::get<0>(x), std::get<1>(x), std::get<2>(x)};
        return tb::fourier_eval<coeff_dim, 3>(g.mesh().R_mat(), g.data(), k);
      } else if constexpr (nda::MemoryArrayOfRank<Xd, 2>) {
        return tb::fourier_eval<coeff_dim>(g.mesh().R_mat(), g.data(), nda::array_const_view<double, 2>(x));
      } else if constexpr (std::ranges::range<Xd>
                           and requires(std::ranges::range_value_t<Xd> v) {
                                 requires std::tuple_size_v<decltype(v)> == 3;
                                 requires nda::AnyOf<typename decltype(v)::value_type, double, float>;
                               }) {
        return tb::fourier_eval<coeff_dim, 3>(g.mesh().R_mat(), g.data(), x);
      } else {
        auto l = [&g](auto &&...ys) -> decltype(auto) { return g[ys...]; };
        return make_regular(evaluate(g.mesh(), l, std::forward<X>(x)));
      }
    }
  };
} // namespace triqs::gfs
