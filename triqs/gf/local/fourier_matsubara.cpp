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

#include "fourier_matsubara.hpp"
#include <fftw3.h>

namespace triqs { namespace gf { 

 namespace impl_local_matsubara {

  inline dcomplex oneFermion(dcomplex a,double b,double tau,double Beta) {
   return -a*( b >=0 ? exp(-b*tau)/(1+exp(-Beta*b)) : exp(b*(Beta-tau))/(1+exp(Beta*b)) );
  }

  inline dcomplex oneBoson(dcomplex a,double b,double tau,double Beta) {
   return a*( b >=0 ? exp(-b*tau)/(exp(-Beta*b)-1) : exp(b*(Beta-tau))/(1-exp(b*Beta)) );
  }
 }

 //--------------------------------------------------------------------------------------

 void fourier_impl  (gf_view<imfreq> &gw , gf_view<imtime> const & gt){

  // set behavior according to mesh kind
  double shift;
  size_t L;
  switch(gt.mesh().kind()) {
    case half_bins: shift = 0.5; L = gt.mesh().size(); break;
    case full_bins: shift = 0.0; L = gt.mesh().size()-1; break;
    case without_last: shift = 0.0; L = gt.mesh().size(); break;
  }

  auto ta = gt(freq_infty());
  long numberTimeSlices = gt.mesh().size();
  double Beta = gt.domain().beta, Pi = std::acos(-1);
  dcomplex I(0,1);
  tqa::vector<dcomplex> g_in(gt.mesh().size()), g_out (gw.mesh().size()); 

  using namespace impl_local_matsubara;
  for (size_t n1=0; n1<gw.data().shape()[1];n1++)
   for (size_t n2=0; n2<gw.data().shape()[2];n2++) {
    dcomplex d= ta(1)(n1,n2), A= ta.get_or_zero(2)(n1,n2),B = ta.get_or_zero(3)(n1,n2);
    //dcomplex d= ta(1)(n1,n2), A= ta(2)(n1,n2),B = ta(3)(n1,n2);
    double b1, b2, b3;
    dcomplex a1, a2, a3;
    if (gw.domain().statistic == Fermion){ b1 = 0; b2 =1; b3 =-1; a1 = d-B; a2 = (A+B)/2; a3 = (B-A)/2; }
    else { b1 = -0.5; b2 =-1; b3 =1; a1=4*(d-B)/3; a2=B-(d+A)/2; a3=d/6+A/2+B/3; }
    g_in() = 0;

    if (gw.domain().statistic == Fermion){
     for (auto & t : gt.mesh())  
      g_in(t.index()) = exp(I*Pi*t/Beta)*( gt(t)(n1,n2) - ( oneFermion(a1,b1,t,Beta) + oneFermion(a2,b2,t,Beta)+ oneFermion(a3,b3,t,Beta) ) );
    }
    else { 
     for (auto & t : gt.mesh()) 
      g_in(t.index()) =  gt(t)(n1,n2) -  ( oneBoson(a1,b1,t,Beta) + oneBoson(a2,b2,t,Beta) + oneBoson(a3,b3,t,Beta) );  
    }

    g_in *= Beta/numberTimeSlices;

    details::fourier_base(g_in, g_out, L, true);

    for (auto & w : gw.mesh()) {
     gw(w)(n1,n2) = g_out(w.index())*exp(2*I*w.index()*shift*Pi/Beta*gt.mesh().delta()) + a1/(w-b1) + a2/(w-b2) + a3/(w-b3); 
    }

    // set tail
    gw.singularity() = gt.singularity();

   }
 }
 //---------------------------------------------------------------------------

 template<typename GfElementType> GfElementType convert_green ( dcomplex const & x) { return x;}
 template<> double convert_green<double> ( dcomplex const & x) { return real(x);}

 //---------------------------------------------------------------------------

