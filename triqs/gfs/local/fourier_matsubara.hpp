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
#pragma once

#include "fourier_base.hpp"
#include <triqs/gfs/imfreq.hpp>
#include <triqs/gfs/imtime.hpp>

namespace triqs {
namespace gfs {

 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf_keeper<tags::fourier, imtime, Target, Singularity, Opt> fourier(gf_impl<imtime, Target, Singularity, Opt, V, C> const& g) {
  return {g};
 }
 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf_keeper<tags::fourier, imfreq, Target, Singularity, Opt> inverse_fourier(gf_impl<imfreq, Target, Singularity, Opt, V, C> const& g) {
  return {g};
 }

 // The fourier transform with the tail
 void triqs_gf_view_assign_delegation(gf_view<imfreq, scalar_valued> g, gf_keeper<tags::fourier, imtime, scalar_valued> const& L);
 void triqs_gf_view_assign_delegation(gf_view<imfreq, matrix_valued> g, gf_keeper<tags::fourier, imtime, matrix_valued> const& L);
 void triqs_gf_view_assign_delegation(gf_view<imtime, scalar_valued> g, gf_keeper<tags::fourier, imfreq, scalar_valued> const& L);
 void triqs_gf_view_assign_delegation(gf_view<imtime, matrix_valued> g, gf_keeper<tags::fourier, imfreq, matrix_valued> const& L);

 // The version without tail : only possible in one direction
 void triqs_gf_view_assign_delegation(gf_view<imfreq, scalar_valued, no_tail> g,
                                      gf_keeper<tags::fourier, imtime, scalar_valued, no_tail> const& L);
 void triqs_gf_view_assign_delegation(gf_view<imfreq, matrix_valued, no_tail> g,
                                      gf_keeper<tags::fourier, imtime, matrix_valued, no_tail> const& L);

 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf<imfreq, Target, Singularity, Opt> make_gf_from_fourier(gf_impl<imtime, Target, Singularity, Opt, V, C> const& gt, int n_iw) {
  auto m = gf_mesh<imfreq, Opt>{gt.mesh().domain(), n_iw};
  auto gw = gf<imfreq, Target, Singularity, Opt>{m, get_target_shape(gt)};
  gw() = fourier(gt);
  return gw;
 }

 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf<imfreq, Target, Singularity, Opt> make_gf_from_fourier(gf_impl<imtime, Target, Singularity, Opt, V, C> const& gt) {
  return make_gf_from_fourier(gt, (gt.mesh().size() - (gt.mesh().kind() == full_bins ? 1 : 0)) / 2);
 }

 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf<imtime, Target, Singularity, Opt> make_gf_from_inverse_fourier(gf_impl<imfreq, Target, Singularity, Opt, V, C> const& gw, int n_tau,
                                                      mesh_kind mk = full_bins) {
  auto m = gf_mesh<imtime, Opt>{gw.mesh().domain(), n_tau};
  auto gt = gf<imtime, Target, Singularity, Opt>{m, get_target_shape(gw)};
  gt() = inverse_fourier(gw);
  return gt;
 }

 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf<imtime, Target, Singularity, Opt> make_gf_from_inverse_fourier(gf_impl<imfreq, Target, Singularity, Opt, V, C> const& gw, mesh_kind mk = full_bins) {
  return make_gf_from_inverse_fourier(gw, 2 * gw.mesh().size() + (mk == full_bins ? 1 : 0), mk);
 }
}

namespace clef {
 TRIQS_CLEF_MAKE_FNT_LAZY(fourier);
 TRIQS_CLEF_MAKE_FNT_LAZY(inverse_fourier);
}
}

