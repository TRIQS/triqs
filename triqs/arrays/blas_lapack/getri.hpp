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
#ifndef TRIQS_ARRAYS_BLAS_LAPACK_GETRI_H
#define TRIQS_ARRAYS_BLAS_LAPACK_GETRI_H
#include <complex>
#include "./tools.hpp"
#include "./qcache.hpp"

namespace triqs { namespace arrays { namespace lapack { 

 using namespace blas_lapack_tools;
 namespace f77 { // overload

  extern "C" { 
  void TRIQS_FORTRAN_MANGLING(dgetri) (const int & , double *, const int & ,int *,double *,const int & ,int & );
  void TRIQS_FORTRAN_MANGLING(zgetri) (const int & , std::complex<double> *, const int & ,int *,std::complex<double> *,const int & ,int & );
   }

  inline void getri (const int & N, double* A, const int & LDA, int * ipiv, double * work, const int & lwork, int & info) { 
   TRIQS_FORTRAN_MANGLING(dgetri)(N,A,LDA,ipiv,work, lwork, info);
  }
  inline void getri (const int & N, std::complex<double>* A, const int & LDA, int * ipiv, std::complex<double> * work, const int & lwork, int & info) { 
   TRIQS_FORTRAN_MANGLING(zgetri)(N,A,LDA,ipiv,work, lwork, info);
  }
 }

 inline size_t r_round(double x) { return round(x);}
 inline size_t r_round(std::complex<double> x) { return round(real(x));}
 /**
  * Calls getri on a matrix or view
  * Takes care of making temporary copies if necessary
  */
 template<typename MT> 
  typename std::enable_if< is_blas_lapack_type<typename MT::value_type>::value, int >::type 
  getri (MT & A, arrays::vector<int> & ipiv) { 
   //getri (MT & A, arrays::vector<int> & ipiv, arrays::vector<typename MT::value_type> & work ) { 
   reflexive_qcache<MT> Ca(A);
   auto dm = std::min(Ca().dim0(), Ca().dim1());
   if (ipiv.size() < dm) TRIQS_RUNTIME_ERROR << "getri : error in ipiv size : found "<<ipiv.size()<< " while it should be at least" << dm; 
   int info;
   typename MT::value_type work1[2];
   // first call to get the optimal lwork
   f77::getri ( get_n_rows(Ca()), Ca().data_start(), get_ld(Ca()), ipiv.data_start(), work1, -1, info); 
   int lwork = r_round(work1[0]);
   //std::cerr << " Optimisation lwork "<< lwork<< std::endl;
   arrays::vector<typename MT::value_type> work(lwork); 
   f77::getri ( get_n_rows(Ca()), Ca().data_start(), get_ld(Ca()), ipiv.data_start(), work.data_start(), lwork, info); 
   return info;
  }
  }}}// namespace

#endif

