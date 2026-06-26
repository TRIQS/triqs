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
 * @brief Compile-time count of types in a parameter pack that derive from a given type `T`.
 */

#pragma once

#include <type_traits>

namespace triqs {

  /**
   * @addtogroup triqs-utility-traits
   * @{
   */

  /**
   * @brief Number of types in a parameter pack that derive from (or are equal to) a given type.
   *
   * @details It uses `std::is_base_of` to determine whether a type derives from `T`.
   * 
   * Useful when restricting variadic template overloads to a maximum number of arguments of a certain category.
   * 
   * @warning This is unused. It might be removed in the future.
   * 
   * @tparam T Base type to look for.
   * @tparam A Parameter pack of candidate types.
   */
  template <typename T, typename... A> struct count_type_occurrence;

  // Specialization of count_type_occurrence for at least one type in the parameter pack.
  template <typename T, typename A0, typename... A>
  struct count_type_occurrence<T, A0, A...> : std::integral_constant<int, std::is_base_of_v<T, A0> + count_type_occurrence<T, A...>::value> {};

  // Specialization of count_type_occurrence for an empty parameter pack.
  template <typename T> struct count_type_occurrence<T> : std::integral_constant<int, 0> {};

  /**
   * @brief Number of types in a parameter pack that do not derive from (or are equal to) a given type.
   * 
   * @details Complement of triqs::count_type_occurrence.
   * 
   * @warning This is unused. It might be removed in the future.
   * 
   * @tparam T Base type to look for.
   * @tparam A Parameter pack of candidate types.
   */
  template <typename T, typename... A> struct count_type_occurrence_not;

  // Specialization of count_type_occurrence_not for at least one type in the parameter pack.
  template <typename T, typename A0, typename... A>
  struct count_type_occurrence_not<T, A0, A...> : std::integral_constant<int, !(std::is_base_of_v<T, A0>)+count_type_occurrence_not<T, A...>::value> {
  };

  // Specialization of count_type_occurrence_not for an empty parameter pack.
  template <typename T> struct count_type_occurrence_not<T> : std::integral_constant<int, 0> {};

  /** @} */

} // namespace triqs
