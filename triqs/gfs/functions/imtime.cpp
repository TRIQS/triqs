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
#include "../../gfs.hpp"
#include <triqs/utility/legendre.hpp>

namespace triqs {
namespace gfs {

 using arrays::array;

 //-------------------------------------------------------
 // For Imaginary Time functions
 // ------------------------------------------------------
 gf<imtime> rebinning_tau(gf_const_view<imtime> const& g, int new_n_tau) {

  auto const& old_m = g.mesh();
  gf<imtime> new_gf{{old_m.domain().beta, old_m.domain().statistic, new_n_tau}, g.target_shape()};
  auto const& new_m = new_gf.mesh();
  new_gf.data()() = 0;
  long prev_index = 0;
  long norm = 0;
  for (auto const& tau : old_m) {
   long index = std::round((double(tau) - new_m.x_min()) / new_m.delta());
   if (index == prev_index) { norm++; } else {
    new_gf[index - 1] /= double(norm);
    prev_index = index;
    norm = 1;
   }
   new_gf[index] += g[tau];
  }
  if (norm != 1) new_gf[new_m.size() - 1] /= norm;
  new_gf.singularity() = g.singularity();
  return new_gf;
 }
}
}
