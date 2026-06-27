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
 * @brief Provides the CLEF auto-assignment hooks for block Green's functions.
 */

#pragma once

#include "./block_gf.hpp"

#include <utility>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-clef
   * @{
   */

  // auto assignment of the gf (gf(om_) << expression fills the functions by evaluation of expression)

  /**
   * @brief CLEF auto-assignment into a block Green's function view, i.e. `g(om_) << expr`.
   *
   * @details Iterates over the blocks and fills each block with the evaluation of the right hand side at the block
   * index.
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @param g Block Green's function view to fill.
   * @param rhs CLEF expression to evaluate at each block.
   */
  template <typename RHS, typename M, typename T> void clef_auto_assign(block_gf_view<M, T> g, RHS const &rhs) {
    for (int i = 0; i < g.size(); ++i) triqs_bgf_clef_auto_assign_impl_aux_assign(g[i], rhs(i));
  }

  /**
   * @brief CLEF auto-assignment into a two-index block Green's function view, i.e. `g(i_, j_) << expr`.
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @param g Two-index block Green's function view to fill.
   * @param rhs CLEF expression to evaluate at each block.
   */
  template <typename RHS, typename M, typename T> void clef_auto_assign(block2_gf_view<M, T> g, RHS const &rhs) {
    for (int i = 0; i < g.size1(); ++i)
      for (int j = 0; j < g.size2(); ++j) triqs_bgf_clef_auto_assign_impl_aux_assign(g(i, j), rhs(i, j));
  }

  /**
   * @brief CLEF auto-assignment into a (owning) block Green's function (delegates to the view overload).
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @param g Block Green's function to fill.
   * @param rhs CLEF expression to evaluate at each block.
   */
  template <typename RHS, typename M, typename T> void clef_auto_assign(block_gf<M, T> &g, RHS const &rhs) { clef_auto_assign(g(), rhs); }

  /**
   * @brief CLEF auto-assignment into a (owning) two-index block Green's function (delegates to the view overload).
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @param g Two-index block Green's function to fill.
   * @param rhs CLEF expression to evaluate at each block.
   */
  template <typename RHS, typename M, typename T> void clef_auto_assign(block2_gf<M, T> &g, RHS const &rhs) { clef_auto_assign(g(), rhs); }

  /**
   * @brief CLEF auto-assignment via subscript into a block Green's function view, i.e. `g[om_] << expr`.
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @param g Block Green's function view to fill.
   * @param rhs CLEF expression to evaluate at each block.
   */
  template <typename RHS, typename M, typename T> void clef_auto_assign_subscript(block_gf_view<M, T> g, RHS const &rhs) { clef_auto_assign(g, rhs); }

  /**
   * @brief CLEF auto-assignment via subscript into a (owning) block Green's function (delegates to the view overload).
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @param g Block Green's function to fill.
   * @param rhs CLEF expression to evaluate at each block.
   */
  template <typename RHS, typename M, typename T> void clef_auto_assign_subscript(block_gf<M, T> &g, RHS const &rhs) { clef_auto_assign(g(), rhs); }

  // Helper that assigns a (non-CLEF) right hand side to a block element.
  template <typename G, typename RHS> void triqs_bgf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs) {
    std::forward<G>(g) = std::forward<RHS>(rhs);
  }

  // Helper overload that recurses into a CLEF function object right hand side.
  template <typename G, typename Expr, int... Is> void triqs_bgf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs) {
    clef_auto_assign(std::forward<G>(g), std::move(rhs));
  }

  /** @} */

} // namespace triqs::gfs
