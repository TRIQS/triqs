/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include <tuple>
//#include <experimental/tuple>
#include "./tuple_tools.hpp"

namespace triqs {
 namespace utility {

#ifdef __cpp_if_constexpr

  namespace details {

   // a little struct to delay [][][][] and rewrite it.
   template <int r, typename Lambda, typename Tu> struct _bra;

   template <int r, typename F, typename Tu> _bra<r, F, Tu> make_bra(F&& f, Tu&& tu) {
    return {std::forward<F>(f), std::forward<Tu>(tu)};
   }

   template <int r, typename Lambda, typename Tu> struct _bra {
    Lambda f;
    Tu tu;
    template <typename U> decltype(auto) operator[](U const& u) {
     if constexpr (r == 1)
      return std::apply(f, std::tuple_cat(tu, std::tie(u)));
     else
      return make_bra<r - 1>{f, std::tuple_cat(tu, std::tie(u))};
    }
   };

   /**
      * @tparam r The total number of [][] expected
      * @param f The lambda to be called with the accumulated arguments
      *
      */
   template <int NArgs, typename Lambda, typename T> decltype(auto) make_lazy_bracket(Lambda&& f, T const& x) {
    if constexpr (NArgs == 1)
     return f(x);
    else
     return details::make_bra<NArgs - 1>(std::forward<Lambda>(f), std::tie(x));
   }
#else
  namespace details {

   // a little struct to delay [][][][] and rewrite it.
   template <int r, typename Lambda, typename Tu> struct _bra;

   template <int r, typename F, typename Tu> _bra<r, F, Tu> make_bra(F&& f, Tu&& tu) {
    return {std::forward<F>(f), std::forward<Tu>(tu)};
   }

   template <int r, typename Lambda, typename Tu> struct _bra {
    static_assert(r > 1, "Internal error in calling make_lazy_bracket : rank is incorrect");
    Lambda f;
    Tu tu;
    template <typename U> decltype(auto) operator[](U const& u) { return make_bra<r - 1>(f, std::tuple_cat(tu, std::tie(u))); }
    template<typename T> void operator=(T&&) = delete; // IF YOU SEE THIS, YOU PROBABLY FORGOT A []
    _bra & operator=(_bra const &) = delete;
   };

   // r=1 is special
   template <typename Lambda, typename Tu> struct _bra<1, Lambda, Tu> {
    Lambda f;
    Tu tu;
    template <typename U> decltype(auto) operator[](U const& u) {
     return triqs::tuple::apply(f, std::tuple_cat(tu, std::tie(u)));
    }
    template<typename T> void operator=(T&&) = delete;  // IF YOU SEE THIS, YOU PROBABLY FORGOT A []
    _bra & operator=(_bra const &) = delete;
   };

   // dispatch the constexpr
   template <int NArgs, typename Lambda, typename T> decltype(auto) _make_lazy_bracket(Lambda&& f, T const& x, std::false_type) {
    return details::make_bra<NArgs - 1>(std::forward<Lambda>(f), std::tie(x));
   }

   template <int NArgs, typename Lambda, typename T> decltype(auto) _make_lazy_bracket(Lambda&& f, T const& x, std::true_type) {
    return f(x);
   }

  } // namespace details

  /**
    * @tparam r The total number of [][] expected
    * @param f The lambda to be called with the accumulated arguments
    *
    */
  template <int NArgs, typename Lambda, typename T> decltype(auto) make_lazy_bracket(Lambda&& f, T const& x) {
   return details::_make_lazy_bracket<NArgs>(std::forward<Lambda>(f), x, std::integral_constant<bool, (NArgs == 1)>{});
  }
#endif
  } // namespace utility
 }  // namespace triqs
