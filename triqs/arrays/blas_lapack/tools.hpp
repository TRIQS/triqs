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
#ifndef TRIQS_ARRAYS_IS_BLAS_LAPACK_TYPE_H
#define TRIQS_ARRAYS_IS_BLAS_LAPACK_TYPE_H 
#include <type_traits>
#include <triqs/utility/fortran_mangling.hpp>

namespace triqs { namespace arrays { 

  // a trait to detect all types for which blas/lapack bindings is defined
  // at the moment double and std::complex<double>
  // We don't need simple precision for the moment... To be added ?
  template<typename T> struct is_blas_lapack_type : std::false_type{};
  template<> struct is_blas_lapack_type <double> : std::true_type{};
  template<> struct is_blas_lapack_type <std::complex<double>> : std::true_type{};
  template<typename T> struct is_blas_lapack_type< const T> : is_blas_lapack_type<T>{};

  // true iif all T::value_type are the same
  template<typename ... T> struct have_same_value_type;
  template<typename T0> struct have_same_value_type<T0>: std::true_type{}; 
  template<typename T0, typename T1, typename ... T> struct have_same_value_type<T0,T1,T...>: 
   std::integral_constant<bool, 
   std::is_same< typename std::remove_cv<typename T0::value_type>::type, typename std::remove_cv<typename T1::value_type>::type> ::value 
   && have_same_value_type<T1,T...>::value> {}; 

  namespace blas_lapack_tools {

   template <typename MatrixType> char get_trans(MatrixType const & A, bool transpose) { 
    return (A.memory_layout_is_fortran() ? (transpose ? 'T' : 'N') : (transpose ? 'N' : 'T') );
   }

   // returns the # of rows of the matrix *seen* as fortran matrix
   template <typename MatrixType> size_t get_n_rows (MatrixType const & A) {
    return (A.memory_layout_is_fortran() ? first_dim(A) : second_dim(A));
   }
   // returns the # of cols of the matrix *seen* as fortran matrix
   template <typename MatrixType> size_t get_n_cols (MatrixType const & A) {
    return (A.memory_layout_is_fortran() ? second_dim(A) : first_dim(A));
   }

   template <typename MatrixType> int get_ld (MatrixType const & A) { 
    return A.indexmap().strides()[A.memory_layout_is_fortran() ? 1 : 0];
   }

  }
}}// namespace

#endif

