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
#pragma once

#include <complex>
#include "./tools.hpp"
#include "./qcache.hpp"
#include "./gesvd.hpp"

namespace triqs::arrays::lapack {

  using namespace blas_lapack_tools;
  namespace f77 { // overload

    extern "C" {
    void TRIQS_FORTRAN_MANGLING(dgelss)(const int &, const int &, const int &, double[], const int &, double[], const int &, double[], const double &,
                                        int &, double[], const int &, int &);
    void TRIQS_FORTRAN_MANGLING(zgelss)(const int &, const int &, const int &, std::complex<double>[], const int &, std::complex<double>[],
                                        const int &, double[], const double &, int &, std::complex<double>[], const int &, double[], int &);
    }

    inline void gelss(const int &M, const int &N, const int &NRHS, double *A, const int &LDA, double *B, const int &LDB, double *S,
                      const double &RCOND, int &RANK, double *WORK, const int &LWORK, int &INFO) {
      TRIQS_FORTRAN_MANGLING(dgelss)(M, N, NRHS, A, LDA, B, LDB, S, RCOND, RANK, WORK, LWORK, INFO);
    }

    inline void gelss(const int &M, const int &N, const int &NRHS, std::complex<double> *A, const int &LDA, std::complex<double> *B, const int &LDB,
                      double *S, const double &RCOND, int &RANK, std::complex<double> *WORK, const int &LWORK, double *RWORK, int &INFO) {
      TRIQS_FORTRAN_MANGLING(zgelss)(M, N, NRHS, A, LDA, B, LDB, S, RCOND, RANK, WORK, LWORK, RWORK, INFO);
    }

  } // namespace f77

  /**
  * Calls gelss on a matrix or view
  * Takes care of making temporary copies if necessary
  */
  template <typename MTA, typename MTB, typename VCS>
  typename std::enable_if_t<is_blas_lapack_type<typename MTA::value_type>::value && is_blas_lapack_type<typename MTB::value_type>::value
                               && is_blas_lapack_type<typename VCS::value_type>::value,
                            int>
  gelss(MTA A, MTB &B, VCS &S, double rcond, int &rank) {
    if (A.memory_layout_is_c() || B.memory_layout_is_c()) TRIQS_RUNTIME_ERROR << "matrix passed to gelss is not in Fortran order";

    reflexive_qcache<MTA> Ca(A);
    reflexive_qcache<MTB> Cb(B);

    auto dm = std::min(first_dim(Ca()), second_dim(Ca()));
    if (S.size() < dm) S.resize(dm);

    reflexive_qcache<VCS> Cs(S);

    int info;
    int nrhs = get_n_cols(Cb());

    if constexpr (std::is_same<typename MTA::value_type, double>::value) {

      // first call to get the optimal lwork
      typename MTA::value_type work1[1];
      f77::gelss(get_n_rows(Ca()), get_n_cols(Ca()), nrhs, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond,
                 rank, work1, -1, info);

      int lwork = r_round(work1[0]);
      arrays::vector<typename MTA::value_type> work(lwork);

      f77::gelss(get_n_rows(Ca()), get_n_cols(Ca()), nrhs, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond,
                 rank, work.data_start(), lwork, info);

    } else if constexpr (std::is_same<typename MTA::value_type, std::complex<double>>::value) {

      auto rwork = array<double, 1>(5*dm);

      // first call to get the optimal lwork
      typename MTA::value_type work1[1];
      f77::gelss(get_n_rows(Ca()), get_n_cols(Ca()), nrhs, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond,
                 rank, work1, -1, rwork.data_start(), info);

      int lwork = r_round(work1[0]);
      arrays::vector<typename MTA::value_type> work(lwork);

      f77::gelss(get_n_rows(Ca()), get_n_cols(Ca()), nrhs, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond,
                 rank, work.data_start(), lwork, rwork.data_start(), info);
    } else {
      TRIQS_RUNTIME_ERROR << "Error in gelss : only implemented for value_type double and std::complex<double>";
    }

    if (info) TRIQS_RUNTIME_ERROR << "Error in gelss : info = " << info;
    return info;
  }

  template <typename value_type> struct gelss_cache {

    // cf. Notation in https://math.stackexchange.com/questions/772039/how-does-the-svd-solve-the-least-squares-problem

    private:
    // True if M == N
    bool is_square;

    // The matrix V * Diag(S_vec)^{-1} * UT for the least square procedure
    matrix<value_type> V_x_InvS_x_UT;

    // The part of UT fixing the error of the LLS
    matrix<value_type> UT_NULL;

    // Vector containing the singular values
    vector<double> _S_vec;

    public:
    vector<double> const &S_vec() const { return _S_vec; }

    gelss_cache(matrix_const_view<value_type> _A) : _S_vec(std::min(first_dim(_A), second_dim(_A))) {

      if (second_dim(_A) > first_dim(_A))
        TRIQS_RUNTIME_ERROR << "ERROR: Matrix A for linear least square procedure cannot have more columns than rows";

      matrix<value_type> A(_A, FORTRAN_LAYOUT);

      size_t M = get_n_rows(A);
      size_t N = get_n_cols(A);
      is_square = (M == N);

      matrix<value_type> U{M, M, FORTRAN_LAYOUT};
      matrix<value_type> VT{N, N, FORTRAN_LAYOUT};

      // Calculate the SVD A = U * Diag(S_vec) * VT
      lapack::gesvd(A, _S_vec, U, VT);

      // Calculate the matrix V * Diag(S_vec)^{-1} * UT for the least square procedure
      matrix<double> S_inv{N, M, FORTRAN_LAYOUT};
      S_inv() = 0.;
      for (int i : range(std::min(M, N))) S_inv(i, i) = 1.0 / _S_vec(i);
      V_x_InvS_x_UT = VT.transpose() * S_inv * U.transpose();

      // Read off U_Null for defining the error of the least square procedure
      UT_NULL = U.transpose()(range(N, M), range(M));
    }

    std::pair<matrix<value_type>, double> operator()(matrix_const_view<value_type> B) const {
      return std::make_pair(V_x_InvS_x_UT * B, is_square ? 0.0 : frobenius_norm(UT_NULL * B));
    }
  };

} // namespace triqs::arrays::lapack
