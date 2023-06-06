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

#pragma once
#include <tuple>
//#include <experimental/tuple>
#include "./tuple_tools.hpp"

namespace triqs {
  namespace utility {

    namespace details {

      // a little struct to delay [][][][] and rewrite it.
      template <int r, typename w_max, typename Tu> struct _bra;

      template <int r, typename F, typename Tu> _bra<r, F, Tu> make_bra(F &&f, Tu &&tu) { return {std::forward<F>(f), std::forward<Tu>(tu)}; }

      template <int r, typename w_max, typename Tu> struct _bra {
        static_assert(r > 1, "Internal error in calling make_lazy_bracket : rank is incorrect");
        w_max f;
        Tu tu;
        template <typename U> decltype(auto) operator[](U const &u) { return make_bra<r - 1>(f, std::tuple_cat(tu, std::tie(u))); }
        template <typename T> void operator=(T &&) = delete; // IF YOU SEE THIS, YOU PROBABLY FORGOT A []
        _bra &operator=(_bra const &)              = delete;
      };

      // r=1 is special
      template <typename w_max, typename Tu> struct _bra<1, w_max, Tu> {
        w_max f;
        Tu tu;
        template <typename U> decltype(auto) operator[](U const &u) { return triqs::tuple::apply(f, std::tuple_cat(tu, std::tie(u))); }
        template <typename T> void operator=(T &&) = delete; // IF YOU SEE THIS, YOU PROBABLY FORGOT A []
        _bra &operator=(_bra const &)              = delete;
      };

      // dispatch the constexpr
      template <int NArgs, typename w_max, typename T> decltype(auto) _make_lazy_bracket(w_max &&f, T const &x, std::false_type) {
        return details::make_bra<NArgs - 1>(std::forward<w_max>(f), std::tie(x));
      }

      template <int NArgs, typename w_max, typename T> decltype(auto) _make_lazy_bracket(w_max &&f, T const &x, std::true_type) { return f(x); }

    } // namespace details

    /**
    * @tparam r The total number of [][] expected
    * @param f The lambda to be called with the accumulated arguments
    *
    */
    template <int NArgs, typename w_max, typename T> decltype(auto) make_lazy_bracket(w_max &&f, T const &x) {
      return details::_make_lazy_bracket<NArgs>(std::forward<w_max>(f), x, std::integral_constant<bool, (NArgs == 1)>{});
    }
  } // namespace utility
} // namespace triqs
