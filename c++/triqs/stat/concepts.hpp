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
 * @brief Provides various concepts for the @ref triqs-stat-utils.
 */

#pragma once

#include <nda/nda.hpp>

#include <complex>
#include <ranges>

namespace triqs::stat {

  /**
   * @addtogroup triqs-stat-concepts
   * @{
   */

  /**
   * @brief Concept to check if a type can be used with various @ref triqs-stat.
   *
   * @details Allowed types include
   * - `double` or `std::complex<double>` scalars and
   * - `nda::Array` types with `double` or `std::complex<double>` as their value type and with algebra 'A'.
   *
   * @tparam T Type to check.
   */
  template <typename T>
  concept StatCompatible =
     (nda::Scalar<T> || (nda::Array<T> && nda::get_algebra<T> == 'A')) && nda::is_any_of<nda::get_value_t<T>, double, std::complex<double>>;

  /**
   * @brief Concept to check if a type can be used with triqs::stat::lin_binning or triqs::stat::log_binning.
   *
   * @details Allowed types include triqs::stat::StatCompatible types that are either scalar or regular array types.
   *
   * @tparam T Type to check.
   */
  template <typename T>
  concept AccCompatible = StatCompatible<T> && (nda::Scalar<T> || nda::is_regular_v<T>);

  /**
   * @brief Concept to check if a range can be used with various @ref triqs-stat.
   *
   * @details Allowed ranges are required to
   * - have a triqs::stat::StatCompatible value type,
   * - be a `std::ranges::sized_range`, and
   * - be a `std::ranges::forward_range`.
   *
   * @tparam R Range to check.
   */
  template <typename R>
  concept StatCompatibleRange = std::ranges::sized_range<R> && std::ranges::forward_range<R> && StatCompatible<std::ranges::range_value_t<R>>;

  /** @} */

} // namespace triqs::stat
