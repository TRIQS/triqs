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
// Authors: Philipp Dumitrescu, Olivier Parcollet

#pragma once

#include <concepts>
#include <ranges>
#include <string>

#include <h5/h5.hpp>

namespace triqs {
  /**
   * @brief True iif T is same_as any U 
   */
  template <typename T, typename... U>
  concept any_of = (std::same_as<T, U> || ...);
}

namespace triqs::mesh {

  /// The type is hashable with std::hash
  template <typename T>
  concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
  };

  // ----------- MeshPoint  ------------------------

  template <typename MP>
  concept MeshPoint = requires(MP const &mp) {
    // The parent mesh type
    typename MP::mesh_t;

    // The associated index, e.g. Matsubara Index or a long
    { mp.idx } -> std::same_as<typename MP::mesh_t::idx_t const &>;

    // The index into the data array
    { mp.datidx } -> std::same_as<typename MP::mesh_t::datidx_t const &>;

    // Hash for easy checking of MeshPoint and Mesh compatibility
    { mp.mesh_hash };
  };

  // ----------- Mesh ------------------------

  /// A mesh is a range of mesh points that can optionally have values, e.g. in [0,beta].
  /// Each mesh point holds an index (e.g. Matsubara index) and a data index for 
  /// corresponding to the Green function data array in memory
  template <typename M>
  concept Mesh = std::regular<M> and h5::Storable<M> and requires(M const &m) {
    
    // Mesh has a mesh point
    typename M::mesh_point_t;
    requires MeshPoint<typename M::mesh_point_t>;

    // Mesh is a range of mesh point
#if not(defined(_LIBCPP_VERSION) and (__clang_major__ < 16))
    // Do NOT check on old compiler ....
    requires std::ranges::forward_range<M>;
    requires std::ranges::sized_range<M>;
#endif
    { *std::begin(m) } -> std::same_as<typename M::mesh_point_t>;

    // The type of the index, e.g. Matsubara Index
    typename M::idx_t;

    // The type of the index indexing the data array
    typename M::datidx_t;
  }
  // we now check some operations combining indices, mesh, mesh_points 
  and requires(M const &m, typename M::idx_t idx, typename M::datidx_t datidx, typename M::mesh_point_t mp) {
    // Validity of the index
    { m.is_idx_valid(idx) } -> std::same_as<bool>;

    // Conversion
    // NB the data idx-> idx could be slow (unflatten) but is necessary in some cases.
    { m.to_datidx(idx) } -> std::same_as<typename M::datidx_t>;         // idx -> data idx
    { m.to_datidx(mp) } -> std::same_as<typename M::datidx_t>;          // mesh point -> data idx
    { m.to_idx(datidx) } -> std::same_as<typename M::idx_t>;            // data idx -> idx
    { m.operator[](datidx) } -> std::same_as<typename M::mesh_point_t>; // data idx -> mesh point
    { m.operator()(idx) } -> std::same_as<typename M::mesh_point_t>;    // idx -> mesh point
  };

  // ------------- MeshWithValues -----------------

  template <typename T> struct closest_mesh_point_t; // Forward declaration

  // Some meshes have points that can take value in a domain, 
  // e.g. a k mesh in a Brillouin zone, k can be a R3 vector.
  template <typename M>
  concept MeshWithValues = Mesh<M> and requires(M const &m) {
    // The value type of the domain
    typename M::value_t;

    // OPFIXME .to_value(idx) in the concept ? 

    // Mesh Points can return their value and are castable
    { (*std::begin(m)).value() } -> any_of<typename M::value_t, typename M::value_t const &>;
    { static_cast<typename M::value_t>(*std::begin(m)) };
  };

} // namespace triqs::mesh
