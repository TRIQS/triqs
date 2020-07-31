// Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet

// additional functions for debug sections

namespace triqs::gfs {

  template <typename... Ms> struct cartesian_product;

  // check if mesh_point mp is compatible with mesh m. Intented for debug mode only.
  template <typename MP, typename T> bool mesh_point_compatible_to_mesh(MP const &mp, gf_mesh<T> const &m) { return mp.mesh() == m; }

  // special case for imtime : do not check the statistic !
  template <typename MP> bool mesh_point_compatible_to_mesh(MP const &mp, gf_mesh<imtime> const &m) {
    return (std::abs(m.domain().beta - mp.mesh().domain().beta) < 1.e-15) and (m.size() == mp.mesh().size());
  }

  // special case for cartesian product, it must be checked for every component.
  template <typename MP, typename... M, size_t... Is>
  bool mesh_point_compatible_to_mesh_impl(std::index_sequence<Is...>, MP const &mp, gf_mesh<cartesian_product<M...>> const &m) {
    return (mesh_point_compatible_to_mesh(std::get<Is>(mp.components_tuple()), std::get<Is>(m.components())) and ...);
  }
  template <typename... M>
  bool mesh_point_compatible_to_mesh(mesh_point<gf_mesh<cartesian_product<M...>>> const &mp, gf_mesh<cartesian_product<M...>> const &m) {
    return mesh_point_compatible_to_mesh_impl(std::index_sequence_for<M...>{}, mp, m);
  }
} // namespace triqs::gfs
