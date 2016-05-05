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
#include <triqs/arrays/vector.hpp>

namespace triqs { namespace gfs {

 namespace details {

   namespace tqa = triqs::arrays;
   using dcomplex = std::complex<double>;

   void fourier_base(const tqa::vector<dcomplex> &in, tqa::vector<dcomplex> &out, size_t L, bool direct);
   void fourier_base(const tqa::vector<dcomplex> &in, tqa::vector<dcomplex> &out, size_t L1, size_t L2, bool direct);
 }

 namespace tags { struct fourier{}; }

 /*------------------------------------------------------------------------------------------------------
  *                                  For lazy transformation
  *-----------------------------------------------------------------------------------------------------*/

 template <typename Tag, typename G> struct tagged_cview { typename G::const_view_type g; };

 /// FIXME : remove this 
 template <typename Tag, typename D, typename Target = matrix_valued> struct gf_keeper { gf_const_view<D, Target> g; };



 // -------------------------------------------------------------------

 // TO BE REPLACED BY A DIRECT CALL to many_fft in fftw, like for lattice case.
 // The implementation of the Fourier transformation
 // Reduce Matrix case to the scalar case.
 template <typename X, typename Y>
 void _fourier_impl(gf_view<X, matrix_valued> gw, gf_const_view<Y, matrix_valued> gt) {
  if (gt.data().shape().front_pop() != gw.data().shape().front_pop())
   TRIQS_RUNTIME_ERROR << "Fourier : matrix size of target mismatch";
  for (size_t n1 = 0; n1 < gt.data().shape()[1]; n1++)
   for (size_t n2 = 0; n2 < gt.data().shape()[2]; n2++) {
    auto gw_sl = slice_target_to_scalar(gw, n1, n2);
    auto gt_sl = slice_target_to_scalar(gt, n1, n2);
    _fourier_impl(gw_sl, gt_sl);
   }
 }

 // second part of the implementation
 template <typename X, typename Y, typename T>
 void triqs_gf_view_assign_delegation(gf_view<X, T> g, gf_keeper<tags::fourier, Y, T> const &L) {
  _fourier_impl(g, L.g);
  }

 template <typename X, typename G, typename T>
 void triqs_gf_view_assign_delegation(gf_view<X, T> g, tagged_cview<tags::fourier, G> const &L) {
  _fourier_impl(g, L.g);
  }
}}
