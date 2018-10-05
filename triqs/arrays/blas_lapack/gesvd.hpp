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
    void TRIQS_FORTRAN_MANGLING(dgesvd)(const char &, const char &, const int &, const int &, double[], const int &, double[], double[], const int &,
                                        double[], const int &, double[], const int &, int &);
    void TRIQS_FORTRAN_MANGLING(zgesvd)(const char &, const char &, const int &, const int &, std::complex<double>[], const int &, double[],
                                        std::complex<double>[], const int &, std::complex<double>[], const int &, std::complex<double>[], const int &,
                                        double[], int &);
    }

    inline void gesvd(const char &JOBU, const char &JOBVT, const int &M, const int &N, double *A, const int &LDA, double *S, double *U,
                      const int &LDU, double *VT, const int &LDVT, double *WORK, const int &LWORK, int &INFO) {
      TRIQS_FORTRAN_MANGLING(dgesvd)(JOBU, JOBVT, M, N, A, LDA, S, U, LDU, VT, LDVT, WORK, LWORK, INFO);
    }

    inline void gesvd(const char &JOBU, const char &JOBVT, const int &M, const int &N, std::complex<double> *A, const int &LDA, double *S,
                      std::complex<double> *U, const int &LDU, std::complex<double> *VT, const int &LDVT, std::complex<double> *WORK,
                      const int &LWORK, double *RWORK, int &INFO) {
      TRIQS_FORTRAN_MANGLING(zgesvd)(JOBU, JOBVT, M, N, A, LDA, S, U, LDU, VT, LDVT, WORK, LWORK, RWORK, INFO);
    }

  } // namespace f77

  /**
  * Calls gesvd on a matrix or view
  * Takes care of making temporary copies if necessary
  */
  template <typename MTA, typename VCS, typename MTU, typename MTVT>
  typename std::enable_if_t<is_blas_lapack_type<typename MTA::value_type>::value && is_blas_lapack_type<typename VCS::value_type>::value
                               && is_blas_lapack_type<typename MTU::value_type>::value && is_blas_lapack_type<typename MTVT::value_type>::value,
                            int>
  gesvd(MTA const &A, VCS &S, MTU &U, MTVT &VT) {

    int info;

    if (!A.memory_layout_is_fortran()){
      auto A_FL  = typename MTA::regular_type{A, FORTRAN_LAYOUT};
      info       = gesvd(A_FL, S, U, VT);
      return info;
    }
    
    if (!U.memory_layout_is_fortran()){
      auto U_FL  = typename MTU::regular_type{U, FORTRAN_LAYOUT};
      info       = gesvd(A, S, U_FL, VT);
      U()        = U_FL();
      return info;
    }

    if (!VT.memory_layout_is_fortran()) {
      auto VT_FL = typename MTVT::regular_type{VT, FORTRAN_LAYOUT};
      info       = gesvd(A, S, U, VT_FL);
      VT()       = VT_FL();
      return info;
    }

    // Copy A, since it is altered by gesvd
    typename MTA::regular_type _A{A, FORTRAN_LAYOUT};

    reflexive_qcache<MTA> Ca(_A);
    reflexive_qcache<VCS> Cs(S);
    reflexive_qcache<MTU> Cu(U);
    reflexive_qcache<MTVT> Cvt(VT);

    if constexpr (std::is_same<typename MTA::value_type, double>::value) {

      // first call to get the optimal lwork
      typename MTA::value_type work1[1];
      f77::gesvd('A', 'A', get_n_rows(Ca()), get_n_cols(Ca()), Ca().data_start(), get_ld(Ca()), Cs().data_start(), Cu().data_start(), get_ld(Cu()),
                 Cvt().data_start(), get_ld(Cvt()), work1, -1, info);

      int lwork = r_round(work1[0]);
      arrays::vector<typename MTA::value_type> work(lwork);

      f77::gesvd('A', 'A', get_n_rows(Ca()), get_n_cols(Ca()), Ca().data_start(), get_ld(Ca()), Cs().data_start(), Cu().data_start(), get_ld(Cu()),
                 Cvt().data_start(), get_ld(Cvt()), work.data_start(), lwork, info);

    } else if constexpr (std::is_same<typename MTA::value_type, std::complex<double>>::value) {

      auto rwork = array<double, 1>(5 * std::min(first_dim(Ca()), second_dim(Ca())));

      // first call to get the optimal lwork
      typename MTA::value_type work1[1];
      f77::gesvd('A', 'A', get_n_rows(Ca()), get_n_cols(Ca()), Ca().data_start(), get_ld(Ca()), Cs().data_start(), Cu().data_start(), get_ld(Cu()),
                 Cvt().data_start(), get_ld(Cvt()), work1, -1, rwork.data_start(), info);

      int lwork = r_round(work1[0]);
      arrays::vector<typename MTA::value_type> work(lwork);

      f77::gesvd('A', 'A', get_n_rows(Ca()), get_n_cols(Ca()), Ca().data_start(), get_ld(Ca()), Cs().data_start(), Cu().data_start(), get_ld(Cu()),
                 Cvt().data_start(), get_ld(Cvt()), work.data_start(), lwork, rwork.data_start(), info);

    } else {
      TRIQS_RUNTIME_ERROR << "Error in gesvd : only implemented for value_type double and std::complex<double>";
    }

    if (info) TRIQS_RUNTIME_ERROR << "Error in gesvd : info = " << info;
    return info;
  }
} // namespace triqs::arrays::lapack
