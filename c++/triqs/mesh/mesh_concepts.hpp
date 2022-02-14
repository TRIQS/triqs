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

#include <concepts>
#include <ranges>
#include <string>

#include <h5/h5.hpp>

namespace triqs::mesh {

  // Forward Declaration
  template <typename M> struct mesh_point;

  template <typename T>
  concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
  };

  template <typename T>
  concept H5Serializable = requires(T const &tcr, T &tr, h5::group g, std::string const &subgroup_name) {
    { tcr.hdf5_format() } -> std::convertible_to<std::string>;
    {h5_write(g, subgroup_name, tcr)};
    {h5_read(g, subgroup_name, tr)};
  };

  // A domain is a set of type point_t elements and knows if a point_t is included in it
  template <typename D>
  concept Domain = std::regular<D> && H5Serializable<D> && requires(D d, typename D::point_t const &pt) {
    typename D::point_t;
    { d.contains(pt) } -> std::same_as<bool>;
  };

  // Unused: Remove?
  // template <typename D>
  // concept IntervalDomain = Domain<D> && requires(D const &m) {
  //   requires std::totally_ordered<typename D::point_t>;
  //   requires std::regular<typename D::point_t>;
  //   { m.min() } -> std::same_as<typename D::point_t>;
  //   { m.max() } -> std::same_as<typename D::point_t>;
  // };

  template <typename MP>
  concept MeshPoint = requires(MP const &mp) {
    typename MP::mesh_t;
    // { mp.index() } -> std::same_as<typename MP::mesh_t::index_t>;
    { mp.linear_index() } -> std::same_as<typename MP::mesh_t::linear_index_t>;
    { mp.value() } -> std::same_as<typename MP::mesh_t::domain_t::point_t>;
    { mp.mesh_hash() } -> std::convertible_to<std::size_t>;
  };

  // A mesh is a domain and a iterator ("std::range") over a subset of that domain (which may be the full domain).
  // Mesh must be h5 serializable and hashable (to quickly check if a mesh point originates from a mesh)
  template <typename M>
  concept Mesh = std::regular<M> && H5Serializable<M> && requires(M const &m) {
    // Has a domain:
    typename M::domain_t;
    requires Domain<typename M::domain_t>;
    std::same_as<std::remove_cvref_t<decltype(m.domain())>, typename M::domain_t>;

    // mesh_point_t class is generally defined in the mesh itself
    typename M::mesh_point_t;
    requires MeshPoint<typename M::mesh_point_t>;

    // Is range which iterates over mesh points:

// libc++ does not yet fully implement ranges for now: use itertools
#if defined(_LIBCPP_VERSION) and (__clang_major__ < 14)
    // requires std::forward_iterator<std::ranges::iterator_t<M>>;  // Currently itertools::transform is not constructible
    {m.size()};
#else
    requires std::ranges::random_access_range<M>;
    requires std::ranges::sized_range<M>;
#endif

    { *std::begin(m) } -> std::same_as<typename M::mesh_point_t>;

    // Has raw_index and index as well as a bijection between them
    typename M::raw_index_t; // What is used to internally access data in gf.data[r_idx]
    typename M::index_t;

  } && requires(M const &m, typename M::index_t const &index) {
    { m.raw_index_from_index(index) } -> std::same_as<typename M::raw_index_t>;
    { m.operator[](index) } -> std::same_as<typename M::mesh_point_t>;
  } && requires(M const &m, typename M::raw_index_t const &r_index) {
    { m.index_from_raw_index(r_index) } -> std::same_as<typename M::index_t>;
    { m.mesh_pt_from_raw_index(r_index) } -> std::same_as<typename M::mesh_point_t>;
  };



  

} // namespace triqs::mesh
