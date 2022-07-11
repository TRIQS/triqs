// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Michel Ferrero, Laura Messio, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell, tayral

#include "../../gfs.hpp"
#include "./fourier_common.hpp"

namespace triqs::gfs {

  namespace {

    // NB : will return an expression template, but compute the number after a*
    template <typename A> auto oneFermion(A &&a, double b, double tau, double beta) {
      return -a * (b >= 0 ? exp(-b * tau) / (1 + exp(-beta * b)) : exp(b * (beta - tau)) / (1 + exp(beta * b)));
    }

    template <typename A> auto oneBoson(A &&a, double b, double tau, double beta) {
      return a * (b >= 0 ? exp(-b * tau) / (exp(-beta * b) - 1) : exp(b * (beta - tau)) / (1 - exp(b * beta)));
    }
  } // namespace

  //-------------------------------------

  array<dcomplex, 2> fit_tail(gf_const_view<imtime, tensor_valued<1>> gt) {
    using matrix_t   = nda::matrix<dcomplex>;
    int fit_order    = 8;
    auto _           = range::all;
    auto d_vec_left  = matrix_t(fit_order, gt.target_shape()[0]);
    auto d_vec_right = d_vec_left;
    int n_tau        = gt.mesh().size();
    for (int m : range(1, fit_order + 1)) {
      d_vec_left(m - 1, _)  = (gt[m] - gt[0]) / gt.mesh()[m];                     // Values around 0
      d_vec_right(m - 1, _) = (gt[n_tau - 1] - gt[n_tau - 1 - m]) / gt.mesh()[m]; // Values around beta
    }

    // Inverse of the Vandermonde matrix V_{m,j} = m^{j-1}
    // FIXME : add here teh python code that generated it
    auto V_inv = matrix_t{{8.000000000008853, -28.000000000055913, 56.000000000151815, -70.00000000021936, 56.00000000020795, -28.000000000115904,
                           8.00000000003683, -1.0000000000049232},
                          {-13.742857142879107, 62.10000000013776, -133.5333333337073, 172.75000000055635, -141.00000000052023, 71.43333333362274,
                           -20.600000000091182, 2.592857142869304},
                          {9.694444444465196, -50.98055555568518, 119.55000000035201, -161.8888888894241, 135.86111111160685, -70.12500000027573,
                           20.494444444530682, -2.6055555555670558},
                          {-3.6555555555654573, 21.23472222228416, -53.60000000016842, 76.34027777803497, -66.27777777801586, 35.03750000013273,
                           -10.422222222263619, 1.3430555555610917},
                          {0.7986111111137331, -4.97222222223865, 13.312500000044732, -19.888888888957325, 17.923611111174495, -9.750000000035401,
                           2.9652777777888164, -0.38888888889036743},
                          {-0.1013888888892789, 0.6638888888913357, -1.862500000006671, 2.902777777787997, -2.71527777778725, 1.5250000000052975,
                           -0.47638888889054154, 0.06388888888911043},
                          {0.0069444444444749, -0.047222222222413485, 0.13750000000052204, -0.22222222222302282, 0.2152777777785205,
                           -0.12500000000041575, 0.04027777777790756, -0.00555555555557296},
                          {-0.0001984126984136688, 0.0013888888888949882, -0.00416666666668333, 0.006944444444470023, -0.006944444444468193,
                           0.004166666666679969, -0.0013888888888930434, 0.00019841269841325577}};

    /*auto V_inv = matrix_t{
      {7.000000000000351, -21.00000000000213, 35.00000000000503, -35.00000000000608, 21.000000000003606, -7.000000000001135, 1.000000000000174},
      {-11.150000000000887, 43.95000000000505, -79.08333333334492, 82.00000000001403, -50.25000000000872, 16.98333333333621, -2.450000000000441},
      {7.088888888889711, -32.74166666667111, 64.83333333334329, -70.69444444445642, 44.666666666674445, -15.408333333336019, 2.255555555555966},
      {-2.312500000000358, 11.833333333335242, -25.395833333337578, 29.333333333338373, -19.270833333336668, 6.83333333333452,
       -1.0208333333335127},
      {0.409722222222303, -2.25000000000043, 5.145833333334286, -6.277777777778903, 4.3125000000007505, -1.5833333333336026,
       0.24305555555559613},
      {-0.037500000000009116, 0.21666666666671525, -0.5208333333334408, 0.6666666666667935, -0.47916666666675145, 0.18333333333336377,
       -0.029166666666671254},
      {0.001388888888889295, -0.008333333333335498, 0.02083333333333812, -0.027777777777783428, 0.020833333333337107, -0.008333333333334688,
       0.0013888888888890932}};
   */

    // Calculate the 2nd
    matrix_t g_vec_left  = V_inv * d_vec_left;
    matrix_t g_vec_right = V_inv * d_vec_right;
    double sign          = (gt.mesh().statistic == Fermion) ? -1 : 1;
    auto tail            = make_zero_tail(gt, 4);
    tail(1, _)           = -(gt[0] - sign * gt[n_tau - 1]);                                        // 1st order moment
    tail(2, _)           = g_vec_left(0, _) - sign * g_vec_right(0, _);                            // 2nd order moment
    tail(3, _)           = -(g_vec_left(1, _) + sign * g_vec_right(1, _)) * 2 / gt.mesh().delta(); // 3rd order moment
    return tail;
  }

