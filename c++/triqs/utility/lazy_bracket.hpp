// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
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
 * @brief Lazy function call through chained `[]` operators.
 */

#pragma once

#include "./tuple_tools.hpp"

#include <tuple>
#include <type_traits>
#include <utility>

namespace triqs::utility {

  // Implementation details for make_lazy_bracket.
  namespace details {

    template <int r, typename Lambda, typename Tu> struct _bra;

    template <int r, typename F, typename Tu> _bra<r, F, Tu> make_bra(F &&f, Tu &&tu) { return {std::forward<F>(f), std::forward<Tu>(tu)}; }

    template <int r, typename Lambda, typename Tu> struct _bra {
      static_assert(r > 1, "Internal error in calling make_lazy_bracket : rank is incorrect");
      Lambda f;
      Tu tu;
      template <typename U> decltype(auto) operator[](U const &u) { return make_bra<r - 1>(f, std::tuple_cat(tu, std::tie(u))); }
      template <typename T> void operator=(T &&) = delete;
      _bra &operator=(_bra const &)              = delete;
    };

    template <typename Lambda, typename Tu> struct _bra<1, Lambda, Tu> {
      Lambda f;
      Tu tu;
      template <typename U> decltype(auto) operator[](U const &u) { return triqs::tuple::apply(f, std::tuple_cat(tu, std::tie(u))); }
      template <typename T> void operator=(T &&) = delete;
      _bra &operator=(_bra const &)              = delete;
    };

    template <int NArgs, typename Lambda, typename T> decltype(auto) _make_lazy_bracket(Lambda &&f, T const &x, std::false_type) {
      return details::make_bra<NArgs - 1>(std::forward<Lambda>(f), std::tie(x));
    }

    template <int NArgs, typename Lambda, typename T> decltype(auto) _make_lazy_bracket(Lambda &&f, T const &x, std::true_type) {
      return std::forward<Lambda>(f)(x);
    }

  } // namespace details

  /**
   * @ingroup triqs-utility-other
   * @brief Invoke a callable lazily by accumulating arguments through chained `operator[]` calls.
   * 
   * @warning This is unused. It might be removed in the future.
   * 
   * @tparam NArgs Total number of expected arguments.
   * @tparam Lambda Callable type.
   * @tparam T Type of the first argument.
   * @param f Callable to invoke once all indices are collected.
   * @param x First argument.
   * @return Either `f(x)` (when `NArgs == 1`) or a lazy object expecting more `operator[]` calls.
   */
  template <int NArgs, typename Lambda, typename T> decltype(auto) make_lazy_bracket(Lambda &&f, T const &x) {
    return details::_make_lazy_bracket<NArgs>(std::forward<Lambda>(f), x, std::integral_constant<bool, (NArgs == 1)>{});
  }

} // namespace triqs::utility
