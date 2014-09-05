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

  template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf_keeper<tags::fourier, retime, Target, Singularity> fourier(gf_impl<retime, Target, Singularity, Opt, V, C> const& g) {
  return {g};
 }
 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf_keeper<tags::fourier, refreq, Target, Singularity> inverse_fourier(gf_impl<refreq, Target, Singularity, Opt, V, C> const& g) {
  return {g};
 }

 void triqs_gf_view_assign_delegation(gf_view<refreq, scalar_valued> g, gf_keeper<tags::fourier, retime, scalar_valued> const& L);
 void triqs_gf_view_assign_delegation(gf_view<refreq, matrix_valued> g, gf_keeper<tags::fourier, retime, matrix_valued> const& L);
 void triqs_gf_view_assign_delegation(gf_view<retime, scalar_valued> g, gf_keeper<tags::fourier, refreq, scalar_valued> const& L);
 void triqs_gf_view_assign_delegation(gf_view<retime, matrix_valued> g, gf_keeper<tags::fourier, refreq, matrix_valued> const& L);

 template <typename Opt> gf_mesh<refreq, Opt> make_mesh_fourier_compatible(gf_mesh<retime, Opt> const& m) {
  int L = m.size();
  double pi = std::acos(-1);
  double wmin = -pi * (L - 1) / (L * m.delta());
  double wmax = pi * (L - 1) / (L * m.delta());
  return {wmin, wmax, L};
 }

 template <typename Opt>
 gf_mesh<retime, Opt> make_mesh_fourier_compatible(gf_mesh<refreq, Opt> const& m, mesh_kind mk = full_bins) {
  double pi = std::acos(-1);
  int L = m.size();
  double tmin = -pi * (L-1) / (L*m.delta());
  double tmax =  pi * (L-1) / (L*m.delta());
  return {tmin, tmax, L};
 }

  template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf_view<refreq, Target> make_gf_from_fourier(gf_impl<retime, Target, Singularity, Opt, V, C> const& gt) {
  auto gw = gf<refreq, Target>{make_mesh_fourier_compatible(gt.mesh()), get_target_shape(gt)};
  gw() = fourier(gt);
  return gw;
 }

 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf_view<retime, Target> make_gf_from_inverse_fourier(gf_impl<refreq, Target, Singularity, Opt, V, C> const& gw) {
  auto gt = gf<retime, Target>{make_mesh_fourier_compatible(gw.mesh()), get_target_shape(gw)};
  gt() = inverse_fourier(gw);
  return gt;
 }

}}

