/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include <type_traits>
#include "../array.hpp"
#include "../matrix.hpp"
#include "../vector.hpp"
#include <triqs/utility/exceptions.hpp>

namespace triqs {
  namespace arrays {
    namespace linalg {

      extern "C" {
      void TRIQS_FORTRAN_MANGLING(dsyev)(char *, char *, // JOBZ and UPLO
                                         int &,          // Matrix Size
                                         double[],       // matrix
                                         int &,          // LDA of the matrix
                                         double[],       // Eigenvalues array
                                         double[],       // WORK
                                         int &,          // LWORK
                                         int &           // INFO
      );

      void TRIQS_FORTRAN_MANGLING(zheev)(char *, char *,         // JOBZ and UPLO
                                         int &,                  // Matrix Size
                                         std::complex<double>[], // matrix
                                         int &,                  // LDA of the matrix
                                         double[],               // Eigenvalues array
                                         std::complex<double>[], // WORK
                                         int &,                  // LWORK
                                         double[],               // WORK2
                                         int &                   // INFO
      );
      }

      /**
   * A worker to call lapack routine with the matrices. Handles both real and complex case.
   */
      template <typename T> class eigenelements_worker {
        public:
        eigenelements_worker() = default;

        /// The eigenvalues
        template <typename M> array<double, 1> eigenvalues(M &mat) const {
          _prepare(mat);
          _invoke(is_complex<T>(), 'N', mat);
          return ev;
        }

        /// The eigensystems
        template <typename M> std::pair<array<double, 1>, matrix<T>> eigenelements(M &mat) const {
          _prepare(mat);
          _invoke(is_complex<T>(), 'V', mat);
          return {ev, _conj(mat, is_complex<T>())};
        }

        private:
        mutable array<double, 1> ev, work2; // work2 only used for T complex
        mutable array<T, 1> work;
        mutable int dim, lwork, info;

        // dispatch the implementation of invoke for T = double or complex
        void _invoke(std::false_type, char compz, matrix_view<double> mat) const { // the case double (is_complex = false)
          char uplo = 'U';
          TRIQS_FORTRAN_MANGLING(dsyev)(&compz, &uplo, dim, mat.data_start(), dim, ev.data_start(), work.data_start(), lwork, info);
          if (info) TRIQS_RUNTIME_ERROR << "eigenelements_worker :error code dsyev : " << info << " for matrix " << mat;
        }

        void _invoke(std::true_type, char compz, matrix_view<std::complex<double>> mat) const { // the case complex (is_complex = true)
          char uplo = 'U';
          TRIQS_FORTRAN_MANGLING(zheev)
          (&compz, &uplo, dim, mat.data_start(), dim, ev.data_start(), work.data_start(), lwork, work2.data_start(), info);
          if (info) TRIQS_RUNTIME_ERROR << "eigenelements_worker :error code zheev : " << info << " for matrix " << mat;
        }

        template <typename M> void _prepare(M const &mat) const {
          if (mat.is_empty()) TRIQS_RUNTIME_ERROR << "eigenelements_worker : the matrix is empty : matrix =  " << mat << "  ";
          if (!mat.is_square()) TRIQS_RUNTIME_ERROR << "eigenelements_worker : the matrix " << mat << " is not square ";
          if (!mat.indexmap().is_contiguous()) TRIQS_RUNTIME_ERROR << "eigenelements_worker : the matrix " << mat << " is not contiguous in memory";
          dim = first_dim(mat);
          ev.resize(dim);
          lwork = 64 * dim;
          work.resize(lwork);
          if (is_complex<T>::value) work2.resize(lwork);
        }

        template <typename M> matrix<double> _conj(M const &m, std::false_type) const { return m; }

        // impl : since we call fortran lapack, if the order is C (!), the matrix is transposed, or conjugated, so we obtain
        // the conjugate of the eigenvectors... Fix #119.
        // Do nothing if the order is fortran already...
        template <typename M> matrix<std::complex<double>> _conj(M const &m, std::true_type) const {
          if (m.memory_layout_is_c())
            return conj(m);
          else
            return m.transpose(); // the matrix mat is understood as a fortran matrix. After the lapack, in memory, it contains the
                                  // correct answer.
                                  // but since it is a fortran matrix, the C will see its transpose. We need to compensate this transpose (!).
        }
      };

      //--------------------------------

      /**
   * Simple diagonalization call, return all eigenelements.
   * Handles both real and complex case.
   * @param M : the matrix or view.
   */
      template <typename M> std::pair<array<double, 1>, matrix<typename M::value_type>> eigenelements(M const &m) {
        auto m2 = make_clone(m);
        return eigenelements_worker<typename M::value_type>().eigenelements(m2);
      }

      //--------------------------------

      /**
   * Simple diagonalization call, return all eigenelements.
   * Handles both real and complex case.
   * Works in place, i.e. changes the matrix
   * @param M : the matrix or view.
   */
      template <typename M> std::pair<array<double, 1>, matrix<typename M::value_type>> eigenelements_in_place(M *m) {
        return eigenelements_worker<typename M::value_type>().eigenelements(*m);
      }

      //--------------------------------

      /**
   * Simple diagonalization call, returning only the eigenvalues.
   * Handles both real and complex case.
   * @param M : the matrix VIEW : it MUST be contiguous
   */
      template <typename M> array<double, 1> eigenvalues(M const &m) {
        auto m2 = make_clone(m);
        return eigenelements_worker<typename M::value_type>().eigenvalues(m2);
      }

      //--------------------------------

      /**
   * Simple diagonalization call, returning only the eigenvalues.
   * Handles both real and complex case.
   * @param M : the matrix VIEW : it MUST be contiguous
   */
      template <typename M> array<double, 1> eigenvalues_in_place(M *m) { return eigenelements_worker<typename M::value_type>().eigenvalues(*m); }
    } // namespace linalg
  }   // namespace arrays
} // namespace triqs
