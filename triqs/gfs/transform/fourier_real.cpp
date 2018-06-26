/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by M. Ferrero, O. Parcollet
 * Copyright (C) 2018- by Simons Foundation
 *               authors : O. Parcollet, N. Wentzell
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

  gf_vec_t<refreq> _fourier_impl(gf_mesh<refreq> const &w_mesh, gf_vec_cvt<retime> gt, arrays::array_const_view<dcomplex, 2> known_moments) {

    arrays::array_const_view<dcomplex, 2> mom_12;
    if (known_moments.is_empty())
      mom_12.rebind(make_zero_tail(gt, 2));
    else {
      TRIQS_ASSERT2(known_moments.shape()[0] >= 3, " Direct RealTime Fourier transform requires known moments up to order 3.")
      double _abs_tail0 = max_element(abs(known_moments(0, range())));
      TRIQS_ASSERT2((_abs_tail0 < 1e-8),
                    "ERROR: Direct Fourier implementation requires vanishing 0th moment\n  error is :" + std::to_string(_abs_tail0));
      mom_12.rebind(known_moments(range(1, 3), range()));
    }

    size_t L = gt.mesh().size();
    if (w_mesh.size() != L) TRIQS_RUNTIME_ERROR << "Meshes are different";
    double test = std::abs(gt.mesh().delta() * w_mesh.delta() * L / (2 * M_PI) - 1);
    if (test > 1.e-10) TRIQS_RUNTIME_ERROR << "Meshes are not compatible";

    long n_others = second_dim(gt.data());
    array<dcomplex, 2> _gout(L, n_others);
    array<dcomplex, 2> _gin(L, n_others);

    const double tmin = gt.mesh().x_min();
    const double wmin = w_mesh.x_min();
    //a is a number very larger than delta_w and very smaller than wmax-wmin, used in the tail computation
    const double a = w_mesh.delta() * sqrt(double(L));

    auto _  = range();
    auto m1 = mom_12(0, _);
    auto m2 = mom_12(1, _);

    array<dcomplex, 1> a1 = (m1 + I * m2 / a) / 2., a2 = (m1 - I * m2 / a) / 2.;

    for (auto const &t : gt.mesh()) _gin(t.index(), _) = (gt[t] - (a1 * th_expo(t, a) + a2 * th_expo_neg(t, a))) * std::exp(I * t * wmin);

    int dims[] = {int(L)};
    _fourier_base(_gin, _gout, 1, dims, n_others, FFTW_BACKWARD);

    auto gw = gf_vec_t<refreq>{w_mesh, {int(n_others)}};
    for (auto const &w : w_mesh)
      gw[w] = gt.mesh().delta() * std::exp(I * (w - wmin) * tmin) * _gout(w.index(), _) + a1 * th_expo_inv(w, a) + a2 * th_expo_neg_inv(w, a);

    return std::move(gw);
  }

  // ------------------------ INVERSE TRANSFORM --------------------------------------------

  gf_vec_t<retime> _fourier_impl(gf_mesh<retime> const &t_mesh, gf_vec_cvt<refreq> gw, arrays::array_const_view<dcomplex, 2> known_moments) {

    arrays::array_const_view<dcomplex, 2> mom_12;

    // Assume vanishing 0th moment in tail fit
    if (known_moments.is_empty()) known_moments.rebind(make_zero_tail(gw, 1));

    double _abs_tail0 = max_element(abs(known_moments(0, range())));
    TRIQS_ASSERT2((_abs_tail0 < 1e-8),
                  "ERROR: Inverse Fourier implementation requires vanishing 0th moment\n  error is :" + std::to_string(_abs_tail0));

    if (known_moments.shape()[0] < 3) {
      auto [tail, error] = fit_tail(gw, known_moments);
      TRIQS_ASSERT2((error < 1e-3), "ERROR: High frequency moments have an error greater than 1e-3.\n  Error = " + std::to_string(error) + "\n Please make sure you treat the constant offset analytically!");
      if (error > 1e-6)
        std::cerr << "WARNING: High frequency moments have an error greater than 1e-6.\n Error = " << error
                  << "\n Please make sure you treat the constant offset analytically!";
      TRIQS_ASSERT2((first_dim(tail) > 2), "ERROR: Inverse Fourier implementation requires at least a proper 2nd high-frequency moment\n");
      mom_12.rebind(tail(range(1, 3), range()));
    } else
      mom_12.rebind(known_moments(range(1, 3), range()));

    size_t L = gw.mesh().size();
    if (L != t_mesh.size()) TRIQS_RUNTIME_ERROR << "Meshes are of different size: " << L << " vs " << t_mesh.size();
    double test = std::abs(t_mesh.delta() * gw.mesh().delta() * L / (2 * M_PI) - 1);
    if (test > 1.e-10) TRIQS_RUNTIME_ERROR << "Meshes are not compatible";

    const double tmin = t_mesh.x_min();
    const double wmin = gw.mesh().x_min();
    //a is a number very larger than delta_w and very smaller than wmax-wmin, used in the tail computation
    const double a = gw.mesh().delta() * sqrt(double(L));

    long n_others = second_dim(gw.data());

    array<dcomplex, 2> _gin(L, n_others);
    array<dcomplex, 2> _gout(L, n_others);

    auto _  = range();
    auto m1 = mom_12(0, _);
    auto m2 = mom_12(1, _);

    array<dcomplex, 1> a1 = (m1 + I * m2 / a) / 2., a2 = (m1 - I * m2 / a) / 2.;

    for (auto const &w : gw.mesh()) _gin(w.index(), _) = (gw[w] - a1 * th_expo_inv(w, a) - a2 * th_expo_neg_inv(w, a)) * std::exp(-I * w * tmin);

    int dims[] = {int(L)};
    _fourier_base(_gin, _gout, 1, dims, n_others, FFTW_FORWARD);

    auto gt           = gf_vec_t<retime>{t_mesh, {int(n_others)}};
    const double corr = 1.0 / (t_mesh.delta() * L);
    for (auto const &t : t_mesh) gt[t] = corr * std::exp(I * wmin * (tmin - t)) * _gout(t.index(), _) + a1 * th_expo(t, a) + a2 * th_expo_neg(t, a);

    return std::move(gt);
  }

} // namespace triqs::gfs
