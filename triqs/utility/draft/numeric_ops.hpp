/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by I. Krivenko
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
#include <limits>
#include <type_traits>
#include <triqs/utility/is_complex.hpp>

namespace triqs {
namespace utility {

// Useful numeric operations which have to be implemented separately for different categories of builtin types.
// Support for user-defined types can be added through specializations of struct numeric_ops
template<typename T>
struct numeric_ops {
 
 //
 // Zero value tests
 //
 template<typename U = T> // Integral types
 static bool is_zero(typename std::enable_if<std::is_integral<U>::value,U>::type x) { return x==0; }
 
 template<typename U = T> // Floating-point types
 static bool is_zero(typename std::enable_if<std::is_floating_point<U>::value,U>::type x,
                     U tolerance = 100*std::numeric_limits<T>::epsilon()) {
  return std::abs(x) < tolerance;
 }
 
 template<typename U = T> // std::complex
 static bool is_zero(typename std::enable_if<triqs::is_complex<U>::value,U>::type x) {
  using r_t = typename T::value_type;
  return numeric_ops<r_t>::is_zero(std::real(x)) && numeric_ops<r_t>::is_zero(std::imag(x));
 }
 
 //
 // Complex conjugate
 //
 template<typename U = T> // std::complex
 static U conj(typename std::enable_if<triqs::is_complex<U>::value,U>::type x) { return std::conj(x); }
 
 // All other types are considered real by default
 template<typename U = T>
 static U conj(typename std::enable_if<!triqs::is_complex<U>::value,U>::type x) { return x; }
};

}}