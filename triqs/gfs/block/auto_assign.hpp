/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

namespace triqs {
namespace gfs {
 
 /*------------------------------------------------------------------------------------------------------
  *             Interaction with the CLEF library : auto assignment implementation
  *-----------------------------------------------------------------------------------------------------*/

 // auto assignment of the gf (gf(om_) << expression fills the functions by evaluation of expression)

 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign(block_gf_view<M, T> g, RHS const &rhs) {
  for (int i = 0; i < g.size(); ++i) triqs_bgf_clef_auto_assign_impl_aux_assign(g[i], rhs(i));
 }

 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign(block2_gf_view<M, T> g, RHS const &rhs) {
  for (int i = 0; i < g.size1(); ++i)
   for (int j = 0; j < g.size2(); ++j) triqs_bgf_clef_auto_assign_impl_aux_assign(g(i, j), rhs(i, j));
 }

 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign(block_gf<M, T> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g(), rhs);
 }

 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign(block2_gf<M, T> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g(), rhs);
 }

 // enable the writing g[om_] << .... also
 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign_subscript(block_gf_view<M, T> g, RHS const &rhs) {
  triqs_clef_auto_assign(g, rhs);
 }

 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign_subscript(block_gf<M, T> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g(), rhs);
 }

 template <typename G, typename RHS> void triqs_bgf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs) {
  std::forward<G>(g) = std::forward<RHS>(rhs);
 }

 template <typename G, typename Expr, int... Is>
 void triqs_bgf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs) {
  triqs_clef_auto_assign(std::forward<G>(g), std::forward<clef::make_fun_impl<Expr, Is...>>(rhs));
 }

/*
 template <typename G, typename RHS> void triqs_clef_auto_assign_impl_b(G &g, RHS const &rhs, std::true_type) {
  for (int i = 0; i < g.size(); ++i) {
   triqs_bgf_clef_auto_assign_impl_aux_assign(g[i], triqs::tuple::apply(rhs, w.components_tuple()));
  }
 }
*/

}}

