// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
// Copyright (c) 2014-2016 Igor Krivenko
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
// Authors: Igor Krivenko, Nils Wentzell

#pragma once

#include <complex>
#include <limits>
#include <type_traits>
#include <triqs/utility/is_complex.hpp>

namespace triqs {
  namespace utility {

    // Useful numeric operations which have to be implemented separately for different categories of builtin types.

    //
    // Zero value tests
    //
    template <typename T> // Integral types
    std::enable_if_t<std::is_integral<T>::value, bool> is_zero(T const &x) {
      return x == 0;
    }

    template <typename T> // Floating-point types
    std::enable_if_t<std::is_floating_point<T>::value, bool> is_zero(T const &x, T tolerance = 100 * std::numeric_limits<T>::epsilon()) {
      return std::abs(x) < tolerance;
    }

    template <typename value_t> // std::complex
    bool is_zero(std::complex<value_t> const &x, value_t tolerance = 100 * std::numeric_limits<value_t>::epsilon()) {
      return is_zero(std::real(x), tolerance) && is_zero(std::imag(x), tolerance);
    }

    //
    // Complex conjugate
    //
    template <typename T> // Integral types
    std::enable_if_t<std::is_integral<T>::value, T> conj(T const &x) {
      return x;
    }

    template <typename T> // Floating-point types
    std::enable_if_t<std::is_floating_point<T>::value, T> conj(T const &x) {
      return x;
    }

    template <typename T> // std::complex
    std::enable_if_t<triqs::is_complex<T>::value, T> conj(T const &x) {
      return std::conj(x);
    }

    //
    // Real part
    //
    template <typename T> // Integral types
    std::enable_if_t<std::is_integral<T>::value, T> real(T const &x) {
      return x;
    }

    template <typename T> // Floating-point types
    std::enable_if_t<std::is_floating_point<T>::value, T> real(T const &x) {
      return x;
    }

    template <typename T> // std::complex
    std::enable_if_t<triqs::is_complex<T>::value, T> real(T const &x) {
      return std::real(x);
    }

    //
    // Imaginary part
    //
    template <typename T> // Integral types
    std::enable_if_t<std::is_integral<T>::value, T> imag(T const &x) {
      return T{};
    }

    template <typename T> // Floating-point types
    std::enable_if_t<std::is_floating_point<T>::value, T> imag(T const &x) {
      return T{};
    }

    template <typename T> // std::complex
    std::enable_if_t<triqs::is_complex<T>::value, T> imag(T const &x) {
      return std::imag(x);
    }

  } // namespace utility
} // namespace triqs
