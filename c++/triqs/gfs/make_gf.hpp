// Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2020 Simons Foundation
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
 * @brief Provides factory functions to construct Green's functions from a mesh and target, or from another Green's function.
 */

#pragma once

#include "./gf/gf.hpp"

#include <type_traits>
#include <utility>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-factories
   * @{
   */

  /**
   * @brief Construct a Green's function from a mesh and a target.
   *
   * @details The target type is taken from `T::target_t` and the target shape from `t.shape()`.
   *
   * @tparam Mesh The type of the mesh.
   * @tparam T The type providing the target (e.g. an array determining the target shape and type).
   * @param m The mesh of the resulting Green's function.
   * @param t An object whose `target_t` and `shape()` define the target of the Green's function.
   * @return A triqs::gfs::gf on the mesh `m` with the target deduced from `t`.
   */
  template <typename Mesh, typename T> gf<Mesh, typename T::target_t> make_gf(Mesh m, T const &t) { return {std::move(m), t.shape()}; }

  /**
   * @brief Construct a regular Green's function from another Green's function or view.
   *
   * @details Returns a copy with the regular (owning) type of the input, materializing the data of a view if necessary.
   *
   * @tparam G The type of the input Green's function or view.
   * @param g The Green's function or view to copy.
   * @return A regular Green's function holding a copy of the data of `g`.
   */
  template <typename G> typename std::decay_t<G>::regular_type make_gf(G &&g) { return typename std::decay_t<G>::regular_type{std::forward<G>(g)}; }

  /** @} */

} // namespace triqs::gfs
