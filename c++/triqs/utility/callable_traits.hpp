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
   * @brief Type traits for callable types.
   *
   * @details The `operator()` of the callable type can only have 1 overload.
   *
   * It defines the following traits:
   * - `arity`: Number of arguments the function call operator takes (including default arguments).
   * - `result_type`: Return type of the function call operator.
   * - `arg_t<N>`: Type of the N-th argument.
   * - `decay_arg_t<N>`: Decay type of the N-th argument.
   *
   * @tparam F Callable type.
   */
  template <typename F> struct callable_traits : public detail::callable_traits_impl<decltype(&F::operator())> {};

} // namespace triqs::utility
