// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Michel Ferrero, Laura Messio, Olivier Parcollet, Nils Wentzell

#include "../../gfs.hpp"
#include "./fourier_common.hpp"

namespace triqs::gfs {

  namespace {
    dcomplex I(0, 1);
    inline dcomplex th_expo(double t, double a) { return (t > 0 ? -I * exp(-a * t) : (t < 0 ? 0 : -0.5 * I * exp(-a * t))); }
    inline dcomplex th_expo_neg(double t, double a) { return (t < 0 ? I * exp(a * t) : (t > 0 ? 0 : 0.5 * I * exp(a * t))); }
    inline dcomplex th_expo_inv(double w, double a) { return 1. / (w + I * a); }
    inline dcomplex th_expo_neg_inv(double w, double a) { return 1. / (w - I * a); }
  } // namespace

  // ------------------------ DIRECT TRANSFORM --------------------------------------------

  gf_vec_t<refreq> _fourier_impl(mesh::refreq const &w_mesh, gf_vec_cvt<retime> gt, nda::array_const_view<dcomplex, 2> known_moments) {

    nda::array_const_view<dcomplex, 2> mom_12;
    if (known_moments.is_empty())
      // If no known_moments are passed we do a fourier transform on the raw data
      return _fourier_impl(w_mesh, gt, make_zero_tail(gt, 3));
    else {
      TRIQS_ASSERT2(known_moments.shape()[0] >= 3, " Direct RealTime Fourier transform requires known moments up to order 2.")
      double _abs_tail0 = max_element(abs(known_moments(0, range::all)));
      TRIQS_ASSERT2((_abs_tail0 < 1e-8),
                    "ERROR: Direct Fourier implementation requires vanishing 0th moment\n  error is :" + std::to_string(_abs_tail0));
      mom_12.rebind(known_moments(range(1, 3), range::all));
    }

    size_t L = gt.mesh().size();
    if (w_mesh.size() != L) TRIQS_RUNTIME_ERROR << "Meshes are different";
    double test = std::abs(gt.mesh().delta() * w_mesh.delta() * L / (2 * M_PI) - 1);
    if (test > 1.e-10) TRIQS_RUNTIME_ERROR << "Meshes are not compatible";

    long n_others = second_dim(gt.data());
    array<dcomplex, 2> _gout(L, n_others);
    array<dcomplex, 2> _gin(L, n_others);

    const double tmin = gt.mesh().t_min();
    const double wmin = w_mesh.w_min();
    //a is a number very larger than delta_w and very smaller than wmax-wmin, used in the tail computation
    const double a = w_mesh.delta() * std::sqrt(double(L));

    auto _  = range::all;
    auto m1 = mom_12(0, _);
    auto m2 = mom_12(1, _);

    array<dcomplex, 1> a1 = (m1 + I * m2 / a) / 2., a2 = (m1 - I * m2 / a) / 2.;

    for (auto t : gt.mesh()) _gin(t.index(), _) = (gt[t] - (a1 * th_expo(t, a) + a2 * th_expo_neg(t, a))) * std::exp(I * t * wmin);

    int dims[] = {int(L)};
    _fourier_base(_gin, _gout, 1, dims, n_others, FFTW_BACKWARD);

    auto gw = gf_vec_t<mesh::refreq>{w_mesh, {n_others}};
    for (auto w : w_mesh)
      gw[w] = gt.mesh().delta() * std::exp(I * (w - wmin) * tmin) * _gout(w.index(), _) + a1 * th_expo_inv(w, a) + a2 * th_expo_neg_inv(w, a);

    return std::move(gw);
  }

  // ------------------------ INVERSE TRANSFORM --------------------------------------------

  gf_vec_t<retime> _fourier_impl(mesh::retime const &t_mesh, gf_vec_cvt<refreq> gw, nda::array_const_view<dcomplex, 2> known_moments) {

    nda::array_const_view<dcomplex, 2> mom_12;

    if (known_moments.is_empty())
      // If no known_moments are passed we do a fourier transform on the raw data
      return _fourier_impl(t_mesh, gw, make_zero_tail(gw, 3));
    else {
      TRIQS_ASSERT2(known_moments.shape()[0] >= 3, " Direct Real-Frequency Fourier transform requires known moments up to order 2.")

      double _abs_tail0 = max_element(abs(known_moments(0, range::all)));
      TRIQS_ASSERT2((_abs_tail0 < 1e-8),
                    "ERROR: Inverse Fourier implementation requires vanishing 0th moment\n  error is :" + std::to_string(_abs_tail0));

      mom_12.rebind(known_moments(range(1, 3), range::all));
    }

    size_t L = gw.mesh().size();
    if (L != t_mesh.size()) TRIQS_RUNTIME_ERROR << "Meshes are of different size: " << L << " vs " << t_mesh.size();
    double test = std::abs(t_mesh.delta() * gw.mesh().delta() * L / (2 * M_PI) - 1);
    if (test > 1.e-10) TRIQS_RUNTIME_ERROR << "Meshes are not compatible";

    const double tmin = t_mesh.t_min();
    const double wmin = gw.mesh().w_min();
    //a is a number very larger than delta_w and very smaller than wmax-wmin, used in the tail computation
    const double a = gw.mesh().delta() * std::sqrt(double(L));

    long n_others = second_dim(gw.data());

    array<dcomplex, 2> _gin(L, n_others);
    array<dcomplex, 2> _gout(L, n_others);

    auto _  = range::all;
    auto m1 = mom_12(0, _);
    auto m2 = mom_12(1, _);

    array<dcomplex, 1> a1 = (m1 + I * m2 / a) / 2., a2 = (m1 - I * m2 / a) / 2.;

    for (auto w : gw.mesh()) _gin(w.index(), _) = (gw[w] - a1 * th_expo_inv(w, a) - a2 * th_expo_neg_inv(w, a)) * std::exp(-I * w * tmin);

    int dims[] = {int(L)};
    _fourier_base(_gin, _gout, 1, dims, n_others, FFTW_FORWARD);

    auto gt           = gf_vec_t<retime>{t_mesh, {n_others}};
    const double corr = 1.0 / (t_mesh.delta() * L);
    for (auto t : t_mesh) gt[t] = corr * std::exp(I * wmin * (tmin - t)) * _gout(t.index(), _) + a1 * th_expo(t, a) + a2 * th_expo_neg(t, a);

    return std::move(gt);
  }

} // namespace triqs::gfs
