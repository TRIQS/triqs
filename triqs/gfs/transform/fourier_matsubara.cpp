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
#include "../../gfs.hpp"
#include <fftw3.h>

namespace triqs {
namespace gfs {

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
   auto ta = gt.singularity();
   direct_impl(gw, gt, ta);
   gw.singularity() = gt.singularity(); // set tail
  }

  //-------------------------------------

  private:
  void direct_impl(gf_view<imfreq, scalar_valued> gw, gf_const_view<imtime, scalar_valued> gt, __tail<scalar_valued> const& ta) {
   // TO BE MODIFIED AFTER SCALAR IMPLEMENTATION TODO
   //dcomplex d = ta(1), A = ta.get_or_zero(2), B = ta.get_or_zero(3);
   dcomplex d = ta.get_or_zero(1), A = ta.get_or_zero(2), B = ta.get_or_zero(3);
   //if (arrays::isnan(d)) TRIQS_RUNTIME_ERROR << " direct fourier : 1/omega order undefined in tail";
   double b1 = 0, b2 = 0, b3 = 0;
   dcomplex a1, a2, a3;
   double beta = gt.mesh().domain().beta;
   auto L = gt.mesh().size() - 1;
   if (L < 2*(gw.mesh().last_index()+1))
    TRIQS_RUNTIME_ERROR << "Fourier: The time mesh mush be at least twice as long as the number of positive frequencies :\n gt.mesh().size() =  " << gt.mesh().size()
                        << " gw.mesh().last_index()" << gw.mesh().last_index();
   double fact = beta / L;
   dcomplex iomega = M_PI * 1_j / beta;
   bool is_fermion = (gw.domain().statistic == Fermion);

   if (is_fermion) {
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

   //debug: check it is useless ...
   //a1=0; a2=0;a3=0;

   g_in.resize(L + 1);
   g_out.resize(L);
   g_in() = 0;
 
   if (is_fermion) {
    for (auto const & t : gt.mesh())
       g_in[t.index()] = fact * exp(iomega * t) *
                         (gt[t] - (oneFermion(a1, b1, t, beta) + oneFermion(a2, b2, t, beta) + oneFermion(a3, b3, t, beta)));
   } else {
    for (auto const & t : gt.mesh())
       g_in[t.index()] = fact * (gt[t] - (oneBoson(a1, b1, t, beta) + oneBoson(a2, b2, t, beta) + oneBoson(a3, b3, t, beta)));
   }
   g_in[L] = 0;

   auto g_in_fft = reinterpret_cast<fftw_complex*>(g_in.data_start());
   auto g_out_fft = reinterpret_cast<fftw_complex*>(g_out.data_start());
 
   auto p = fftw_plan_dft_1d(L, g_in_fft, g_out_fft, FFTW_BACKWARD, FFTW_ESTIMATE); // in our convention backward is direct
   fftw_execute(p); 
   fftw_destroy_plan(p); 

   //details::fourier_base(g_in, g_out, L, true);

   // I do not see the use of this term
   // If the discontinuity is perfectly correct, this term is 0.
   // We manually remove half of the first time point contribution and add half
   // of the last time point contribution. This is necessary to make sure that no symmetry is lost
   dcomplex corr = -0.5 * fact * (gt[0] + d + (is_fermion ? 1 : -1) * gt[L]);
   for (auto const& w : gw.mesh()) gw[w] = g_out((w.index() + L) % L) + corr + a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3);
   
   //for (auto const& w : gw.mesh()) gw[w] = g_out((w.index() + L) % L) + a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3);
  }

  public:
  //-------------------------------------

  void inverse(gf_view<imtime, scalar_valued> gt, gf_const_view<imfreq, scalar_valued> gw) {
   if (gw.mesh().positive_only()) TRIQS_RUNTIME_ERROR << "Fourier is only implemented for g(i omega_n) with full mesh (positive and negative frequencies)";
  
   auto const ta = gw.singularity();
   const dcomplex d = ta.get_or_zero(1), A = ta.get_or_zero(2), B = ta.get_or_zero(3);
   if (arrays::isnan(d)) TRIQS_RUNTIME_ERROR << " inverse fourier : 1/omega order undefined in tail";
   double b1, b2, b3;
   dcomplex a1, a2, a3;
   double beta = gw.domain().beta;
   long L = gt.mesh().size() - 1;
   if (L < 2*(gw.mesh().last_index()+1))
    TRIQS_RUNTIME_ERROR << "Inverse Fourier: The time mesh mush be at least twice as long as the freq mesh :\n gt.mesh().size() =  " << gt.mesh().size()
                        << " gw.mesh().last_index()" << gw.mesh().last_index();
   dcomplex iomega = M_PI* 1_j / beta;
   double fact = 1.0 / beta;
   bool is_fermion = (gw.domain().statistic == Fermion);

   if (is_fermion) {
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

   g_in.resize(L); // L>=2*(gw.mesh().last_index()+1) , we will fill the middle array with 0
   g_out.resize(L + 1);

   g_in() = 0;
   for (auto const& w : gw.mesh()) g_in[(w.index() + L) % L] = fact * (gw[w] - (a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3)));

   auto g_in_fft = reinterpret_cast<fftw_complex*>(g_in.data_start());
   auto g_out_fft = reinterpret_cast<fftw_complex*>(g_out.data_start());
 
   auto p = fftw_plan_dft_1d(L, g_in_fft, g_out_fft, FFTW_FORWARD, FFTW_ESTIMATE); // in our convention backward is inverse FFT
   fftw_execute(p); 
   fftw_destroy_plan(p); 

   if (is_fermion) {
    for (auto const & t : gt.mesh()) {
      gt[t] = g_out(t.index()) * exp(-iomega * t) + oneFermion(a1, b1, t, beta) + oneFermion(a2, b2, t, beta) +
              oneFermion(a3, b3, t, beta);
    }
   } else {
    for (auto const & t : gt.mesh())
      gt[t] = g_out(t.index()) + oneBoson(a1, b1, t, beta) + oneBoson(a2, b2, t, beta) + oneBoson(a3, b3, t, beta);
   }
   double pm = (is_fermion ? -1 : 1);
   gt.on_mesh(L) = pm * (gt.on_mesh(0) + d); 
   // set tail
   gt.singularity() = gw.singularity();
  }

 }; // class worker

 //--------------------------------------------

 // Direct transformation imtime -> imfreq, with a tail
 void _fourier_impl(gf_view<imfreq, scalar_valued> gw, gf_const_view<imtime, scalar_valued> gt) {
  impl_worker w;
  w.direct(gw, gt);
 }


 // Inverse transformation imfreq -> imtime: tail is mandatory
 void _fourier_impl(gf_view<imtime, scalar_valued> gt, gf_const_view<imfreq, scalar_valued> gw) {
  impl_worker w;
  w.inverse(gt, gw);
 }
}
}

