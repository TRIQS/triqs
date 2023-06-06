// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

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
        std::vector<std::invoke_result_t<F, T>> res;
        res.reserve(V.size());
        for (auto &x : V) res.emplace_back(f(x));
        return res;
      }

      template <typename F, typename T> auto _map(F &&f, std::vector<T> &V) {
        std::vector<std::invoke_result_t<F, T>> res;
        res.reserve(V.size());
        for (auto &x : V) res.emplace_back(f(x));
        return res;
      }

      template <typename F, typename T> auto _map(F &&f, std::vector<std::vector<T>> const &V) {
        std::vector<std::vector<std::invoke_result_t<F, T>>> res;
        res.reserve(V.size());
        for (auto &x : V) res.push_back(_map(f, x));
        return res;
      }

      template <typename F, typename T> auto _map(F &&f, std::vector<std::vector<T>> &V) {
        std::vector<std::vector<std::invoke_result_t<F, T>>> res;
        res.reserve(V.size());
        for (auto &x : V) res.push_back(_map(f, x));
        return res;
      }

      // implementation is dispatched according to R
      template <typename F, typename G, typename R = std::decay_t<std::invoke_result_t<F, typename std::decay_t<G>::g_t>>> struct map;

      // general case
      template <typename F, typename G, typename R> struct map {
        static auto invoke(F &&f, G &&g) { return _map(std::forward<F>(f), std::forward<G>(g).data()); }
      };

      // now , when R is a gf, gf_view, a gf_const_view
      template <typename F, typename G, typename... T> struct map<F, G, gf<T...>> {
        static auto invoke(F &&f, G &&g) {
          if constexpr (std::remove_reference_t<G>::arity == 1)
            return make_block_gf(g.block_names(), _map(std::forward<F>(f), std::forward<G>(g).data()));
          else
            return make_block2_gf(g.block_names()[0], g.block_names()[1], _map(std::forward<F>(f), std::forward<G>(g).data()));
        }
      };

      template <typename F, typename G, typename... T> struct map<F, G, gf_view<T...>> {
        static auto invoke(F &&f, G &&g) {
          if constexpr (std::remove_reference_t<G>::arity == 1)
            return make_block_gf_view(g.block_names(), _map(std::forward<F>(f), std::forward<G>(g).data()));
          else
            return make_block2_gf_view(g.block_names()[0], g.block_names()[1], _map(std::forward<F>(f), std::forward<G>(g).data()));
        }
      };

      template <typename F, typename G, typename... T> struct map<F, G, gf_const_view<T...>> {
        static auto invoke(F &&f, G &&g) {
          if constexpr (std::remove_reference_t<G>::arity == 1)
            return make_block_gf_const_view(g.block_names(), _map(std::forward<F>(f), std::forward<G>(g).data()));
          else
            return make_block2_gf_const_view(g.block_names()[0], g.block_names()[1], _map(std::forward<F>(f), std::forward<G>(g).data()));
        }
      };
    } // namespace impl

    template <typename F, typename G> auto map_block_gf(F &&f, G &&g) {
      static_assert(is_block_gf_v<std::decay_t<G>>, "map_block_gf requires a block gf");
      return impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g));
    }

    // the map function itself...
    template <typename F, typename G>
    auto map(F &&f, G &&g)
      requires(is_block_gf_v<std::decay_t<G>>)
    {
      return impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g));
    }
  } // namespace gfs
} // namespace triqs
