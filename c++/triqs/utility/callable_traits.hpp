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
 * @brief Provides a trait inspecting the `operator()` of a callable type.
 */

#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace triqs::utility {

  namespace detail {

    template <typename F> struct callable_traits_impl;

    template <typename ClassType, typename ReturnType, typename... Args> struct callable_traits_impl<ReturnType (ClassType::*)(Args...) const> {
      static constexpr int arity                 = sizeof...(Args);
      using result_type                          = ReturnType;
      template <std::size_t N> using arg_t       = std::tuple_element_t<N, std::tuple<Args...>>;
      template <std::size_t N> using decay_arg_t = std::decay_t<arg_t<N>>;
    };

    template <typename ClassType, typename ReturnType, typename... Args>
    struct callable_traits_impl<ReturnType (ClassType::*)(Args...)> : callable_traits_impl<ReturnType (ClassType::*)(Args...) const> {};

  } // namespace detail

  /**
   * @ingroup triqs-utility-traits
   * @brief Type trait for a callable type with a single, non-overloaded `operator()`.
   *
   * @details Provides the following (static, constexpr) members:
   * - `arity`: Number of arguments the call operator takes (including defaulted arguments).
   * - `result_type`: Return type of the call operator.
   * - `arg_t<N>`: Type of the N-th argument.
   * - `decay_arg_t<N>`: `std::decay_t` applied to `arg_t<N>`.
   *
   * The `operator()` of `F` must have exactly one overload. Ambiguous callables are not supported.
   *
   * @tparam F Callable type.
   */
  template <typename F> struct callable_traits : public detail::callable_traits_impl<decltype(&F::operator())> {};

} // namespace triqs::utility
