/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by M. Ferrero, O. Parcollet
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
#pragma once
#include "fourier_base.hpp"
#include <triqs/gfs/refreq.hpp> 
#include <triqs/gfs/retime.hpp> 

namespace triqs { namespace gfs {

 /**
  * TBR
  */
 template <typename G> std14::enable_if_t<is_gf_or_view<G, retime>::value, tagged_cview<tags::fourier, G>> fourier(G const& g) {
  return {g};
 }

 /**
  * TBR
  */ 
 template <typename G> std14::enable_if_t<is_gf_or_view<G, refreq>::value, tagged_cview<tags::fourier, G>> inverse_fourier(G const& g) {
  return {g};
 }

 void _fourier_impl(gf_view<refreq, scalar_valued> gw, gf_const_view<retime, scalar_valued> gt);
 void _fourier_impl(gf_view<retime, scalar_valued> gt, gf_const_view<refreq, scalar_valued> gw);

 // helper functions
 gf_mesh<refreq> make_mesh_fourier_compatible(gf_mesh<retime> const& m);
 gf_mesh<retime> make_mesh_fourier_compatible(gf_mesh<refreq> const& m);

 /**
  * TBR
  */
 template <typename Target, typename Singularity, typename Evaluator>
 gf<refreq, Target, Singularity> make_gf_from_fourier(gf_const_view<retime, Target, Singularity, Evaluator> const& gt) {
  auto gw = gf<refreq, Target>{make_mesh_fourier_compatible(gt.mesh()), get_target_shape(gt)};
  gw() = fourier(gt);
  return gw;
 }

 template <typename T, typename S, typename E>
 gf<refreq, T, S> make_gf_from_fourier(gf_view<retime, T, S, E> const& gt) {
  return make_gf_from_fourier(gt());
 }

 template <typename T, typename S, typename E>
 gf<refreq, T, S> make_gf_from_fourier(gf<retime, T, S, E> const& gt) {
  return make_gf_from_fourier(gt());
 }

 /**
  * TBR
  */
 template <typename Target, typename Singularity, typename Evaluator>
 gf<retime, Target, Singularity> make_gf_from_inverse_fourier(gf_const_view<refreq, Target, Singularity, Evaluator> const& gw) {
  auto gt = gf<retime, Target>{make_mesh_fourier_compatible(gw.mesh()), get_target_shape(gw)};
  gt() = inverse_fourier(gw);
  return gt;
 }

 template <typename T, typename S, typename E>
 gf<retime, T, S> make_gf_from_inverse_fourier(gf_view<refreq, T, S, E> const& gw) {
  return make_gf_from_inverse_fourier(gw());
 }

 template <typename T, typename S, typename E>
 gf<retime, T, S> make_gf_from_inverse_fourier(gf<refreq, T, S, E> const& gw) {
  return make_gf_from_inverse_fourier(gw());
 }
}}

