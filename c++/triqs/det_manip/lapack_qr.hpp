// Copyright (c) 2024 Simons Foundation
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

// ---------------------------------------------------------------------------------------------------
//
//  LAPACK QR helpers for det_manip_qr.
//
//  This header collects two things:
//
//   1. An nda-style wrapper for the LAPACK `trtrs` (triangular solve) routine, which nda does not yet
//      provide. It is written in the style of nda/lapack/geqp3.hpp so that it can be migrated into nda
//      verbatim later (move the `nda::lapack` part into nda/lapack/trtrs.hpp + interface/cxx_interface).
//
//   2. Two higher-level helpers, qr_determinant and qr_inverse, built on the column-pivoted QR
//      (nda::lapack::geqp3, A P = Q R) already provided by nda, mirroring the technique used by the
//      w2dynamics CT-QMC solver (src/ctqmc_fortran/QRDecomposition.tmpl.F90, qr_det). These are
//      application logic (not pure LAPACK), so they live in triqs::det_manip.
//
// ---------------------------------------------------------------------------------------------------

#pragma once

#include <nda/nda.hpp>
#include <nda/lapack/geqp3.hpp>
#include <nda/lapack/orgqr.hpp>
#include <nda/lapack/ungqr.hpp>

#include <complex>
#include <vector>

namespace nda::lapack {

  namespace f77 {

    // Defined in det_manip/lapack_qr.cpp (compiled into libtriqs, which links LAPACK via nda::nda_c)
    // so that the raw LAPACK symbol is resolved inside the library rather than in every caller's TU.
    // Mirrors nda's f77 layer (interface/cxx_interface.{hpp,cpp}); migrate there together with trtrs.
    void trtrs(char uplo, char trans, char diag, int n, int nrhs, double const *a, int lda, double *b, int ldb, int &info);
    void trtrs(char uplo, char trans, char diag, int n, int nrhs, std::complex<double> const *a, int lda, std::complex<double> *b, int ldb, int &info);

  } // namespace f77

  /**
   * @ingroup linalg_lapack
   * @brief Interface to the LAPACK `trtrs` routine: solve a triangular system op(A) X = B (B overwritten).
   * @param uplo  'U' if A is upper triangular, 'L' if lower triangular.
   * @param trans 'N' (op = A), 'T' (op = A^T) or 'C' (op = A^H).
   * @param diag  'N' if A is non-unit triangular, 'U' if unit triangular.
   */
  template <MemoryMatrix A, MemoryMatrix B>
    requires(have_same_value_type_v<A, B> and is_blas_lapack_v<get_value_t<A>>)
  int trtrs(char uplo, char trans, char diag, A const &a, B &&b) {
    static_assert(has_F_layout<A>, "Error in nda::lapack::trtrs: A must have Fortran layout");
    static_assert(has_F_layout<std::remove_reference_t<B>>, "Error in nda::lapack::trtrs: B must have Fortran layout");

    EXPECTS(a.shape()[0] == a.shape()[1]);
    EXPECTS(b.extent(0) == a.shape()[0]);
    EXPECTS(a.indexmap().min_stride() == 1);
    EXPECTS(b.indexmap().min_stride() == 1);

    int info = 0;
    f77::trtrs(uplo, trans, diag, static_cast<int>(a.shape()[0]), static_cast<int>(b.shape()[1]), a.data(), get_ld(a), b.data(), get_ld(b), info);
    return info;
  }

} // namespace nda::lapack

namespace triqs::det_manip {

  /// Determinant of a single Householder reflector H = I - tau v v^H given its tau factor.
  /// (LAPACK convention, cf. w2dynamics QRDecomposition.tmpl.F90:113-138.)
  template <typename V> V householder_det_factor(V tau) {
    if (tau == V{0}) return V{1};
    if constexpr (nda::is_complex_v<V>) {
      V const s = tau / std::abs(tau);
      return -s * s;
    } else {
      return V{-1};
    }
  }

