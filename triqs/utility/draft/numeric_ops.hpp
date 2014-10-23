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

using triqs::is_complex;

// Useful numeric operations which have to be implemented separately for different categories of builtin types.

//
// Zero value tests
//
template<typename T> // Integral types
typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value,bool>::type is_zero(T && x) {
 return x==0;
}

template<typename T> // Floating-point types
typename std::enable_if<std::is_floating_point<typename std::decay<T>::type>::value,bool>::type is_zero(T && x,
             typename std::decay<T>::type tolerance = 100*std::numeric_limits<typename std::decay<T>::type>::epsilon()) {
 return std::abs(x) < tolerance;
}

template<typename T> // std::complex
typename std::enable_if<is_complex<typename std::decay<T>::type>::value,bool>::type is_zero(T && x) {
 return is_zero(std::real(x)) && is_zero(std::imag(x));
}

//
// Complex conjugate
//
template<typename T> // Integral types
typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value,typename std::decay<T>::type>::type _conj(T && x) { return x; }

template<typename T> // Floating-point types
typename std::enable_if<std::is_floating_point<typename std::decay<T>::type>::value,typename std::decay<T>::type>::type _conj(T && x) { return x; }

template<typename T> // std::complex
typename std::enable_if<is_complex<typename std::decay<T>::type>::value,typename std::decay<T>::type>::type _conj(T && x) { return std::conj(x); }

}}