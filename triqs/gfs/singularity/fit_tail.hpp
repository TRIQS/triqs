/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by H. Hafermann, O. Parcollet
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
#include <triqs/gfs.hpp>
#include <triqs/arrays/blas_lapack/gelss.hpp>

namespace triqs { namespace gfs { 
 using triqs::arrays::array_const_view;

 /// routine for fitting the tail (singularity) of a real Matsubara Green's function
 /**
  This is a *linear* least squares problem (with non-linear basis functions)
  which is solved by singular value decomposition of the design matrix
  @note the routine fits the real part (even moments) and the imaginary part (odd moments) separately for stability reasons

  @param gf the input gf<imfreq> Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the index of the first frequency to fit (included)
  @param n_max the index of the last frequency to fit (included)

  @return the tail obtained by fitting
 */
 __tail<matrix_valued> fit_real_tail_impl(gf_view<imfreq> gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int n_min, int n_max) ;

 /// routine for fitting the tail (singularity) of a complex Matsubara Green's function
 /**
  This is a *linear* least squares problem (with non-linear basis functions)
  which is solved by singular value decomposition of the design matrix
  @note the routine fits the real part (even moments) and the imaginary part (odd moments) separately for stability reasons

  @param gf the input gf<imfreq> Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the index of the first frequency to fit (included)
  @param n_max the index of the last frequency to fit (included)

  @return the tail obtained by fitting
 */
 __tail<matrix_valued> fit_complex_tail_impl(gf_view<imfreq> gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int n_min, int n_max) ;

 ///Fit the tail of a real (in tau) gf
 /**
  @param gf the input gf<imfreq> Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the Matsubara index of the first frequency to fit (must be positive)
  @param n_max the Matsubara index of the last frequency to fit (included)
  @param replace_by_fit if true, replace the gf data with the asymptotic behavior obtained by fitting the tails.
  @note Based on [[fit_tail_impl]]. Works for functions with positive only or all Matsubara frequencies.
 */
 void fit_tail(gf_view<imfreq> gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int n_min, int n_max,
   bool replace_by_fit = false) ;

 ///Fit the tail of a complex (in tau) gf
 /**
  @param gf the input gf<imfreq> Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param neg_n_min the Matsubara index of the first frequency to fit (in the negative frequencies)
  @param neg_n_max the Matsubara index of the last frequency to fit (in the negative frequencies)
  @param pos_n_min the Matsubara index of the first frequency to fit (in the positive frequencies)
  @param pos_n_max the Matsubara index of the last frequency to fit (in the positive frequencies)
  @param replace_by_fit if true, replace the gf data with the asymptotic behavior obtained by fitting the tails.
  @note Based on [[fit_tail_impl]]. Works for functions with positive only or all Matsubara frequencies.
 */
 void fit_tail(gf_view<imfreq> gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int neg_n_min, int neg_n_max,
   int pos_n_min, int pos_n_max, bool replace_by_fit = false);

 ///Fit the tail of a block_gf
 /**
  @param gf the input Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the Matsubara index of the first frequency to fit (included)
  @param n_max the Matsubara index of the last frequency to fit (included)
  @param replace_by_fit if true, replace the gf data with the asymptotic behavior obtained by fitting the tails.
  Range of replacement if replace_by_fit is true:
  
  - if the gf is real in tau, replace all frequencies w_n>= w_{n_min} and w_n <= - w_{n_min} 

  - otherwise, 

        -if n_min>0 (and n_max>0), replace all frequencies w_n >= w_n{n_min}
        
        -if n_min<0 (and n_max<0), replace all frequencies w_n <= w_n{n_max}
  @note Based on [[fit_tail_impl]]
 */
 void fit_tail(block_gf_view<imfreq> block_gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int n_min,
   int n_max, bool replace_by_fit = false) ;

 ///Fit the tail of a gf (scalar-valued)
 /**
  @param gf the input gf<imfreq> Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the Matsubara index of the first frequency to fit (included)
  @param n_max the Matsubara index of the last frequency to fit (included)
  @param replace_by_fit if true, replace the gf data with the asymptotic behavior obtained by fitting the tails.
  Range of replacement if replace_by_fit is true:
  
  - if the gf is real in tau, replace all frequencies w_n>= w_{n_min} and w_n <= - w_{n_min} 

  - otherwise, 

        -if n_min>0 (and n_max>0), replace all frequencies w_n >= w_n{n_min}
        
        -if n_min<0 (and n_max<0), replace all frequencies w_n <= w_n{n_max}
  @note Based on [[fit_tail_impl]]
 */
 void fit_tail(gf_view<imfreq, scalar_valued> gf, __tail_const_view<scalar_valued> known_moments, int max_moment, int n_min, int n_max, bool replace_by_fit = false) ;
 void fit_tail(gf_view<imfreq, scalar_valued> gf, __tail_const_view<scalar_valued> known_moments, int max_moment, int neg_n_min, int neg_n_max,int pos_n_min, int pos_n_max, bool replace_by_fit = false) ;

 ///fit tail of tensor_valued Gf, rank 3
 array<__tail<scalar_valued>, 3> fit_tail(gf_const_view<imfreq, tensor_valued<3>> g, array_const_view<__tail<scalar_valued>,3> known_moments, int max_moment, int n_min, int n_max);

}} // namespace
