// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
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
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell

#pragma once

#include <triqs/utility/macros.hpp>
#include <h5/h5.hpp>

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <utility>

namespace triqs::utility {

  /**
   * Discretized imaginary time point.
   *
   * Represents a point \f$\tau \in [0, \beta]\f$ on a very fine integer grid.
   * The position is stored as a `uint64_t`, allowing exact comparisons
   * that would be dangerous on floating-point numbers.
   *
   * The inverse temperature `beta` is stored as a static member shared by all instances.
   * It must be set via `set_beta()` before use.
   */
  class tau_t {
    std::uint64_t n            = 0;
    inline static double _beta = 0;

    public:
    /// Maximum grid index, representing \f$\tau = \beta\f$.
    static constexpr std::uint64_t n_max = std::numeric_limits<std::uint64_t>::max();

    /// Set the inverse temperature for all tau_t instances.
    static void set_beta(double beta) { _beta = beta; }

    /// Get the current inverse temperature.
    static double get_beta_value() { return _beta; }

    // --- Construction ---

    tau_t() = default;

    /// Construct from a raw grid index.
    explicit tau_t(std::uint64_t n_) : n(n_) {}

    /// Assignment from double is forbidden.
    tau_t &operator=(double) = delete;

    // --- Static factories ---

    /// \f$\tau = 0\f$
    static tau_t zero() { return tau_t{std::uint64_t{0}}; }

    /// Smallest representable positive time, \f$\epsilon = \beta / N_\mathrm{max}\f$.
    static tau_t epsilon() { return tau_t{std::uint64_t{1}}; }

    /// \f$\tau = \beta\f$
    static tau_t beta() { return tau_t{n_max}; }

    // --- Random generation ---

    /// Uniform random point in \f$[0, \beta)\f$.
    template <typename RNG> static tau_t random(RNG &rng) { return tau_t{rng(n_max)}; }

    /// Uniform random point in \f$]0, \tau[\f$. Requires \f$\tau \geq 2\epsilon\f$.
    template <typename RNG> static tau_t random(RNG &rng, tau_t tau) {
      EXPECTS(tau.n >= 2);
      return tau_t{rng(tau.n - 1) + 1};
    }

    /// Uniform random point in \f$]\tau_1, \tau_2[\f$. Requires \f$\tau_2 \geq \tau_1 + 2\epsilon\f$.
    template <typename RNG> static tau_t random(RNG &rng, tau_t tau1, tau_t tau2) {
      EXPECTS(tau1 < tau2 && tau2.n - tau1.n >= 2);
      auto n1 = tau1.n + 1;
      return tau_t{rng(tau2.n - n1) + n1};
    }

    // --- Conversion ---

    /// Cast to double: \f$\tau = \beta \cdot n / N_\mathrm{max}\f$.
    explicit operator double() const { return _beta * (static_cast<double>(n) / static_cast<double>(n_max)); }

    /// Access the raw grid index.
    [[nodiscard]] std::uint64_t get_n() const { return n; }

    // --- Comparisons (using integer, so it is safe) ---

    auto operator<=>(tau_t const &) const = default;
    bool operator==(tau_t const &) const  = default;

    // --- Cyclic arithmetic (unsigned wrapping) ---

    friend tau_t operator+(tau_t a, tau_t b) { return tau_t{a.n + b.n}; }
    friend tau_t operator-(tau_t a, tau_t b) { return tau_t{a.n - b.n}; }

    /// Additive inverse: `a + (-a) == zero()`. Use `beta() - a` for the reflection \f$\beta - \tau\f$.
    friend tau_t operator-(tau_t a) { return tau_t{-a.n}; }

    // --- Integer scaling ---

    friend tau_t mult_by_int(tau_t a, std::size_t b) { return tau_t{a.n * b}; }
    friend tau_t div_by_int(tau_t a, std::size_t b) { return tau_t{a.n / b}; }
    friend std::size_t floor_div(tau_t a, tau_t b) { return a.n / b.n; }

    // --- Construction from double ---

    /// Create a tau_t from a double value in \f$[0, \beta]\f$.
    static tau_t from_double(double x) {
      EXPECTS(0 <= x && x <= _beta);
      if (x == _beta) return tau_t{n_max};
      return tau_t{static_cast<std::uint64_t>((x / _beta) * static_cast<double>(n_max))};
    }

    // --- IO ---

    friend std::ostream &operator<<(std::ostream &out, tau_t const &p) {
      return out << double(p) << " [tau_t : beta = " << _beta << " n = " << p.n << "]";
    }

    // --- HDF5 ---

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "tau_t"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, tau_t const &t) {
      auto gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, t); // NOLINT
      h5_write(gr, "n", t.n);
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, tau_t &t) {
      auto gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, t); // NOLINT
      h5_read(gr, "n", t.n);
    }
  };

  // --- Mixed arithmetic with double (decay to double) ---

  inline double operator*(tau_t a, tau_t b) { return double(a) * double(b); }
  inline double operator/(tau_t a, tau_t b) { return double(a) / double(b); }

  inline double operator+(tau_t x, double y) { return double(x) + y; }
  inline double operator+(double y, tau_t x) { return y + double(x); }

  inline double operator-(tau_t x, double y) { return double(x) - y; }
  inline double operator-(double y, tau_t x) { return y - double(x); }

  inline double operator*(tau_t x, double y) { return double(x) * y; }
  inline double operator*(double y, tau_t x) { return y * double(x); }

  inline double operator/(tau_t x, double y) { return double(x) / y; }
  inline double operator/(double y, tau_t x) { return y / double(x); }

  // --- Free function helpers ---

  /// Cyclic difference of two tau_t points, returning (sign, |tau1 - tau2| mod beta).
  inline std::pair<double, double> cyclic_difference(tau_t tau1, tau_t tau2) {
    double sign  = tau2 > tau1 ? -1.0 : 1.0;
    double value = double(tau_t{tau1.get_n() - tau2.get_n()});
    return {sign, value};
  }

  /// Cyclic difference of two double-valued times.
  std::pair<double, double> cyclic_difference(double tau1, double tau2);

} // namespace triqs::utility
