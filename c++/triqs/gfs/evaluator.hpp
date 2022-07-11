// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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

namespace triqs::gfs {

  // evaluator by default forwards everything to evaluate
  // specialize e.g. for tail where gf data is used

  /*----------------------------------------------------------
  *  Default
  *--------------------------------------------------------*/

  template <Mesh M> struct gf_evaluator {

    template <typename G, typename... XS> requires(is_gf_v<G>)
    auto operator()(G const &g, XS &&... xs) const {
      auto l = [&g](auto &&...ys) -> decltype(auto) { return g.operator[](ys...); };
      using r_t = decltype(make_regular(evaluate(g.mesh(), l, std::forward<XS>(xs)...)));
      if constexpr (nda::Array<r_t> or nda::is_scalar_v<r_t>) {
        // Return zero if any mesh evaluates to zero
        if (detail::eval_to_zero(g.mesh(), xs...)) {
          return r_t{nda::zeros<typename G::target_t::scalar_t>(g.target_shape())};
	}
      }
      return make_regular(evaluate(g.mesh(), l, std::forward<XS>(xs)...));
    }
  };

  /*----------------------------------------------------------
   *  mesh::imfreq
   *--------------------------------------------------------*/

  template <> struct gf_evaluator<mesh::imfreq> {

    template <typename G>
    auto operator()(G const &g, matsubara_freq const &f) const -> typename G::target_t::value_t {

      if (g.mesh().is_idx_valid(f.n)) return g[f.n];
      if (g.mesh().positive_only()) {
        int sh = (g.mesh().statistic == Fermion ? 1 : 0);
        if (g.mesh().is_idx_valid(-f.n - sh)) return conj(g[-f.n - sh]);
        TRIQS_RUNTIME_ERROR << " ERROR: Cannot evaluate Green function with positive only mesh outside grid ";
      }

      auto [tail, err] = fit_tail_no_normalize(g);
      dcomplex x       = std::abs(g.mesh().w_max()) / f;
      auto res   = nda::zeros<dcomplex>(g.target_shape()); // a new array

      dcomplex z = 1.0;
      for (int n : range(tail.extent(0))) {
        res += tail(n, ellipsis()) * z;
        z = z * x;
      }
      return res;
    }

    // int -> replace by matsubara_freq
    template <typename G> decltype(auto) operator()(G const &g, int n) const { return g(matsubara_freq(n, g.mesh().beta, g.mesh().statistic)); }
  };

} // namespace triqs::gfs
