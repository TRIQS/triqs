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

} // namespace triqs::arrays::lapack
