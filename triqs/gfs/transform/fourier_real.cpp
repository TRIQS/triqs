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
//#include "fourier_real.hpp"
#include <fftw3.h>

namespace triqs { namespace gfs { 
 
 gf_mesh<refreq> make_mesh_fourier_compatible(gf_mesh<retime> const& m) {
  int L = m.size();
  double pi = std::acos(-1);
  double wmin = -pi * (L - 1) / (L * m.delta());
  double wmax = pi * (L - 1) / (L * m.delta());
  return {wmin, wmax, L};
 }

 gf_mesh<retime> make_mesh_fourier_compatible(gf_mesh<refreq> const& m) {
  double pi = std::acos(-1);
  int L = m.size();
  double tmin = -pi * (L-1) / (L*m.delta());
  double tmax =  pi * (L-1) / (L*m.delta());
  return {tmin, tmax, L};
 }

 namespace {
  double pi = std::acos(-1);
  dcomplex I(0,1);
  inline dcomplex th_expo(double t, double a )     { return (t > 0 ? -I * exp(-a*t) : ( t < 0 ? 0 : -0.5 * I * exp(-a*t) ) ) ; }
  inline dcomplex th_expo_neg(double t, double a ) { return (t < 0 ?  I * exp( a*t) : ( t > 0 ? 0 :  0.5 * I * exp( a*t) ) ) ; }
  inline dcomplex th_expo_inv(double w, double a )     { return 1./(w+I*a) ; }
  inline dcomplex th_expo_neg_inv(double w, double a ) { return 1./(w-I*a) ; }
 }
 
 struct impl_worker {
  
  arrays::vector<dcomplex> g_in, g_out;
  
  void direct (gf_view<refreq,scalar_valued> gw, gf_const_view<retime,scalar_valued> gt){
   
   size_t L = gt.mesh().size();
   if (gw.mesh().size() != L) TRIQS_RUNTIME_ERROR << "Meshes are different";
   double test = std::abs(gt.mesh().delta() * gw.mesh().delta() * L / (2*pi) -1);
   if (test > 1.e-10) TRIQS_RUNTIME_ERROR << "Meshes are not compatible";
   
   const double tmin = gt.mesh().x_min();
   const double wmin = gw.mesh().x_min(); 
   //a is a number very larger than delta_w and very smaller than wmax-wmin, used in the tail computation
   const double a = gw.mesh().delta() * sqrt( double(L) );
   
   auto ta = gt.singularity();
   g_in.resize(L);
   g_in() = 0;
   g_out.resize(L);
   
   dcomplex t1 = ta(1), t2= ta.get_or_zero(2);
   dcomplex a1 = (t1 + I * t2/a )/2., a2 = (t1 - I * t2/a )/2.;
   
   for (auto const & t : gt.mesh())
    g_in[t.index()] = (gt[t] - (a1*th_expo(t,a) + a2*th_expo_neg(t,a))) * std::exp(I*t*wmin);
   
   details::fourier_base(g_in, g_out, L, true);
   
   for (auto const & w : gw.mesh())
    gw[w] = gt.mesh().delta() * std::exp(I*(w-wmin)*tmin) * g_out(w.index())
    + a1*th_expo_inv(w,a) + a2*th_expo_neg_inv(w,a);
   
   gw.singularity() = gt.singularity();// set tail
   
  }
  
  void inverse(gf_view<retime,scalar_valued> gt, gf_const_view<refreq,scalar_valued> gw){
   
   size_t L = gw.mesh().size();
   if ( L != gt.mesh().size()) TRIQS_RUNTIME_ERROR << "Meshes are different";
   double test = std::abs(gt.mesh().delta() * gw.mesh().delta() * L / (2*pi) -1);
   if (test > 1.e-10) TRIQS_RUNTIME_ERROR << "Meshes are not compatible";
   
   const double tmin = gt.mesh().x_min();
   const double wmin = gw.mesh().x_min();
   //a is a number very larger than delta_w and very smaller than wmax-wmin, used in the tail computation
   const double a = gw.mesh().delta() * sqrt( double(L) );
   
   auto ta = gw.singularity();
   arrays::vector<dcomplex> g_in(L), g_out(L);
   
   dcomplex t1 = ta(1), t2 = ta.get_or_zero(2);
   dcomplex a1 = (t1 + I * t2/a )/2., a2 = (t1 - I * t2/a )/2.;
   g_in() = 0;
   
   for (auto const & w: gw.mesh())
    g_in(w.index()) = (gw[w] - a1*th_expo_inv(w,a) - a2*th_expo_neg_inv(w,a)  ) * std::exp(-I*w*tmin);
   
   details::fourier_base(g_in, g_out, L, false);
   
   const double corr = 1.0/(gt.mesh().delta()*L);
   for (auto const & t : gt.mesh())
    gt[t] = corr * std::exp(I*wmin*(tmin-t)) *
    g_out[ t.index() ] + a1 * th_expo(t,a) + a2 * th_expo_neg(t,a) ;
 
   // set tail
   gt.singularity() = gw.singularity();
  }
  
 };

 //--------------------------------------------------------------------------------------

 void _fourier_impl(gf_view<refreq, scalar_valued> gw, gf_const_view<retime, scalar_valued> gt) {
  impl_worker w;
  w.direct(gw, gt);
 }

 void _fourier_impl(gf_view<retime, scalar_valued> gt, gf_const_view<refreq, scalar_valued> gw) {
  impl_worker w;
  w.inverse(gt, gw);
 }
}
}
