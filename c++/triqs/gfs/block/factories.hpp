// Copyright (c) 2020 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet

/**
 * @file
 * @brief Provides the free factory functions for block Green's functions.
 */

#pragma once

#include "./block_gf.hpp"
#include "../../utility/exceptions.hpp"

#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-factories
   * @{
   */

  // -------------------------------   Free Factories for regular type  --------------------------------------------------

  /**
   * @brief Make a triqs::gfs::block_gf of `n` copies of a Green's function (block names default to "0", "1", ...).
   * 
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type.
   * @param n Number of blocks.
   * @param g Green's function to copy into each block.
   * @return The block Green's function.
   */
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(int n, gf<V, T, L> const &g) { return {n, g}; }

  /**
   * @brief Make a triqs::gfs::block_gf from a vector of Green's functions (block names default to "0", "1", ...).
   * 
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type.
   * @param v Blocks (Green's functions).
   * @return The block Green's function.
   */
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(std::vector<gf<V, T, L>> v) { return {std::move(v)}; }

  /**
   * @brief Make a triqs::gfs::block_gf from an initializer list of Green's functions (block names default to "0", ...).
   * 
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type.
   * @param v Blocks (Green's functions).
   * @return The block Green's function.
   */
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(std::initializer_list<gf<V, T, L>> const &v) { return {v}; }

  /**
   * @brief Make a triqs::gfs::block_gf from block names and one Green's function copied into every block.
   * 
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type.
   * @param b Block names.
   * @param g Green's function to copy into each block.
   * @return The block Green's function.
   */
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(std::vector<std::string> const &b, gf<V, T, L> const &g) {
    return {b, g};
  }

  /**
   * @brief Make a triqs::gfs::block_gf from block names and a vector of Green's functions.
   * 
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type.
   * @param b Block names.
   * @param v Blocks (Green's functions).
   * @return The block Green's function.
   */
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(std::vector<std::string> const &b, std::vector<gf<V, T, L>> v) {
    return {b, std::move(v)};
  }

  /**
   * @brief Make a triqs::gfs::block_gf from block names and an initializer list of Green's functions.
   * 
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type.
   * @param b Block names.
   * @param v Blocks (Green's functions).
   * @return The block Green's function.
   */
  template <typename V, typename T, typename L>
  block_gf<V, T, L> make_block_gf(std::vector<std::string> b, std::initializer_list<gf<V, T, L>> const &v) {
    return {b, v};
  }

  // -------------------------------   Free Factories for block_gf_view and block_gf_const_view

  /**
   * @brief Make a triqs::gfs::block_gf_view from a list of Green's function views (block names default to "0", "1", 
   * ...).
   * 
   * @tparam G0 Type of the first view. @tparam G Types of the remaining views.
   * @param g0 First Green's function (view).
   * @param g Remaining Green's functions (views).
   * @return The block Green's function view.
   */
  template <typename G0, typename... G> block_gf_view_of<G0> make_block_gf_view(G0 &&g0, G &&...g) {
    return {details::_make_block_names1(sizeof...(G) + 1), {std::forward<G0>(g0), std::forward<G>(g)...}};
  }

  /**
   * @brief Make a triqs::gfs::block_gf_view from a vector of views (block names default to "0", "1", ...).
   * 
   * @tparam Gf View type (must be a view).
   * @param v Vector of Green's function views.
   * @return The block Green's function view.
   */
  template <typename Gf> block_gf_view_of<Gf> make_block_gf_view(std::vector<Gf> &v) {
    static_assert(Gf::is_view);
    return {details::_make_block_names1(v.size()), v};
  }

  /// Rvalue overload of make_block_gf_view from a vector of views.
  template <typename Gf> block_gf_view_of<Gf> make_block_gf_view(std::vector<Gf> &&v) {
    static_assert(Gf::is_view);
    return {details::_make_block_names1(v.size()), std::move(v)};
  }

  /**
   * @brief Make a triqs::gfs::block_gf_view from block names and a vector of views.
   * 
   * @tparam Gf View type (must be a view).
   * @param b Block names.
   * @param v Vector of Green's function views.
   * @return The block Green's function view.
   */
  template <typename Gf> block_gf_view_of<Gf> make_block_gf_view(std::vector<std::string> b, std::vector<Gf> &v) {
    static_assert(Gf::is_view);
    return {std::move(b), v};
  }

  /// Rvalue overload of make_block_gf_view from block names and a vector of views.
  template <typename Gf> block_gf_view_of<Gf> make_block_gf_view(std::vector<std::string> b, std::vector<Gf> &&v) {
    static_assert(Gf::is_view);
    return {std::move(b), std::move(v)};
  }

  // --- same with const_view

  /**
   * @brief Make a triqs::gfs::block_gf_const_view from a list of Green's function views (block names default to "0",
   * ...).
   * 
   * @tparam G0 Type of the first view. @tparam G Types of the remaining views.
   * @param g0 First Green's function (view).
   * @param g Remaining Green's functions (views).
   * @return The block Green's function const view.
   */
  template <typename G0, typename... G> block_gf_const_view_of<G0> make_block_gf_const_view(G0 &&g0, G &&...g) {
    return {details::_make_block_names1(sizeof...(G) + 1), {std::forward<G0>(g0), std::forward<G>(g)...}};
  }

  /**
   * @brief Make a triqs::gfs::block_gf_const_view from a vector of views (block names default to "0", "1", ...).
   * 
   * @tparam Gf View type (must be a view).
   * @param v Vector of Green's function views.
   * @return The block Green's function const view.
   */
  template <typename Gf> block_gf_const_view_of<Gf> make_block_gf_const_view(std::vector<Gf> &v) {
    static_assert(Gf::is_view);
    return {details::_make_block_names1(v.size()), v};
  }

  /// Rvalue overload of make_block_gf_const_view from a vector of views.
  template <typename Gf> block_gf_const_view_of<Gf> make_block_gf_const_view(std::vector<Gf> &&v) {
    static_assert(Gf::is_view);
    return {details::_make_block_names1(v.size()), std::move(v)};
  }

  /**
   * @brief Make a triqs::gfs::block_gf_const_view from block names and a vector of views.
   * 
   * @tparam Gf View type (must be a view).
   * @param b Block names.
   * @param v Vector of Green's function views.
   * @return The block Green's function const view.
   */
  template <typename Gf> block_gf_const_view_of<Gf> make_block_gf_const_view(std::vector<std::string> b, std::vector<Gf> &v) {
    static_assert(Gf::is_view);
    return {std::move(b), v};
  }

  /// Rvalue overload of make_block_gf_const_view from block names and a vector of views.
  template <typename Gf> block_gf_const_view_of<Gf> make_block_gf_const_view(std::vector<std::string> b, std::vector<Gf> &&v) {
    static_assert(Gf::is_view);
    return {std::move(b), std::move(v)};
  }

  // -------------------------------   Free Factories for block2_gf   --------------------------------------------------

  /**
   * @brief Make a triqs::gfs::block2_gf of `n` x `p` copies of a Green's function.
   * 
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type.
   * @param n Number of blocks along the first index.
   * @param p Number of blocks along the second index.
   * @param g Green's function to copy into each block.
   * @return The two-index block Green's function.
   */
  template <typename V, typename T, typename L> block2_gf<V, T, L> make_block2_gf(int n, int p, gf<V, T, L> const &g) { return {n, p, g}; }

  /**
   * @brief Make a triqs::gfs::block2_gf from two lists of block names and a matrix of Green's functions.
   *
   * @details The outer and inner vector sizes must match the two block-name lists (throws a triqs::runtime_error
   * otherwise).
   *
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type.
   * @param block_names1 Block names along the first index.
   * @param block_names2 Block names along the second index.
   * @param vv Blocks (vector of vectors of Green's functions).
   * @return The two-index block Green's function.
   */
  template <typename V, typename T, typename L>
  block2_gf<V, T, L> make_block2_gf(std::vector<std::string> const &block_names1, std::vector<std::string> const &block_names2,
                                    std::vector<std::vector<gf<V, T, L>>> vv) {
    if (block_names1.size() != vv.size())
      TRIQS_RUNTIME_ERROR << "make_block2_gf(vector<string>, vector<string>>, vector<vector<gf>>): incompatible outer vector size!";
    for (auto const &v : vv) {
      if (block_names2.size() != v.size())
        TRIQS_RUNTIME_ERROR << "make_block2_gf(vector<string>, vector<string>>, vector<vector<gf>>): incompatible inner vector size!";
    }
    return {{block_names1, block_names2}, std::move(vv)};
  }

  // -------------------------------   Free Factories for block2_gf_view and block2_gf_const_view  --------------------------------------------------

  /**
   * @brief Make a triqs::gfs::block2_gf_view from a matrix of views (block names default to "0", "1", ...).
   * 
   * @tparam Gf View type (must be a view).
   * @param v Blocks (vector of vectors of views).
   * @return The two-index block Green's function view.
   */
  template <typename Gf> block2_gf_view_of<Gf> make_block2_gf_view(std::vector<std::vector<Gf>> &v) {
    static_assert(Gf::is_view);
    if (v.size() == 0) return {details::_make_block_names2(0, 0), v};
    return {details::_make_block_names2(v.size(), v[0].size()), v};
  }

  /// Rvalue overload of make_block2_gf_view from a matrix of views.
  template <typename Gf> block2_gf_view_of<Gf> make_block2_gf_view(std::vector<std::vector<Gf>> &&v) {
    static_assert(Gf::is_view);
    if (v.size() == 0) return {details::_make_block_names2(0, 0), v};
    return {details::_make_block_names2(v.size(), v[0].size()), std::move(v)};
  }

  /**
   * @brief Make a triqs::gfs::block2_gf_view from two lists of block names and a matrix of views.
   * 
   * @tparam Gf View type (must be a view).
   * @param block_names1 Block names along the first index.
   * @param block_names2 Block names along the second index.
   * @param v Blocks (vector of vectors of views).
   * @return The two-index block Green's function view.
   */
  template <typename Gf>
  block2_gf_view_of<Gf> make_block2_gf_view(std::vector<std::string> block_names1, std::vector<std::string> block_names2,
                                            std::vector<std::vector<Gf>> &v) {
    static_assert(Gf::is_view);
    return {{std::move(block_names1), std::move(block_names2)}, v};
  }

  /// Rvalue overload of make_block2_gf_view from two lists of block names and a matrix of views.
  template <typename Gf>
  block2_gf_view_of<Gf> make_block2_gf_view(std::vector<std::string> block_names1, std::vector<std::string> block_names2,
                                            std::vector<std::vector<Gf>> &&v) {
    static_assert(Gf::is_view);
    return {{std::move(block_names1), std::move(block_names2)}, std::move(v)};
  }

  // --- same for const_view

  /**
   * @brief Make a triqs::gfs::block2_gf_const_view from a matrix of views (block names default to "0", "1", ...).
   * 
   * @tparam Gf View type (must be a view).
   * @param v Blocks (vector of vectors of views).
   * @return The two-index block Green's function const view.
   */
  template <typename Gf> block2_gf_const_view_of<Gf> make_block2_gf_const_view(std::vector<std::vector<Gf>> &v) {
    static_assert(Gf::is_view);
    if (v.size() == 0) return {details::_make_block_names2(0, 0), v};
    return {details::_make_block_names2(v.size(), v[0].size()), v};
  }

  /// Rvalue overload of make_block2_gf_const_view from a matrix of views.
  template <typename Gf> block2_gf_const_view_of<Gf> make_block2_gf_const_view(std::vector<std::vector<Gf>> &&v) {
    static_assert(Gf::is_view);
    if (v.size() == 0) return {details::_make_block_names2(0, 0), v};
    return {details::_make_block_names2(v.size(), v[0].size()), std::move(v)};
  }

  /**
   * @brief Make a triqs::gfs::block2_gf_const_view from two lists of block names and a matrix of views.
   * 
   * @tparam Gf View type (must be a view).
   * @param block_names1 Block names along the first index.
   * @param block_names2 Block names along the second index.
   * @param v Blocks (vector of vectors of views).
   * @return The two-index block Green's function const view.
   */
  template <typename Gf>
  block2_gf_const_view_of<Gf> make_block2_gf_const_view(std::vector<std::string> block_names1, std::vector<std::string> block_names2,
                                                        std::vector<std::vector<Gf>> &v) {
    static_assert(Gf::is_view);
    return {{std::move(block_names1), std::move(block_names2)}, v};
  }

  /// Rvalue overload of make_block2_gf_const_view from two lists of block names and a matrix of views.
  template <typename Gf>
  block2_gf_const_view_of<Gf> make_block2_gf_const_view(std::vector<std::string> block_names1, std::vector<std::string> block_names2,
                                                        std::vector<std::vector<Gf>> &&v) {
    static_assert(Gf::is_view);
    return {{std::move(block_names1), std::move(block_names2)}, std::move(v)};
  }

  /** @} */

} // namespace triqs::gfs
