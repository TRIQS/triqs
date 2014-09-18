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
#include <triqs/gfs/bz.hpp> 
#include <triqs/gfs/cyclic_lattice.hpp>

namespace triqs {
namespace gfs {

 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf_keeper<tags::fourier, cyclic_lattice, Target, Singularity>
 fourier(gf_impl<cyclic_lattice, Target, Singularity, Opt, V, C> const& g) {
  return {g};
 }
 
 template <typename Target, typename Singularity, typename Opt, bool V, bool C>
 gf_keeper<tags::fourier, bz, Target, Singularity> inverse_fourier(gf_impl<bz, Target, Singularity, Opt, V, C> const& g) {
  return {g};
 }

 // implementation 
 void _fourier_impl(gf_view<bz, scalar_valued> gk, gf_const_view<cyclic_lattice, scalar_valued> gr);
 void _fourier_impl(gf_view<bz, matrix_valued> gk, gf_const_view<cyclic_lattice, matrix_valued> gr);
 
 void _fourier_impl(gf_view<cyclic_lattice, scalar_valued> gr, gf_const_view<bz, scalar_valued> gk);
 void _fourier_impl(gf_view<cyclic_lattice, matrix_valued> gr, gf_const_view<bz, matrix_valued> gk);

}}

