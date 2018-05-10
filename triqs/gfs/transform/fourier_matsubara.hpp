/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2015 by M. Ferrero, O. Parcollet
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
#include "./fourier_base.hpp"

namespace triqs {
namespace gfs {

 /**
  * TBR
  */
 template <typename G> std14::enable_if_t<is_gf<G, imtime>::value, tagged_cview<tags::fourier, G>> fourier(G const& g) {
  return {g};
 }

 /**
  * TBR
  */ 
 template <typename G> std14::enable_if_t<is_gf<G, imfreq>::value, tagged_cview<tags::fourier, G>> inverse_fourier(G const& g) {
  return {g};
 }

 gf<imfreq, tensor_valued<1>> _fourier_impl(gf_mesh<imfreq> const &iw_mesh, gf<imtime, tensor_valued<1>> &&gt, arrays::array_const_view<dcomplex, 2> mom_23 = {});
 gf<imtime, tensor_valued<1>> _fourier_impl(gf_mesh<imtime> const &tau_mesh, gf<imfreq, tensor_valued<1>> &&gw, arrays::array_const_view<dcomplex, 2> mom_123 = {});

 /**
  *
  */
 template <typename T>
 gf<imfreq, T> make_gf_from_fourier(gf_const_view<imtime, T> const& gt, int n_iw = -1) {
  if (n_iw == -1) n_iw = (gt.mesh().size() - 1) / 2;
  auto m = gf_mesh<imfreq>{gt.mesh().domain(), n_iw};
  return _make_fourier(gt, m);
 }

 /**
  *
  */
 template <typename T>
 gf<imtime, T> make_gf_from_inverse_fourier(gf_const_view<imfreq, T> const& gw, int n_tau = -1) {
  if (n_tau == -1) n_tau = 2*(gw.mesh().last_index() + 1) +1;
  auto m = gf_mesh<imtime>{gw.mesh().domain(), n_tau};
  return _make_fourier(gw, m);
 }

}

namespace clef {
 TRIQS_CLEF_MAKE_FNT_LAZY(fourier);
 TRIQS_CLEF_MAKE_FNT_LAZY(inverse_fourier);
}
}
