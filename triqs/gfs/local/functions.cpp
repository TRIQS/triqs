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

 dcomplex F(dcomplex a,double b,double Beta) {return -a/(1+exp(-Beta*b));}
 using arrays::array;

 //-------------------------------------------------------
 // For Imaginary Matsubara Frequency functions
 // ------------------------------------------------------
 arrays::matrix<double> density( gf_view<imfreq> const & G) { 
  dcomplex I(0,1);
  auto sh = G.data().shape().front_pop();
  auto Beta = G.domain().beta;
  local::tail_view t = G(freq_infty());
  if (!t.is_decreasing_at_infinity())  TRIQS_RUNTIME_ERROR<<" density computation : Green Function is not as 1/omega or less !!!";
  const size_t N1=sh[0], N2 = sh[1];
  arrays::array<dcomplex,2> dens_part(sh), dens_tail(sh), dens(sh);
  arrays::matrix<double> res(sh);
  dens_part()=0;dens()=0;dens_tail()=0;
  for (size_t n1=0; n1<N1;n1++) 
   for (size_t n2=0; n2<N2;n2++) {
    dcomplex d= t(1)(n1,n2) , A=t(2)(n1,n2),B = t(3)(n1,n2) ;
    double b1 = 0,b2 =1, b3 =-1;
    dcomplex a1 = d-B, a2 = (A+B)/2, a3 = (B-A)/2;
    for (auto & w : G.mesh())  dens_part(n1,n2)+= G[w](n1,n2)  -  (a1/(w - b1) + a2 / (w-b2) + a3/(w-b3)); 
    dens_part(n1,n2) = dens_part(n1,n2)/Beta;
    dens_tail(n1,n2) = d + F(a1,b1,Beta) + F(a2,b2,Beta)+ F(a3,b3,Beta);
    // If  the Green function are NOT complex, then one use the symmetry property
    // fold the sum and get a factor 2
    //double fact = (Green_Function_Are_Complex_in_time ? 1 : 2);
    //dens_part(n1,n2) = dens_part(n1,n2)*(fact/Beta)  + (d + F(a1,b1,Beta) + F(a2,b2,Beta)+ F(a3,b3,Beta));
    //if (!Green_Function_Are_Complex_in_time) dens_part  = 0+real(dens_part);
   }

  for (size_t n1=0; n1<N1;n1++) 
   for (size_t n2=0; n2<N2;n2++) {
    dens_part(n1,n2) = dens_part(n1,n2) + real(dens_part(n2,n1)) - I * imag(dens_part(n2,n1)) + dens_tail(n1,n2);
    // ?? STRANGE ??
    dens_part(n2,n1) = real(dens_part(n1,n2)) - I * imag(dens_part(n1,n2));
   }

  for (size_t n1=0; n1<N1;n1++) 
   for (size_t n2=0; n2<N2;n2++) {
    res(n1,n2) = real(dens_part(n1,n2));
   }

  return res;
 }


 arrays::matrix<double> density( gf_view<legendre> const & gl) { 

   auto sh = gl.data().shape().front_pop();
   arrays::matrix<double> res(sh);
   res() = 0.0;

   for (auto l : gl.mesh()) {
     res -= sqrt(2*l.index()+1) * gl[l];
   }
   res /= gl.domain().beta;

   return res;

 }

 // compute a tail from the Legendre GF
 // this is Eq. 8 of our paper
 local::tail_view get_tail(gf_const_view<legendre> gl, int size = 10, int omin = -1) {

   auto sh = gl.data().shape().front_pop();
   local::tail t(sh, size, omin);
   t.data() = 0.0;

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

   arrays::array<double,2> corr(disc.shape()); corr() = 0;
   for (auto l : gl.mesh()) {
     corr += t(l.index()) * gl[l];
   }

   arrays::range R;
   for (auto l : gl.mesh()) {
     gl.data()(l.index(),R,R) += (disc - corr) * t(l.index()) / norm;
   }

 }


}}
