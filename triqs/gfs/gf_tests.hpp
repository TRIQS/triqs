/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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

 /// check tails are close (does nothing)
 /**
   * @tparam TailType a tail type (incl. nothing)
   */
 template<typename T1, typename T2>
 void assert_tails_are_close(T1 const &x, T2 const &y, double precision) {
  static_assert(std::is_same<typename T1::const_view_type, typename T2::const_view_type>::value, "Tails are of different types");
  if (x.n_valid_orders() != y.n_valid_orders())
   TRIQS_RUNTIME_ERROR << "Tails have different n_valid_orders: "<<x.n_valid_orders() << " and " << y.n_valid_orders() ;
  auto n = x.n_valid_orders();
  if (n <= 0) return;
  auto _ = range(0, n);
  if (max_element(abs(x.data()(_,ellipsis()) - y.data()(_,ellipsis()))) > precision) TRIQS_RUNTIME_ERROR << "Tails have different data"<< x.data() << y.data();
 }

 inline void assert_tails_are_close(nothing, nothing, double) {}

 // for m_tail which are a tail for g(k,om) and also a g(k) -> tail_valued.
 template <typename M, typename Ta>
 void assert_tails_are_close(gf_const_view<M, Ta> const &x, gf_const_view<M, Ta> const &y, double precision) {
  assert_gfs_are_close(x, y, precision);
 }
 template <typename M, typename Ta> void assert_tails_are_close(gf<M, Ta> const &x, gf<M, Ta> const &y, double precision) {
  assert_gfs_are_close(x, y, precision);
 }

 // check gfs are close
 template<typename X, typename Y>
 void assert_gfs_are_close(X const &x, Y const &y, double precision) {

  // meshes must be identical and data close
  if (x.mesh() != y.mesh()) TRIQS_RUNTIME_ERROR << "GFs have different meshes";
  if (max_element(abs(x.data() - y.data())) > precision) TRIQS_RUNTIME_ERROR << "Gfs have different data. max(abs(x-y)) = " << max_element(abs(x.data() - y.data())) ;

  // tails must be close
  assert_tails_are_close(x.singularity(), y.singularity(), precision);

 }

 // check block gfs are close
 template<typename X, typename Y>
 void assert_block_gfs_are_close(X const &x, Y const &y, double precision) {

  if (x.size() != y.size()) TRIQS_RUNTIME_ERROR << "Block GFs have different number of blocks";
  for (int u = 0; u < x.size(); ++u) assert_gfs_are_close(x[u], y[u], precision);

 }

}}
