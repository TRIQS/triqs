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
#ifndef TRIQS_ARRAYS_BLAS_LAPACK_SCAL_H
#define TRIQS_ARRAYS_BLAS_LAPACK_SCAL_H
#include "./tools.hpp"

namespace triqs {
  namespace arrays {
    namespace blas {

      namespace f77 { // overload

        extern "C" {
        void TRIQS_FORTRAN_MANGLING(dscal)(const int &N, const double &alpha, double *x, const int &incx);
        void TRIQS_FORTRAN_MANGLING(zscal)(const int &N, const std::complex<double> &alpha, std::complex<double> *x, const int &incx);
        }

        inline void scal(const int &M, const double &alpha, double *x, const int &incx) { TRIQS_FORTRAN_MANGLING(dscal)(M, alpha, x, incx); }
        inline void scal(const int &M, const std::complex<double> &alpha, std::complex<double> *x, const int &incx) {
          TRIQS_FORTRAN_MANGLING(zscal)(M, alpha, x, incx);
        }
      } // namespace f77

      /**
  * Blas1 : scal
  */
      template <typename VT>
      std14::enable_if_t<is_blas_lapack_type<typename VT::value_type>::value && have_same_value_type<VT>::value>
      scal(typename VT::value_type const &alpha, VT &X) {
        static_assert(is_amv_value_or_view_class<VT>::value, "blas1 bindings only take vector and vector_view");
        f77::scal(X.size(), alpha, X.data_start(), X.stride());
      }

      /**
  * Non blas equivalent
  */
      template <typename VT>
      std14::enable_if_t<!(is_blas_lapack_type<typename VT::value_type>::value && have_same_value_type<VT>::value)>
      scal(typename VT::value_type const &alpha, VT &X) {
        static_assert(is_amv_value_or_view_class<VT>::value, "blas1 bindings only take vector and vector_view");
        for (size_t i = 0; i < X.size(); ++i) X(i) = X(i) * alpha;
      }

    } // namespace blas
  }   // namespace arrays
} // namespace triqs

#endif
