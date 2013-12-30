/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include "./mesh_tools.hpp"
#include "../domains/matsubara.hpp"
#include "./linear.hpp"

namespace triqs {
namespace gfs {

 struct matsubara_time_mesh : linear_mesh<matsubara_time_domain> {
  private:
  using B = linear_mesh<matsubara_time_domain>;

  public:
  matsubara_time_mesh() = default;
  matsubara_time_mesh(matsubara_time_mesh const &x)  = default;
  matsubara_time_mesh(matsubara_time_domain d, int n_time_slices, mesh_kind mk = half_bins) : B(d, 0, d.beta, n_time_slices, mk) {}
  matsubara_time_mesh(double beta, statistic_enum S, int n_time_slices, mesh_kind mk = half_bins)
     : matsubara_time_mesh({beta, S}, n_time_slices, mk) {}
 };


 //-------------------------------------------------------

 /** \brief foreach for this mesh
  *
  *  @param m : a mesh
  *  @param F : a function of synopsis auto F (matsubara_time_mesh::mesh_point_t)
  *
  *  Calls F on each point of the mesh, in arbitrary order.
  **/
 template <typename Lambda> void foreach(matsubara_time_mesh const &m, Lambda F) {
  for (auto const &w : m) F(w);
 }
}
}

