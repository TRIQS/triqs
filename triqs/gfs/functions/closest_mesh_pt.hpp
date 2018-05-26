/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet
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

 
 //-------------------------------------------------------
 // closest mesh point on the grid
 // ------------------------------------------------------

 template <typename Target> struct gf_closest_point<imtime, Target> {
  // index_t is int
  template <typename M, typename T> static int invoke(M const &mesh, closest_pt_wrap<T> const &p) {
   double x = double(p.value) + 0.5 * mesh.delta();
   int n = std::floor(x / mesh.delta());
   return n;
  }
 };

 //-------------------------------------------------------
 // closest mesh point on the grid
 // ------------------------------------------------------

  struct gf_closest_point_linear_mesh{
  // index_t is int
  template <typename M, typename T> static int invoke(M const &mesh, closest_pt_wrap<T> const &p) {
   double x = double(p.value) - mesh.x_min() + 0.5 * mesh.delta();
   int n = std::floor(x / mesh.delta());
   return n;
  }
 };

 //-------------------------------------------------------
 // For all meshes represented by a linear grid, the code is the same

 //template <typename Target> struct gf_closest_point<imtime, Target> : gf_closest_point_linear_mesh{};
 template <typename Target> struct gf_closest_point<retime, Target> : gf_closest_point_linear_mesh{};
 template <typename Target> struct gf_closest_point<refreq, Target> : gf_closest_point_linear_mesh{};


}
}


