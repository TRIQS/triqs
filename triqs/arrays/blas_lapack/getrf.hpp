/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_BLAS_LAPACK_GETRF_H
#define TRIQS_ARRAYS_BLAS_LAPACK_GETRF_H
#include <complex>
#include "./tools.hpp"
#include "./qcache.hpp"

namespace triqs {
  namespace arrays {
    namespace lapack {

      using namespace blas_lapack_tools;
      namespace f77 { // overload
        typedef std::complex<double> dcomplex;

        extern "C" {
        void TRIQS_FORTRAN_MANGLING(dgetrf)(const int &, const int &, double *, const int &, int *, int &);
        void TRIQS_FORTRAN_MANGLING(zgetrf)(const int &, const int &, std::complex<double> *, const int &, int *, int &);
        }

        inline void getrf(const int &M, const int &N, double *A, const int &LDA, int *ipiv, int &info) {
          TRIQS_FORTRAN_MANGLING(dgetrf)(M, N, A, LDA, ipiv, info);
        }
        inline void getrf(const int &M, const int &N, std::complex<double> *A, const int &LDA, int *ipiv, int &info) {
          TRIQS_FORTRAN_MANGLING(zgetrf)(M, N, A, LDA, ipiv, info);
        }
      } // namespace f77

      /**
  * Calls getrf on a matrix or view
  * Takes care of making temporary copies if necessary
  */
      template <typename MT>
      typename std::enable_if<is_blas_lapack_type<typename MT::value_type>::value, int>::type getrf(MT &A, arrays::vector<int> &ipiv,
                                                                                                    bool assert_fortran_order = false) {
        if (assert_fortran_order && A.memory_layout_is_c()) TRIQS_RUNTIME_ERROR << "matrix passed to getrf is not in Fortran order";
        reflexive_qcache<MT> Ca(A);
        auto dm = std::min(first_dim(Ca()), second_dim(Ca()));
        if (ipiv.size() < dm) ipiv.resize(dm);
        int info;
        f77::getrf(get_n_rows(Ca()), get_n_cols(Ca()), Ca().data_start(), get_ld(Ca()), ipiv.data_start(), info);
        return info;
      }
    } // namespace lapack
  }   // namespace arrays
} // namespace triqs

#endif
