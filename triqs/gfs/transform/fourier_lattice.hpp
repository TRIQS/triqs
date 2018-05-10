/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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

namespace triqs::gfs {

 /**
  * TBR
  */
 template <typename G> std14::enable_if_t<is_gf<G, cyclic_lattice>::value, tagged_cview<tags::fourier, G>> fourier(G const& g) {
  return {g};
 }

 /**
  * TBR
  */
 template <typename G> std14::enable_if_t<is_gf<G, brillouin_zone>::value, tagged_cview<tags::fourier, G>> inverse_fourier(G const& g) {
  return {g};
 }

 gf<brillouin_zone, tensor_valued<1>> _fourier_impl(gf_mesh<cyclic_lattice> const & r_mesh, gf<brillouin_zone, tensor_valued<1>>&& gk);
 gf<cyclic_lattice, tensor_valued<1>> _fourier_impl(gf_mesh<brillouin_zone> const & k_mesh, gf<cyclic_lattice, tensor_valued<1>>&& gr);

 /**
  *
  */
 template <typename T>
 gf<brillouin_zone, T> make_gf_from_fourier(gf_const_view<cyclic_lattice, T> const& gr) {
  auto m = gf_mesh<brillouin_zone>{gr.mesh().domain(), gr.mesh().periodization_matrix};
  return _make_fourier(gr, m);
 }

 /**
  *
  */
 template <typename T>
 gf<cyclic_lattice, T> make_gf_from_inverse_fourier(gf_const_view<brillouin_zone, T> const& gk) {
  auto m = gf_mesh<cyclic_lattice>{gk.mesh().domain(), gk.mesh().periodization_matrix};
  return _make_fourier(gk, m);
 }

}
