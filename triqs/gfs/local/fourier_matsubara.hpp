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

 // only a few functions allowed:
 
 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf_keeper<tags::fourier, imtime, Target, Singularity> fourier(gf_impl<imtime, Target, Singularity, Evaluator, V, C> const& g) {
  return {g};
 }
 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf_keeper<tags::fourier, imfreq, Target, Singularity> inverse_fourier(gf_impl<imfreq, Target, Singularity, Evaluator, V, C> const& g) {
  return {g};
 }

 /// 
 void _fourier_impl(gf_view<imfreq, scalar_valued, tail> gw, gf_const_view<imtime, scalar_valued, tail> gt);
 void _fourier_impl(gf_view<imfreq, scalar_valued, no_tail> gw, gf_const_view<imtime, scalar_valued, no_tail> gt);
 void _fourier_impl(gf_view<imtime, scalar_valued, tail> gt, gf_const_view<imfreq, scalar_valued, tail> gw);

 /// A few helper functions
 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf<imfreq, Target, Singularity, Evaluator> make_gf_from_fourier(gf_impl<imtime, Target, Singularity, Evaluator, V, C> const& gt, int n_iw) {
  auto m = gf_mesh<imfreq>{gt.mesh().domain(), n_iw};
  auto gw = gf<imfreq, Target, Singularity, Evaluator>{m, get_target_shape(gt)};
  gw() = fourier(gt);
  return gw;
 }

 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf<imfreq, Target, Singularity, Evaluator> make_gf_from_fourier(gf_impl<imtime, Target, Singularity, Evaluator, V, C> const& gt) {
  return make_gf_from_fourier(gt, (gt.mesh().size() - 1) / 2);
 }

 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf<imtime, Target, Singularity, Evaluator> make_gf_from_inverse_fourier(gf_impl<imfreq, Target, Singularity, Evaluator, V, C> const& gw, int n_tau) {
  auto m = gf_mesh<imtime>{gw.mesh().domain(), n_tau};
  auto gt = gf<imtime, Target, Singularity, Evaluator>{m, get_target_shape(gw)};
  gt() = inverse_fourier(gw);
  return gt;
 }

 template <typename Target, typename Singularity, typename Evaluator, bool V, bool C>
 gf<imtime, Target, Singularity, Evaluator> make_gf_from_inverse_fourier(gf_impl<imfreq, Target, Singularity, Evaluator, V, C> const& gw) {
  return make_gf_from_inverse_fourier(gw, 2 * gw.mesh().size() + 1);
 }
}

namespace clef {
 TRIQS_CLEF_MAKE_FNT_LAZY(fourier);
 TRIQS_CLEF_MAKE_FNT_LAZY(inverse_fourier);
}
}

