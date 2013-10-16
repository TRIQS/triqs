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
#ifndef TRIQS_GF_LOCAL_FOURIER_MATSU_H 
#define TRIQS_GF_LOCAL_FOURIER_MATSU_H

#include "fourier_base.hpp"
#include <triqs/gfs/imfreq.hpp> 
#include <triqs/gfs/imtime.hpp> 

namespace triqs { namespace gfs {
 
 // First the implementation of the fourier transform
 void fourier_impl         (gf_view<imfreq,scalar_valued> gw , gf_view<imtime,scalar_valued> const gt, scalar_valued);
 void fourier_impl         (gf_view<imfreq,matrix_valued> gw , gf_view<imtime,matrix_valued> const gt, matrix_valued);
 void inverse_fourier_impl (gf_view<imtime,scalar_valued> gt,  gf_view<imfreq,scalar_valued> const gw, scalar_valued);
 void inverse_fourier_impl (gf_view<imtime,matrix_valued> gt,  gf_view<imfreq,matrix_valued> const gw, matrix_valued);
 
 inline gf_view<imfreq,matrix_valued> fourier (gf_view<imtime,matrix_valued> const gt) {
  int L = (gt.mesh().kind() == full_bins ? gt.mesh().size()-1 : gt.mesh().size() );
  auto gw = gf<imfreq,matrix_valued>{ {gt.domain(),L}, gt.data().shape().front_pop() };
  auto V = gw();
  fourier_impl(V, gt, matrix_valued());
  return gw;
 }
 inline gf_view<imfreq,scalar_valued> fourier (gf_view<imtime,scalar_valued> const gt) {
  int L = (gt.mesh().kind() == full_bins ? gt.mesh().size()-1 : gt.mesh().size() );
  auto gw = gf<imfreq,scalar_valued>{ {gt.domain(),L} };
  auto V = gw();
  fourier_impl(V, gt, scalar_valued());
  return gw;
 }
 
 inline gf_view<imtime, matrix_valued> inverse_fourier (gf_view<imfreq, matrix_valued> const gw, mesh_kind mk = half_bins) {
  double pi = std::acos(-1);
  int L = (mk == full_bins ? gw.mesh().size()+1 : gw.mesh().size() );
  auto gt = gf<imtime,matrix_valued>{ {gw.domain(),L}, gw.data().shape().front_pop()};
  auto V = gt();
  inverse_fourier_impl(V, gw, matrix_valued());
  return gt;
 }
 inline gf_view<imtime,scalar_valued> inverse_fourier (gf_view<imfreq,scalar_valued> const gw, mesh_kind mk = half_bins) {
  double pi = std::acos(-1);
  int L = (mk == full_bins ? gw.mesh().size()+1 : gw.mesh().size() );
  auto gt = gf<imtime,scalar_valued>{ {gw.domain(),L} };
  auto V = gt();
  inverse_fourier_impl(V, gw,scalar_valued());
  return gt;
 }
 
 inline gf_keeper<tags::fourier,imtime,scalar_valued> lazy_fourier         (gf_view<imtime,scalar_valued> const & g) { return g;}
 inline gf_keeper<tags::fourier,imfreq,scalar_valued> lazy_inverse_fourier (gf_view<imfreq,scalar_valued> const & g) { return g;}
 inline gf_keeper<tags::fourier,imtime,matrix_valued> lazy_fourier         (gf_view<imtime,matrix_valued> const & g) { return g;}
 inline gf_keeper<tags::fourier,imfreq,matrix_valued> lazy_inverse_fourier (gf_view<imfreq,matrix_valued> const & g) { return g;}
 
 void triqs_gf_view_assign_delegation( gf_view<imfreq,scalar_valued> g, gf_keeper<tags::fourier,imtime,scalar_valued> const & L);
 void triqs_gf_view_assign_delegation( gf_view<imfreq,matrix_valued> g, gf_keeper<tags::fourier,imtime,matrix_valued> const & L);
 void triqs_gf_view_assign_delegation( gf_view<imtime,scalar_valued> g, gf_keeper<tags::fourier,imfreq,scalar_valued> const & L);
 void triqs_gf_view_assign_delegation( gf_view<imtime,matrix_valued> g, gf_keeper<tags::fourier,imfreq,matrix_valued> const & L);
}}

namespace triqs { namespace clef {
TRIQS_CLEF_MAKE_FNT_LAZY (lazy_fourier);
TRIQS_CLEF_MAKE_FNT_LAZY (lazy_inverse_fourier);
}}
#endif

