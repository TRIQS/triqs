/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
 * Copyright (C) 2018 The Simons Foundation, Author: H. UR Strand
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

namespace triqs::gfs {

  using arrays::array;

  //-------------------------------------------------------
  // For Imaginary Matsubara Frequency functions
  // ------------------------------------------------------
  arrays::matrix<dcomplex> density(gf_const_view<imfreq> g) {

    if (g.mesh().positive_only())
      TRIQS_RUNTIME_ERROR << "density is only implemented for g(i omega_n) with full mesh (positive and negative frequencies)";

    auto [tail, err] = fit_tail(g);
    if (err > 1e-6) std::cerr << "WARNING: High frequency moments have an error greater than 1e-6.\n";
    if (err > 1e-3) TRIQS_RUNTIME_ERROR << "ERROR: High frequency moments have an error greater than 1e-3.\n";
    if (first_dim(tail) < 4) TRIQS_RUNTIME_ERROR << "ERROR: Density implementation requires at least a proper 3rd high-frequency moment in tail\n";
    auto mt = max_element(abs(tail(0, ellipsis())));
    if (mt > 1e-12) TRIQS_RUNTIME_ERROR << "ERROR: Density implementation requires vanishing 0th moment\n" << mt;

    if (g.mesh().positive_only()) TRIQS_RUNTIME_ERROR << " imfreq gf: full mesh required in density computation";
    auto sh = g.target_shape();
    int N1 = sh[0], N2 = sh[1];
    arrays::matrix<dcomplex> res(sh);
    auto beta = g.domain().beta;

    auto S = g.mesh().domain().statistic;
    double b1, b2, b3; // pole location for tail model
    double xi; // +1, -1 for boson/fermion

    if( S == Fermion ) {
      xi = -1.;
      b1 = 0; b2 = 1; b3 = -1;
    } else if( S == Boson ) {
      xi = 1.;
      b1 = -1.; b2 = 1.; b3 = 1./2.;
    } else
      TRIQS_RUNTIME_ERROR << "ERROR: Unknown statistic in density\n";

    // exact expression for sum over Matsubara frequencies for a single pole
    // located at b with amplitude a
    auto F = [&beta, &xi](dcomplex a, double b) { return xi * a / (-xi + exp(-beta * b)); };

    for (int n1 = 0; n1 < N1; n1++)
      for (int n2 = n1; n2 < N2; n2++) {
        dcomplex m1 = tail(1, n1, n2), m2 = tail(2, n1, n2), m3 = tail(3, n1, n2);

	// inverse of the Vandermonte matrix
	//
	// V =
	// [ 1,    1,    1    ]
	// [ b1,   b2,   b3   ]
	// [ b1^2, b2^2, b3^2 ]
	//
	// V * a = m => a = V^{-1} * m

	dcomplex a1, a2, a3; // amplitudes for each pole in tail model
	
	if( S == Fermion ) {
	  a1 = m1 - m3; a2 = (m2 + m3) / 2; a3 = (m3 - m2) / 2;
	} else if ( S == Boson ) {
	  a1 = m1/6. - m2/2. + m3/3.;
	  a2 = -m1/2. + m2/2. + m3;
	  a3 = 4.*m1/3. - 4.*m3/3.;
	} else
	  TRIQS_RUNTIME_ERROR << "ERROR: Unknown statistic in density\n";

	dcomplex r = 0;
        for (auto const &w : g.mesh()) r += g[w](n1, n2) - (a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3));
        res(n1, n2) = r / beta + m1 + F(a1, b1) + F(a2, b2) + F(a3, b3);
	res(n1, n2) *= -xi;

        if (n2 > n1) res(n2, n1) = conj(res(n1, n2));
      }

    return res;
  }

  //-------------------------------------------------------
  dcomplex density(gf_const_view<imfreq, scalar_valued> g) { return density(reinterpret_scalar_valued_gf_as_matrix_valued(g))(0, 0); }

  //-------------------------------------------------------
  arrays::matrix<dcomplex> density(gf_const_view<legendre> gl) {
    arrays::matrix<dcomplex> res(gl.target_shape());
    res() = 0.0;
    for (auto const &l : gl.mesh()) res -= sqrt(2 * l.index() + 1) * gl[l];
    res /= gl.domain().beta;
    return res;
  }

  //-------------------------------------------------------
  dcomplex density(gf_const_view<legendre, scalar_valued> g) {
   return density(reinterpret_scalar_valued_gf_as_matrix_valued(g))(0, 0);
  }

} // namespace triqs::gfs
