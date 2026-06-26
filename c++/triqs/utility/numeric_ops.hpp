// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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

/**
 * @file
 * @brief Numeric helpers overloaded for various types.
 */

#pragma once

#include "./is_complex.hpp"

#include <cmath>
#include <complex>
#include <concepts>
#include <limits>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-math
   * @{
   */

  /**
   * @brief Exact zero check for integral values.
   * 
   * @tparam I Integral type.
   * @param x Value to test.
   * @return True if the given integral value is exactly zero, false otherwise.
   */
  template <std::integral I> bool is_zero(I const &x) { return x == 0; }

  /**
   * @brief Zero check for floating-point values.
   * 
   * @tparam T Floating-point type.
   * @param x Value to test.
   * @param tolerance Absolute tolerance \f$ \epsilon \f$.
   * @return True if \f$ |x| < \epsilon \f$, false otherwise.
   */
  template <std::floating_point T> bool is_zero(T const &x, T tolerance = 100 * std::numeric_limits<T>::epsilon()) { return std::abs(x) < tolerance; }

  /**
   * @brief Zero check for complex values.
   * 
   * @tparam T Value type.
   * @param z Value to test.
   * @param tolerance Absolute tolerance \f$ \epsilon \f$.
   * @return True if \f$ |\Re(z)| < \epsilon \f$ and \f$ |\Im(z)| < \epsilon \f$, false otherwise.
   */
  template <typename T> bool is_zero(std::complex<T> const &z, T tolerance = 100 * std::numeric_limits<T>::epsilon()) {
    return is_zero(std::real(z), tolerance) && is_zero(std::imag(z), tolerance);
  }

  /**
   * @brief Complex conjugate of an integral value.
   * 
   * @tparam I Integral type.
   * @param x Value to conjugate.
   * @return Value \f$ x \f$ unchanged.
   */
  template <std::integral I> I conj(I const &x) { return x; }

  /**
   * @brief Complex conjugate of a floating-point value.
   * 
   * @tparam T Floating-point type.
   * @param x Value to conjugate.
   * @return Value \f$ x \f$ unchanged.
   */
  template <std::floating_point T> T conj(T const &x) { return x; }

  /**
   * @brief Complex conjugate of a complex value.
   * 
   * @tparam Z Complex type.
   * @param z Value to conjugate.
   * @return \f$ z^* \f$.
   */
  template <typename Z>
    requires(triqs::is_complex<Z>::value)
  Z conj(Z const &z) {
    return std::conj(z);
  }

  /**
   * @brief Real part of an integral value.
   * 
   * @tparam I Integral type.
   * @param x Value to inspect.
   * @return Value \f$ x \f$ unchanged.
   */
  template <std::integral I> I real(I const &x) { return x; }

  /**
   * @brief Real part of a floating-point value.
   * 
   * @tparam T Floating-point type.
   * @param x Value to inspect.
   * @return Value \f$ x \f$ unchanged.
   */
  template <std::floating_point T> T real(T const &x) { return x; }

  /**
   * @brief Real part of a complex value.
   * 
   * @tparam Z Complex type.
   * @param z Value to inspect.
   * @return Real part of \f$ z \f$, i.e. \f$ \Re(z) \f$.
   */
  template <typename Z>
    requires(triqs::is_complex<Z>::value)
  Z real(Z const &z) {
    return std::real(z);
  }

  /**
   * @brief Imaginary part of an integral value.
   * 
   * @tparam I Integral type.
   * @param x Value to inspect.
   * @return Zero.
   */
  template <std::integral I> I imag([[maybe_unused]] I const &x) { return I{}; }

  /**
   * @brief Imaginary part of a floating-point value.
   * 
   * @tparam T Floating-point type.
   * @param x Value to inspect.
   * @return Zero.
   */
  template <std::floating_point T> T imag([[maybe_unused]] T const &x) { return T{}; }

  /**
   * @brief Imaginary part of a complex value.
   * 
   * @tparam Z Complex type.
   * @param z Value to inspect.
   * @return Imaginary part of \f$ z \f$, i.e. \f$ \Im(z) \f$.
   */
  template <typename Z>
    requires(triqs::is_complex<Z>::value)
  Z imag(Z const &z) {
    return std::imag(z);
  }

  /** @} */

} // namespace triqs::utility
