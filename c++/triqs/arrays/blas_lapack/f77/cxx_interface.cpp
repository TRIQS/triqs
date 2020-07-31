// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Philipp D, Nils Wentzell

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

  void axpy(int N, double alpha, const double *x, int incx, double *Y, int incy) { F77_daxpy(&N, &alpha, x, &incx, Y, &incy); }
  void axpy(int N, std::complex<double> alpha, const std::complex<double> *x, int incx, std::complex<double> *Y, int incy) {
    F77_zaxpy(&N, reinterpret_cast<const double *>(&alpha), reinterpret_cast<const double *>(x), &incx, reinterpret_cast<double *>(Y), &incy);
  }
  // No Const In Wrapping!
  void copy(int N, const double *x, int incx, double *Y, int incy) { F77_dcopy(&N, x, &incx, Y, &incy); }
  void copy(int N, const std::complex<double> *x, int incx, std::complex<double> *Y, int incy) {
    F77_zcopy(&N, reinterpret_cast<const double *>(x), &incx, reinterpret_cast<double *>(Y), &incy);
  }

  double dot(int M, const double *x, int incx, const double *Y, int incy) { return F77_ddot(&M, x, &incx, Y, &incy); }

  void gemm(char trans_a, char trans_b, int M, int N, int K, double alpha, const double *A, int LDA, const double *B, int LDB, double beta, double *C,
            int LDC) {
    F77_dgemm(&trans_a, &trans_b, &M, &N, &K, &alpha, A, &LDA, B, &LDB, &beta, C, &LDC);
  }
  void gemm(char trans_a, char trans_b, int M, int N, int K, std::complex<double> alpha, const std::complex<double> *A, int LDA,
            const std::complex<double> *B, int LDB, std::complex<double> beta, std::complex<double> *C, int LDC) {
    F77_zgemm(&trans_a, &trans_b, &M, &N, &K, reinterpret_cast<const double *>(&alpha), reinterpret_cast<const double *>(A), &LDA,
              reinterpret_cast<const double *>(B), &LDB, reinterpret_cast<const double *>(&beta), reinterpret_cast<double *>(C), &LDC);
  }

  void gemv(char *trans, int M, int N, double &alpha, const double *A, int &LDA, const double *x, int incx, double &beta, double *Y, int incy) {
    F77_dgemv(trans, &M, &N, &alpha, A, &LDA, x, &incx, &beta, Y, &incy);
  }
  void gemv(char *trans, int M, int N, std::complex<double> &alpha, const std::complex<double> *A, int &LDA, const std::complex<double> *x, int incx,
            std::complex<double> &beta, std::complex<double> *Y, int incy) {
    F77_zgemv(trans, &M, &N, reinterpret_cast<const double *>(&alpha), reinterpret_cast<const double *>(A), &LDA, reinterpret_cast<const double *>(x),
              &incx, reinterpret_cast<const double *>(&beta), reinterpret_cast<double *>(Y), &incy);
  }

  void ger(int M, int N, double alpha, const double *x, int incx, const double *Y, int incy, double *A, int LDA) {
    F77_dger(&M, &N, &alpha, x, &incx, Y, &incy, A, &LDA);
  }
  void ger(int M, int N, std::complex<double> alpha, const std::complex<double> *x, int incx, const std::complex<double> *Y, int incy,
           std::complex<double> *A, int LDA) {
    F77_zgeru(&M, &N, reinterpret_cast<const double *>(&alpha), reinterpret_cast<const double *>(x), &incx, reinterpret_cast<const double *>(Y),
              &incy, reinterpret_cast<double *>(A), &LDA);
  }

  void scal(int M, double alpha, double *x, int incx) { F77_dscal(&M, &alpha, x, &incx); }
  void scal(int M, std::complex<double> alpha, std::complex<double> *x, int incx) {
    F77_zscal(&M, reinterpret_cast<const double *>(&alpha), reinterpret_cast<double *>(x), &incx);
  }

  void swap(int N, double *x, int incx, double *Y, int incy) { F77_dswap(&N, x, &incx, Y, &incy); }
  void swap(int N, std::complex<double> *x, int incx, std::complex<double> *Y, int incy) {
    F77_zswap(&N, reinterpret_cast<double *>(x), &incx, reinterpret_cast<double *>(Y), &incy);
  }

} // namespace triqs::arrays::blas::f77

namespace triqs::arrays::lapack::f77 {

  void gelss(int M, int N, int NRHS, double *A, int LDA, double *B, int LDB, double *S, double RCOND, int &RANK, double *WORK, int LWORK, int &INFO) {
    LAPACK_dgelss(&M, &N, &NRHS, A, &LDA, B, &LDB, S, &RCOND, &RANK, WORK, &LWORK, &INFO);
  }
  void gelss(int M, int N, int NRHS, std::complex<double> *A, int LDA, std::complex<double> *B, int LDB, double *S, double RCOND, int &RANK,
             std::complex<double> *WORK, int LWORK, double *RWORK, int &INFO) {
    LAPACK_zgelss(&M, &N, &NRHS, A, &LDA, B, &LDB, S, &RCOND, &RANK, WORK, &LWORK, RWORK, &INFO);
  }

  void gesvd(const char &JOBU, const char &JOBVT, int M, int N, double *A, int LDA, double *S, double *U, int LDU, double *VT, int LDVT, double *WORK,
             int LWORK, int &INFO) {
    LAPACK_dgesvd(&JOBU, &JOBVT, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, WORK, &LWORK, &INFO);
  }
  void gesvd(const char &JOBU, const char &JOBVT, int M, int N, std::complex<double> *A, int LDA, double *S, std::complex<double> *U, int LDU,
             std::complex<double> *VT, int LDVT, std::complex<double> *WORK, int LWORK, double *RWORK, int &INFO) {
    LAPACK_zgesvd(&JOBU, &JOBVT, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, WORK, &LWORK, RWORK, &INFO);
  }

  void getrf(int M, int N, double *A, int LDA, int *ipiv, int &info) { LAPACK_dgetrf(&M, &N, A, &LDA, ipiv, &info); }
  void getrf(int M, int N, std::complex<double> *A, int LDA, int *ipiv, int &info) { LAPACK_zgetrf(&M, &N, A, &LDA, ipiv, &info); }

  void getri(int N, double *A, int LDA, int *ipiv, double *work, int lwork, int &info) { LAPACK_dgetri(&N, A, &LDA, ipiv, work, &lwork, &info); }
  void getri(int N, std::complex<double> *A, int LDA, int *ipiv, std::complex<double> *work, int lwork, int &info) {
    LAPACK_zgetri(&N, A, &LDA, ipiv, work, &lwork, &info);
  }

  void gtsv(int N, int NRHS, double *DL, double *D, double *DU, double *B, int LDB, int &info) { LAPACK_dgtsv(&N, &NRHS, DL, D, DU, B, &LDB, &info); }
  void gtsv(int N, int NRHS, std::complex<double> *DL, std::complex<double> *D, std::complex<double> *DU, std::complex<double> *B, int LDB,
            int &info) {
    LAPACK_zgtsv(&N, &NRHS, DL, D, DU, B, &LDB, &info);
  }

  void stev(char J, int N, double *D, double *E, double *Z, int ldz, double *work, int &info) { LAPACK_dstev(&J, &N, D, E, Z, &ldz, work, &info); }

} // namespace triqs::arrays::lapack::f77
