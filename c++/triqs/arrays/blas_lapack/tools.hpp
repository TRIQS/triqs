// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
//
// Authors: Philipp D, Olivier Parcollet, Nils Wentzell

#pragma once

#include <complex>
#include <type_traits>
#include <triqs/utility/fortran_mangling.hpp>

namespace triqs {
  namespace arrays {

    // a trait to detect all types for which blas/lapack bindings is defined
    // at the moment double and std::complex<double>
    // We don't need simple precision for the moment... To be added ?
    template <typename T> struct is_blas_lapack_type : std::false_type {};
    template <> struct is_blas_lapack_type<double> : std::true_type {};
    template <> struct is_blas_lapack_type<std::complex<double>> : std::true_type {};
    template <typename T> struct is_blas_lapack_type<const T> : is_blas_lapack_type<T> {};

    // FIXME : replace by one TRAIT 
    template<typename ...T> constexpr bool have_same_value_type_and_is_RC() { 
          return (std::is_same_v<T, double> and ...) or (std::is_same_v<T, std::complex<double>> and ...);
    }	  

    // true iif all T::value_type are the same
    template <typename... T> struct have_same_value_type;
    template <typename T0> struct have_same_value_type<T0> : std::true_type {};
    template <typename T0, typename T1, typename... T>
    struct have_same_value_type<T0, T1, T...>
       : std::integral_constant<
            bool,
            std::is_same<typename std::remove_cv<typename T0::value_type>::type, typename std::remove_cv<typename T1::value_type>::type>::value
               && have_same_value_type<T1, T...>::value> {};

    namespace blas_lapack_tools {

      template <typename MatrixType> char get_trans(MatrixType const &A, bool transpose) {
        return (A.memory_layout_is_fortran() ? (transpose ? 'T' : 'N') : (transpose ? 'N' : 'T'));
      }

      // returns the # of rows of the matrix *seen* as fortran matrix
      template <typename MatrixType> size_t get_n_rows(MatrixType const &A) { return (A.memory_layout_is_fortran() ? first_dim(A) : second_dim(A)); }
      // returns the # of cols of the matrix *seen* as fortran matrix
      template <typename MatrixType> size_t get_n_cols(MatrixType const &A) { return (A.memory_layout_is_fortran() ? second_dim(A) : first_dim(A)); }

      template <typename MatrixType> int get_ld(MatrixType const &A) { return A.indexmap().strides()[A.memory_layout_is_fortran() ? 1 : 0]; }

    } // namespace blas_lapack_tools
  }   // namespace arrays
} // namespace triqs
