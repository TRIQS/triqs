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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Building blocks for expression-template and other type traits.
 */

#pragma once

#include "./macros.hpp"

#include <complex>
#include <type_traits>
#include <utility>

namespace triqs::utility {

  /**
   * @addtogroup triqs-utility-traits
   * @{
   */

  /**
   * @brief Strip rvalue references from `T` while turning lvalue references into `const &`.
   *
   * @details Convenient when building expression trees where rvalue operands should be stored by value while lvalue 
   * operands should be stored by const reference.
   * 
   * @tparam T Type to transform.
   */
  template <class T> struct remove_rvalue_ref {
    using type = T;
  };
  template <class T> struct remove_rvalue_ref<T &> {
    using type = T const &;
  };
  template <class T> struct remove_rvalue_ref<T &&> {
    using type = T;
  };

  /// Alias for the nested `type` in remove_rvalue_ref.
  template <class T> using remove_rvalue_ref_t = typename remove_rvalue_ref<T>::type;

  /// Namespace for empty tag types identifying the four basic arithmetic operations used in expression templates.
  namespace tags {

    /// Tag for `+` operation.
    struct plus {};

    /// Tag for `-` operation.
    struct minus {};

    /// Tag for `*` operation.
    struct multiplies {};

    /// Tag for `/` operation.
    struct divides {};

  } // namespace tags

  /**
   * @brief Callable wrapper that evaluates the operation identified by `Tag` on two operands.
   *
   * @details Specialized for the four tags in namespace `triqs::utility::tags`. Each specialization provides a 
   * templated `operator()(L &&l, R &&r)` and a static `char name` (`+`, `-`, `*`, `/`) suitable for printing.
   * 
   * @tparam Tag Type of the operation.
   */
  template <typename Tag> struct operation;

  // Spezialization of triqs::utility::operation for plus operation: returns `l + r`.
  template <> struct operation<tags::plus> {
    static const char name = '+';
    template <typename L, typename R> auto operator()(L &&l, R &&r) const DECL_AND_RETURN(std::forward<L>(l) + std::forward<R>(r))
  };

  // Spezialization of triqs::utility::operation for minus operation: returns `l - r`.
  template <> struct operation<tags::minus> {
    static const char name = '-';
    template <typename L, typename R> auto operator()(L &&l, R &&r) const DECL_AND_RETURN(std::forward<L>(l) - std::forward<R>(r))
  };

  // Spezialization of triqs::utility::operation for multiplies operation: returns `l * r`.
  template <> struct operation<tags::multiplies> {
    static const char name = '*';
    template <typename L, typename R> auto operator()(L &&l, R &&r) const DECL_AND_RETURN(std::forward<L>(l) * std::forward<R>(r))
  };

  // Spezialization of triqs::utility::operation for divides operation: returns `l / r`.
  template <> struct operation<tags::divides> {
    static const char name = '/';
    template <typename L, typename R> auto operator()(L &&l, R &&r) const DECL_AND_RETURN(std::forward<L>(l) / std::forward<R>(r))
  };

  /**
   * @brief Trait identifying types that lie in \f$ \mathbb{Z} \cup \mathbb{R} \cup \mathbb{C} \f$.
   *
   * @details True for all arithmetic types (including `bool`) and for `std::complex<T>`.
   * 
   * @tparam T Type to inspect.
   */
  template <typename T> struct is_in_ZRC : std::is_arithmetic<T> {};
  template <> struct is_in_ZRC<bool> : std::true_type {};
  template <typename T> struct is_in_ZRC<std::complex<T>> : std::true_type {};
  template <typename T> struct is_in_ZRC<T &> : is_in_ZRC<T> {};
  template <typename T> struct is_in_ZRC<T &&> : is_in_ZRC<T> {};
  template <typename T> struct is_in_ZRC<const T> : is_in_ZRC<T> {};

  /**
   * @brief Trait whose nested `type` is `decltype(a * b)` with cv-/ref-qualifiers stripped from the operands.
   * 
   * @warning This is unused. It might be removed in the future.
   * 
   * @tparam A Left operand type.
   * @tparam B Right operand type.
   */
  template <typename A, typename B> struct type_of_mult {
    using type = decltype(std::declval<std::remove_reference_t<A>>() * std::declval<std::remove_reference_t<B>>());
  };

  /** @} */

} // namespace triqs::utility
