// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Thomas Ayral, Philipp Dumitrescu, Laura Messio, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a struct to represent Matsubara frequencies.
 */

#pragma once

#include "./utils.hpp"
#include "../utility/kronecker.hpp"

#include <complex>
#include <iostream>
#include <numbers>
#include <type_traits>
#include <utility>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-matsubara
   * @{
   */

  /**
   * @brief Represents a Matsubara frequency \f$ i\omega_n \f$.
   *
   * @details A Matsubara frequency is defined by an index \f$ n \f$, the inverse temperature \f$ \beta \f$ and the
   * particle statistics triqs::mesh::statistic_enum:
   * - for fermions, it takes the values \f$ i \omega_n = i\frac{2 \pi (n + 1)}{\beta} \f$ and
   * - for bosons, it takes the values \f$ i\omega_n = i\frac{2 \pi n}{\beta} \f$.
   */
  struct matsubara_freq {
    /// Value type of a Matsubara frequency.
    using cast_t = std::complex<double>;

    /// Matsubara index \f$ n \f$.
    long n = 0;

    /// Inverse temperature \f$ \beta \f$.
    double beta = 0.0;

    /// Particle statistics.
    statistic_enum statistic = Fermion;

    /**
     * @brief Default constructor initializes the Matsubara frequency with zero index, zero inverse temperature and
     * fermionic statistics.
     */
    matsubara_freq() = default;

    /**
     * @brief Construct a Matsubara frequency with a given index \f$ n \f$, inverse temperature \f$ \beta \f$ and 
     * particle statistics.
     *
     * @param n_idx Matsubara index \f$ n \f$.
     * @param b Inverse temperature \f$ \beta \f$.
     * @param stat Particle statistics.
     */
    matsubara_freq(long n_idx, double b, statistic_enum stat) : n(n_idx), beta(b), statistic(stat) {}

    /// Get the index \f$ n \f$ of the Matsubara frequency.
    [[nodiscard]] long index() const { return n; }

    /// Conversion to `std::complex<double>`.
    operator cast_t() const { return std::complex<double>{0, std::numbers::pi * static_cast<double>(2 * n + statistic) / beta}; }
  };

  /**
   * @brief Write a triqs::mesh::matsubara_freq to a `std::ostream`.
   *
   * @param sout `std::ostream` object.
   * @param iw Frequency to be written.
   * @return Reference to `std::ostream` object.
   */
  inline std::ostream &operator<<(std::ostream &sout, matsubara_freq const &iw) { return sout << std::complex<double>(iw); }

  /**
   * @brief Addition operator for two triqs::mesh::matsubara_freq objects.
   *
   * @details The addition is performed directly on the indices. The resulting Matsubara frequency has
   * - fermionic statistics, if one and only one of the operands has fermionic statistics and
   * - bosonic statistics otherwise.
   *
   * @param iw1 Left hand side frequency operand.
   * @param iw2 Right hand side frequency operand.
   * @return Resulting triqs::mesh::matsubara_freq object.
   */
  [[nodiscard]] inline matsubara_freq operator+(matsubara_freq const &iw1, matsubara_freq const &iw2) {
    return {iw1.n + iw2.n + (iw1.statistic & iw2.statistic), iw1.beta, ((iw1.statistic ^ iw2.statistic) == 1 ? Fermion : Boson)};
  }

  /**
   * @brief Subtraction operator for two triqs::mesh::matsubara_freq objects.
   *
   * @details The subtraction is performed directly on the indices. The resulting Matsubara frequency has
   * - fermionic statistics, if one and only one of the operands has fermionic statistics and
   * - bosonic statistics otherwise.
   *
   * @param iw1 Left hand side frequency operand.
   * @param iw2 Right hand side frequency operand.
   * @return Resulting triqs::mesh::matsubara_freq object.
   */
  [[nodiscard]] inline matsubara_freq operator-(matsubara_freq const &iw1, matsubara_freq const &iw2) {
    return {iw1.n - iw2.n - (~iw1.statistic & iw2.statistic), iw1.beta, ((iw1.statistic ^ iw2.statistic) == 1 ? Fermion : Boson)};
  }

  /**
   * @brief Unary minus operator for a triqs::mesh::matsubara_freq object.
   *
   * @details For fermions, it adds \f$ +1 \f$ to the index and flips its sign. For bosons, it just flips the sign of
   * the index.
   *
   * @param iw Frequency operand.
   * @return Resulting triqs::mesh::matsubara_freq object.
   */
  [[nodiscard]] inline matsubara_freq operator-(matsubara_freq const &iw) {
    return {-(iw.n + (iw.statistic == Fermion ? 1 : 0)), iw.beta, iw.statistic};
  }

  /**
   * @brief Divison operator for two triqs::mesh::matsubara_freq objects.
   *
   * @details The division is performed on the complex numbers corresponding to the Matsubara frequencies.
   *
   * @param iw1 Left hand side frequency operand.
   * @param iw2 Right hand side frequency operand.
   * @return Resulting `std::complex<double>`.
   */
  [[nodiscard]] inline auto operator/(matsubara_freq const &iw1, matsubara_freq const &iw2) {
    return std::complex<double>{iw1} / std::complex<double>{iw2};
  }

  /**
   * @brief Multiplication operator for two triqs::mesh::matsubara_freq objects.
   *
   * @details The mulitiplication is performed on the complex numbers corresponding to the Matsubara frequencies.
   *
   * @param iw1 Left hand side frequency operand.
   * @param iw2 Right hand side frequency operand.
   * @return Resulting `std::complex<double>`.
   */
  [[nodiscard]] inline auto operator*(matsubara_freq const &iw1, matsubara_freq const &iw2) {
    return std::complex<double>{iw1} * std::complex<double>{iw2};
  }

#define IMPL_OP(OP)                                                                                                                                  \
  /** @brief Binary `OP` operation for a triqs::mesh::matsubara_freq and some numeric type. */                                                       \
  template <typename T>                                                                                                                              \
    requires(not std::is_base_of_v<matsubara_freq, std::decay_t<T>>)                                                                                 \
  [[nodiscard]] inline auto operator OP(matsubara_freq const &iw, T &&y) {                                                                           \
    return std::complex<double>(iw) OP std::forward<T>(y);                                                                                           \
  }                                                                                                                                                  \
  /** @brief Binary `OP` operation for some numeric type and a triqs::mesh::matsubara_freq. */                                                       \
  template <typename T>                                                                                                                              \
    requires(not std::is_base_of_v<matsubara_freq, std::decay_t<T>>)                                                                                 \
  [[nodiscard]] inline auto operator OP(T &&x, matsubara_freq const &iw) {                                                                           \
    return std::forward<T>(x) OP std::complex<double>(iw);                                                                                           \
  }
  IMPL_OP(+);
  IMPL_OP(-);
  IMPL_OP(*);
  IMPL_OP(/);
#undef IMPL_OP

  /**
   * @brief Kronecker delta function for a single Matsubara frequency.
   *
   * @param iw triqs::mesh::matsubara_freq object.
   * @return True, if the index of the Matsubara frequency is zero, false otherwise.
   */
  [[nodiscard]] inline bool kronecker(matsubara_freq const &iw) { return iw.n == 0; }

  /**
   * @brief Kronecker delta function for two Matsubara frequencies.
   *
   * @param iw1 triqs::mesh::matsubara_freq object.
   * @param iw2 triqs::mesh::matsubara_freq object.
   * @return True, if the indices of the Matsubara frequencies are equal, false otherwise.
   */
  [[nodiscard]] inline bool kronecker(matsubara_freq const &iw1, matsubara_freq const &iw2) { return iw1.n == iw2.n; }

  /** @} */

} // namespace triqs::mesh
