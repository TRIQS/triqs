// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides various utilities used with @ref triqs-meshes.
 */

#pragma once

#include "./concepts.hpp"

#include <nda/nda.hpp>

#include <cstdint>
#include <functional>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>

namespace triqs::mesh {

  // Aliases and type definitions.
  using dcomplex = std::complex<double>;
  using all_t    = nda::range::all_t;
  using nda::array;
  using nda::array_view;
  using nda::ellipsis;
  using nda::eye;
  using nda::matrix;
  using nda::matrix_const_view;
  using nda::matrix_view;
  using nda::range;

  /**
   * @addtogroup triqs-meshes-utils
   * @{
   */

  /**
   * @brief Generic hash function for multiple arguments.
   *
   * @details It simply uses `std::hash` for each argument and adds the results.
   *
   * @tparam Ts Argument types.
   * @param ts Objects to be hashed.
   * @return Combined hash value of all arguments.
   */
  template <typename... Ts> [[nodiscard]] uint64_t hash(Ts &&...ts) { return (std::hash<std::decay_t<Ts>>()(std::forward<Ts>(ts)) + ...); }

  /**
   * @brief Calculate the positive modulo of two integer numbers.
   *
   * @param x Left-hand side operand \f$ x \f$ of the modulo operation.
   * @param y Right-hand side operand \f$ y \geq 0 \f$ of the modulo operation.
   * @return 
   * \f[
   *   \begin{cases}
   *     x \mod y & \text{if } x \geq 0 \\
   *     x \mod y + y & \text{if } \text{x < 0}
   *   \end{cases}
   * \f]
   */
  [[nodiscard]] inline long positive_modulo(long x, long y) {
    EXPECTS(y >= 0);
    long res = x % y;
    return (res >= 0 ? res : res + y);
  }

  /**
   * @brief Get the values of all mesh points in a mesh.
   *
   * @tparam M triqs::mesh::MeshWithValues type.
   * @param m %Mesh object.
   * @return `nda::vector` containing the values of all mesh points.
   */
  template <MeshWithValues M> [[nodiscard]] auto values(M const &m) {
    auto res = nda::vector<typename M::value_t>(m.size());
    for (auto i : nda::range(m.size())) res(i) = m[i].value();
    return res;
  }

  /// Constexpr bool that is true if the given triqs::mesh::Mesh type is a product of meshes, i.e. a triqs::mesh::prod.
  template <Mesh M> static constexpr bool is_product = false;

  /// Constexpr variable that holds the number of meshes in a triqs::mesh::Mesh type (\f$ 1 \f$ for non-product meshes).
  template <Mesh M> static constexpr int n_variables = 1;

  /**
   * @brief Enum to specify particle statistics.
   *
   * @details The following statistics are supported:
   * - `Boson` and
   * - `Fermion`.
   */
  enum statistic_enum { Boson = 0, Fermion = 1 };

  /**
   * @brief Get the sign associated with the given particle statistics.
   *
   * @param s triqs::mesh::statistic_enum value.
   * @return \f$ 1 \f$ for bosons and \f$ -1 \f$ for fermions.
   */
  [[nodiscard]] inline int sign(statistic_enum s) { return (s == Boson ? 1 : -1); }

  /**
   * @brief Multiplication operator for two triqs::mesh::statistic_enum objects.
   *
   * @param s1 Left-hand side operand.
   * @param s2 Right-hand side operand.
   * @return Boson statistics if `s1 == s2`, otherwise Fermion statistics.
   */
  [[nodiscard]] inline auto operator*(statistic_enum s1, statistic_enum s2) { return (s1 == s2 ? Boson : Fermion); }

  /**
   * @brief Lazy struct used in various function overloads as a placeholder for the closest mesh point to a given value.
   * @tparam T Value type of the mesh.
   */
  template <typename T> struct closest_mesh_point_t {
    /// Use the mesh point closest to this value.
    T value;
  };

  /**
   * @brief Construct a triqs::mesh::closest_mesh_point_t object for a single value or a `std::tuple` of
   * triqs::mesh::closest_mesh_point_t objects for multiple values.
   *
   * @tparam Ts Value types.
   * @param ts Values to be wrapped in triqs::mesh::closest_mesh_point_t objects.
   * @return Either a triqs::mesh::closest_mesh_point_t object or a tuple of such objects.
   */
  template <typename... Ts> [[nodiscard]] auto closest_mesh_pt(Ts &&...ts) {
    if constexpr (sizeof...(Ts) == 1)
      return closest_mesh_point_t<std::decay_t<Ts>...>{std::forward<Ts>(ts)...};
    else
      return std::tuple{closest_mesh_point_t<std::decay_t<Ts>>{std::forward<Ts>(ts)}...};
  }

  /** @} */

  namespace details {

    // Apply a function to each element of a range and sum the results into a regular type.
    [[nodiscard]] auto sum_to_regular(std::ranges::forward_range auto &&rg, auto f) {
      auto it  = std::ranges::begin(rg);
      auto e   = std::ranges::end(rg);
      auto res = nda::make_regular(f(*it));
      for (++it; it != e; ++it) res += f(*it);
      return res;
    }

  } // namespace details

} // namespace triqs::mesh
