/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by O. Parcollet
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

 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign(gf_view<M, T> g, RHS const &rhs) {
  triqs_clef_auto_assign_impl(g, rhs, typename std::is_base_of<tag::composite, gf_mesh<M>>::type());
  assign_singularity_from_function(g.singularity(), rhs);
 }

 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign(gf<M, T> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g(), rhs);
 }

 // enable the writing g[om_] << .... also
 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign_subscript(gf_view<M, T> g, RHS const &rhs) {
  triqs_clef_auto_assign(g, rhs);
 }

 template <typename RHS, typename M, typename T> void triqs_clef_auto_assign_subscript(gf<M, T> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g(), rhs);
 }

 template <typename G, typename RHS> void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs) {
  std::forward<G>(g) = std::forward<RHS>(rhs);
 }

 template <typename G, typename Expr, int... Is>
 void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs) {
  triqs_clef_auto_assign(std::forward<G>(g), std::forward<clef::make_fun_impl<Expr, Is...>>(rhs));
 }

 template <typename G, typename RHS> void triqs_clef_auto_assign_impl(G &g, RHS const &rhs, std::false_type) {
  for (auto const &w : g.mesh()) { triqs_gf_clef_auto_assign_impl_aux_assign(g[w], rhs(w)); }
 }

 template <typename G, typename RHS> void triqs_clef_auto_assign_impl(G &g, RHS const &rhs, std::true_type) {
  for (auto const &w : g.mesh()) {
   triqs_gf_clef_auto_assign_impl_aux_assign(g[w], triqs::tuple::apply(rhs, w.components_tuple()));
  }
 }
} // triqs::gfs
}

