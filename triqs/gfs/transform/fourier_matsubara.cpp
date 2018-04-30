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
   TRIQS_RUNTIME_ERROR << "DISABLED direct fourier, high frequency moments unknown\n";
   auto tail = array<dcomplex, 1>{ 0.0, 0.0, 0.0, 0.0 };
   direct(gw, gt, tail);
  }

  //-------------------------------------

  private:
  void direct(gf_view<imfreq, scalar_valued> gw, gf_const_view<imtime, scalar_valued> gt, array_const_view<dcomplex, 1> tail) {
   if (std::abs(tail(0)) > 1e-15) TRIQS_RUNTIME_ERROR << "ERROR: Direct Fourier implementation requires vanishing 0th moment\n";
   if (first_dim(tail) < 3) TRIQS_RUNTIME_ERROR << "ERROR: Direct Fourier implementation requires at least a proper 2nd high-frequency moment\n";

   double beta = gt.mesh().domain().beta;
   auto L = gt.mesh().size() - 1;
   if (L < 2*(gw.mesh().last_index()+1))
    TRIQS_RUNTIME_ERROR << "Fourier: The time mesh mush be at least twice as long as the number of positive frequencies :\n gt.mesh().size() =  " << gt.mesh().size()
                        << " gw.mesh().last_index()" << gw.mesh().last_index();
   double fact = beta / L;
   dcomplex iomega = M_PI * 1_j / beta;
   bool is_fermion = (gw.domain().statistic == Fermion);

   double b1, b2, b3;
   dcomplex a1, a2, a3;
   dcomplex d = tail(1), A = tail(2), B = (first_dim(tail) == 3) ? 0 : tail(3);

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
  
   auto [tail, error] = get_tail(gw);
   if (error > 1e-6) std::cerr << "WARNING: High frequency moments have an error greater than 1e-6.\n";
   if (error > 1e-3) TRIQS_RUNTIME_ERROR << "ERROR: High frequency moments have an error greater than 1e-3.\n";
   if (first_dim(tail) < 3) TRIQS_RUNTIME_ERROR << "ERROR: Inverse Fourier implementation requires at least a proper 2nd high-frequency moment\n";
   if (std::abs(tail(0)) > 1e-15) TRIQS_RUNTIME_ERROR << "ERROR: Inverse Fourier implementation requires vanishing 0th moment\n";

   double beta = gw.domain().beta;
   long L = gt.mesh().size() - 1;
   if (L < 2*(gw.mesh().last_index()+1))
    TRIQS_RUNTIME_ERROR << "Inverse Fourier: The time mesh mush be at least twice as long as the freq mesh :\n gt.mesh().size() =  " << gt.mesh().size()
                        << " gw.mesh().last_index()" << gw.mesh().last_index();
   dcomplex iomega = M_PI* 1_j / beta;
   double fact = 1.0 / beta;
   bool is_fermion = (gw.domain().statistic == Fermion);

   double b1, b2, b3;
   dcomplex a1, a2, a3;
   dcomplex d = tail(1), A = tail(2), B = (first_dim(tail) == 3) ? 0 : tail(3);

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

