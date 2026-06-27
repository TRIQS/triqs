// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides the function applying a callable block by block to a block Green's function.
 */

#pragma once

#include "./factories.hpp"

#include <type_traits>
#include <utility>
#include <vector>

namespace triqs::gfs {

  // -------------------------------   Map --------------------------------------------------
  // map takes a function f, a block_gf or its view g
  // then it computes f(g[i]) for all i
  // If the result of f is :
  //  * a gf             : then map returns a block_gf
  //  * a gf_view        : then map returns a block_gf_view
  //  * a gf_const_view  : then map returns a block_gf_const_view
  //  * otherwise        : then map returns a std::vector<>
  namespace impl {

    // NOLINTBEGIN(cppcoreguidelines-missing-std-forward,cppcoreguidelines-rvalue-reference-param-not-moved): f is applied
    // once per element (so it cannot be forwarded); in `invoke`, F/G are class-template parameters that collapse to
    // lvalue refs for lvalue operands, so std::forward (not std::move) is the correct cast.

    // Apply f to each element of a (const) vector, collecting the results in a new vector.
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

    // Apply f to each element of a (const) vector of vectors, collecting the results in a new vector of vectors.
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

    // Dispatch the result type R of f: build a block_gf(_view) when R is a gf(_view), else a std::vector.
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
    // NOLINTEND(cppcoreguidelines-missing-std-forward,cppcoreguidelines-rvalue-reference-param-not-moved)
  } // namespace impl

  /**
   * @ingroup triqs-gfs-map
   * @brief Apply a callable to each block of a block Green's function.
   *
   * @details Computes `f(g[i])` for every block. The return type follows the result of `f`: a triqs::gfs::block_gf if
   * `f` returns a triqs::gfs::gf, a triqs::gfs::block_gf_view / triqs::gfs::block_gf_const_view if it returns a view,
   * and a `std::vector` otherwise.
   *
   * @tparam F Type of the callable.
   * @tparam G Type of the block Green's function.
   * @param f Callable applied to each block.
   * @param g Block Green's function (or view).
   * @return The block-wise result (see details).
   */
  template <typename F, typename G> auto map_block_gf(F &&f, G &&g) {
    static_assert(is_block_gf_v<G>, "map_block_gf requires a block gf");
    return impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g));
  }

  /**
   * @ingroup triqs-gfs-map
   * @brief Apply a callable to each block of a block Green's function (alias of triqs::gfs::map_block_gf).
   *
   * @tparam F Type of the callable.
   * @tparam G Type of the block Green's function.
   * @param f Callable applied to each block.
   * @param g Block Green's function (or view).
   * @return The block-wise result (see triqs::gfs::map_block_gf).
   */
  template <typename F, typename G>
  auto map(F &&f, G &&g)
    requires(is_block_gf_v<G>)
  {
    return impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g));
  }

} // namespace triqs::gfs
