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
  using B = linear_mesh<matsubara_time_domain>;

  gf_mesh() = default;
  gf_mesh(gf_mesh const &x) = default;
  gf_mesh(matsubara_time_domain d, int n_time_slices) : B(d, 0, d.beta, n_time_slices) {}
  gf_mesh(double beta, statistic_enum S, int n_time_slices) : gf_mesh({beta, S}, n_time_slices) {}

  // redefine the interpolation to handle the anti-periodicity of fermions.
  // It also reduces tau to [0,beta]
  // because the get_interpolation_data is not virtual, to preserve inlining
  // I simply rewrite evaluate again, it is short (same code as linear).
  // it is necessary to do it as the level of the get_interpolation_data
  // in order e.g. g(x,tau) to behave properly
  B::interpol_data_t get_interpolation_data(interpol_t::Linear1d, double tau) const {
   
   // reduce tau into the [0,beta] segment
   double beta = this->domain().beta;
   int p = std::floor(tau / beta);
   tau -= p * beta;

   // use regular interpolation of linear class.
   auto id = B::get_interpolation_data(interpol_t::Linear1d{}, tau);

   // if necessary, add the - sign
   if ((this->domain().statistic == Fermion) && (p % 2 != 0)) {
    id.w0 = -id.w0;
    id.w1 = -id.w1;
   }
   return id;
  }

  // evaluation
  template <typename F> auto evaluate(interpol_t::Linear1d, F const &f, double x) const 
#ifdef TRIQS_CPP11 
  ->decltype(0.0*f[0] + 1.0*f[0]) 
#endif
  {
   auto id = this->get_interpolation_data(default_interpol_policy{}, x);
   return id.w0 * f[id.i0] + id.w1 * f[id.i1];
  }
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

