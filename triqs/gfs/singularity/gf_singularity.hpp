/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by O. Parcollet
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

namespace triqs {
namespace gfs {

 /*----------------------------------------------------------
  *  Default
  *--------------------------------------------------------*/

 template <typename Var, typename Target> struct gf_singularity { using type = nothing; };

 /*----------------------------------------------------------
  *  Factories for singularity
  *--------------------------------------------------------*/

 template <typename S> struct gf_singularity_factory {
  template <typename M, typename T> static S make(M const &m, T const &shape) { return S{shape}; }
 };

 /*----------------------------------------------------------
     imfreq, imtime, refreq, retime
  *--------------------------------------------------------*/

 template <typename Target> struct gf_singularity<imfreq, Target> { using type = __tail<Target>; };
 template <typename Target> struct gf_singularity<imtime, Target> { using type = __tail<Target>; };
 template <typename Target> struct gf_singularity<refreq, Target> { using type = __tail<Target>; };
 template <typename Target> struct gf_singularity<retime, Target> { using type = __tail<Target>; };

 // special case : the real valued G(tau) has the same
 template <int R> struct gf_singularity<imtime, tensor_real_valued<R>> { using type = __tail<tensor_valued<R>>; };
 template <> struct gf_singularity<imtime, matrix_real_valued> { using type = __tail<matrix_valued>; };
 template <> struct gf_singularity<imtime, scalar_real_valued> { using type = __tail<scalar_valued>; };

}
}

