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

 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf_keeper<tags::fourier, retime, Target, Singularity> fourier(gf_impl<retime, Target, Singularity, Evaluator, V, C> const& g) {
  return {g};
 }
 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf_keeper<tags::fourier, refreq, Target, Singularity>
 inverse_fourier(gf_impl<refreq, Target, Singularity, Evaluator, V, C> const& g) {
  return {g};
 }

 void _fourier_impl(gf_view<refreq, scalar_valued> gw, gf_const_view<retime, scalar_valued> gt);
 void _fourier_impl(gf_view<retime, scalar_valued> gt, gf_const_view<refreq, scalar_valued> gw);

 // helper functions
 gf_mesh<refreq> make_mesh_fourier_compatible(gf_mesh<retime> const& m);
 gf_mesh<retime> make_mesh_fourier_compatible(gf_mesh<refreq> const& m, mesh_kind mk = full_bins);

  template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf_view<refreq, Target> make_gf_from_fourier(gf_impl<retime, Target, Singularity, Evaluator, V, C> const& gt) {
  auto gw = gf<refreq, Target>{make_mesh_fourier_compatible(gt.mesh()), get_target_shape(gt)};
  gw() = fourier(gt);
  return gw;
 }

 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf_view<retime, Target> make_gf_from_inverse_fourier(gf_impl<refreq, Target, Singularity, Evaluator, V, C> const& gw) {
  auto gt = gf<retime, Target>{make_mesh_fourier_compatible(gw.mesh()), get_target_shape(gw)};
  gt() = inverse_fourier(gw);
  return gt;
 }

}}

