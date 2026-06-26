// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
// Copyright (c) 2015 Igor Krivenko
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a type that decides at runtime whether it is real or complex.
 */

#pragma once

#include "./exceptions.hpp"
#include "./numeric_ops.hpp"

#include <cmath>
#include <complex>
#include <limits>
#include <ostream>
#include <utility>

namespace triqs::utility {

  /**
   * @addtogroup triqs-utility-math
   * @{
   */

  /**
   * @brief Type that can represent either a real or a complex number.
   *
   * @details Internally stores a `std::complex<double>` and a `bool` flag indicating whether the imaginary part
   * is known to be zero.
   *
   * Arithmetic operations (`+`, `-`, `*`, `/`) and compound assignment operations (`+=`, `-=`, `*=`, `/=`) are provided
   * for all combinations of `real_or_complex`, `std::complex<double>` and `double` operands. The result is real if and
   * only if both operands are real.
   *
   * Once the value is considered complex, conversion back to real throws.
   */
  class real_or_complex {
    bool _is_real = true;
    std::complex<double> _x;

    public:
    /// Default construct a real value initialized to zero.
    real_or_complex() : _x(0) {}

    /**
     * @brief Construct a real value.
     * @param x Real value to store.
     */
    real_or_complex(double x) : _x(x) {}

    /**
     * @brief Construct a complex value.
     * @param x Complex value to store.
     */
    real_or_complex(std::complex<double> x) : _is_real(false), _x(std::move(x)) {}

    /**
     * @brief Whether the stored value is real.
     * @return True if the stored value is real, false if it is complex.
     */
    bool is_real() const { return _is_real; }

    /**
     * @brief Explicit conversion to `std::complex<double>`.
     * @return The stored value as a complex number.
     */
    explicit operator std::complex<double>() const { return _x; }

    /**
     * @brief Explicit conversion to `double`.
     * @details It throws a triqs::runtime_error if the stored value is complex.
     * @return The real part of the stored value.
     */
    explicit operator double() const {
      if (!_is_real) TRIQS_RUNTIME_ERROR << "Logic error : the number is not real, it is complex";
      return real(_x);
    }

    /**
     * @brief Unary minus operator for a utility::real_or_complex value.
     *
     * @param a Value to negate.
     * @return Negated value.
     */
    friend real_or_complex operator-(real_or_complex a) {
      a._x = -a._x;
      return a;
    }

    /**
     * @brief Real part of a utility::real_or_complex value.
     *
     * @param x Value to inspect.
     * @return The real part.
     */
    friend double real(real_or_complex const &x) { return real(x._x); }

    /**
     * @brief Imaginary part of a utility::real_or_complex value.
     *
     * @param x Value to inspect.
     * @return The imaginary part.
     */
    friend double imag(real_or_complex const &x) { return imag(x._x); }

    /**
     * @brief Absolute value of a utility::real_or_complex value.
     *
     * @param x Value to inspect.
     * @return The absolute value.
     */
    friend double abs(real_or_complex const &x) { return std::abs(x._x); }

    /**
     * @brief Complex conjugate of a utility::real_or_complex value.
     *
     * @param x Value to conjugate.
     * @return The complex conjugate.
     */
    friend real_or_complex conj(real_or_complex x) {
      x._x = std::conj(x._x);
      return x;
    }

    /**
     * @brief Zero check for a utility::real_or_complex value.
     *
     * @param x Value to test.
     * @param tolerance Absolute tolerance \f$ \epsilon \f$.
     * @return True if the absolute values of real and imaginary parts are both less than \f$ \epsilon \f$, false
     * otherwise.
     */
    friend bool is_zero(real_or_complex const &x, double tolerance = 100 * std::numeric_limits<double>::epsilon()) {
      using triqs::utility::is_zero;
      if (x.is_real()) return is_zero(double(x), tolerance);
      return is_zero(x._x, tolerance);
    }

    /**
     * @brief Write a utility::real_or_complex value to an output stream.
     *
     * @param out Output stream to write to.
     * @param x Value to write.
     * @return Reference to the output stream.
     */
    friend std::ostream &operator<<(std::ostream &out, real_or_complex const &x) {
      if (x._is_real)
        out << double(x);
      else {
        out << '(' << x._x.real();
        if (not std::signbit(x._x.imag())) out << '+';
        out << x._x.imag() << "j)";
      }
      return out;
    }

    /**
     * @brief Equal-to operator compares the underlying complex values exactly.
     *
     * @param x Value to compare against.
     * @return True if the underlying complex values are exactly equal, false otherwise.
     */
    bool operator==(real_or_complex const &x) const { return abs(_x - x._x) == 0; }

    /**
     * @brief Not-equal-to operator compares the underlying complex values exactly.
     *
     * @param x Value to compare against.
     * @return True if the underlying complex values differ, false otherwise.
     */
    bool operator!=(real_or_complex const &x) const { return !operator==(x); }

    // Compound assignment operators (+=, -=, *=, /=).
#define MAKE_OP(OP)                                                                                                                                  \
  inline real_or_complex &operator OP(double y) {                                                                                                    \
    _x OP y;                                                                                                                                         \
    return *this;                                                                                                                                    \
  }                                                                                                                                                  \
  inline real_or_complex &operator OP(std::complex<double> const &y) {                                                                               \
    _x OP y;                                                                                                                                         \
    _is_real = false;                                                                                                                                \
    return *this;                                                                                                                                    \
  }                                                                                                                                                  \
  inline real_or_complex &operator OP(real_or_complex const &y) {                                                                                    \
    _x OP y._x;                                                                                                                                      \
    _is_real &= y._is_real;                                                                                                                          \
    return *this;                                                                                                                                    \
  }

    MAKE_OP(+=);
    MAKE_OP(-=);
    MAKE_OP(*=);
    MAKE_OP(/=);
#undef MAKE_OP
  };

  // Arithmetic operators (+, -, *, /) for utility::real_or_complex values.
#define MAKE_OP(OP, OPC)                                                                                                                             \
  inline real_or_complex operator OP(real_or_complex a, real_or_complex const &b) {                                                                  \
    a OPC b;                                                                                                                                         \
    return a;                                                                                                                                        \
  }                                                                                                                                                  \
  inline real_or_complex operator OP(real_or_complex a, std::complex<double> const &b) {                                                             \
    a OPC b;                                                                                                                                         \
    return a;                                                                                                                                        \
  }                                                                                                                                                  \
  inline real_or_complex operator OP(real_or_complex a, double b) {                                                                                  \
    a OPC b;                                                                                                                                         \
    return a;                                                                                                                                        \
  }                                                                                                                                                  \
  inline real_or_complex operator OP(std::complex<double> const &a, real_or_complex b) { return a OP std::complex<double>(b); }                      \
  inline real_or_complex operator OP(double a, real_or_complex b) {                                                                                  \
    if (b.is_real())                                                                                                                                 \
      return a OP double(b);                                                                                                                         \
    else                                                                                                                                             \
      return a OP std::complex<double>(b);                                                                                                           \
  }

  MAKE_OP(+, +=);
  MAKE_OP(-, -=);
  MAKE_OP(*, *=);
  MAKE_OP(/, /=);
#undef MAKE_OP

  /** @} */

} // namespace triqs::utility