  // ------------------------ DIRECT TRANSFORM --------------------------------------------

  gf_vec_t<imfreq> _fourier_impl(mesh::imfreq const &iw_mesh, gf_vec_cvt<imtime> gt, nda::array_const_view<dcomplex, 2> known_moments) {

    nda::array<dcomplex, 2> tail;

    if (known_moments.is_empty()) {
      // A simple check on whether or not we are dealing with noisy data
      auto dat   = gt.data();
      int n_tau  = gt.mesh().size();
      auto der_1 = max_element(abs(dat(1, range::all) - dat(0, range::all)) + abs(dat(n_tau - 2, range::all) - dat(n_tau - 1, range::all))) / gt.mesh().delta();
      auto der_2 =
         0.5 * max_element(abs(dat(2, range::all) - dat(0, range::all)) + abs(dat(n_tau - 3, range::all) - dat(n_tau - 1, range::all))) / gt.mesh().delta();
      if (der_1 < 0.95 * der_2 or der_1 > 1.05 * der_2) {
        std::cerr << "WARNING: Direct Fourier cannot deduce the high-frequency moments of G(tau) due to noise or a coarse tau-grid. \
	  Please specify the high-frequency moments for higher accuracy.\n";
        return _fourier_impl(iw_mesh, gt, make_zero_tail(gt, 4));
      } else {
	return _fourier_impl(iw_mesh, gt, fit_tail(gt));
      }
    } else {
      double _abs_tail0 = max_element(abs(known_moments(0, range::all)));
      TRIQS_ASSERT2((_abs_tail0 < 1e-8),
                    "ERROR: Direct Fourier implementation requires vanishing 0th moment\n  error is :" + std::to_string(_abs_tail0));

      int n_known_moments = std::min<size_t>(known_moments.shape()[0], 4);
      tail = make_zero_tail(gt, 4);
      tail(range(0, n_known_moments), range::all) = known_moments(range(0, n_known_moments), range::all);
    }

    double beta = gt.mesh().beta;
    auto L      = gt.mesh().size() - 1;
    if (L < 2 * (iw_mesh.last_idx() + 1))
      TRIQS_RUNTIME_ERROR << "Fourier: The time mesh mush be at least twice as long as the number of positive frequencies :\n gt.mesh().size() =  "
                          << gt.mesh().size() << " gw.mesh().last_idx()" << iw_mesh.last_idx();

    if (L < 6 * (iw_mesh.last_idx() + 1))
      std::cerr << "[Direct Fourier] WARNING: The imaginary time mesh is less than six times as long as the number of positive frequencies.\n"
                << "This can lead to substantial numerical inaccuracies at the boundary of the frequency mesh.\n";

    long n_others = second_dim(gt.data());

    array<dcomplex, 2> _gout(L, n_others); // FIXME Why do we need this dimension to be one less than gt.mesh().size() ?
    array<dcomplex, 2> _gin(L + 1, n_others);

    bool is_fermion = (iw_mesh.statistic == Fermion);
    double fact     = beta / L;
    dcomplex iomega = M_PI * 1i / beta;

    double b1, b2, b3;
    array<dcomplex, 1> a1, a2, a3;
    auto _  = range::all;
    auto m1 = tail(1, _);
    auto m2 = tail(2, _);
    auto m3 = tail(3, _);

    if (is_fermion) {
      b1 = 0;
      b2 = 1;
      b3 = -1;
      a1 = m1 - m3;
      a2 = (m2 + m3) / 2;
      a3 = (m3 - m2) / 2;

      for (auto const &t : gt.mesh())
        _gin(t.idx, _) = fact * exp(iomega * t) * (gt[t] - (oneFermion(a1, b1, t, beta) + oneFermion(a2, b2, t, beta) + oneFermion(a3, b3, t, beta)));

    } else {
      b1 = -0.5;
      b2 = -1;
      b3 = 1;
      a1 = 4 * (m1 - m3) / 3;
      a2 = m3 - (m1 + m2) / 2;
      a3 = m1 / 6 + m2 / 2 + m3 / 3;

      for (auto const &t : gt.mesh())
        _gin(t.idx, _) = fact * (gt[t] - (oneBoson(a1, b1, t, beta) + oneBoson(a2, b2, t, beta) + oneBoson(a3, b3, t, beta)));
    }

    int dims[] = {int(L)};
    _fourier_base(_gin, _gout, 1, dims, n_others, FFTW_BACKWARD);

    auto gw = gf_vec_t<imfreq>{iw_mesh, {int(n_others)}};

    // Correction term to account for proper Trapezoidal integration
    // FIXME Avoid copy, by doing proper in-place operation
    auto corr = -0.5 * fact * (gt[0] + m1 + (is_fermion ? 1 : -1) * gt[L]);
    for (auto const &iw : iw_mesh) gw[iw] = _gout((iw.n + L) % L, _) + corr + a1 / (iw - b1) + a2 / (iw - b2) + a3 / (iw - b3);
    
    return std::move(gw);
  }

