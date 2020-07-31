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

#pragma once

#include <complex>

namespace triqs::arrays::blas::f77 {
  // typedef std::complex<double> dcomplex;

  void axpy(int N, double alpha, const double *x, int incx, double *Y, int incy);
  void axpy(int N, std::complex<double> alpha, const std::complex<double> *x, int incx, std::complex<double> *Y, int incy);

  void copy(int N, const double *x, int incx, double *Y, int incy);
  void copy(int N, const std::complex<double> *x, int incx, std::complex<double> *Y, int incy);

  double dot(int M, const double *x, int incx, const double *Y, int incy);
  //std::complex<double> dot (int  M, const std::complex<double>* x, int  incx, const std::complex<double>* Y, int  incy) ;

  void gemm(char trans_a, char trans_b, int M, int N, int K, double alpha, const double *A, int LDA, const double *B, int LDB, double beta, double *C,
            int LDC);
  void gemm(char trans_a, char trans_b, int M, int N, int K, std::complex<double> alpha, const std::complex<double> *A, int LDA,
            const std::complex<double> *B, int LDB, std::complex<double> beta, std::complex<double> *C, int LDC);

  void gemv(char *trans, int M, int N, double &alpha, const double *A, int &LDA, const double *x, int incx, double &beta, double *Y, int incy);
  void gemv(char *trans, int M, int N, std::complex<double> &alpha, const std::complex<double> *A, int &LDA, const std::complex<double> *x, int incx,
            std::complex<double> &beta, std::complex<double> *Y, int incy);

  void ger(int M, int N, double alpha, const double *x, int incx, const double *Y, int incy, double *A, int LDA);
  void ger(int M, int N, std::complex<double> alpha, const std::complex<double> *x, int incx, const std::complex<double> *Y, int incy,
           std::complex<double> *A, int LDA);

  void scal(int M, double alpha, double *x, int incx);
  void scal(int M, std::complex<double> alpha, std::complex<double> *x, int incx);

  void swap(int N, double *x, int incx, double *Y, int incy);
  void swap(int N, std::complex<double> *x, int incx, std::complex<double> *Y, int incy);

} // namespace triqs::arrays::blas::f77

namespace triqs::arrays::lapack::f77 {

  void gelss(int M, int N, int NRHS, double *A, int LDA, double *B, int LDB, double *S, double RCOND, int &RANK, double *WORK, int LWORK, int &INFO);
  void gelss(int M, int N, int NRHS, std::complex<double> *A, int LDA, std::complex<double> *B, int LDB, double *S, double RCOND, int &RANK,
             std::complex<double> *WORK, int LWORK, double *RWORK, int &INFO);

  void gesvd(const char &JOBU, const char &JOBVT, int M, int N, double *A, int LDA, double *S, double *U, int LDU, double *VT, int LDVT, double *WORK,
             int LWORK, int &INFO);
  void gesvd(const char &JOBU, const char &JOBVT, int M, int N, std::complex<double> *A, int LDA, double *S, std::complex<double> *U, int LDU,
             std::complex<double> *VT, int LDVT, std::complex<double> *WORK, int LWORK, double *RWORK, int &INFO);

  void getrf(int M, int N, double *A, int LDA, int *ipiv, int &info);
  void getrf(int M, int N, std::complex<double> *A, int LDA, int *ipiv, int &info);

  void getri(int N, double *A, int LDA, int *ipiv, double *work, int lwork, int &info);
  void getri(int N, std::complex<double> *A, int LDA, int *ipiv, std::complex<double> *work, int lwork, int &info);

  void gtsv(int N, int NRHS, double *DL, double *D, double *DU, double *B, int LDB, int &info);
  void gtsv(int N, int NRHS, std::complex<double> *DL, std::complex<double> *D, std::complex<double> *DU, std::complex<double> *B, int LDB,
            int &info);

  void stev(char J, int N, double *D, double *E, double *Z, int ldz, double *work, int &info);

} // namespace triqs::arrays::lapack::f77
