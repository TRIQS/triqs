// Copyright (c) 2022-2023 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides concepts for mesh points and meshes.
 */

#pragma once

#include <h5/h5.hpp>
#include <nda/concepts.hpp>

#include <cstdint>
#include <concepts>
#include <cstdint>
#include <ranges>
#include <string>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-concepts
   * @{
   */

  /**
   * @brief Concept for a mesh point.
   *
   * @details Every mesh point `mp` of type `MP`
   * - belongs to a mesh `MP::mesh_t`,
   * - has a unique index `mp.index()` that identifies a mesh point within the mesh,
   * - has a data index `mp.data_index()` that is used to access function values/coefficients stored in GF containers 
   * and
   * - has a hash value `mp.mesh_hash()` that is used to check if a mesh point belongs to or is compatible with a mesh.
   *
   * @tparam MP Mesh point type.
   */
  template <typename MP>
  concept MeshPoint = requires(MP const &mp) {
    // parent mesh type
    typename MP::mesh_t;

    // unique index, e.g. Matsubara index, a simple long or a std::array<long, 3> for k-points
    { mp.index() } -> nda::AnyOf<typename MP::mesh_t::index_t, typename MP::mesh_t::index_t const &>;

    // the data index to access data arrays of GF containers
    { mp.data_index() } -> nda::AnyOf<typename MP::mesh_t::data_index_t, typename MP::mesh_t::data_index_t const &>;

    // hash of the parent mesh to check for compatibility
    { mp.mesh_hash() } -> std::same_as<uint64_t>;
  };

  /**
   * @brief Concept for a mesh.
   *
   * @details Every mesh `m` of type `M`
   * - is a `std::regular` type and `h5::Storable`,
   * - has an associated triqs::mesh::MeshPoint type `M::mesh_point_t`, index type `M::index_t` and data index type
   * `M::data_index_t`,
   * - is a sized, forward range of mesh points,
   * - provides an overload for the subscript operator to access a certain mesh point by data index,
   * - provides an overload for the function call operator to access a certain mesh point by index,
   * - provides the functions `m.to_data_index(index)` and `m.to_index(data_index)` to convert between indices and data
   * indices and
   * - provides the function `m.is_index_valid(index)` to check if an index is valid.
   *
   * @tparam M Mesh type.
   */
  template <typename M>
  concept Mesh = std::regular<M> and h5::Storable<M> and requires(M const &m) {
    // mesh point type
    typename M::mesh_point_t;
    requires MeshPoint<typename M::mesh_point_t>;

    // sized, forward range of mesh points
#if not(defined(_LIBCPP_VERSION) and (__clang_major__ < 16))
    requires std::ranges::forward_range<M>;
    requires std::ranges::sized_range<M>;
#endif
    { *std::begin(m) } -> std::same_as<typename M::mesh_point_t>;

    // index type
    typename M::index_t;

    // data index type
    typename M::data_index_t;
  }
  and requires(M const &m, typename M::index_t index, typename M::data_index_t data_index, typename M::mesh_point_t mp) {
    // check validity of an index
    { m.is_index_valid(index) } -> std::same_as<bool>;

    // index <-> data index conversion
    { m.to_data_index(index) } -> std::same_as<typename M::data_index_t>;
    { m.to_index(data_index) } -> std::same_as<typename M::index_t>;

    // access mesh points
    { m.operator[](data_index) } -> std::same_as<typename M::mesh_point_t>;
    { m.operator()(index) } -> std::same_as<typename M::mesh_point_t>;

    // Hash for easy checking of MeshPoint and Mesh compatibility
    { m.mesh_hash() } -> std::same_as<uint64_t>;
  };

  /**
   * @brief Concept for a mesh with values.
   *
   * @details Every mesh with values `m` of type `M`
   * - is a triqs::mesh::Mesh,
   * - has mesh points which have a value of type `M::value_t` and which can be cast to this type and
   * - provides a function `m.to_value(index)` to convert an index of a mesh point to its value.
   *
   * @tparam M Mesh type.
   */
  template <typename M>
  concept MeshWithValues = Mesh<M> and requires(M const &m, typename M::index_t index) {
    // value type
    typename M::value_t;

    // convert an index to its value
    { m.to_value(index) } -> std::same_as<typename M::value_t>;

    // mesh points can return their value and are castable
    { (*std::begin(m)).value() } -> nda::AnyOf<typename M::value_t, typename M::value_t const &>;
    { static_cast<typename M::value_t>(*std::begin(m)) };
  };

  /** @} */

} // namespace triqs::mesh