  // ------------------------ INVERSE TRANSFORM --------------------------------------------

  gf_vec_t<imtime> _fourier_impl(mesh::imtime const &tau_mesh, gf_vec_cvt<imfreq> gw, nda::array_const_view<dcomplex, 2> known_moments) {

    TRIQS_ASSERT2(!gw.mesh().positive_only(), "Fourier is only implemented for g(i omega_n) with full mesh (positive and negative frequencies)");

    nda::array_const_view<dcomplex, 2> tail;

    // Assume vanishing 0th moment in tail fit
    if (known_moments.is_empty()) return _fourier_impl(tau_mesh, gw, make_zero_tail(gw, 1));

    double _abs_tail0 = max_element(abs(known_moments(0, range::all)));
    TRIQS_ASSERT2((_abs_tail0 < 1e-8),
                  "ERROR: Inverse Fourier implementation requires vanishing 0th moment\n  error is :" + std::to_string(_abs_tail0) + "\n");

    if (known_moments.shape()[0] < 4) {
      auto [t, err] = fit_tail(gw, known_moments);
      TRIQS_ASSERT2((err < 1e-2),
                    "ERROR: High frequency moments have an error greater than 1e-2.\n  Error = " + std::to_string(err)
                       + "\n Please make sure you treat the constant offset analytically!\n");
      if (err > 1e-4)
        std::cerr << "WARNING: High frequency moments have an error greater than 1e-4.\n Error = " << err
                  << "\n Please make sure you treat the constant offset analytically!\n";
      TRIQS_ASSERT2((first_dim(t) > 3), "ERROR: Inverse Fourier implementation requires at least a proper 3rd high-frequency moment\n");
      return _fourier_impl(tau_mesh, gw, t);
    } else
      tail.rebind(known_moments); // known_moments is fine

    double beta = tau_mesh.beta;
    long L      = tau_mesh.size() - 1;
    if (L < 2 * (gw.mesh().last_idx() + 1))
      TRIQS_RUNTIME_ERROR << "Inverse Fourier: The time mesh mush be at least twice as long as the freq mesh :\n gt.mesh().size() =  "
                          << tau_mesh.size() << " gw.mesh().last_idx()" << gw.mesh().last_idx() << "\n";

    long n_others = second_dim(gw.data());

    array<dcomplex, 2> _gin(L, n_others); // FIXME Why do we need this dimension to be one less than gt.mesh().size() ?
    array<dcomplex, 2> _gout(L + 1, n_others);

    bool is_fermion = (gw.mesh().statistic == Fermion);
    double fact     = 1.0 / beta;
    dcomplex iomega = M_PI * 1i / beta;

    double b1, b2, b3;
    array<dcomplex, 1> a1, a2, a3;
    auto _  = range::all;
    auto m1 = tail(1, _);
    auto m2 = tail(2, _);
    auto m3 = tail(3, _);

    if (is_fermion) {
      b1 = 0;
      b2 = 1;
      b3 = -1;
      a1 = m1 - m3;
      a2 = (m2 + m3) / 2;
      a3 = (m3 - m2) / 2;
    } else {
      b1 = -0.5;
      b2 = -1;
      b3 = 1;
      a1 = 4 * (m1 - m3) / 3;
      a2 = m3 - (m1 + m2) / 2;
      a3 = m1 / 6 + m2 / 2 + m3 / 3;
    }

    for (auto const &iw : gw.mesh()) _gin((iw.n + L) % L, _) = fact * (gw[iw] - (a1 / (iw - b1) + a2 / (iw - b2) + a3 / (iw - b3)));

    int dims[] = {int(L)};
    _fourier_base(_gin, _gout, 1, dims, n_others, FFTW_FORWARD);

    auto gt = gf_vec_t<imtime>{tau_mesh, {int(n_others)}};

    if (is_fermion)
      for (auto const &t : tau_mesh)
        gt[t] = _gout(t.idx, _) * exp(-iomega * t) + oneFermion(a1, b1, t, beta) + oneFermion(a2, b2, t, beta) + oneFermion(a3, b3, t, beta);
    else
      for (auto const &t : tau_mesh) gt[t] = _gout(t.idx, _) + oneBoson(a1, b1, t, beta) + oneBoson(a2, b2, t, beta) + oneBoson(a3, b3, t, beta);

    double pm = (is_fermion ? -1 : 1);
    gt[L]     = pm * (gt[0] + m1);

    return std::move(gt);
  }

} // namespace triqs::gfs
