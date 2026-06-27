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
 * @brief Provides the CLEF auto-assignment hooks for Green's functions.
 */

#pragma once

#include "./gf.hpp"

#include "../../utility/macros.hpp"
#include "../../utility/tuple_tools.hpp"

#include <utility>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-clef
   * @{
   */

  // auto assignment of the gf (gf(om_) << expression fills the functions by evaluation of expression)

  // Helper that assigns a (non-CLEF) right hand side to a Green's function element.
  template <typename G, typename RHS> FORCEINLINE void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs) {
    std::forward<G>(g) = std::forward<RHS>(rhs);
  }

  // Helper overload that recurses into a CLEF function object right hand side.
  template <typename G, typename Expr, int... Is>
  FORCEINLINE void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs) {
    clef_auto_assign(std::forward<G>(g), std::move(rhs));
  }

  /**
   * @brief CLEF auto-assignment into a Green's function view, i.e. `g(om_) << expr`.
   *
   * @details Iterates over the mesh and fills each mesh point with the evaluation of the right hand side. For product
   * meshes the mesh point is unpacked into the multiple arguments of the expression.
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @tparam L Layout type.
   * @param g Green's function view to fill.
   * @param rhs CLEF expression to evaluate at each mesh point.
   */
  template <typename RHS, typename M, typename T, typename L> FORCEINLINE void clef_auto_assign(gf_view<M, T, L> g, RHS const &rhs) {
    if constexpr (mesh::is_product<M>) {
      for (auto w : g.mesh()) { triqs_gf_clef_auto_assign_impl_aux_assign(g[w], triqs::tuple::apply(rhs, w)); }
    } else {
      for (auto w : g.mesh()) { triqs_gf_clef_auto_assign_impl_aux_assign(g[w], rhs(w)); }
    }
  }

  /**
   * @brief CLEF auto-assignment into a (owning) Green's function (delegates to the view overload).
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @tparam L Layout type.
   * @param g Green's function to fill.
   * @param rhs CLEF expression to evaluate at each mesh point.
   */
  template <typename RHS, typename M, typename T, typename L> FORCEINLINE void clef_auto_assign(gf<M, T, L> &g, RHS const &rhs) {
    clef_auto_assign(g(), rhs);
  }

  /**
   * @brief CLEF auto-assignment via subscript into a Green's function view, i.e. `g[om_] << expr`.
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @tparam L Layout type.
   * @param g Green's function view to fill.
   * @param rhs CLEF expression to evaluate at each mesh point.
   */
  template <typename RHS, typename M, typename T, typename L> FORCEINLINE void clef_auto_assign_subscript(gf_view<M, T, L> g, RHS const &rhs) {
    clef_auto_assign(g, rhs);
  }

  /* template <typename RHS, typename M, typename T, typename L>*/
  /*FORCEINLINE void clef_auto_assign_subscript(gf<M, T, L> const &g, RHS const &rhs) { return 1;} // = delete;*/

  /**
   * @brief CLEF auto-assignment via subscript into a (owning) Green's function (delegates to the view overload).
   *
   * @tparam RHS Type of the CLEF expression on the right hand side.
   * @tparam M Mesh type.
   * @tparam T Target type.
   * @tparam L Layout type.
   * @param g Green's function to fill.
   * @param rhs CLEF expression to evaluate at each mesh point.
   */
  template <typename RHS, typename M, typename T, typename L> FORCEINLINE void clef_auto_assign_subscript(gf<M, T, L> &g, RHS const &rhs) {
    clef_auto_assign(g, rhs);
  }

  /** @} */

} // namespace triqs::gfs
