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
 // For Imaginary Matsubara Frequency functions
 // ------------------------------------------------------
 arrays::matrix<dcomplex> density(gf_const_view<imfreq> g) {

  if (g.mesh().positive_only())
   TRIQS_RUNTIME_ERROR << "density is only implemented for g(i omega_n) with full mesh (positive and negative frequencies)";

  auto t = g.singularity();
  if (!t.is_decreasing_at_infinity())
   TRIQS_RUNTIME_ERROR << " density computation : green Function is not as 1/omega or less !!!\n" << t;

  if (g.mesh().positive_only()) TRIQS_RUNTIME_ERROR << " imfreq gF : full mesh required in density computation";
  auto sh = g.target_shape();
  int N1 = sh[0], N2 = sh[1];
  arrays::matrix<dcomplex> res(sh);
  auto beta = g.domain().beta;
  double b1 = 0, b2 = 1, b3 = -1;
  auto F = [&beta](dcomplex a, double b) { return -a / (1 + exp(-beta * b)); };

  for (int n1 = 0; n1 < N1; n1++)
   for (int n2 = n1; n2 < N2; n2++) {
    dcomplex d = t(1)(n1, n2), A = t(2)(n1, n2), B = t(3)(n1, n2);
    dcomplex a1 = d - B, a2 = (A + B) / 2, a3 = (B - A) / 2;
    dcomplex r = 0;
    for (auto const& w : g.mesh()) r += g[w](n1, n2) - (a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3));
    res(n1, n2) = r / beta + d + F(a1, b1) + F(a2, b2) + F(a3, b3);
    if (n2 > n1) res(n2, n1) = conj(res(n1, n2));
   }

  return res;
 }

 //-------------------------------------------------------
 dcomplex density(gf_const_view<imfreq, scalar_valued> g) {
  return density(reinterpret_scalar_valued_gf_as_matrix_valued(g))(0, 0);
 }

 //-------------------------------------------------------
 arrays::matrix<dcomplex> density(gf_const_view<legendre> gl) {
  arrays::matrix<dcomplex> res(gl.target_shape());
  res() = 0.0;
  for (auto const& l : gl.mesh()) res -= sqrt(2 * l.index() + 1) * gl[l];
  res /= gl.domain().beta;
  return res;
 }
}
}
