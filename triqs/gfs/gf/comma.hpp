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

namespace triqs {
  namespace gfs {

    using clef::_ph; // placeholder

    /// the dummy variable
    struct var_t {};

    /* ---------------------------------------------------------------------------------------------------
  * Tuple of placeholder 
  *  --------------------------------------------------------------------------------------------------- */

    template <typename... T> struct _tuple { std::tuple<T...> _t; };
    template <typename... T> _tuple<T...> _make_tuple(std::tuple<T...> &&_t) { return {_t}; }

    // placeholder, mesh_point and var_t absorbs argument in a _tuple
    template <int N, typename X> _tuple<_ph<N>, std::decay_t<X>> operator,(_ph<N> p, X &&x) { return {std::make_tuple(p, std::forward<X>(x))}; }

    template <typename X> _tuple<var_t, std::decay_t<X>> operator,(var_t, X &&x) { return {std::make_tuple(var_t{}, std::forward<X>(x))}; }

    template <typename M, typename X> _tuple<mesh_point<M>, std::decay_t<X>> operator,(mesh_point<M> m, X &&x) {
      return {std::make_tuple(std::move(m), std::forward<X>(x))};
    }
    
    template <int N, typename X> _tuple<long, _ph<N>> operator,(long i, _ph<N> p) { return {std::make_tuple(i, p)}; }
    inline _tuple<long, var_t> operator,(long i, var_t p) { return {std::make_tuple(i, p)}; }
    template <typename M> _tuple<long, mesh_point<M>> operator,(long i, mesh_point<M> m) { return {std::make_tuple(i, std::move(m))}; }

    template <typename T, int n, typename X> _tuple<mini_vector<T,n>, X> operator,(mini_vector<T,n> const & v, X const &x) { return {std::make_tuple(v,x)}; } 
    template <typename X> _tuple<matsubara_freq, X> operator,(matsubara_freq const & m, X const &x) { return {std::make_tuple(m,x)}; } 

    // _tuple absorbs anything
    template <typename X, typename... T, size_t... Is>
    FORCEINLINE _tuple<T..., std::decay_t<X>> __comma_impl(_tuple<T...> &&tu, X &&x, std14::index_sequence<Is...>) {
      return {std::make_tuple(std::get<Is>(std::move(tu._t))..., std::forward<X>(x))};
    }

    template <typename X, typename... T> FORCEINLINE _tuple<T..., std::decay_t<X>> operator,(_tuple<T...> &&t, X &&x) {
      return __comma_impl(std::move(t), std::forward<X>(x), std14::make_index_sequence<sizeof...(T)>());
    }

  } // namespace gfs

  namespace clef {

    // _tuple is lazy if any of T is.
    template <typename... T> struct is_any_lazy<gfs::_tuple<T...>> : is_any_lazy<T...> {};

    // The case A[x_,y_] = RHS : we form the function (make_function) and call auto_assign (by ADL)
    template <typename F, typename RHS, int... Is> FORCEINLINE void operator<<(expr<tags::subscript, F, gfs::_tuple<_ph<Is>...>> &&ex, RHS &&rhs) {
      triqs_clef_auto_assign(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), _ph<Is>()...));
    }

    // _tuple can be evaluated
    template <typename... T, typename... Contexts> struct evaluator<gfs::_tuple<T...>, Contexts...> {
      static constexpr bool is_lazy = false;
      FORCEINLINE decltype(auto) operator()(gfs::_tuple<T...> const &tu, Contexts const &... contexts) const {
        auto l  = [&contexts...](auto &&y) -> decltype(auto) { return eval(y, contexts...); };
        auto _t = triqs::tuple::map(l, tu._t);
        return triqs::gfs::_make_tuple(std::move(_t));
      }
    };

  } // namespace clef
} // namespace triqs
