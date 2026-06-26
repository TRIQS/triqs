// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Traits used in **TRIQS**.
 */

#pragma once

#include <type_traits>

namespace triqs {

  /**
   * @addtogroup triqs-utility-traits
   * @{
   */

  /**
   * @brief Boolean OR fold over a parameter pack of `integral_constant`-like traits.
   * @warning This is unused. It might be removed in the future.
   */
  template <typename... T> struct _or;
  template <typename T0, typename... T> struct _or<T0, T...> : std::integral_constant<bool, T0::value || _or<T...>::value> {};
  template <> struct _or<> : std::false_type {};

  /**
   * @brief Boolean AND fold over a parameter pack of `integral_constant`-like traits.
   * @warning This is unused. It might be removed in the future.
   */
  template <typename... T> struct _and;
  template <typename T0, typename... T> struct _and<T0, T...> : std::integral_constant<bool, T0::value && _and<T...>::value> {};
  template <> struct _and<> : std::true_type {};

  /**
   * @brief Alias for `std::integral_constant<bool, B>`.
   * @warning This is unused. It might be removed in the future.
   */
  template <bool B> using bool_constant = std::integral_constant<bool, B>;

  /**
   * @brief Strip cv- and reference-qualifiers from a type.
   * @warning This is unused. Use std::remove_cvref in C++20 instead.
   * @tparam T Type to strip.
   */
  template <typename T> struct remove_cv_ref : std::remove_cv<std::remove_reference_t<T>> {};

  /**
   * @brief Empty tag inherited by every view type in **TRIQS**. See also @ref is_view.
   */
  struct is_view_tag {};

  /**
   * @brief Trait that detects view types by checking for inheritance from @ref is_view_tag.
   * @tparam T Type to check.
   */
  template <typename T> struct is_view : std::is_base_of<is_view_tag, T> {};

  /** @} */

} // namespace triqs
