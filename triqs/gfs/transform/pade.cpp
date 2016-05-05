/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet, I. Krivenko
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU grneral Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU grneral Public License for more
 * details.
 *
 * You should have received a copy of the GNU grneral Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "../../gfs.hpp"
//#include "pade.hpp"
#include <triqs/arrays.hpp>
#include <triqs/utility/pade_approximants.hpp>

namespace triqs { namespace gfs {

 typedef std::complex<double> dcomplex;

 void pade (gf_view<refreq> &gr, gf_view<imfreq> const &gw, int n_points, double freq_offset) {

  // make sure the GFs have the same structure
  //assert(gw.shape() == gr.shape());

  // copy the tail. it doesn't need to conform to the pade approximant
  gr.singularity() = gw.singularity();
  gr() = 0.0;

  auto sh = gw.data().shape().front_pop();
  int N1 = sh[0], N2 = sh[1];
  for (int n1=0; n1<N1; n1++) {
    for (int n2=0; n2<N2; n2++) {

      arrays::vector<dcomplex> z_in(n_points); // complex points
      arrays::vector<dcomplex> u_in(n_points); // values at these points

      for (int i=0; i < n_points; ++i) z_in(i) = gw.mesh()[i];
      for (int i=0; i < n_points; ++i) u_in(i) = gw.on_mesh(i)(n1,n2);

      triqs::utility::pade_approximant PA(z_in,u_in);

      for (auto om : gr.mesh()) {
        dcomplex e = om + dcomplex(0.0,1.0)*freq_offset;
        gr[om](n1,n2) = PA(e);
      }

    }
  }

 }

}}
