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

      auto rwork = array<double, 1>{dm, FORTRAN_LAYOUT};

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

  struct gelss_cache {

    private:
    size_t M, N;
    matrix<value_type> V_x_InvS_x_UT;

    public:
    vector<double> S_vec;

    gelss_cache(matrix_const_view<value_type> _A) : M{get_n_rows(_A)}, N{get_n_cols(_A)}, V_x_InvS_x_UT{N, M}, S_vec(std::min(M, N)) {

      matrix<value_type> A{_A, FORTRAN_LAYOUT};
      matrix<value_type> U{M, M, FORTRAN_LAYOUT};
      matrix<value_type> VT{N, N, FORTRAN_LAYOUT};

      lapack::gesvd(A, S_vec, U, VT);

      matrix<double> S_inv{N, M, FORTRAN_LAYOUT};
      S_inv() = 0.;
      for (int i : range(std::min(M, N))) S_inv(i, i) = 1.0 / S_vec(i);
      V_x_InvS_x_UT = VT.transpose() * S_inv * U.transpose();
    }

    matrix<value_type> operator()(matrix_const_view<value_type> B) const { return V_x_InvS_x_UT * B; }
  };

} // namespace triqs::arrays::lapack
