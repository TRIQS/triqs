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
#ifndef TRIQS_GF_LOCAL_FOURIER_REAL_H 
#define TRIQS_GF_LOCAL_FOURIER_REAL_H

#include "fourier_base.hpp"
#include <triqs/gf/refreq.hpp> 
#include <triqs/gf/retime.hpp> 

namespace triqs { namespace gf { 

 namespace impl_local_real {
   dcomplex I(0,1);
   double pi = std::acos(-1);
 }

 // First the implementation of the fourier transform
 void fourier_impl         (gf_view<refreq> &gw , gf_view<retime> const & gt);
 void inverse_fourier_impl (gf_view<retime> &gt,  gf_view<refreq> const & gw);

 gf_view<refreq> fourier (gf_view<retime> const & gt) { 
   size_t L = gt.mesh().size();
   double wmin = -impl_local_real::pi * (L-1) / (L*gt.mesh().delta());
   double wmax =  impl_local_real::pi * (L-1) / (L*gt.mesh().delta());
   auto gw = make_gf<refreq>(wmin, wmax, L, gt.data().shape().front_pop());
   auto V = gw();
   fourier_impl(V, gt);
   return gw;
 }

 gf_view<retime> inverse_fourier (gf_view<refreq> const & gw) { 
   size_t L = gw.mesh().size();
   double tmin = -impl_local_real::pi * (L-1) / (L*gw.mesh().delta());
   double tmax =  impl_local_real::pi * (L-1) / (L*gw.mesh().delta());
   auto gt = make_gf<retime>(tmin, tmax, L, gw.data().shape().front_pop());
   auto V = gt();
   inverse_fourier_impl(V, gw);
   return gt;
 }

 gf_keeper<tags::fourier,retime> lazy_fourier         (gf_view<retime> const & g);
 gf_keeper<tags::fourier,refreq> lazy_inverse_fourier (gf_view<refreq> const & g);

 void triqs_gf_view_assign_delegation( gf_view<refreq> &g, gf_keeper<tags::fourier,retime> const & L);
 void triqs_gf_view_assign_delegation( gf_view<retime> &g, gf_keeper<tags::fourier,refreq> const & L);

}}
#endif

