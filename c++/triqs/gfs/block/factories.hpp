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

#pragma once

#include "./block_gf.hpp"

namespace triqs::gfs {

  // -------------------------------   Free Factories for regular type  --------------------------------------------------

  ///
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(int n, gf<V, T, L> const &g) { return {n, g}; }

  ///
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(std::vector<gf<V, T, L>> v) { return {std::move(v)}; }

  ///
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(std::initializer_list<gf<V, T, L>> const &v) { return {v}; }

  ///
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(std::vector<std::string> const &b, gf<V, T, L> const &g) {
    return {b, g};
  }

  ///
  template <typename V, typename T, typename L> block_gf<V, T, L> make_block_gf(std::vector<std::string> const &b, std::vector<gf<V, T, L>> v) {
    return {b, std::move(v)};
  }

  ///
  template <typename V, typename T, typename L>
  block_gf<V, T, L> make_block_gf(std::vector<std::string> b, std::initializer_list<gf<V, T, L>> const &v) {
    return {b, v};
  }

  // -------------------------------   Free Factories for block_gf_view and block_gf_const_view

  /// Make a block_gf_view from the G. Indices are '0', '1', '2', ....
  template <typename G0, typename... G> block_gf_view_of<G0> make_block_gf_view(G0 &&g0, G &&...g) {
    return {details::_make_block_names1(sizeof...(G) + 1), {std::forward<G0>(g0), std::forward<G>(g)...}};
  }

  // Create block_gf_view from vector of views
  template <typename Gf> block_gf_view_of<Gf> make_block_gf_view(std::vector<Gf> &v) {
    static_assert(Gf::is_view);
    return {details::_make_block_names1(v.size()), v};
  }
  template <typename Gf> block_gf_view_of<Gf> make_block_gf_view(std::vector<Gf> &&v) {
    static_assert(Gf::is_view);
    return {details::_make_block_names1(v.size()), std::move(v)};
  }

  // Create block_gf_view from block_names and vector of views
  template <typename Gf> block_gf_view_of<Gf> make_block_gf_view(std::vector<std::string> b, std::vector<Gf> &v) {
    static_assert(Gf::is_view);
    return {std::move(b), v};
  }
  template <typename Gf> block_gf_view_of<Gf> make_block_gf_view(std::vector<std::string> b, std::vector<Gf> &&v) {
    static_assert(Gf::is_view);
    return {std::move(b), std::move(v)};
  }

  // --- same with const_view

  template <typename G0, typename... G> block_gf_const_view_of<G0> make_block_gf_const_view(G0 &&g0, G &&...g) {
    return {details::_make_block_names1(sizeof...(G) + 1), {std::forward<G0>(g0), std::forward<G>(g)...}};
  }

  // Create block_gf_const_view from vector of views
  template <typename Gf> block_gf_const_view_of<Gf> make_block_gf_const_view(std::vector<Gf> &v) {
    static_assert(Gf::is_view);
    return {details::_make_block_names1(v.size()), v};
  }
  template <typename Gf> block_gf_const_view_of<Gf> make_block_gf_const_view(std::vector<Gf> &&v) {
    static_assert(Gf::is_view);
    return {details::_make_block_names1(v.size()), std::move(v)};
  }

  // Create block_gf_const_view from block_names and vector of views
  template <typename Gf> block_gf_const_view_of<Gf> make_block_gf_const_view(std::vector<std::string> b, std::vector<Gf> &v) {
    static_assert(Gf::is_view);
    return {std::move(b), v};
  }
  template <typename Gf> block_gf_const_view_of<Gf> make_block_gf_const_view(std::vector<std::string> b, std::vector<Gf> &&v) {
    static_assert(Gf::is_view);
    return {std::move(b), std::move(v)};
  }

  // -------------------------------   Free Factories for block2_gf   --------------------------------------------------

  /// From the size n x p and the g from a number and a gf to be copied
  template <typename V, typename T, typename L> block2_gf<V, T, L> make_block2_gf(int n, int p, gf<V, T, L> const &g) { return {n, p, g}; }

  // from vector<tuple<string,string>>, vector<gf>
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

  // Create block2_gf_view from vector of views
  template <typename Gf> block2_gf_view_of<Gf> make_block2_gf_view(std::vector<std::vector<Gf>> &v) {
    static_assert(Gf::is_view);
    if (v.size() == 0) return {details::_make_block_names2(0, 0), v};
    return {details::_make_block_names2(v.size(), v[0].size()), v};
  }
  template <typename Gf> block2_gf_view_of<Gf> make_block2_gf_view(std::vector<std::vector<Gf>> &&v) {
    static_assert(Gf::is_view);
    if (v.size() == 0) return {details::_make_block_names2(0, 0), v};
    return {details::_make_block_names2(v.size(), v[0].size()), std::move(v)};
  }

  // Create block2_gf_view from block_names and vector of views
  template <typename Gf>
  block2_gf_view_of<Gf> make_block2_gf_view(std::vector<std::string> block_names1, std::vector<std::string> block_names2,
                                            std::vector<std::vector<Gf>> &v) {
    static_assert(Gf::is_view);
    return {{std::move(block_names1), std::move(block_names2)}, v};
  }
  template <typename Gf>
  block2_gf_view_of<Gf> make_block2_gf_view(std::vector<std::string> block_names1, std::vector<std::string> block_names2,
                                            std::vector<std::vector<Gf>> &&v) {
    static_assert(Gf::is_view);
    return {{std::move(block_names1), std::move(block_names2)}, std::move(v)};
  }

  // --- same for const_view

  // Create block2_gf_const_view from vector of views
  template <typename Gf> block2_gf_const_view_of<Gf> make_block2_gf_const_view(std::vector<std::vector<Gf>> &v) {
    static_assert(Gf::is_view);
    if (v.size() == 0) return {details::_make_block_names2(0, 0), v};
    return {details::_make_block_names2(v.size(), v[0].size()), v};
  }
  template <typename Gf> block2_gf_const_view_of<Gf> make_block2_gf_const_view(std::vector<std::vector<Gf>> &&v) {
    static_assert(Gf::is_view);
    if (v.size() == 0) return {details::_make_block_names2(0, 0), v};
    return {details::_make_block_names2(v.size(), v[0].size()), std::move(v)};
  }

  // Create block2_gf_const_view from block_names and vector of views
  template <typename Gf>
  block2_gf_const_view_of<Gf> make_block2_gf_const_view(std::vector<std::string> block_names1, std::vector<std::string> block_names2,
                                                        std::vector<std::vector<Gf>> &v) {
    static_assert(Gf::is_view);
    return {{std::move(block_names1), std::move(block_names2)}, v};
  }
  template <typename Gf>
  block2_gf_const_view_of<Gf> make_block2_gf_const_view(std::vector<std::string> block_names1, std::vector<std::string> block_names2,
                                                        std::vector<std::vector<Gf>> &&v) {
    static_assert(Gf::is_view);
    return {{std::move(block_names1), std::move(block_names2)}, std::move(v)};
  }
} // namespace triqs::gfs
