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

/**
 * @file
 * @brief Provides the gf_evaluator that evaluates a Green's function at arbitrary mesh points.
 */

#pragma once

#include "./functions/functions2.hpp"
#include "./gf/defs.hpp"
#include "./gf/gf.hpp"

#include "../mesh/imfreq.hpp"

namespace triqs::gfs {

  // evaluator by default forwards everything to evaluate
  // specialize e.g. for tail where gf data is used

  /**
   * @addtogroup triqs-gfs-evaluation
   * @{
   */

  /**
   * @brief Default evaluator of a Green's function at arbitrary points of its mesh.
   *
   * @details The primary template forwards the evaluation to the mesh's `evaluate` function, which interpolates
   * the underlying data. It is specialized for specific meshes (e.g. triqs::mesh::imfreq) where evaluation
   * outside the stored grid requires dedicated handling.
   *
   * @tparam M The mesh type of the Green's function.
   */
  template <Mesh M> struct gf_evaluator {

    /**
     * @brief Evaluate a Green's function at a set of mesh coordinates.
     *
     * @fn auto gf_evaluator::operator()(G const &g, XS &&...xs) const
     * @details Interpolates the data of `g` via the mesh's `evaluate` function. If any of the coordinates makes
     * a mesh evaluate to zero, a zero-valued result of the appropriate target shape is returned.
     *
     * @tparam G The Green's function type (must satisfy `is_gf_v`).
     * @tparam XS The types of the evaluation coordinates.
     * @param g The Green's function to evaluate.
     * @param xs The coordinates at which to evaluate, one per mesh component.
     * @return The interpolated value at the given coordinates.
     */
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

  // Evaluator specialization for Matsubara-frequency Green's functions (internal; the primary gf_evaluator is the
  // documented API). Off the stored grid it uses the analytic structure: on a positive-only mesh negative
  // frequencies are reconstructed by conjugation, and outside the grid the high-frequency tail is fitted and summed.
  template <> struct gf_evaluator<mesh::imfreq> {

    // Evaluate at a Matsubara frequency: stored value on the grid, conjugate on a positive-only mesh, else tail sum.
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

    // Integer overload: convert n to a matsubara_freq and delegate to the matsubara_freq overload.
    template <typename G> decltype(auto) operator()(G const &g, int n) const { return g(matsubara_freq(n, g.mesh().beta(), g.mesh().statistic())); }
  };

  /*----------------------------------------------------------
   *  mesh::dlr2d
   *--------------------------------------------------------*/

  template <> struct gf_evaluator<mesh::dlr2d> {

    // Handle pair of matsubara_freq: G({iw1, iw2})
    template <typename G> auto operator()(G const &g, std::pair<matsubara_freq, matsubara_freq> const &iw_pair) const {
      auto l = [&g](auto i) -> decltype(auto) { return g[i]; };
      return make_regular(evaluate(g.mesh(), l, iw_pair));
    }
  };

  /** @} */

} // namespace triqs::gfs
