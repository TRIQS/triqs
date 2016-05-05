/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
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

namespace triqs {
namespace gfs {

 // -------------------------------   Map --------------------------------------------------
 // map takes a function f, a block_gf or its view g
 // then it computes f(g[i]) for all i
 // If the result of f is :
 //  * a gf             : then map returns a block_gf
 //  * a gf_view        : then map returns a block_gf_view
 //  * a gf_const_view  : then map returns a block_gf_const_view
 //  * otherwise        : then map returns a std::vector<>
 namespace impl {

  template <typename F, typename T> auto _map(F &&f, std::vector<T> const &V) {
   std::vector<std14::result_of_t<F(T)>> res;
   res.reserve(V.size());
   for (auto &x : V) res.emplace_back(f(x));
   return res;
  }

  template <typename F, typename T> auto _map(F &&f, std::vector<T> &V) {
   std::vector<std14::result_of_t<F(T)>> res;
   res.reserve(V.size());
   for (auto &x : V) res.emplace_back(f(x));
   return res;
  }

  template <typename F, typename T>
  auto _map(F &&f, std::vector<std::vector<T>> const &V) {
   std::vector<std::vector<std14::result_of_t<F(T)>>> res;
   res.reserve(V.size());
   for (auto &x : V) res.push_back(_map(f,x));
   return res;
  }

  template <typename F, typename T>
  auto _map(F &&f, std::vector<std::vector<T>> &V) {
   std::vector<std::vector<std14::result_of_t<F(T)>>> res;
   res.reserve(V.size());
   for (auto &x : V) res.push_back(_map(f,x));
   return res;
  }

  // implementation is dispatched according to R
  template <typename F, typename G, typename R = std14::decay_t<std14::result_of_t<F(typename std14::decay_t<G>::g_t)>>>
  struct map;

  // general case
  template <typename F, typename G, typename R> struct map {
   static auto invoke(F &&f, G &&g) {return _map(std::forward<F>(f), std::forward<G>(g).data());}
  };

  // now , when R is a gf, gf_view, a gf_const_view
  template <typename F, typename G, typename... T> struct map<F, G, gf<T...>> {
   static auto invoke(F &&f, G &&g)
       {return make_block_gf(g.block_names(), _map(std::forward<F>(f), std::forward<G>(g).data()));}
  };

  template <typename F, typename G, typename... T> struct map<F, G, gf_view<T...>> {
   static auto invoke(F &&f, G &&g)
       {return make_block_gf_view(g.block_names(), _map(std::forward<F>(f), std::forward<G>(g).data()));}
  };

  template <typename F, typename G, typename... T> struct map<F, G, gf_const_view<T...>> {
   static auto invoke(F &&f, G &&g)
       {return make_block_gf_const_view(g.block_names(), _map(std::forward<F>(f), std::forward<G>(g).data()));}
  };
 }

 template <typename F, typename G> auto map_block_gf(F &&f, G &&g) {
  static_assert(is_block_gf_or_view<G>::value, "map_block_gf requires a block gf");
  return impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g));
 }

 // the map function itself...
 template <typename F, typename G>
 auto map(F &&f, G &&g)
     -> std14::enable_if_t<is_block_gf_or_view<G>::value,
                           decltype(impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g)))> {
  return impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g));
 }
}
}

