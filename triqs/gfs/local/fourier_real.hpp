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
#include <triqs/gfs/refreq.hpp> 
#include <triqs/gfs/retime.hpp> 

namespace triqs { namespace gfs { 

 // First the implementation of the fourier transform
 void fourier_impl         (gf_view<refreq,scalar_valued> gw , gf_view<retime,scalar_valued> const gt, scalar_valued);
 void fourier_impl         (gf_view<refreq,matrix_valued> gw , gf_view<retime,matrix_valued> const gt, matrix_valued);
 void inverse_fourier_impl (gf_view<retime,scalar_valued> gt,  gf_view<refreq,scalar_valued> const gw, scalar_valued);
 void inverse_fourier_impl (gf_view<retime,matrix_valued> gt,  gf_view<refreq,matrix_valued> const gw, matrix_valued);
 
 inline gf_view<refreq,matrix_valued> fourier (gf_view<retime, matrix_valued> const gt) { 
  double pi = std::acos(-1);
  int L = gt.mesh().size();
  double wmin = -pi * (L-1) / (L*gt.mesh().delta());
  double wmax =  pi * (L-1) / (L*gt.mesh().delta());
  auto gw = gf<refreq,matrix_valued>{ {wmin, wmax, L}, gt.data().shape().front_pop()};
  auto V = gw();
  fourier_impl(V, gt, matrix_valued());
  return gw;
 }
 inline gf_view<refreq,scalar_valued> fourier (gf_view<retime, scalar_valued> const gt) { 
  double pi = std::acos(-1);
  int L = gt.mesh().size();
  double wmin = -pi * (L-1) / (L*gt.mesh().delta());
  double wmax =  pi * (L-1) / (L*gt.mesh().delta());
  auto gw = gf<refreq,scalar_valued>{ {wmin, wmax, L} };
  auto V = gw();
  fourier_impl(V, gt, scalar_valued());
  return gw;
 }
 
 inline gf_view<retime,matrix_valued> inverse_fourier (gf_view<refreq,matrix_valued> const gw) { 
  double pi = std::acos(-1);
  int L = gw.mesh().size();
  double tmin = -pi * (L-1) / (L*gw.mesh().delta());
  double tmax =  pi * (L-1) / (L*gw.mesh().delta());
  auto gt = gf<retime,matrix_valued>{{ tmin, tmax, L} , gw.data().shape().front_pop()};
  auto V = gt();
  inverse_fourier_impl(V, gw, matrix_valued());
  return gt;
 }
 inline gf_view<retime,scalar_valued> inverse_fourier (gf_view<refreq,scalar_valued> const gw) { 
  double pi = std::acos(-1);
  int L = gw.mesh().size();
  double tmin = -pi * (L-1) / (L*gw.mesh().delta());
  double tmax =  pi * (L-1) / (L*gw.mesh().delta());
  auto gt = gf<retime,scalar_valued>{ {tmin, tmax, L} };
  auto V = gt();
  inverse_fourier_impl(V, gw, scalar_valued());
  return gt;
 }
 
 inline gf_keeper<tags::fourier,retime,scalar_valued> lazy_fourier         (gf_view<retime,scalar_valued> const & g) { return g;}
 inline gf_keeper<tags::fourier,refreq,scalar_valued> lazy_inverse_fourier (gf_view<refreq,scalar_valued> const & g) { return g;}
 inline gf_keeper<tags::fourier,retime,matrix_valued> lazy_fourier         (gf_view<retime,matrix_valued> const & g) { return g;}
 inline gf_keeper<tags::fourier,refreq,matrix_valued> lazy_inverse_fourier (gf_view<refreq,matrix_valued> const & g) { return g;}

 void triqs_gf_view_assign_delegation( gf_view<refreq,scalar_valued> g, gf_keeper<tags::fourier,retime,scalar_valued> const & L);
 void triqs_gf_view_assign_delegation( gf_view<refreq,matrix_valued> g, gf_keeper<tags::fourier,retime,matrix_valued> const & L);
 void triqs_gf_view_assign_delegation( gf_view<retime,scalar_valued> g, gf_keeper<tags::fourier,refreq,scalar_valued> const & L);
 void triqs_gf_view_assign_delegation( gf_view<retime,matrix_valued> g, gf_keeper<tags::fourier,refreq,matrix_valued> const & L);

}}
#endif

