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

 struct imtime {};

 template <> struct gf_mesh<imtime> : linear_mesh<matsubara_time_domain> {
  private:
  using B = linear_mesh<matsubara_time_domain>;

  public:
  gf_mesh() = default;
  gf_mesh(gf_mesh const &x)  = default;
  gf_mesh(matsubara_time_domain d, int n_time_slices) : B(d, 0, d.beta, n_time_slices) {}
  gf_mesh(double beta, statistic_enum S, int n_time_slices) : gf_mesh({beta, S}, n_time_slices) {}
 };

 //-------------------------------------------------------

 /** \brief foreach for this mesh
  *
  *  @param m : a mesh
  *  @param F : a function of synopsis auto F (matsubara_time_mesh::mesh_point_t)
  *
  *  Calls F on each point of the mesh, in arbitrary order.
  **/
 template <typename Lambda> void foreach(gf_mesh<imtime> const &m, Lambda F) {
  for (auto const &w : m) F(w);
 }
}
}

