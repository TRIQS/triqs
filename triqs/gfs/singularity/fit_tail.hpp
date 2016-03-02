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

 /// routine for fitting the tail (singularity) of a Matsubara Green's function
 /**
  This is a *linear* least squares problem (with non-linear basis functions)
  which is solved by singular value decomposition of the design matrix
  @note the routine fits the real part (even moments) and the imaginary part (odd moments) separately for stability reasons

  @param gf the input gf<imfreq> Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the index of the first frequency to fit
  @param n_max the index of the last frequency to fit (included)

  @return the tail obtained by fitting
 */
 tail fit_tail_impl(gf_view<imfreq> gf, const tail_view known_moments, int max_moment, int n_min, int n_max) ;

 ///Fit the tail of a gf
 /**
  @param gf the input gf<imfreq> Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the Matsubara index of the first frequency to fit
  @param n_max the Matsubara index of the last frequency to fit (included)
  @param replace_by_fit if true, replace the gf data from n_min to the last index of the mesh by the asymptotic behavior obtained by fitting the tails
  @note Based on [[fit_tail_impl]]
 */
 void fit_tail(gf_view<imfreq> gf, tail_view known_moments, int max_moment, int n_min, int n_max,
   bool replace_by_fit = false) ;

 ///Fit the tail of a block_gf
 /**
  @param gf the input Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the Matsubara index of the first frequency to fit
  @param n_max the Matsubara index of the last frequency to fit (included)
  @param replace_by_fit if true, replace the gf data from n_min to the last index of the mesh by the asymptotic behavior obtained by fitting the tails
  @note Based on [[fit_tail_impl]]
 */
 void fit_tail(gf_view<block_index, gf<imfreq>> block_gf, tail_view known_moments, int max_moment, int n_min,
   int n_max, bool replace_by_fit = false) ;

 ///Fit the tail of a gf (scalar-valued)
 /**
  @param gf the input gf<imfreq> Green's function
  @param known_moments the known moments in the form of a tail(_view)
  @param max_moment the highest fitted moment and the highest moment in the tail after the fit: max_moment
  @param n_min the Matsubara index of the first frequency to fit
  @param n_max the Matsubara index of the last frequency to fit (included)
  @param replace_by_fit if true, replace the gf data from n_min to the last index of the mesh by the asymptotic behavior obtained by fitting the tails
  @note Based on [[fit_tail_impl]]
 */
 void fit_tail(gf_view<imfreq, scalar_valued> gf, tail_view known_moments, int max_moment, int n_min, int n_max, bool replace_by_fit = false) ;
}} // namespace