 void inverse_fourier_impl (gf_view<imtime> &gt,  gf_view<imfreq> const & gw) { 

  // set behavior according to mesh kind
  double shift;
  size_t L;
  switch(gt.mesh().kind()) {
    case half_bins: shift = 0.5; L = gt.mesh().size(); break;
    case full_bins: shift = 0.0; L = gt.mesh().size()-1; break;
    case without_last: shift = 0.0; L = gt.mesh().size(); break;
  }

  static bool Green_Function_Are_Complex_in_time = false;
  auto ta = gw(freq_infty());

  double Beta = gt.domain().beta, Pi = std::acos(-1);
  dcomplex I(0,1);
  tqa::vector<dcomplex> g_in(gw.mesh().size()), g_out (gt.mesh().size());

  using namespace impl_local_matsubara;
  for (size_t n1=0; n1<gt.data().shape()[1];n1++)
   for (size_t n2=0; n2<gt.data().shape()[2];n2++) {
    dcomplex d= ta(1)(n1,n2), A= ta.get_or_zero(2)(n1,n2),B = ta.get_or_zero(3)(n1,n2);
    //dcomplex d= ta(1)(n1,n2), A= ta(2)(n1,n2),B = ta(3)(n1,n2);

    double b1, b2, b3;
    dcomplex a1, a2, a3;
    if (gw.domain().statistic == Fermion){ b1 = 0; b2 =1; b3 =-1; a1 = d-B; a2 = (A+B)/2; a3 = (B-A)/2; }
    else { b1 = -0.5; b2 =-1; b3 =1; a1=4*(d-B)/3; a2=B-(d+A)/2; a3=d/6+A/2+B/3; }
    g_in() = 0;

    for (auto & w: gw.mesh()) {
     g_in(w.index()) =  exp(-I*2*w.index()*shift*Pi/Beta*gt.mesh().delta()) * ( gw(w)(n1,n2) - (a1/(w-b1) + a2/(w-b2) + a3/(w-b3)) );
    }
    // for bosons GF(w=0) is divided by 2 to avoid counting it twice
    if (gw.domain().statistic == Boson && !Green_Function_Are_Complex_in_time ) g_in(0) *= 0.5; 

    details::fourier_base(g_in, g_out, L, false);

    // If the Green function are NOT complex, then one use the symmetry property
    // fold the sum and get a factor 2
    double fact = (Green_Function_Are_Complex_in_time ? 1 : 2);
    g_out = g_out*(fact/Beta);

    // CORRECT FOR COMPLEX G(tau) !!!
    typedef double gt_result_type;
    //typedef boost::mpl::if_<gt_result_type;
    //typedef typename gf<imtime>::mesh_type::gf_result_type gt_result_type;

    if (gw.domain().statistic == Fermion){
     for (auto & t : gt.mesh()) 
       gt(t)(n1,n2) = convert_green<gt_result_type> (g_out(t.index())*exp(-I*Pi*t/Beta)
                         + oneFermion(a1,b1,t,Beta) + oneFermion(a2,b2,t,Beta)+ oneFermion(a3,b3,t,Beta) );
    }
    else {
     for (auto & t : gt.mesh()) 
       gt(t)(n1,n2) = convert_green<gt_result_type> (g_out(t.index())
                         + oneBoson(a1,b1,t,Beta) + oneBoson(a2,b2,t,Beta) + oneBoson(a3,b3,t,Beta) );
    }

    if (gt.mesh().kind() == full_bins)
      gt.on_mesh(L)(n1,n2) = -gt.on_mesh(0)(n1,n2)-convert_green<gt_result_type>(ta(1)(n1,n2));

    // set tail
    gt.singularity() = gw.singularity();

   }
 }


 gf_keeper<tags::fourier,imtime> lazy_fourier         (gf_view<imtime> const & g) { return g;}
 gf_keeper<tags::fourier,imfreq> lazy_inverse_fourier (gf_view<imfreq> const & g) { return g;}

 void triqs_gf_view_assign_delegation( gf_view<imfreq> &g, gf_keeper<tags::fourier,imtime> const & L) { fourier_impl (g,L.g);}
 void triqs_gf_view_assign_delegation( gf_view<imtime> &g, gf_keeper<tags::fourier,imfreq> const & L) { inverse_fourier_impl(g,L.g);}

}}

