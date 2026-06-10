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
//   2. Small reusable helpers for a column-pivoted QR (nda::lapack::geqp3, A P = Q R), mirroring the
//      w2dynamics CT-QMC solver (src/ctqmc_fortran/QRDecomposition.tmpl.F90, qr_det):
//      householder_det_factor (det of a reflector from its tau) and perm_sign (det of the column pivot).
//      The determinant/inverse that use them live in det_manip_qr itself, which caches Q, R and the
//      pivot as member data.
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

    // Generate a plane (Givens) rotation [[c, s], [-conj(s), c]] mapping (f, g) -> (r, 0). (LAPACK lartg)
    void lartg(double f, double g, double &c, double &s, double &r);
    void lartg(std::complex<double> f, std::complex<double> g, double &c, std::complex<double> &s, std::complex<double> &r);

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

  /// Sign (determinant) of a permutation given by its 0-based images p (p is a permutation of 0..n-1).
  inline int permutation_sign(nda::vector<int> const &p) {
    long const n = p.size();
    std::vector<char> seen(n, 0);
    int sign = 1;
    for (long i = 0; i < n; ++i) {
      if (seen[i]) continue;
      long j = i, len = 0;
      while (!seen[j]) {
        seen[j] = 1;
        j       = p(j);
        ++len;
      }
      if (len % 2 == 0) sign = -sign; // a cycle of even length is an odd permutation
    }
    return sign;
  }

  // ---------------------------------------------------------------------------------------------------
  //  Givens-rotation helpers for the incremental QR update/downdate, ported from w2dynamics
  //  QRDecomposition.tmpl.F90. A plane rotation is the 2x2 matrix [[c, s], [-conj(s), c]] (c real).
  // ---------------------------------------------------------------------------------------------------

  template <typename V> V qr_conj(V x) {
    if constexpr (nda::is_complex_v<V>)
      return std::conj(x);
    else
      return x;
  }

  template <typename V> struct plane_rotation {
    decltype(std::abs(V{})) c; // cosine (real)
    V s;                       // sine
  };

  /// Generate a Givens rotation G that maps (f, g) -> (r, 0), i.e. G [f; g] = [r; 0]; returns G and sets r.
  template <typename V> plane_rotation<V> make_givens(V f, V g, V &r) {
    plane_rotation<V> rot;
    nda::lapack::f77::lartg(f, g, rot.c, rot.s, r);
    return rot;
  }

  /// Apply G to rows (p, q) of M over columns [c0, c1):  [M(p,:); M(q,:)] <- G [M(p,:); M(q,:)].
  template <typename M, typename V> void apply_rows(M &m, long p, long q, plane_rotation<V> const &rot, long c0, long c1) {
    for (long t = c0; t < c1; ++t) {
      V const a = m(p, t), b = m(q, t);
      m(p, t) = rot.c * a + rot.s * b;
      m(q, t) = -qr_conj(rot.s) * a + rot.c * b;
    }
  }

  /// Apply the adjoint G^H to columns (p, q) of Q (all `nrow` rows):  Q <- Q G^H. Pairs with apply_rows so
  /// that (Q G^H)(G M) = Q M.
  template <typename M, typename V> void apply_cols_adjoint(M &q_mat, long nrow, long p, long q, plane_rotation<V> const &rot) {
    for (long r = 0; r < nrow; ++r) {
      V const a = q_mat(r, p), b = q_mat(r, q);
      q_mat(r, p) = rot.c * a + qr_conj(rot.s) * b;
      q_mat(r, q) = -rot.s * a + rot.c * b;
    }
  }

  /// Apply G to columns (p, q) of Q (all `nrow` rows):  Q <- Q G  (right multiplication). Used to reduce a
  /// chosen row of Q; pairs with apply_rows_adjoint so that (Q G)(G^H M) = Q M.
  template <typename M, typename V> void apply_cols(M &q_mat, long nrow, long p, long q, plane_rotation<V> const &rot) {
    for (long r = 0; r < nrow; ++r) {
      V const a = q_mat(r, p), b = q_mat(r, q);
      q_mat(r, p) = rot.c * a + rot.s * b;
      q_mat(r, q) = -qr_conj(rot.s) * a + rot.c * b;
    }
  }

  /// Apply the adjoint G^H to rows (p, q) of M over columns [c0, c1):  M <- G^H M.
  template <typename M, typename V> void apply_rows_adjoint(M &m, long p, long q, plane_rotation<V> const &rot, long c0, long c1) {
    for (long t = c0; t < c1; ++t) {
      V const a = m(p, t), b = m(q, t);
      m(p, t) = rot.c * a + qr_conj(rot.s) * b;
      m(q, t) = -rot.s * a + rot.c * b;
    }
  }

} // namespace triqs::det_manip
