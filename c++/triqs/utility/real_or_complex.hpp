// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2015 Igor Krivenko
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

#pragma once
#include <triqs/utility/exceptions.hpp>
#include <triqs/utility/numeric_ops.hpp>
#include <complex>

namespace triqs {
  namespace utility {

    /**
  * A simple number which can be real or complex with runtime decision
  * When is becomes complex after some operations, it can not be real anymore.
  *
  */
    class real_or_complex {

      bool _is_real = true;
      std::complex<double> _x;

      public:
      real_or_complex() : _x(0) {}
      real_or_complex(double x) : _x(x) {}
      real_or_complex(std::complex<double> x) : _x(std::move(x)), _is_real(false) {}

      bool is_real() const { return _is_real; }

      explicit operator std::complex<double>() const { return _x; }

      explicit operator double() const {
        if (!_is_real) TRIQS_RUNTIME_ERROR << "Logic error : the number is not real, it is complex";
        return real(_x);
      }

      friend real_or_complex operator-(real_or_complex a) {
        a._x = -a._x;
        return a;
      }

      friend double real(real_or_complex const &x) { return real(x._x); }
      friend double imag(real_or_complex const &x) { return imag(x._x); }
      friend double abs(real_or_complex const &x) { return std::abs(x._x); }
      friend real_or_complex conj(real_or_complex x) {
        x._x = std::conj(x._x);
        return x;
      }

      friend bool is_zero(real_or_complex const &x, double tolerance = 100 * std::numeric_limits<double>::epsilon()) {
        using triqs::utility::is_zero;
        if (x.is_real()) return is_zero(double(x), tolerance);
        return is_zero(x._x, tolerance);
      }

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

      bool operator==(real_or_complex const &x) const { return abs(_x - x._x) == 0; }
      bool operator!=(real_or_complex const &x) const { return !operator==(x); }

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

  } // namespace utility
} // namespace triqs
