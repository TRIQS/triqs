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

namespace triqs::gfs { 
 
 using matrix_t = arrays::matrix<dcomplex>;

 gf_mesh<refreq> make_mesh_fourier_compatible(gf_mesh<retime> const& m) {
  int L = m.size();
  double wmin = -M_PI * (L - 1) / (L * m.delta());
  double wmax = M_PI * (L - 1) / (L * m.delta());
  return {wmin, wmax, L};
 }

 gf_mesh<retime> make_mesh_fourier_compatible(gf_mesh<refreq> const& m) {
  int L = m.size();
  double tmin = -M_PI * (L-1) / (L*m.delta());
  double tmax =  M_PI * (L-1) / (L*m.delta());
  return {tmin, tmax, L};
 }

 namespace {
  dcomplex I(0,1);
  inline dcomplex th_expo(double t, double a )     { return (t > 0 ? -I * exp(-a*t) : ( t < 0 ? 0 : -0.5 * I * exp(-a*t) ) ) ; }
  inline dcomplex th_expo_neg(double t, double a ) { return (t < 0 ?  I * exp( a*t) : ( t > 0 ? 0 :  0.5 * I * exp( a*t) ) ) ; }
  inline dcomplex th_expo_inv(double w, double a )     { return 1./(w+I*a) ; }
  inline dcomplex th_expo_neg_inv(double w, double a ) { return 1./(w-I*a) ; }
 }
 
  // ------------------------ DIRECT TRANSFORM --------------------------------------------

  gf<refreq, tensor_valued<1>> _fourier_impl(gf_mesh<refreq> const &w_mesh, gf<retime, tensor_valued<1>> &&gt, arrays::array_const_view<dcomplex, 2> mom_12){
   
   if (mom_12.is_empty()) TRIQS_RUNTIME_ERROR << " Tail fit not IMPLEMENTED";

   size_t L = gt.mesh().size();
   if (w_mesh.size() != L) TRIQS_RUNTIME_ERROR << "Meshes are different";
   double test = std::abs(gt.mesh().delta() * w_mesh.delta() * L / (2*M_PI) -1);
   if (test > 1.e-10) TRIQS_RUNTIME_ERROR << "Meshes are not compatible";

    long n_others = second_dim(gt.data());
    matrix_t _gout(L, n_others);
    matrix_t _gin(L, n_others);
   
   const double tmin = gt.mesh().x_min();
   const double wmin = w_mesh.x_min(); 
   //a is a number very larger than delta_w and very smaller than wmax-wmin, used in the tail computation
   const double a = w_mesh.delta() * sqrt( double(L) );
   
   auto m1 = mom_12(0, range());
   auto m2 = mom_12(1, range());

   auto a1 = (m1 + I * m2/a )/2., a2 = (m1 - I * m2/a )/2.;
   
   auto _ = range();
   for (auto const & t : gt.mesh())
     _gin(t.index(),_) = (gt[t] - (a1*th_expo(t,a) + a2*th_expo_neg(t,a))) * std::exp(I*t*wmin);
   
   // FIXME ADJUST fourier_base and reuse fourier_base also in fourier_matsubara
   details::fourier_base(_gin, _gout, L, true);
   
   auto gw = gf<refreq, tensor_valued<1>>{w_mesh, {n_others}};
   for (auto const & w : w_mesh)
    gw[w] = gt.mesh().delta() * std::exp(I*(w-wmin)*tmin) * _gout(w.index(),_)
    + a1*th_expo_inv(w,a) + a2*th_expo_neg_inv(w,a);

   return std::move(gw);
  }
  
  // ------------------------ INVERSE TRANSFORM --------------------------------------------

  gf<retime, tensor_valued<1>> _fourier_impl(gf_mesh<retime> const & t_mesh, gf<refreq, tensor_valued<1>> &&gw, arrays::array_const_view<dcomplex, 2> mom_12){
   
   if (mom_12.is_empty()) TRIQS_RUNTIME_ERROR << " Tail fit not IMPLEMENTED"; 

   size_t L = gw.mesh().size();
   if ( L != t_mesh.size()) TRIQS_RUNTIME_ERROR << "Meshes are different";
   double test = std::abs(t_mesh.delta() * gw.mesh().delta() * L / (2*M_PI) -1);
   if (test > 1.e-10) TRIQS_RUNTIME_ERROR << "Meshes are not compatible";
   
   const double tmin = t_mesh.x_min();
   const double wmin = gw.mesh().x_min();
   //a is a number very larger than delta_w and very smaller than wmax-wmin, used in the tail computation
   const double a = gw.mesh().delta() * sqrt( double(L) );
   
   long n_others = second_dim(gw.data());

   matrix_t _gin(L, n_others);
   matrix_t _gout(L, n_others);
  
   auto a1 = (m1 + I * m2/a )/2., a2 = (m1 - I * m2/a )/2.;

   auto _ = range();
   auto m1 = mom_12(0, _);
   auto m2 = mom_12(1, _);
   
   for (auto const & w: gw.mesh())
    _gin(w.index(),_) = (gw[w] - a1*th_expo_inv(w,a) - a2*th_expo_neg_inv(w,a)  ) * std::exp(-I*w*tmin);
   
   // FIXME ADJUST fourier_base and reuse fourier_base also in fourier_matsubara
   details::fourier_base(_gin, _gout, L, false);
   
   auto gt = gf<refreq, tensor_valued<1>>{t_mesh, {n_others}};
   const double corr = 1.0/(t_mesh.delta()*L);
   for (auto const & t : t_mesh)
    gt[t] = corr * std::exp(I*wmin*(tmin-t)) *
    _gout(t.index(),_) + a1 * th_expo(t,a) + a2 * th_expo_neg(t,a);
 
   return std::move(gt);
  }

}
