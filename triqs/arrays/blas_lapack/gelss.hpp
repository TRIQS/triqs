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
#ifndef TRIQS_ARRAYS_BLAS_LAPACK_GELSS_H
#define TRIQS_ARRAYS_BLAS_LAPACK_GELSS_H
#include <complex>
#include "./tools.hpp"
#include "./qcache.hpp"

namespace triqs { namespace arrays { namespace lapack { 
 
 using namespace blas_lapack_tools;
 namespace f77 { // overload

  extern "C" { 
   void TRIQS_FORTRAN_MANGLING(dgelss) (const int &, const int &, const int &, double[], const int &, double[], const int &,
                                        double[], const double &, int &, double[], const int &, int &);
   void TRIQS_FORTRAN_MANGLING(zgelss) (const int &, const int &, const int &, std::complex<double>[], const int &, std::complex<double>[], const int &,
                                        double[], const double &, int &, std::complex<double>[], const int &, double[], int &);
  }

  inline void gelss (const int & M, const int & N, const int & NRHS, double* A, const int & LDA, double* B, const int & LDB,
                     double* S, const double & RCOND, int & RANK, double* WORK, const int & LWORK, int & INFO) {
    TRIQS_FORTRAN_MANGLING(dgelss)(M,N,NRHS,A,LDA,B,LDB,S,RCOND,RANK,WORK,LWORK,INFO);
  }

  inline void gelss (const int & M, const int & N, const int & NRHS, std::complex<double>* A, const int & LDA, std::complex<double>* B, const int & LDB,
                     double* S, const double & RCOND, int & RANK, std::complex<double>* WORK, const int & LWORK, double* RWORK, int & INFO) {
    TRIQS_FORTRAN_MANGLING(zgelss)(M,N,NRHS,A,LDA,B,LDB,S,RCOND,RANK,WORK,LWORK,RWORK,INFO);
  }

 }//namespace

 /**
  * Calls gelss on a matrix or view
  * Takes care of making temporary copies if necessary
  */
 template<typename MTA, typename MTB, typename VCS> 
  typename std::enable_if< 
             is_blas_lapack_type<typename MTA::value_type>::value &&
             is_blas_lapack_type<typename MTB::value_type>::value &&
             is_blas_lapack_type<typename VCS::value_type>::value &&
	     std::is_same<typename MTA::value_type, double>::value, int >::type //for double; driver for std::complex<double> needs to be added
  gelss (MTA & A, MTB & B, VCS & S, double rcond, int & rank, bool assert_fortran_order = true ) {
   if (assert_fortran_order && (A.memory_layout_is_c()|| B.memory_layout_is_c() )) TRIQS_RUNTIME_ERROR<< "matrix passed to getrf is not in Fortran order";

   reflexive_qcache<MTA> Ca(A);
   reflexive_qcache<MTB> Cb(B);

   auto dm = std::min(first_dim(Ca()), second_dim(Ca()));
   if (S.size() < dm) S.resize(dm); 

   reflexive_qcache<VCS> Cs(S);

   int info;
   int nrhs=get_n_cols(Cb());

   typename MTA::value_type work1[1];
   // first call to get the optimal lwork
   f77::gelss ( get_n_rows(Ca()), get_n_cols(Ca()), nrhs , Ca().data_start(), get_ld(Ca()),
                Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond, rank, work1, -1, info); 

   int lwork = r_round(work1[0]);
   arrays::vector<typename MTA::value_type> work(lwork); 

   f77::gelss ( get_n_rows(Ca()), get_n_cols(Ca()), nrhs , Ca().data_start(), get_ld(Ca()),
                Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond, rank, work.data_start(), lwork, info); 
   return info;
  }

}}}// namespace


#endif

