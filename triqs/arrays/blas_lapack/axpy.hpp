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
#ifndef TRIQS_ARRAYS_BLAS_LAPACK_AXPY_H
#define TRIQS_ARRAYS_BLAS_LAPACK_AXPY_H
#include "./tools.hpp"
//#include "./qcache.hpp"

namespace triqs { namespace arrays { namespace blas { 

 namespace f77 { // overload

  extern "C" { 
   void TRIQS_FORTRAN_MANGLING(daxpy)(const int & N, const double & alpha, const double * x, const int& incx, double * y, const int& incy);
   void TRIQS_FORTRAN_MANGLING(zaxpy)(const int & N, const std::complex<double> & alpha, const std::complex<double> * x, const int& incx, std::complex<double> * y, const int& incy);
  }

  inline void axpy (const int & N, const double & alpha, const double* x, const int & incx, double* Y, const int & incy)  { 
   TRIQS_FORTRAN_MANGLING(daxpy)(N, alpha, x, incx, Y, incy);
  }
  inline void axpy (const int & N, const std::complex<double> & alpha, const std::complex<double>* x, const int & incx, std::complex<double>* Y, const int & incy)  { 
   TRIQS_FORTRAN_MANGLING(zaxpy)(N, alpha, x, incx, Y, incy);
  }
 }

 /**
  * Blas 1: copy
  */
 template< typename VTX, typename VTY> 
  typename std::enable_if< is_blas_lapack_type<typename VTX::value_type>::value && have_same_value_type< VTX, VTY>::value >::type 
  axpy (typename VTX::value_type const & alpha ,VTX const & X, VTY & Y) {
   static_assert( is_amv_value_or_view_class<VTX>::value, "blas1 bindings only take vector and vector_view");
   static_assert( is_amv_value_or_view_class<VTY>::value, "blas1 bindings only take vector and vector_view");
   //if (( X.size() != Y.size()) ) Y.resize(X.size()); 
   //const_qcache<VTX> Cx(X); // mettre la condition a la main
   //f77::copy(X.size(), Cx().data_start(), Cx().stride(), Y.data_start(), Y.stride());
   f77::axpy(X.size(), alpha, X.data_start(), X.stride(), Y.data_start(), Y.stride());
  }

}}}// namespace

#endif

