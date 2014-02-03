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
#ifndef TRIQS_GF_LOCAL_FIT_TAIL_H
#define TRIQS_GF_LOCAL_FIT_TAIL_H
#include <triqs/gfs/imfreq.hpp>
#include <triqs/gfs/block.hpp>
#include <triqs/gfs/local/tail.hpp>
#include <triqs/arrays/blas_lapack/gelss.hpp>
#include <triqs/python_tools/cython_proxy.hpp>

namespace triqs {
namespace gfs {
 namespace local {

  using triqs::gfs::imfreq;
  using triqs::gfs::block_index;
  using triqs::gfs::block_index;

  namespace tgl = triqs::gfs::local;

  // routine for fitting the tail (singularity) of a Matsubara Green's function
  // this is a *linear* least squares problem (with non-linear basis functions)
  // which is solved by singular value decomposition of the design matrix
  // the routine fits the real part (even moments) and the imaginary part
  //(odd moments) separately, since this is more stable

  // input:
  // the input gf<imfreq> Green's function: gf
  // the known moments in the form of a tail(_view): known_moments
  // the TOTAL number of desired moments (including the known ones): n_moments
  // the index of the first and last frequency to fit (the last one is included): wn_min, wn_max

  // output: returns the tail obtained by fitting

  tail fit_tail_impl(gf<imfreq> &gf, const tail_view known_moments, int n_moments, int wn_min, int wn_max) {

   tail res(get_target_shape(gf), n_moments, known_moments.order_min());
   if (known_moments.size())
    for (int i = known_moments.order_min(); i <= known_moments.order_max(); i++) res(i) = known_moments(i);

   // if known_moments.size()==0, the lowest order to be obtained from the fit is determined by order_min in known_moments
   // if known_moments.size()==0, the lowest order is the one following order_max in known_moments

   const double beta = gf.mesh().domain().beta;

   int n_unknown_moments = n_moments - known_moments.size();
   if (n_unknown_moments < 1) return known_moments;

   // get the number of even unknown moments: it is n_unknown_moments/2+1 if the first
   // moment is even and n_moments is odd; n_unknown_moments/2 otherwise
   int omin = known_moments.size() == 0 ? known_moments.order_min() : known_moments.order_max() + 1; // smallest unknown moment
   int omin_even = omin % 2 == 0 ? omin : omin + 1;
   int omin_odd = omin % 2 != 0 ? omin : omin + 1;
   int size_even = n_unknown_moments / 2;
   if (n_unknown_moments % 2 != 0 && omin % 2 == 0) size_even += 1;
   int size_odd = n_unknown_moments - size_even;

   int size1 = wn_max - wn_min + 1;
   // size2 is the number of moments

   arrays::matrix<double, 2> A(size1, std::max(size_even, size_odd), FORTRAN_LAYOUT);
   arrays::matrix<double, 2> B(size1, 1, FORTRAN_LAYOUT);
   arrays::vector<double> S(std::max(size_even, size_odd));
   const double rcond = 0.0;
   int rank;

   for (size_t i = 0; i < get_target_shape(gf)[0]; i++) {
    for (size_t j = 0; j < get_target_shape(gf)[1]; j++) {

     // fit the odd moments
     // S.resize(size_odd);
     // A.resize(size1,size_odd); //when resizing, gelss segfaults
     for (int k = 0; k < size1; k++) {
      auto n = wn_min + k;
      auto iw = std::complex<double>(0.0, (2 * n + 1) * M_PI / beta);

      B(k, 0) = imag(gf.data()(n, i, j));
      // subtract known tail if present
      if (known_moments.size() > 0)
       B(k, 0) -= imag(slice_target(known_moments, arrays::range(i, i + 1), arrays::range(j, j + 1)).evaluate(iw)(0, 0));

      for (int l = 0; l < size_odd; l++) {
       int order = omin_odd + 2 * l;
       A(k, l) = imag(pow(iw, -1.0 * order)); // set design matrix for odd moments
      }
     }

     arrays::lapack::gelss(A, B, S, rcond, rank);
     for (int m = 0; m < size_odd; m++) {
      res(omin_odd + 2 * m)(i, j) = B(m, 0);
     }

     // fit the even moments
     // S.resize(size_even);
     // A.resize(size1,size_even); //when resizing, gelss segfaults
     for (int k = 0; k < size1; k++) {
      auto n = wn_min + k;
      auto iw = std::complex<double>(0.0, (2 * n + 1) * M_PI / beta);

      B(k, 0) = real(gf.data()(n, i, j));
      // subtract known tail if present
      if (known_moments.size() > 0)
       B(k, 0) -= real(slice_target(known_moments, arrays::range(i, i + 1), arrays::range(j, j + 1)).evaluate(iw)(0, 0));

      for (int l = 0; l < size_even; l++) {
       int order = omin_even + 2 * l;
       A(k, l) = real(pow(iw, -1.0 * order)); // set design matrix for odd moments
      }
     }

     arrays::lapack::gelss(A, B, S, rcond, rank);
     for (int m = 0; m < size_even; m++) {
      res(omin_even + 2 * m)(i, j) = B(m, 0);
     }
    }
   }

   return res; // return tail
  }


  void set_tail_from_fit(gf<imfreq> &gf, tail_view known_moments, int n_moments, size_t wn_min, size_t wn_max,
                         bool replace_by_fit = false) {
   if (get_target_shape(gf) != known_moments.shape()) TRIQS_RUNTIME_ERROR << "shape of tail does not match shape of gf";
   gf.singularity() = fit_tail_impl(gf, known_moments, n_moments, wn_min, wn_max);

   if (replace_by_fit) { // replace data in the fitting range by the values from the fitted tail
    size_t i = 0;
    for (auto iw : gf.mesh()) { // (arrays::range(wn_min,wn_max+1)) {
     if ((i >= wn_min) && (i <= wn_max)) gf[iw] = gf.singularity().evaluate(iw);
     i++;
    }
   }
  }


  void set_tail_from_fit(gf<block_index, gf<imfreq>> &block_gf, tail_view known_moments, int n_moments, size_t wn_min,
                         size_t wn_max, bool replace_by_fit = false) {
   // for(auto &gf : block_gf) set_tail_from_fit(gf, known_moments, n_moments, wn_min, wn_max, replace_by_fit);
   for (size_t i = 0; i < block_gf.mesh().size(); i++)
    set_tail_from_fit(block_gf[i], known_moments, n_moments, wn_min, wn_max, replace_by_fit);
  }
 }
}
} // namespace
#endif
