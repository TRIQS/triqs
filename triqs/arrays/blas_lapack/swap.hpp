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
#ifndef TRIQS_ARRAYS_BLAS_LAPACK_SWAP_H
#define TRIQS_ARRAYS_BLAS_LAPACK_SWAP_H
#include "./tools.hpp"

namespace triqs { namespace arrays { namespace blas { 

 namespace f77 { // overload

  extern "C" { 
   void TRIQS_FORTRAN_MANGLING(dswap)(const int & N, double * x, const int& incx, double * y, const int& incy);
   void TRIQS_FORTRAN_MANGLING(zswap)(const int & N, std::complex<double> * x, const int& incx, std::complex<double> * y, const int& incy);
  }

  inline void swap (const int & N, double* x, const int & incx, double* Y, const int & incy)  { 
   TRIQS_FORTRAN_MANGLING(dswap)(N, x, incx, Y, incy);
  }
  inline void swap (const int & N, std::complex<double>* x, const int & incx, std::complex<double>* Y, const int & incy)  { 
   TRIQS_FORTRAN_MANGLING(zswap)(N, x, incx, Y, incy);
  }
 }

 /**
  * Blas 1: swap
  */
 template< typename VTX, typename VTY> 
  typename std::enable_if< is_blas_lapack_type<typename VTX::value_type>::value && have_same_value_type< VTX, VTY>::value >::type 
  swap (VTX & X, VTY & Y) { 
   static_assert( is_amv_value_or_view_class<VTX>::value, "blas1 bindings only take vector and vector_view");
   static_assert( is_amv_value_or_view_class<VTY>::value, "blas1 bindings only take vector and vector_view");
   if (( X.size() != Y.size()) ) TRIQS_RUNTIME_ERROR << "Dimension mismatch in dot : X : "<<X().shape()<<" and Y : "<<Y().shape();
   f77::swap(X.size(), X.data_start(), X.stride(), Y.data_start(), Y.stride());
  }

}}}// namespace

#endif

