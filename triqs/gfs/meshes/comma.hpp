/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by O. Parcollet
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
#include <triqs/clef.hpp>

#include "../meshes/mesh_tools.hpp"
// FIXME : include mess
#include "../domains/matsubara.hpp"

namespace triqs {
  namespace gfs {

    using clef::_ph; // placeholder

    /* ---------------------------------------------------------------------------------------------------
  * Tuple of placeholder
  *  --------------------------------------------------------------------------------------------------- */

    template <typename... T> struct tuple_com : std::tuple<T...> { using std::tuple<T...>::tuple; };

    // deduction rule. broken on clang ?
    //    template <typename... T> tuple_com(T&& ...x) -> tuple_com<std::decay_t<T>...>;

    template <typename... T> tuple_com<std::decay_t<T>...> make_tuple_com(T &&... x) { return {std::forward<T>(x)...}; }

    template <typename... T> tuple_com<T...> make_tuple_com_form_tuple(std::tuple<T...> &&x) {
      return std::move(*static_cast<tuple_com<T...> *>(&x));
    }

    // FIXME USE a function  + enable if

    // placeholder, mesh_point a:sorbs argument in a tuple_com
    template <int N, typename X> tuple_com<_ph<N>, std::decay_t<X>> operator,(_ph<N> p, X &&x) { return {p, std::forward<X>(x)}; }

    // all_t
    template <typename X> tuple_com<all_t, std::decay_t<X>> operator,(all_t, X &&x) { return {all_t{}, std::forward<X>(x)}; }
    inline tuple_com<long, all_t> operator,(long i, all_t p) { return {i, p}; }
    //template <typename X> tuple_com<std::decay_t<X>, all_t> operator,(X &&x, all_t) { return {std::forward<X>(x), all_t{}}; }
    //template <typename X> inline tuple_com<all_t, all_t> operator,(all_t, all_t) { return {};}

    template <typename M, typename X> tuple_com<mesh_point<M>, std::decay_t<X>> operator,(mesh_point<M> m, X &&x) {
      return {std::move(m), std::forward<X>(x)};
    }

    template <int N, typename X> tuple_com<long, _ph<N>> operator,(long i, _ph<N> p) { return {i, p}; }
    template <typename M> tuple_com<long, mesh_point<M>> operator,(long i, mesh_point<M> m) { return {i, std::move(m)}; }

    template <typename T, int n, typename X> tuple_com<mini_vector<T, n>, X> operator,(mini_vector<T, n> const &v, X const &x) { return {v, x}; }
    template <typename X> tuple_com<matsubara_freq, X> operator,(matsubara_freq const &m, X const &x) { return {m, x}; }

    // tuple_com absorbs anything
    template <typename X, typename... T, size_t... Is>
    FORCEINLINE tuple_com<T..., std::decay_t<X>> __comma_impl(tuple_com<T...> &&tu, X &&x, std14::index_sequence<Is...>) {
      return {std::get<Is>(std::move(tu))..., std::forward<X>(x)};
    }

    template <typename X, typename... T> FORCEINLINE tuple_com<T..., std::decay_t<X>> operator,(tuple_com<T...> &&t, X &&x) {
      return __comma_impl(std::move(t), std::forward<X>(x), std14::make_index_sequence<sizeof...(T)>());
    }

  } // namespace gfs

  namespace clef {

    // tuple_com is lazy if any of T is.
    template <typename... T> struct is_any_lazy<gfs::tuple_com<T...>> : is_any_lazy<T...> {};

    // The case A[x_,y_] = RHS : we form the function (make_function) and call auto_assign (by ADL)
    template <typename F, typename RHS, int... Is> FORCEINLINE void operator<<(expr<tags::subscript, F, gfs::tuple_com<_ph<Is>...>> &&ex, RHS &&rhs) {
      triqs_clef_auto_assign(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), _ph<Is>()...));
    }

    // tuple_com can be evaluated
    template <typename... T, typename... Contexts> struct evaluator<gfs::tuple_com<T...>, Contexts...> {
      static constexpr bool is_lazy = false;
      FORCEINLINE decltype(auto) operator()(gfs::tuple_com<T...> const &tu, Contexts const &... contexts) const {
        auto l  = [&contexts...](auto &&y) -> decltype(auto) { return eval(y, contexts...); };
        auto _t = triqs::tuple::map(l, static_cast<std::tuple<T...>>(tu));
        return triqs::gfs::make_tuple_com_form_tuple(std::move(_t));
      }
    };

  } // namespace clef
} // namespace triqs
