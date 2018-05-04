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

  // FIXME Generalize to matrix / tensor_valued gf
  void direct(gf_view<imfreq, scalar_valued> gw, gf_const_view<imtime, scalar_valued> gt, dcomplex m2 = 0.0, dcomplex m3 = 0.0) {

   double beta = gt.mesh().domain().beta;
   auto L = gt.mesh().size() - 1;
   if (L < 2*(gw.mesh().last_index()+1))
    TRIQS_RUNTIME_ERROR << "Fourier: The time mesh mush be at least twice as long as the number of positive frequencies :\n gt.mesh().size() =  " << gt.mesh().size()
                        << " gw.mesh().last_index()" << gw.mesh().last_index();

   auto d_vec = vector<dcomplex>(7);
   for (int m : range(1, 8)) d_vec[m - 1] = (gt[m] - gt[0]) / gt.mesh()[m];
   
   // Inverse of the Vandermonde matrix V_{m,j} = m^{j-1}
   auto V_inv = matrix<dcomplex>{
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
   
   // Calculate the 2nd
   auto g_vec = V_inv * d_vec;
   m2 = g_vec[0];
   m3 = -g_vec[1] * 2 / gt.mesh().delta();

   g_in.resize(L + 1);
   g_out.resize(L);
   g_in() = 0;

   bool is_fermion = (gw.domain().statistic == Fermion);
   double fact = beta / L;
   dcomplex iomega = M_PI * 1_j / beta;

   double b1, b2, b3;
   dcomplex a1, a2, a3;

   if (is_fermion) {
    dcomplex m1 = -(gt[0] + gt[L]);
    b1 = 0;
    b2 = 1;
    b3 = -1;
    a1 = m1 - m3;
    a2 = (m2 + m3) / 2;
    a3 = (m3 - m2) / 2;

    for (auto const & t : gt.mesh())
       g_in[t.index()] = fact * exp(iomega * t) *
                         (gt[t] - (oneFermion(a1, b1, t, beta) + oneFermion(a2, b2, t, beta) + oneFermion(a3, b3, t, beta)));

   } else {
    dcomplex m1 = -(gt[0] - gt[L]);
    b1 = -0.5;
    b2 = -1;
    b3 = 1;
    a1 = 4 * (m1 - m3) / 3;
    a2 = m3 - (m1 + m2) / 2;
    a3 = m1 / 6 + m2 / 2 + m3 / 3;

    for (auto const & t : gt.mesh())
       g_in[t.index()] = fact * (gt[t] - (oneBoson(a1, b1, t, beta) + oneBoson(a2, b2, t, beta) + oneBoson(a3, b3, t, beta)));
   }

   g_in[L] = 0;

   auto g_in_fft = reinterpret_cast<fftw_complex*>(g_in.data_start());
   auto g_out_fft = reinterpret_cast<fftw_complex*>(g_out.data_start());
 
   // FIXME Factorize plan into worker
   auto p = fftw_plan_dft_1d(L, g_in_fft, g_out_fft, FFTW_BACKWARD, FFTW_ESTIMATE); // in our convention backward is direct
   fftw_execute(p); 
   fftw_destroy_plan(p); 

   for (auto const& w : gw.mesh()) gw[w] = g_out((w.index() + L) % L) + a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3);
  }

  //-------------------------------------

  // FIXME Generalize to matrix / tensor_valued gf
  // FIXME : doc : we assume that the fuction is at least 1/omega
  void inverse(gf_view<imtime, scalar_valued> gt, gf_const_view<imfreq, scalar_valued> gw) {
   if (gw.mesh().positive_only()) TRIQS_RUNTIME_ERROR << "Fourier is only implemented for g(i omega_n) with full mesh (positive and negative frequencies)";
  
   auto [tail, error] = get_tail(gw, array<dcomplex, 1> {0});
   if (error > 1e-6) std::cerr << "WARNING: High frequency moments have an error greater than 1e-6.\n Error = "<<error;
   if (error > 1e-3) TRIQS_RUNTIME_ERROR << "ERROR: High frequency moments have an error greater than 1e-3.\n  Error = "<<error;
   if (first_dim(tail) < 3) TRIQS_RUNTIME_ERROR << "ERROR: Inverse Fourier implementation requires at least a proper 2nd high-frequency moment\n";
   
   // FIXME
   //if (std::abs(tail(0)) > 1e-10) TRIQS_RUNTIME_ERROR << "ERROR: Inverse Fourier implementation requires vanishing 0th moment\n  error is :" << std::abs(tail(0));

   double beta = gw.domain().beta;
   long L = gt.mesh().size() - 1;
   if (L < 2*(gw.mesh().last_index()+1))
    TRIQS_RUNTIME_ERROR << "Inverse Fourier: The time mesh mush be at least twice as long as the freq mesh :\n gt.mesh().size() =  " << gt.mesh().size()
                        << " gw.mesh().last_index()" << gw.mesh().last_index();

   g_in.resize(L); // L>=2*(gw.mesh().last_index()+1) , we will fill the middle array with 0
   g_out.resize(L + 1);
   g_in() = 0;

   bool is_fermion = (gw.domain().statistic == Fermion);
   double fact = 1.0 / beta;
   dcomplex iomega = M_PI* 1_j / beta;

   double b1, b2, b3;
   dcomplex a1, a2, a3;
   dcomplex m1 = tail(1), m2 = tail(2), m3 = (first_dim(tail) == 3) ? 0 : tail(3);

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

   for (auto const& w : gw.mesh()) g_in[(w.index() + L) % L] = fact * (gw[w] - (a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3)));

   auto g_in_fft = reinterpret_cast<fftw_complex*>(g_in.data_start());
   auto g_out_fft = reinterpret_cast<fftw_complex*>(g_out.data_start());
 
   // FIXME Factorize plan into worker
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
   gt[L] = pm * (gt[0] + m1);
  }

 }; // class worker

 //--------------------------------------------

 // Direct transformation imtime -> imfreq, with a tail
 void _fourier_impl(gf_view<imfreq, scalar_valued> gw, gf_const_view<imtime, scalar_valued> gt, dcomplex m2, dcomplex m3) {
  impl_worker w;
  w.direct(gw, gt, m2, m3);
 }


 // Inverse transformation imfreq -> imtime: tail is mandatory
 void _fourier_impl(gf_view<imtime, scalar_valued> gt, gf_const_view<imfreq, scalar_valued> gw) {
  impl_worker w;
  w.inverse(gt, gw);
 }
}
}

