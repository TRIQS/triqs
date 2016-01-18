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
#include "../gf_classes.hpp"

namespace triqs {
namespace gfs {

 /// check tails are close (does nothing)
 /**
   * @tparam TailType a tail type (incl. tail_zero and nothing)
   */
 template<typename TailType>
 void assert_tails_are_close(TailType const &x, TailType const &y, double precision) {}

 /// check tails are close (if TailType==tail, compare min/max orders and data)
 void assert_tails_are_close(tail const &x, tail const &y, double precision) {

  if (x.order_min() != y.order_min()) TRIQS_RUNTIME_ERROR << "Tails do not have the same order_min";
  if (x.order_max() != y.order_max()) TRIQS_RUNTIME_ERROR << "Tails do not have the same order_max";
  if (max_element(abs(x.data() - y.data())) > precision) TRIQS_RUNTIME_ERROR << "Tails have different data";

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

  if (n_blocks(x) != n_blocks(y)) TRIQS_RUNTIME_ERROR << "Block GFs have different number of blocks";
  for (int u = 0; u < n_blocks(x); ++u) assert_gfs_are_close(x[u], y[u], precision);

 }

}}
