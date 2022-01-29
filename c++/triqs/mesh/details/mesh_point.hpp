// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Philipp Dumitrescu

#pragma once

#include <ranges>
#include <itertools/itertools.hpp>
#include <tuple>
#include <type_traits>
#include "../mesh_concepts.hpp"
#include "triqs/utility/tuple_tools.hpp"
#include "triqs/utility/arithmetic_ops_by_cast.hpp"
#include <iostream>

namespace triqs::mesh {

  // Generic Implemenation of Mesh Point
  template <typename M> struct mesh_point_impl : public utility::arithmetic_ops_by_cast<mesh_point_impl<M>, typename M::domain_t::point_t> {
    typename M::index_t index_{};
    typename M::domain_t::point_t value_{};
    typename M::linear_index_t linear_index_{};
    std::size_t mesh_hash_{};

    // Need Explicit Constructor because of inheritance for arithmetic_ops
    mesh_point_impl(typename M::index_t index, typename M::domain_pt_t value, typename M::linear_index_t linear_index, size_t mesh_hash)
       : index_(index), linear_index_(linear_index), value_(value), mesh_hash_(mesh_hash) {}

    [[nodiscard]] auto index() const { return index_; }
    [[nodiscard]] auto value() const { return value_; }
    [[nodiscard]] auto linear_index() const { return linear_index_; }
    [[nodiscard]] auto mesh_hash() const { return mesh_hash_; }

    using cast_t = typename M::domain_t::point_t;
    operator cast_t() const { return value(); }
  };

  // ----------------------

  // Generic implementation of iterator generating mesh points

  // A "named" lambda function, capturing a mesh const& and calling linear_to_mesh_pt.
  // This is needed for having a consistent type in range transform.
  template <typename M> struct mesh_point_maker {
    M *mesh = nullptr;
    typename M::mesh_point_t operator()(typename M::linear_index_t const &i) const { return mesh->linear_to_mesh_pt(i); }
  };

// libc++ does not yet implement ranges for now: use itertools
#if defined(_LIBCPP_VERSION) and (__clang_major__ < 14)

  template <typename M> auto make_mesh_range(M &m) { return itertools::transform(itertools::range(0, m.size()), mesh_point_maker<M>{&m}); }

  template <typename M> using make_mesh_range_rtype = itertools::details::transformed<itertools::range, triqs::mesh::mesh_point_maker<M>>;

#else

  template <typename M> auto make_mesh_range(M const &m) {
    return std::ranges::views::iota(0, m.size()) | std::ranges::views::transform(mesh_point_maker<M>{m});
  }

  template <typename M>
  using make_mesh_range_rtype = std::ranges::transform_view<std::ranges::iota_view<M::linear_index_t, M::linear_index_t>, mesh_point_maker<M>>;

#endif

  /// ----------------
  // Product mesh specialization:

  template <typename... Ms> class prod;

  template <typename... Ms> struct mesh_point_maker<prod<Ms...>> {
    prod<Ms...> *mesh = nullptr;
    typename prod<Ms...>::mesh_point_t operator()(std::tuple<typename Ms::mesh_point_t...> const &t_mp) const { return {t_mp}; }
  };

  //  product range not part of c++20 standard
  template <typename... Ms> auto make_mesh_prod_range(prod<Ms...> &m) {
    auto f = [](auto... x) { return itertools::details::multiplied<Ms...>(x...); };
    return itertools::transform(triqs::tuple::apply(f, m.components()), mesh_point_maker<prod<Ms...>>{&m});
  }

  template <typename... Ms>
  using make_mesh_range_prod_rtype =
     itertools::details::transformed<itertools::details::multiplied<Ms...>, triqs::mesh::mesh_point_maker<prod<Ms...>>>;

} // namespace triqs::mesh
