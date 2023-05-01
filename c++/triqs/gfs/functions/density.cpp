// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
// Copyright (c) 2023 Hugo U.R. Strand
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#include "../../gfs.hpp"
#include <triqs/utility/legendre.hpp>

namespace triqs::gfs {

  using nda::array;

  //-------------------------------------------------------
  // For Imaginary Matsubara Frequency functions
  // ------------------------------------------------------

  nda::matrix<dcomplex> density(gf_const_view<imfreq> g, array_const_view<dcomplex, 3> known_moments) {

    if (g.mesh().positive_only())
      TRIQS_RUNTIME_ERROR << "density is only implemented for g(i omega_n) with full mesh (positive and negative frequencies)";

    nda::array_const_view<dcomplex, 3> mom_123;

    // Assume vanishing 0th moment in tail fit
    if (known_moments.is_empty()) return density(g, make_zero_tail(g, 1));

    double _abs_tail0 = max_element(abs(known_moments(0, range::all, range::all)));
    TRIQS_ASSERT2((_abs_tail0 < 1e-8),
                  "ERROR: Density implementation requires vanishing 0th moment\n  error is :" + std::to_string(_abs_tail0) + "\n");

    if (known_moments.shape()[0] < 4) {
      auto [tail, error] = fit_tail(g, known_moments);
      TRIQS_ASSERT2((error < 1e-2),
                    "ERROR: High frequency moments have an error greater than 1e-2.\n  Error = " + std::to_string(error)
                       + "\n Please make sure you treat the constant offset analytically!\n");
      if (error > 1e-4)
        std::cerr << "WARNING: High frequency moments have an error greater than 1e-4.\n Error = " << error
                  << "\n Please make sure you treat the constant offset analytically!\n";
      TRIQS_ASSERT2((first_dim(tail) > 3), "ERROR: Density implementation requires at least a proper 3rd high-frequency moment\n");
      return density(g, tail); //(range(1, 4), range::all, range::all));
    } else
      mom_123.rebind(known_moments(range(1, 4), range::all, range::all));

    auto sh = g.target_shape();
    int N1 = sh[0], N2 = sh[1];
    nda::matrix<dcomplex> res(sh);
    auto beta = g.mesh().beta;

    auto S = g.mesh().statistic;
    double b1, b2, b3; // pole location for tail model
    double xi;         // +1, -1 for boson/fermion

    if (S == Fermion) {
      xi = -1.;
      b1 = 0;
      b2 = 1;
      b3 = -1;
    } else if (S == Boson) {
      xi = 1.;
      b1 = -1.;
      b2 = 1.;
      b3 = 1. / 2.;
    } else
      TRIQS_RUNTIME_ERROR << "ERROR: Unknown statistic in density\n";

    // exact expression for sum over Matsubara frequencies for a single pole
    // located at b with amplitude a
    auto F = [&beta, &xi](dcomplex a, double b) { return xi * a / (-xi + exp(-beta * b)); };

    for (int n1 = 0; n1 < N1; n1++)
      for (int n2 = n1; n2 < N2; n2++) {
        dcomplex m1 = mom_123(0, n1, n2), m2 = mom_123(1, n1, n2), m3 = mom_123(2, n1, n2);

        // inverse of the Vandermonte matrix
        //
        // V =
        // [ 1,    1,    1    ]
        // [ b1,   b2,   b3   ]
        // [ b1^2, b2^2, b3^2 ]
        //
        // V * a = m => a = V^{-1} * m

        dcomplex a1, a2, a3; // amplitudes for each pole in tail model

        if (S == Fermion) {
          a1 = m1 - m3;
          a2 = (m2 + m3) / 2;
          a3 = (m3 - m2) / 2;
        } else if (S == Boson) {
          a1 = m1 / 6. - m2 / 2. + m3 / 3.;
          a2 = -m1 / 2. + m2 / 2. + m3;
          a3 = 4. * m1 / 3. - 4. * m3 / 3.;
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
  dcomplex density(gf_const_view<imfreq, scalar_valued> g, array_const_view<dcomplex, 1> known_moments) {
    auto km = array<dcomplex, 3>(make_shape(known_moments.shape()[0], 1, 1));
    if (!known_moments.is_empty()) km(range::all, 0, 0) = known_moments();
    auto res = density(reinterpret_scalar_valued_gf_as_matrix_valued(g), km)(0, 0);
    return res;
  }

  //-------------------------------------------------------
  // For Real Frequency functions
  // ------------------------------------------------------

  /// Zero temperature density from integration on the real frequency axis
  nda::matrix<dcomplex> density(gf_const_view<refreq> g) {

    int N       = g.mesh().size(); // no mesh points
    double wmin = g.mesh().w_min();
    double dw   = g.mesh().delta();

    assert(wmin < 0.);

    int N0     = std::floor(-wmin / dw) + 1; // frequency index at or above w=0
    double dw0 = -wmin - (N0 - 1) * dw; // last interval width to w=0

    nda::matrix<dcomplex> res(g.target_shape());

    // Trapetzoidal integration, with partial right interval
    res = 0.5 * g[0];
    for (int widx : range(1, N0)) res += g[widx];
    if (abs(dw0) > 1e-9) {
      double a = dw0 / dw;
      res += 0.5 * ((a * a - 1.) * g[N0 - 1] + a * (2. - a) * g[N0]);
    } else
      res -= 0.5 * g[N0 - 1];

    // Filter out divergent real parts of g that are inf
    // e.g. flat dos at dos edge (but keep complex matrix structure)
    // FIXME : Use diagonal iteration when implemented.
    for (int idx : range(0, res.shape()[0])) res(idx, idx) = dcomplex(0., imag(res(idx, idx)));

    res *= dcomplex(0., 1.) * dw / M_PI; // scale to density
    
    // writing back into res is problematic due to lazy expressionsreturn
    // return directly instead
    return 0.5 * (res + dagger(res));     // A = i( g - g^+ )
  }

  /// Finite temperature density from integration on the real frequency axis
  nda::matrix<dcomplex> density(gf_const_view<refreq> g, double beta) {

    assert(beta > 0.);

    nda::matrix<dcomplex> res(g.target_shape());
    res() = 0;

    for (auto const &w : g.mesh()) res += g[w] / (1. + exp(beta * w));

    // -- Required to filter out divergent real parts of g that are inf
    // -- eg flat dos at dos edge
    // FIXME : Use diagonal iteration when implemented.
    for (int idx : range(0, res.shape()[0])) res(idx, idx) = dcomplex(0., imag(res(idx, idx)));

    res *= dcomplex(0., 1.) * g.mesh().delta() / M_PI; // scale to density

    // writing back into res is problematic due to lazy expressionsreturn
    // return directly instead
    return 0.5 * (res + dagger(res));     // A = i( g - g^+ )
  }

  //-------------------------------------------------------
  dcomplex density(gf_const_view<refreq, scalar_valued> g, double beta) {
    return density(reinterpret_scalar_valued_gf_as_matrix_valued(g), beta)(0, 0);
  }

  dcomplex density(gf_const_view<refreq, scalar_valued> g) { return density(reinterpret_scalar_valued_gf_as_matrix_valued(g))(0, 0); }

  //-------------------------------------------------------
  // For Legendre functions
  // ------------------------------------------------------

  nda::matrix<dcomplex> density(gf_const_view<legendre> gl) {
    nda::matrix<dcomplex> res(gl.target_shape());
    res() = 0.0;

    // Calculate <cdag_j c_i> = -G_ij(beta^{-}) using Eq. (1,2) of PhysRevB.84.075145
    for (auto const &l : gl.mesh()) res -= std::sqrt(2 * l.idx + 1) * gl[l];
    res /= gl.mesh().beta;

    // Transpose to get <cdag_i c_j> instead of <cdag_j c_i>
    return transpose(res);
  }

  dcomplex density(gf_const_view<legendre, scalar_valued> g) { return density(reinterpret_scalar_valued_gf_as_matrix_valued(g))(0, 0); }

  //-------------------------------------------------------
  // For DLR functions
  // ------------------------------------------------------

  nda::matrix<dcomplex> density(gf_const_view<mesh::dlr_coeffs> gl) {
    nda::matrix<dcomplex> res(gl.target_shape());
    res = -gl(gl.mesh().beta);
    // Transpose to get <cdag_i c_j> instead of <cdag_j c_i>
    return transpose(res);
  }

  dcomplex density(gf_const_view<mesh::dlr_coeffs, scalar_valued> g) { return density(reinterpret_scalar_valued_gf_as_matrix_valued(g))(0, 0); }

} // namespace triqs::gfs
