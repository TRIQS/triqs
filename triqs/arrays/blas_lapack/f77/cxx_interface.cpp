/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, P. Dumitrescu, N. Wentzell
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

#include "cxx_interface.hpp"
// Extracted from Reference Lapack (https://github.com/Reference-LAPACK):
#include "cblas_f77.h" 
#include "lapack.h"

// Manual Include since cblas uses Fortan _sub to wrap function again
#define F77_ddot F77_GLOBAL(ddot, DDOT) //TRIQS Manual
extern "C" {
double F77_ddot(FINT, const double *, FINT, const double *, FINT);
}

namespace triqs::arrays::blas::f77 {

  void axpy(const int &N, const double &alpha, const double *x, const int &incx, double *Y, const int &incy) {
    F77_daxpy(&N, &alpha, x, &incx, Y, &incy);
  }
  void axpy(const int &N, const std::complex<double> &alpha, const std::complex<double> *x, const int &incx, std::complex<double> *Y,
            const int &incy) {
    F77_zaxpy(&N, reinterpret_cast<const double *>(&alpha), reinterpret_cast<const double *>(x), &incx, reinterpret_cast<double *>(Y), &incy);
  }
  // No Const In Wrapping!
  void copy(const int &N, const double *x, const int &incx, double *Y, const int &incy) { F77_dcopy(&N, x, &incx, Y, &incy); }
  void copy(const int &N, const std::complex<double> *x, const int &incx, std::complex<double> *Y, const int &incy) {
    F77_zcopy(&N, reinterpret_cast<const double *>(x), &incx, reinterpret_cast<double *>(Y), &incy);
  }

  double dot(const int &M, const double *x, const int &incx, const double *Y, const int &incy) { return F77_ddot(&M, x, &incx, Y, &incy); }

  void gemm(char trans_a, char trans_b, const int &M, const int &N, const int &K, const double &alpha, const double *A, const int &LDA,
            const double *B, const int &LDB, const double &beta, double *C, const int &LDC) {
    F77_dgemm(&trans_a, &trans_b, &M, &N, &K, &alpha, A, &LDA, B, &LDB, &beta, C, &LDC);
  }
  void gemm(char trans_a, char trans_b, const int &M, const int &N, const int &K, const std::complex<double> &alpha, const std::complex<double> *A,
            const int &LDA, const std::complex<double> *B, const int &LDB, const std::complex<double> &beta, std::complex<double> *C,
            const int &LDC) {
    F77_zgemm(&trans_a, &trans_b, &M, &N, &K, reinterpret_cast<const double *>(&alpha), reinterpret_cast<const double *>(A), &LDA,
              reinterpret_cast<const double *>(B), &LDB, reinterpret_cast<const double *>(&beta), reinterpret_cast<double *>(C), &LDC);
  }

  void gemv(char *trans, const int &M, const int &N, double &alpha, const double *A, int &LDA, const double *x, const int &incx, double &beta,
            double *Y, const int &incy) {
    F77_dgemv(trans, &M, &N, &alpha, A, &LDA, x, &incx, &beta, Y, &incy);
  }
  void gemv(char *trans, const int &M, const int &N, std::complex<double> &alpha, const std::complex<double> *A, int &LDA,
            const std::complex<double> *x, const int &incx, std::complex<double> &beta, std::complex<double> *Y, const int &incy) {
    F77_zgemv(trans, &M, &N, reinterpret_cast<const double *>(&alpha), reinterpret_cast<const double *>(A), &LDA, reinterpret_cast<const double *>(x),
              &incx, reinterpret_cast<const double *>(&beta), reinterpret_cast<double *>(Y), &incy);
  }

  void ger(const int &M, const int &N, const double &alpha, const double *x, const int &incx, const double *Y, const int &incy, double *A,
           const int &LDA) {
    F77_dger(&M, &N, &alpha, x, &incx, Y, &incy, A, &LDA);
  }
  void ger(const int &M, const int &N, const std::complex<double> &alpha, const std::complex<double> *x, const int &incx,
           const std::complex<double> *Y, const int &incy, std::complex<double> *A, const int &LDA) {
    F77_zgeru(&M, &N, reinterpret_cast<const double *>(&alpha), reinterpret_cast<const double *>(x), &incx, reinterpret_cast<const double *>(Y),
              &incy, reinterpret_cast<double *>(A), &LDA);
  }

