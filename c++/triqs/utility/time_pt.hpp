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

/**
 * @file
 * @brief Imaginary-time points on a very fine integer grid, used by continuous-time Monte Carlo solvers.
 */

#pragma once

#include "./first_include.hpp"
#include "./macros.hpp"

#include <h5/h5.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

namespace triqs::utility {

  // Forward declaration.
  struct time_segment;

  /**
   * @addtogroup triqs-utility-math
   * @{
   */

  /**
   * @brief A point in imaginary time, \f$ \tau_n \in [0, \beta] \f$, stored on a very fine integer grid.
   *
   * @details Each time point is identified by an integer position \f$ n \in [0, N_\mathrm{max}] \f$ of type `uint64_t` 
   * and the inverse temperature \f$ \beta \f$. Its floating-point value is \f$ \beta * n / N_\mathrm{max} \f$. 
   * 
   * Comparing two time points reduces to integer comparison, which avoids the well-known pitfalls of floating-point 
   * equality.
   * 
   * Various arithmetic operations are defined between time points and between time points and scalar `double` values.
   */
  struct time_pt {
    // Friend declaration.
    friend struct time_segment;

    /**
     * @brief Default constructor creates the point \f$ \tau_0 \f$ for \f$ \beta = 0 \f$.
     */
    time_pt() = default;

    /**
     * @brief Construct a point \f$ \tau_n \f$ from an integer grid position and the inverse temperature.
     *
     * @param n_ Integer position \f$ n \in [0, N_\text{max}] \f$ on the grid.
     * @param beta_ Inverse temperature \f$ \beta \f$.
     */
    time_pt(uint64_t n_, double beta_) : n(n_), beta(beta_), val(beta_ * (static_cast<double>(n_) / static_cast<double>(Nmax))) {}

    /// Assigning a `double` is deleted to avoid accidental loss of precision.
    time_pt &operator=(double v) = delete;

    /**
     * @brief Three-way comparison operator compares the integer grid positions of two time points.
     *
     * @param tp Right-hand side time point.
     * @returns Ordering of the two integer grid positions.
     */
    auto operator<=>(time_pt const &tp) const { return n <=> tp.n; }

    /**
     * @brief Equal-to operator compares the integer grid positions of two time points.
     *
     * @param tp Right-hand side time point.
     * @returns True if both points share the same grid position.
     */
    bool operator==(time_pt const &tp) const { return n == tp.n; }

    /**
     * @brief Addiditon operator performs cyclic addition on \f$ [0, \beta] \f$.
     *
     * @param tp1 Left-hand side operand \f$ \tau_l \f$.
     * @param tp2 Right-hand side operand \f$ \tau_r \f$.
     * @returns Time point corresponding to the floating-point value \f$ (\tau_l + \tau_r) \mod \beta \f$.
     */
    inline friend time_pt operator+(time_pt const &tp1, time_pt const &tp2) {
      bool wrapped = ((Nmax - std::max(tp1.n, tp2.n)) < std::min(tp1.n, tp2.n));
      if (!wrapped)
        return {(tp1.n + tp2.n) % Nmax, tp1.beta};
      else
        return {((tp1.n + tp2.n) + 1) % Nmax, tp1.beta};
    }

    /**
     * @brief Subtraction operator performs cyclic subtraction on \f$ [0, \beta] \f$.
     *
     * @param tp1 Left-hand side operand \f$ \tau_l \f$.
     * @param tp2 Right-hand side operand \f$ \tau_r \f$.
     * @returns Time point corresponding to the floating-point value \f$ (\tau_l - \tau_r) \mod \beta \f$.
     */
    inline friend time_pt operator-(time_pt const &tp1, time_pt const &tp2) {
      uint64_t nres = (tp1.n >= tp2.n ? (tp1.n - tp2.n) % Nmax : Nmax - (tp2.n - tp1.n));
      return {nres, tp1.beta};
    }

    /**
     * @brief Unary minus operator performs cyclic negation \f$ [0, \beta] \f$.
     *
     * @param tp Time point \f$ \tau_n \f$ to negate.
     * @returns Time point corresponding to the floating-point value \f$ \beta - \tau_n \f$ on the grid.
     */
    friend time_pt operator-(time_pt const &tp) { return {Nmax - tp.n, tp.beta}; }

    /**
     * @brief Divide a time point by an integer.
     *
     * @param tp Time point \f$ \tau_n \f$.
     * @param a Integer divisor \f$ a \f$.
     * @returns Time point with grid position \f$ n / a \f$.
     */
    friend time_pt div_by_int(time_pt const &tp, size_t a) { return {tp.n / a, tp.beta}; }

    /**
     * @brief Multiply a time point by an integer.
     *
     * @param tp Time point \f$ \tau_n \f$.
     * @param a Integer factor \f$ a \f$.
     * @returns Time point with grid position \f$ n * a \f$.
     */
    friend time_pt mult_by_int(time_pt const &tp, size_t a) { return {tp.n * a, tp.beta}; }

    /**
     * @brief Perform floor division between two time points.
     *
     * @param tp1 Left-hand side time point \f$ \tau_l \f$.
     * @param tp2 Right-hand side time point \f$ \tau_r \f$.
     * @returns Time point with grid position \f$ \lfloor l / r \rfloor \f$.
     */
    friend size_t floor_div(time_pt const &tp1, time_pt const &tp2) { return tp1.n / tp2.n; }

    /**
     * @brief Explicit cast of \f$ \tau_n \f$ to the corresponding `double` value.
     * @returns Floating-point value \f$ \beta * n / N_\mathrm{max} \f$.
     */
    explicit operator double() const { return val; }

