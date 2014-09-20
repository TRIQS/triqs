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
#include <triqs/gfs/imfreq.hpp>
#include <triqs/gfs/block.hpp>
#include <triqs/gfs/local/tail.hpp>
#include <triqs/arrays/blas_lapack/gelss.hpp>

namespace triqs { namespace gfs { 

 using triqs::gfs::imfreq;
 using triqs::gfs::block_index;
 using triqs::gfs::block_index;

 // routine for fitting the tail (singularity) of a Matsubara Green's function
 // this is a *linear* least squares problem (with non-linear basis functions)
 // which is solved by singular value decomposition of the design matrix
 // the routine fits the real part (even moments) and the imaginary part
 //(odd moments) separately, since this is more stable

 // input:
 // the input gf<imfreq> Green's function: gf
 // the known moments in the form of a tail(_view): known_moments
 // the TOTAL number of desired moments (including the known ones): n_moments
 // the index of the first and last frequency to fit (the last one is included): n_min, n_max

 // output: returns the tail obtained by fitting

 tail fit_tail_impl(gf_view<imfreq> gf, const tail_view known_moments, int n_moments, int n_min, int n_max) ;

 void fit_tail(gf_view<imfreq> gf, tail_view known_moments, int n_moments, int n_min, int n_max,
   bool replace_by_fit = false) ;

 void fit_tail(gf_view<block_index, gf<imfreq>> block_gf, tail_view known_moments, int n_moments, int n_min,
   int n_max, bool replace_by_fit = false) ;

 void fit_tail(gf_view<imfreq, scalar_valued> gf, tail_view known_moments, int n_moments, int n_min, int n_max, bool replace_by_fit = false) ;
}} // namespace