  void scal(const int &M, const double &alpha, double *x, const int &incx) { F77_dscal(&M, &alpha, x, &incx); }
  void scal(const int &M, const std::complex<double> &alpha, std::complex<double> *x, const int &incx) {
    F77_zscal(&M, reinterpret_cast<const double *>(&alpha), reinterpret_cast<double *>(x), &incx);
  }

  void swap(const int &N, double *x, const int &incx, double *Y, const int &incy) { F77_dswap(&N, x, &incx, Y, &incy); }
  void swap(const int &N, std::complex<double> *x, const int &incx, std::complex<double> *Y, const int &incy) {
    F77_zswap(&N, reinterpret_cast<double *>(x), &incx, reinterpret_cast<double *>(Y), &incy);
  }

} // namespace triqs::arrays::blas::f77

namespace triqs::arrays::lapack::f77 {

  void gelss(const int &M, const int &N, const int &NRHS, double *A, const int &LDA, double *B, const int &LDB, double *S, const double &RCOND,
             int &RANK, double *WORK, const int &LWORK, int &INFO) {
    LAPACK_dgelss(&M, &N, &NRHS, A, &LDA, B, &LDB, S, &RCOND, &RANK, WORK, &LWORK, &INFO);
  }
  void gelss(const int &M, const int &N, const int &NRHS, std::complex<double> *A, const int &LDA, std::complex<double> *B, const int &LDB, double *S,
             const double &RCOND, int &RANK, std::complex<double> *WORK, const int &LWORK, double *RWORK, int &INFO) {
    LAPACK_zgelss(&M, &N, &NRHS, A, &LDA, B, &LDB, S, &RCOND, &RANK, WORK, &LWORK, RWORK, &INFO);
  }

  void gesvd(const char &JOBU, const char &JOBVT, const int &M, const int &N, double *A, const int &LDA, double *S, double *U, const int &LDU,
             double *VT, const int &LDVT, double *WORK, const int &LWORK, int &INFO) {
    LAPACK_dgesvd(&JOBU, &JOBVT, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, WORK, &LWORK, &INFO);
  }
  void gesvd(const char &JOBU, const char &JOBVT, const int &M, const int &N, std::complex<double> *A, const int &LDA, double *S,
             std::complex<double> *U, const int &LDU, std::complex<double> *VT, const int &LDVT, std::complex<double> *WORK, const int &LWORK,
             double *RWORK, int &INFO) {
    LAPACK_zgesvd(&JOBU, &JOBVT, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, WORK, &LWORK, RWORK, &INFO);
  }

  void getrf(const int &M, const int &N, double *A, const int &LDA, int *ipiv, int &info) { LAPACK_dgetrf(&M, &N, A, &LDA, ipiv, &info); }
  void getrf(const int &M, const int &N, std::complex<double> *A, const int &LDA, int *ipiv, int &info) {
    LAPACK_zgetrf(&M, &N, A, &LDA, ipiv, &info);
  }

  void getri(const int &N, double *A, const int &LDA, int *ipiv, double *work, const int &lwork, int &info) {
    LAPACK_dgetri(&N, A, &LDA, ipiv, work, &lwork, &info);
  }
  void getri(const int &N, std::complex<double> *A, const int &LDA, int *ipiv, std::complex<double> *work, const int &lwork, int &info) {
    LAPACK_zgetri(&N, A, &LDA, ipiv, work, &lwork, &info);
  }

  void gtsv(const int &N, const int &NRHS, double *DL, double *D, double *DU, double *B, const int &LDB, int &info) {
    LAPACK_dgtsv(&N, &NRHS, DL, D, DU, B, &LDB, &info);
  }
  void gtsv(const int &N, const int &NRHS, std::complex<double> *DL, std::complex<double> *D, std::complex<double> *DU, std::complex<double> *B,
            const int &LDB, int &info) {
    LAPACK_zgtsv(&N, &NRHS, DL, D, DU, B, &LDB, &info);
  }

  // TODO: WHY IS J NOT CONST HERE AND PASSED BY VALUE? WHY IS LDZ by value? and non-const
  void stev(char J, const int &N, double *D, double *E, double *Z, int ldz, double *work, int &info) {
    LAPACK_dstev(&J, &N, D, E, Z, &ldz, work, &info);
  }

} // namespace triqs::arrays::lapack::f77
