/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
 // rebinning
 // ------------------------------------------------------

 gf<imtime> rebinning_tau(gf_const_view<imtime> const &g, int new_n_tau);

 //-------------------------------------------------------
 // closest mesh point on the grid
 // ------------------------------------------------------

 template <typename Target> struct gf_closest_point<imtime, Target> {
  // index_t is int
  template <typename G, typename T> static int invoke(G const *g, closest_pt_wrap<T> const &p) {
   double x = double(p.value) + 0.5 * g->mesh().delta();
   int n = std::floor(x / g->mesh().delta());
   return n;
  }
 };
}
}