  /// Sign (determinant) of the column-pivot permutation returned by geqp3 (1-based jpvt).
  inline int perm_sign(nda::vector<int> const &jpvt) {
    long const n = jpvt.size();
    std::vector<char> seen(n, 0);
    int sign = 1;
    for (long i = 0; i < n; ++i) {
      if (seen[i]) continue;
      long j = i, len = 0;
      while (!seen[j]) {
        seen[j] = 1;
        j       = jpvt(j) - 1; // jpvt is 1-based
        ++len;
      }
      if (len % 2 == 0) sign = -sign; // a cycle of even length is an odd permutation
    }
    return sign;
  }

  /**
   * Determinant of an N x N matrix from a column-pivoted QR factorization (A P = Q R):
   *
   *     det(A) = det(P) * prod_i R(i,i) * prod_i det(H_i)
   *
   * computed with nda::lapack::geqp3 (= the pivoted QR used by w2dynamics). Orthogonal transforms have a
   * bounded growth factor, so this determinant is backward stable and returns an exact zero for a singular
   * matrix.
   */
  template <typename V> V qr_determinant(nda::matrix_const_view<V> M) {
    long const n = M.extent(0);
    if (n == 0) return V{1};

    nda::matrix<V, nda::F_layout> a{M}; // geqp3 needs Fortran layout
    nda::vector<int> jpvt(n);
    jpvt() = 0; // all columns free
    nda::vector<V> tau(n);
    nda::lapack::geqp3(a, jpvt, tau);

    V det = 1;
    for (long i = 0; i < n; ++i) det *= a(i, i) * householder_det_factor<V>(tau(i));
    return det * V(perm_sign(jpvt));
  }

  /**
   * Inverse of an N x N matrix from a column-pivoted QR factorization:
   *
   *     A P = Q R   ==>   A^{-1} = P R^{-1} Q^H
   *
   * R^{-1} Q^H is obtained with a triangular solve (nda::lapack::trtrs); Q is formed with orgqr/ungqr;
   * the row permutation P is applied from jpvt. (cf. w2dynamics inverse_matrix_from_qr.)
   */
  template <typename V> nda::matrix<V> qr_inverse(nda::matrix_const_view<V> M) {
    long const n = M.extent(0);
    nda::matrix<V> inv(n, n);
    if (n == 0) return inv;

    nda::matrix<V, nda::F_layout> a{M};
    nda::vector<int> jpvt(n);
    jpvt() = 0;
    nda::vector<V> tau(n);
    nda::lapack::geqp3(a, jpvt, tau);

    // Extract R (upper triangle) before geqp3's reflectors get overwritten by orgqr/ungqr.
    nda::matrix<V, nda::F_layout> r(n, n);
    r() = 0;
    for (long j = 0; j < n; ++j)
      for (long i = 0; i <= j; ++i) r(i, j) = a(i, j);

    // Form Q in place (a now holds Q).
    if constexpr (nda::is_complex_v<V>)
      nda::lapack::ungqr(a, tau);
    else
      nda::lapack::orgqr(a, tau);

    // qh = Q^H
    nda::matrix<V, nda::F_layout> qh(n, n);
    for (long i = 0; i < n; ++i)
      for (long j = 0; j < n; ++j) {
        if constexpr (nda::is_complex_v<V>)
          qh(i, j) = std::conj(a(j, i));
        else
          qh(i, j) = a(j, i);
      }

    // Solve R X = Q^H  -> qh holds X = R^{-1} Q^H
    nda::lapack::trtrs('U', 'N', 'N', r, qh);

    // A^{-1} = P X : row jpvt(k)-1 of A^{-1} is row k of X
    for (long k = 0; k < n; ++k)
      for (long j = 0; j < n; ++j) inv(jpvt(k) - 1, j) = qh(k, j);

    return inv;
  }

} // namespace triqs::det_manip
