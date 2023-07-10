// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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

namespace triqs {
  namespace gfs {
    /*------------------------------------------------------------------------------------------------------
  *             Interaction with the CLEF library : auto assignment implementation
  *-----------------------------------------------------------------------------------------------------*/

    // auto assignment of the gf (gf(om_) << expression fills the functions by evaluation of expression)

    template <typename G, typename RHS> FORCEINLINE void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs) {
      std::forward<G>(g) = std::forward<RHS>(rhs);
    }

    template <typename G, typename Expr, int... Is>
    FORCEINLINE void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs) {
      clef_auto_assign(std::forward<G>(g), std::forward<clef::make_fun_impl<Expr, Is...>>(rhs));
    }

    template <typename RHS, typename M, typename T, typename L, typename E> FORCEINLINE void clef_auto_assign(gf_view<M, T, L, E> g, RHS const &rhs) {
      if constexpr (mesh::is_product<M>) {
        for (auto w : g.mesh()) { triqs_gf_clef_auto_assign_impl_aux_assign(g[w], triqs::tuple::apply(rhs, w)); }
      } else {
        for (auto w : g.mesh()) { triqs_gf_clef_auto_assign_impl_aux_assign(g[w], rhs(w)); }
      }
    }

    template <typename RHS, typename M, typename T, typename L, typename E> FORCEINLINE void clef_auto_assign(gf<M, T, L, E> &g, RHS const &rhs) {
      clef_auto_assign(g(), rhs);
    }

    // enable the writing g[om_] << .... also
    template <typename RHS, typename M, typename T, typename L, typename E>
    FORCEINLINE void clef_auto_assign_subscript(gf_view<M, T, L, E> g, RHS const &rhs) {
      clef_auto_assign(g, rhs);
    }

    /* template <typename RHS, typename M, typename T, typename L, typename E>*/
    /*FORCEINLINE void clef_auto_assign_subscript(gf<M, T, L, E> const &g, RHS const &rhs) { return 1;} // = delete;*/

    template <typename RHS, typename M, typename T, typename L, typename E>
    FORCEINLINE void clef_auto_assign_subscript(gf<M, T, L, E> &g, RHS const &rhs) {
      clef_auto_assign(g, rhs);
    }

  } // namespace gfs
} // namespace triqs
