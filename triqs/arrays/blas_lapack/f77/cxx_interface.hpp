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

#pragma once

#include <complex>

namespace triqs::arrays::blas::f77 {
  // typedef std::complex<double> dcomplex;

  void axpy(const int &N, const double &alpha, const double *x, const int &incx, double *Y, const int &incy);
  void axpy(const int &N, const std::complex<double> &alpha, const std::complex<double> *x, const int &incx, std::complex<double> *Y,
            const int &incy);

  void copy(const int &N, const double *x, const int &incx, double *Y, const int &incy);
  void copy(const int &N, const std::complex<double> *x, const int &incx, std::complex<double> *Y, const int &incy);

  double dot(const int &M, const double *x, const int &incx, const double *Y, const int &incy);
  //std::complex<double> dot (const int & M, const std::complex<double>* x, const int & incx, const std::complex<double>* Y, const int & incy) ;

  void gemm(char trans_a, char trans_b, const int &M, const int &N, const int &K, const double &alpha, const double *A, const int &LDA,
            const double *B, const int &LDB, const double &beta, double *C, const int &LDC);
  void gemm(char trans_a, char trans_b, const int &M, const int &N, const int &K, const std::complex<double> &alpha, const std::complex<double> *A,
            const int &LDA, const std::complex<double> *B, const int &LDB, const std::complex<double> &beta, std::complex<double> *C, const int &LDC);

  void gemv(char *trans, const int &M, const int &N, double &alpha, const double *A, int &LDA, const double *x, const int &incx, double &beta,
            double *Y, const int &incy);
  void gemv(char *trans, const int &M, const int &N, std::complex<double> &alpha, const std::complex<double> *A, int &LDA,
            const std::complex<double> *x, const int &incx, std::complex<double> &beta, std::complex<double> *Y, const int &incy);

  void ger(const int &M, const int &N, const double &alpha, const double *x, const int &incx, const double *Y, const int &incy, double *A,
           const int &LDA);
  void ger(const int &M, const int &N, const std::complex<double> &alpha, const std::complex<double> *x, const int &incx,
           const std::complex<double> *Y, const int &incy, std::complex<double> *A, const int &LDA);

  void scal(const int &M, const double &alpha, double *x, const int &incx);
  void scal(const int &M, const std::complex<double> &alpha, std::complex<double> *x, const int &incx);

  void swap(const int &N, double *x, const int &incx, double *Y, const int &incy);
  void swap(const int &N, std::complex<double> *x, const int &incx, std::complex<double> *Y, const int &incy);

} // namespace triqs::arrays::blas::f77

namespace triqs::arrays::lapack::f77 {

  void gelss(const int &M, const int &N, const int &NRHS, double *A, const int &LDA, double *B, const int &LDB, double *S, const double &RCOND,
             int &RANK, double *WORK, const int &LWORK, int &INFO);
  void gelss(const int &M, const int &N, const int &NRHS, std::complex<double> *A, const int &LDA, std::complex<double> *B, const int &LDB, double *S,
             const double &RCOND, int &RANK, std::complex<double> *WORK, const int &LWORK, double *RWORK, int &INFO);

  void gesvd(const char &JOBU, const char &JOBVT, const int &M, const int &N, double *A, const int &LDA, double *S, double *U, const int &LDU,
             double *VT, const int &LDVT, double *WORK, const int &LWORK, int &INFO);
  void gesvd(const char &JOBU, const char &JOBVT, const int &M, const int &N, std::complex<double> *A, const int &LDA, double *S,
             std::complex<double> *U, const int &LDU, std::complex<double> *VT, const int &LDVT, std::complex<double> *WORK, const int &LWORK,
             double *RWORK, int &INFO);

  void getrf(const int &M, const int &N, double *A, const int &LDA, int *ipiv, int &info);
  void getrf(const int &M, const int &N, std::complex<double> *A, const int &LDA, int *ipiv, int &info);

  void getri(const int &N, double *A, const int &LDA, int *ipiv, double *work, const int &lwork, int &info);
  void getri(const int &N, std::complex<double> *A, const int &LDA, int *ipiv, std::complex<double> *work, const int &lwork, int &info);

  void gtsv(const int &N, const int &NRHS, double *DL, double *D, double *DU, double *B, const int &LDB, int &info);
  void gtsv(const int &N, const int &NRHS, std::complex<double> *DL, std::complex<double> *D, std::complex<double> *DU, std::complex<double> *B,
            const int &LDB, int &info);

  void stev(char J, const int &N, double *D, double *E, double *Z, int ldz, double *work, int &info);

} // namespace triqs::arrays::lapack::f77
