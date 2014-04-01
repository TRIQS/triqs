/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
#include "fourier_base.hpp"
#include "fourier_matsubara.hpp"
#include <fftw3.h>

namespace triqs {
namespace gfs {

 template <typename GfElementType> GfElementType convert_green(dcomplex const& x) { return x; }
 template <> double convert_green<double>(dcomplex const& x) { return real(x); }

 //--------------------------------------------------------------------------------------

 struct impl_worker {

  arrays::vector<dcomplex> g_in, g_out;

  dcomplex oneFermion(dcomplex a, double b, double tau, double beta) {
   return -a * (b >= 0 ? exp(-b * tau) / (1 + exp(-beta * b)) : exp(b * (beta - tau)) / (1 + exp(beta * b)));
  }

  dcomplex oneBoson(dcomplex a, double b, double tau, double beta) {
   return a * (b >= 0 ? exp(-b * tau) / (exp(-beta * b) - 1) : exp(b * (beta - tau)) / (1 - exp(b * beta)));
  }

  //-------------------------------------

  void direct(gf_view<imfreq, scalar_valued> gw, gf_const_view<imtime, scalar_valued> gt) {
   auto ta = gt(freq_infty());
   direct_impl(make_gf_view_without_tail(gw), make_gf_view_without_tail(gt), ta);
   gw.singularity() = gt.singularity(); // set tail
  }

  void direct(gf_view<imfreq, scalar_valued, no_tail> gw, gf_const_view<imtime, scalar_valued, no_tail> gt) {
   auto ta = local::tail{1,1};
   direct_impl(gw, gt, ta);
  }
  
  //-------------------------------------

  private:
  void direct_impl(gf_view<imfreq, scalar_valued, no_tail> gw, gf_const_view<imtime, scalar_valued, no_tail> gt,
                   local::tail const& ta) {
   // TO BE MODIFIED AFTER SCALAR IMPLEMENTATION TODO
   dcomplex d = ta(1)(0, 0), A = ta.get_or_zero(2)(0, 0), B = ta.get_or_zero(3)(0, 0);
   double b1 = 0, b2 = 0, b3 = 0;
   dcomplex a1, a2, a3;
   double beta = gt.mesh().domain().beta;
   auto L = (gt.mesh().kind() == full_bins ? gt.mesh().size() - 1 : gt.mesh().size());
   double fact = beta / gt.mesh().size();
   dcomplex iomega = dcomplex(0.0, 1.0) * std::acos(-1) / beta;
   dcomplex iomega2 = iomega * 2 * gt.mesh().delta() * (gt.mesh().kind() == half_bins ? 0.5 : 0.0);
   g_in.resize(gt.mesh().size());
   g_out.resize(gw.mesh().size());
   if (gw.domain().statistic == Fermion) {
    b1 = 0;
    b2 = 1;
    b3 = -1;
    a1 = d - B;
    a2 = (A + B) / 2;
    a3 = (B - A) / 2;
   } else {
    b1 = -0.5;
    b2 = -1;
    b3 = 1;
    a1 = 4 * (d - B) / 3;
    a2 = B - (d + A) / 2;
    a3 = d / 6 + A / 2 + B / 3;
   }
   if (gw.domain().statistic == Fermion) {
    for (auto& t : gt.mesh())
     g_in[t.index()] = fact * exp(iomega * t) *
                       (gt[t] - (oneFermion(a1, b1, t, beta) + oneFermion(a2, b2, t, beta) + oneFermion(a3, b3, t, beta)));
   } else {
    for (auto& t : gt.mesh())
     g_in[t.index()] = fact * (gt[t] - (oneBoson(a1, b1, t, beta) + oneBoson(a2, b2, t, beta) + oneBoson(a3, b3, t, beta)));
   }
   details::fourier_base(g_in, g_out, L, true);
   for (auto& w : gw.mesh()) {
    gw[w] = g_out(w.index()) * exp(iomega2 * w.index()) + a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3);
   }
  }

  public:
  //-------------------------------------

  void inverse(gf_view<imtime, scalar_valued> gt, gf_const_view<imfreq, scalar_valued> gw) {
   static bool Green_Function_Are_Complex_in_time = false;
   // If the Green function are NOT complex, then one use the symmetry property
   // fold the sum and get a factor 2
   auto ta = gw(freq_infty());
   // TO BE MODIFIED AFTER SCALAR IMPLEMENTATION TODO
   dcomplex d = ta(1)(0, 0), A = ta.get_or_zero(2)(0, 0), B = ta.get_or_zero(3)(0, 0);
   double b1, b2, b3;
   dcomplex a1, a2, a3;

   double beta = gw.domain().beta;
   size_t L = gt.mesh().size() - (gt.mesh().kind() == full_bins ? 1 : 0); // L can be different from gt.mesh().size() (depending
                                                                          // on the mesh kind) and is given to the FFT algorithm
   dcomplex iomega = dcomplex(0.0, 1.0) * std::acos(-1) / beta;
   dcomplex iomega2 = -iomega * 2 * gt.mesh().delta() * (gt.mesh().kind() == half_bins ? 0.5 : 0.0);
   double fact = (Green_Function_Are_Complex_in_time ? 1 : 2) / beta;
   g_in.resize(gw.mesh().size());
   g_out.resize(gt.mesh().size());

   if (gw.domain().statistic == Fermion) {
    b1 = 0;
    b2 = 1;
    b3 = -1;
    a1 = d - B;
    a2 = (A + B) / 2;
    a3 = (B - A) / 2;
   } else {
    b1 = -0.5;
    b2 = -1;
    b3 = 1;
    a1 = 4 * (d - B) / 3;
    a2 = B - (d + A) / 2;
    a3 = d / 6 + A / 2 + B / 3;
   }
   g_in() = 0;
   for (auto& w : gw.mesh()) {
    g_in[w.index()] = fact * exp(w.index() * iomega2) * (gw[w] - (a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3)));
   }
   // for bosons GF(w=0) is divided by 2 to avoid counting it twice
   if (gw.domain().statistic == Boson && !Green_Function_Are_Complex_in_time) g_in(0) *= 0.5;

