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

namespace triqs {
namespace gfs {

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

 // implementation 
 void _fourier_impl(gf_view<brillouin_zone, scalar_valued> gk, gf_const_view<cyclic_lattice, scalar_valued> gr);
 void _fourier_impl(gf_view<brillouin_zone, matrix_valued> gk, gf_const_view<cyclic_lattice, matrix_valued> gr);
 
 void _fourier_impl(gf_view<cyclic_lattice, scalar_valued> gr, gf_const_view<brillouin_zone, scalar_valued> gk);
 void _fourier_impl(gf_view<cyclic_lattice, matrix_valued> gr, gf_const_view<brillouin_zone, matrix_valued> gk);

}}

