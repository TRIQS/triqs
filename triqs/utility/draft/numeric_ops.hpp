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
#include <triqs/utility/c14.hpp>
#include <triqs/utility/is_complex.hpp>

namespace triqs {
namespace utility {

// Useful numeric operations which have to be implemented separately for different categories of builtin types.

//
// Zero value tests
//
template<typename T> // Integral types
std14::enable_if_t<std::is_integral<T>::value,bool> is_zero(T const& x) {
 return x==0;
}

template<typename T> // Floating-point types
std14::enable_if_t<std::is_floating_point<T>::value,bool> is_zero(T const& x, T tolerance = 100*std::numeric_limits<T>::epsilon()) {
 return std::abs(x) < tolerance;
}

template<typename T> // std::complex
std14::enable_if_t<triqs::is_complex<T>::value,bool> is_zero(T const& x) {
 return is_zero(std::real(x)) && is_zero(std::imag(x));
}

//
// Complex conjugate
//
template<typename T> // Integral types
std14::enable_if_t<std::is_integral<T>::value,T> _conj(T const& x) { return x; }

template<typename T> // Floating-point types
std14::enable_if_t<std::is_floating_point<T>::value,T> _conj(T const& x) { return x; }

template<typename T> // std::complex
std14::enable_if_t<triqs::is_complex<T>::value,T> _conj(T const& x) { return std::conj(x); }

}}