   details::fourier_base(g_in, g_out, L, false);

   // CORRECT FOR COMPLEX G(tau) !!!
   typedef double gt_result_type;
   // typedef typename gf<imtime>::mesh_type::gf_result_type gt_result_type;
   if (gw.domain().statistic == Fermion) {
    for (auto& t : gt.mesh()) {
     gt[t] =
         convert_green<gt_result_type>(g_out(t.index() == L ? 0 : t.index()) * exp(-iomega * t) + oneFermion(a1, b1, t, beta) +
                                       oneFermion(a2, b2, t, beta) + oneFermion(a3, b3, t, beta));
    }
   } else {
    for (auto& t : gt.mesh())
     gt[t] = convert_green<gt_result_type>(g_out(t.index() == L ? 0 : t.index()) + oneBoson(a1, b1, t, beta) +
                                           oneBoson(a2, b2, t, beta) + oneBoson(a3, b3, t, beta));
   }
   double pm = (gw.domain().statistic == Fermion ? -1.0 : 1.0);
   if (gt.mesh().kind() == full_bins) gt.on_mesh(L) = pm * (gt.on_mesh(0) + convert_green<gt_result_type>(ta(1)(0, 0)));
   // set tail
   gt.singularity() = gw.singularity();
  }

 }; // class worker

 //--------------------------------------------

 template <typename Opt>
 void fourier_impl(gf_view<imfreq, scalar_valued, Opt> gw, gf_const_view<imtime, scalar_valued, Opt> gt) {
  impl_worker w;
  w.direct(gw, gt);
 }

 template <typename Opt>
 void fourier_impl(gf_view<imfreq, matrix_valued, Opt> gw, gf_const_view<imtime, matrix_valued, Opt> gt) {
  impl_worker w;
  for (size_t n1 = 0; n1 < gt.data().shape()[1]; n1++)
   for (size_t n2 = 0; n2 < gt.data().shape()[2]; n2++) {
    auto gw_sl = slice_target_to_scalar(gw, n1, n2);
    auto gt_sl = slice_target_to_scalar(gt, n1, n2);
    w.direct(gw_sl, gt_sl);
   }
 }

 //---------------------------------------------------------------------------

 void inverse_fourier_impl(gf_view<imtime, scalar_valued> gt, gf_const_view<imfreq, scalar_valued> gw) {
  impl_worker w;
  w.inverse(gt, gw);
 }

 void inverse_fourier_impl(gf_view<imtime, matrix_valued> gt, gf_const_view<imfreq, matrix_valued> gw) {
  impl_worker w;
  for (size_t n1 = 0; n1 < gw.data().shape()[1]; n1++)
   for (size_t n2 = 0; n2 < gw.data().shape()[2]; n2++) {
    auto gt_sl = slice_target_to_scalar(gt, n1, n2);
    auto gw_sl = slice_target_to_scalar(gw, n1, n2);
    w.inverse(gt_sl, gw_sl);
   }
 }

 //---------------------------------------------------------------------------
 void triqs_gf_view_assign_delegation(gf_view<imfreq, scalar_valued> g,
                                      gf_keeper<tags::fourier, imtime, scalar_valued> const& L) {
  fourier_impl(g, L.g);
 }
 void triqs_gf_view_assign_delegation(gf_view<imfreq, matrix_valued> g,
                                      gf_keeper<tags::fourier, imtime, matrix_valued> const& L) {
  fourier_impl(g, L.g);
 }
 void triqs_gf_view_assign_delegation(gf_view<imtime, scalar_valued> g,
                                      gf_keeper<tags::fourier, imfreq, scalar_valued> const& L) {
  inverse_fourier_impl(g, L.g);
 }
 void triqs_gf_view_assign_delegation(gf_view<imtime, matrix_valued> g,
                                      gf_keeper<tags::fourier, imfreq, matrix_valued> const& L) {
  inverse_fourier_impl(g, L.g);
 }
 void triqs_gf_view_assign_delegation(gf_view<imfreq, scalar_valued, no_tail> g,
                                      gf_keeper<tags::fourier, imtime, scalar_valued, no_tail> const& L) {
  fourier_impl(g, L.g);
 }
 void triqs_gf_view_assign_delegation(gf_view<imfreq, matrix_valued, no_tail> g,
                                      gf_keeper<tags::fourier, imtime, matrix_valued, no_tail> const& L) {
  fourier_impl(g, L.g);
 }
}
}

