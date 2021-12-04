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
#include <type_traits>
#include "../mesh_concepts.hpp"
#include <iostream>

namespace triqs::mesh {

  // Generic Implemenation of Mesh Point. Can be specialized for specific meshes.
  template <typename M> struct mesh_point {
    typename M::index_t index{};
    typename M::linear_index_t linear_index{};
    typename M::domain_t::point_t value{};
    std::size_t mesh_hash{};
  };

  // ----------------------

  // Generic implementation of iterator generating mesh points

  // A "named" lambda function, capturing a mesh const& and calling linear_to_mesh_pt.
  // This is needed for having a consistent type in range transform.
  template <typename M> struct mesh_point_maker {
    M *mesh = nullptr;
    mesh_point<M> operator()(typename M::linear_index_t const &i) const { return mesh->linear_to_mesh_pt(i); }
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

} // namespace triqs::mesh
