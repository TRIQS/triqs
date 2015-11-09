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
#include "functions.hpp"
#include <triqs/utility/legendre.hpp>

namespace triqs { namespace gfs {

 using arrays::array;

 //-------------------------------------------------------
 // For Imaginary Time functions
 // ------------------------------------------------------
 gf<imtime> rebinning_tau(gf_const_view<imtime> const& g, int new_n_tau) {

  auto const& old_m = g.mesh();
  gf<imtime> new_gf{{old_m.domain().beta, old_m.domain().statistic, new_n_tau}, get_target_shape(g)};
  auto const& new_m = new_gf.mesh();
  new_gf.data()() = 0;
  long prev_index = 0;
  long norm = 0;
  for (auto const & tau : old_m) {
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

 //-------------------------------------------------------
 // For Imaginary Matsubara Frequency functions
 // ------------------------------------------------------
 arrays::matrix<dcomplex> density(gf_const_view<imfreq> g) {

  if (g.mesh().positive_only())
   TRIQS_RUNTIME_ERROR << "density is only implemented for g(i omega_n) with full mesh (positive and negative frequencies)";

  tail_const_view t = g.singularity();
  if (!t.is_decreasing_at_infinity())
   TRIQS_RUNTIME_ERROR << " density computation : green Function is not as 1/omega or less !!!";

  if (g.mesh().positive_only()) TRIQS_RUNTIME_ERROR << " imfreq gF : full mesh required in density computation";
  auto sh = get_target_shape(g);
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
  arrays::matrix<dcomplex> res(get_target_shape(gl));
  res() = 0.0;
  for (auto const& l : gl.mesh()) res -= sqrt(2 * l.index() + 1) * gl[l];
  res /= gl.domain().beta;
  return res;
 }

 //-------------------------------------------------------
 // For Legendre functions
 // ------------------------------------------------------

 // compute a tail from the Legendre GF
 // this is Eq. 8 of our paper
 tail_view get_tail(gf_const_view<legendre> gl, int size = 10, int omin = -1) {

   auto sh = gl.data().shape().front_pop();
   tail t(sh, size, omin);
   t.data()() = 0.0;

   for (int p=1; p<=t.order_max(); p++)
     for (auto l : gl.mesh())
       t(p) += (triqs::utility::legendre_t(l.index(),p)/pow(gl.domain().beta,p)) * gl[l];

   return t;

 }

 // Impose a discontinuity G(\tau=0)-G(\tau=\beta)
 void enforce_discontinuity(gf_view<legendre> & gl, arrays::array_view<double,2> disc) {

   double norm = 0.0;
   arrays::vector<double> t(gl.data().shape()[0]);
   for (int i=0; i<t.size(); ++i) {
     t(i) = triqs::utility::legendre_t(i,1) / gl.domain().beta;
     norm += t(i)*t(i);
   }

   arrays::array<dcomplex, 2> corr(disc.shape());
   corr() = 0;
   for (auto const &l : gl.mesh()) corr += t(l.index()) * gl[l];

   auto _ = arrays::range{};
   for (auto const& l : gl.mesh()) gl.data()(l.index(), _, _) += (disc - corr) * t(l.index()) / norm;
 }


}}
