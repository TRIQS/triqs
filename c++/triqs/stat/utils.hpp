// Copyright (c) 2021 Simons Foundation
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
// Authors: Philipp Dumitrescu, Nils Wentzell

/**
 * @file
 * @brief Provides various utilities for the @ref triqs-stat-utils.
 */

#pragma once

#include "./concepts.hpp"

#include <nda/nda.hpp>

#include <type_traits>
#include <utility>

namespace triqs::stat {

  /**
   * @addtogroup triqs-stat-utils
   * @{
   */

  /**
   * @brief Make a given object real and regular.
   *
   * @details It simply calls `nda::make_regular` and `nda::real` on the input object.
   *
   * The return type of this function depends on the input type `T`.
   *
   * @tparam T Input type.
   * @param t Input object to make real and regular.
   * @return Real, regular copy of the input object.
   */
  template <typename T> [[nodiscard]] auto make_real(T &&t) { return nda::make_regular(nda::real(std::forward<T>(t))); }

  /// Type trait to get the type that would be returned by triqs::stat::make_real.
  template <typename T> using get_real_t = std::remove_cvref_t<decltype(make_real(std::declval<T>()))>;

  /// Type trait to get the type that would be returned by `nda::make_regular`.
  template <typename T> using get_regular_t = std::remove_cvref_t<decltype(nda::make_regular(std::declval<T>()))>;

  /**
   * @brief Get a sample with all elements set to zero.
   *
   * @tparam T triqs::stat::StatCompatible type.
   * @param sample Dummy sample to determine its type and shape in case of an `nda::Array` type.
   * @return Zeroed sample.
   */
  template <StatCompatible T> [[nodiscard]] auto zeroed_sample([[maybe_unused]] T const &sample) {
    if constexpr (nda::Scalar<T>) {
      return T{0};
    } else {
      return get_regular_t<T>::zeros(sample.shape());
    }
  }

  /**
   * @brief Calculate the (elementwise) absolute square of an array/view/scalar.
   *
   * @details The return type depends on the input type.
   *
   * @param x Input array/view/scalar.
   * @return (Elementwise) Absolute square of the input.
   */
  [[nodiscard]] auto abs_square(auto const &x) { return make_real(nda::hadamard(nda::conj(x), x)); }

  /** @} */

} // namespace triqs::stat