    /**
     * @brief Write a @ref time_pt to an output stream.
     *
     * @param out Output stream to write to.
     * @param p Point to print.
     * @returns Reference to the output stream.
     */
    friend std::ostream &operator<<(std::ostream &out, time_pt const &p) {
      return out << p.val << " [time_pt : beta = " << p.beta << " n = " << p.n << "]";
    }

    /// Largest possible grid position.
    static constexpr uint64_t Nmax = std::numeric_limits<uint64_t>::max();

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "time_pt"; }

    /**
     * @brief Write a @ref time_pt to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param tp Time point to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, time_pt const &tp) {
      auto gr = g.create_group(name);
      write_hdf5_format(gr, tp); // NOLINT
      h5_write(gr, "beta", tp.beta);
      h5_write(gr, "val", tp.val);
      h5_write(gr, "n", tp.n);
    }

    /**
     * @brief Read a @ref time_pt from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param tp Time point to read into.
     */
    friend void h5_read(h5::group g, std::string const &name, time_pt &tp) {
      auto gr = g.open_group(name);
      h5_read(gr, "beta", tp.beta);
      h5_read(gr, "val", tp.val);
      h5_read(gr, "n", tp.n);
    }

    private:
    uint64_t n  = 0;
    double beta = 0;
    double val  = 0;
  };

  /**
   * @relates triqs::utility::time_pt
   * @brief Multiplication between two time points.
   *
   * @param tp1 Left-hand side time point \f$ \tau_l \f$.
   * @param tp2 Right-hand side time point \f$ \tau_r \f$.
   * @returns Product of their floating-point values.
   */
  inline double operator*(time_pt const &tp1, time_pt const &tp2) { return double(tp1) * double(tp2); }

  /**
   * @relates triqs::utility::time_pt
   * @brief Division between two time points.
   *
   * @param tp1 Left-hand side time point \f$ \tau_l \f$.
   * @param tp2 Right-hand side time point \f$ \tau_r \f$.
   * @returns Ratio of their floating-point values.
   */
  inline double operator/(time_pt const &tp1, time_pt const &tp2) { return double(tp1) / double(tp2); }

  // Arithmetic operations between time_pt and double values.
#define IMPL_OP(OP)                                                                                                                                  \
  inline double operator OP(time_pt const &x, double y) { return static_cast<double>(x) OP y; }                                                      \
  inline double operator OP(double y, time_pt const &x) { return y OP static_cast<double>(x); }
  IMPL_OP(+);
  IMPL_OP(-);
  IMPL_OP(*);
  IMPL_OP(/);
#undef IMPL_OP

  /**
   * @brief Represents the imaginary-time segment \f$ [0, \beta] \f$.
   */
  struct time_segment {
    /// Inverse temperature \f$ \beta \f$.
    double beta;

    /**
     * @brief Construct a segment \f$ [0, \beta] \f$ from an inverse temperature.
     * @param beta_ Inverse temperature \f$ \beta \f$.
     */
    time_segment(double beta_) : beta(beta_) {}

    /**
     * @brief Draw a uniform random point in \f$ [0, \tau[ \f$.
     *
     * @tparam RNG Random number generator type.
     * @param rng Random number generator.
     * @param tp Exclusive upper bound \f$ \tau \f$.
     * @returns A uniformly distributed time point on \f$ [0, \tau[ \f$.
     */
    template <typename RNG> [[nodiscard]] time_pt get_random_pt(RNG &rng, time_pt tp) const { return {rng(tp.n), beta}; }

    /**
     * @brief Draw a uniform random point in \f$ [0, \beta[ \f$.
     *
     * @tparam RNG Random number generator type.
     * @param rng Random number generator.
     * @returns A uniformly distributed time point on the current segment.
     */
    template <typename RNG> [[nodiscard]] time_pt get_random_pt(RNG &rng) const { return {rng(time_pt::Nmax), beta}; }

    /**
     * @brief Draw a uniform random point in \f$ [\tau_1, \tau_2[ \f$.
     *
     * @tparam RNG Random number generator type.
     * @param rng Random number generator.
     * @param tp1 Inclusive lower bound \f$ \tau_1 \f$.
     * @param tp2 Exclusive upper bound \f$ \tau_2 \f$.
     * @returns A uniformly distributed time point on the interval \f$ [\tau_1, \tau_2[ \f$.
     */
    template <typename RNG> [[nodiscard]] time_pt get_random_pt(RNG &rng, time_pt tp1, time_pt tp2) const {
      return {rng(tp2.n - tp1.n) + tp1.n, beta};
    }

    /**
     * @brief Get the upper end of the segment.
     * @returns Time point with grid position \f$ n = N_\mathrm{max} \f$.
     */
    [[nodiscard]] time_pt get_upper_pt() const { return {time_pt::Nmax, beta}; }

    /**
     * @brief Get the lower end of the segment.
     * @returns Time point with grid position \f$ n = 0 \f$.
     */
    [[nodiscard]] time_pt get_lower_pt() const { return {0, beta}; }

    /**
     * @brief Get a time point representing one grid step \f$ \varepsilon = \beta / N_\text{max} \f$.
     * @returns Time point with grid position \f$ n = 1 \f$.
     */
    [[nodiscard]] time_pt get_epsilon() const { return {1, beta}; }

    /**
     * @brief Convert a `double` value to the nearest point on the grid.
     *
     * @param x Value in \f$ [0, \beta] \f$.
     * @returns The corresponding time point on the grid.
     */
    [[nodiscard]] time_pt make_time_pt(double x) const {
      EXPECTS(0 <= x && x <= beta);
      uint64_t n = static_cast<uint64_t>(static_cast<double>(time_pt::Nmax) * std::min(1.0, (x / beta)));
      return {n, beta};
    }
  };

  /** @} */

} // namespace triqs::utility
