// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides common type aliases, forward declarations and internal helpers for the Green's function containers.
 */

#pragma once

#include "../../arrays.hpp"
#include "../../mesh/concepts.hpp"
#include "../../mesh/matsubara_freq.hpp"
#include "../../mesh/prod.hpp"
#include "../../mesh/utils.hpp"
#include "../../utility/factory.hpp"

#include <array>
#include <complex>
#include <cstddef>
#include <utility>

namespace triqs::gfs {

  // Alias for std::complex<double>.
  using dcomplex = std::complex<double>;

  // Elevate certain types into the triqs::gfs namespace.
  using mesh::all_t;
  using mesh::Boson;
  using mesh::closest_mesh_pt;
  using mesh::Fermion;
  using mesh::matsubara_freq;
  using mesh::Mesh;
  using mesh::n_variables;
  using mesh::statistic_enum;
  using nda::array;
  using nda::array_view;
  using nda::ellipsis;
  using nda::matrix;
  using nda::matrix_const_view;
  using nda::matrix_view;
  using triqs::arrays::make_shape;
  using utility::factory;

  /**
   * @addtogroup triqs-gfs-evaluation
   * @{
   */

  /**
   * @brief Functor that evaluates a Green's function at an arbitrary point of a given mesh.
   *
   * @details This template is specialized for each mesh type to provide the appropriate interpolation scheme used when
   * a Green's function is called with off-mesh arguments.
   *
   * @tparam M triqs::mesh::Mesh type.
   */
  template <Mesh M> struct gf_evaluator;

  /**
   * @brief Default evaluator policy.
   *
   * @details Selects triqs::gfs::gf_evaluator as the evaluation functor for a given mesh type.
   */
  struct default_evaluator {
    /// The evaluator functor associated with the mesh type `Mesh`.
    template <typename Mesh> using evaluator_t = gf_evaluator<Mesh>;
  };

  /** @} */

  /**
   * @ingroup triqs-gfs-io
   * @brief Traits class for reading/writing a Green's function from/to HDF5.
   *
   * @details This template is specialized for the various mesh/target combinations (and for block Green's functions)
   * to provide the concrete HDF5 serialisation logic.
   *
   * @tparam Mesh triqs::mesh::Mesh type.
   * @tparam Target Target type of the Green's function.
   */
  template <typename Mesh, typename Target> struct gf_h5_rw;

  namespace detail {

    // Build a sub-mesh from a product mesh by keeping the components at the given positions.
    template <auto Positions, Mesh M> auto filter_mesh(M const &m) {
      static_assert(Positions.size() > 0);
      if constexpr (Positions.size() == 1) {
        return std::get<Positions[0]>(m);
      } else {
        return [&]<size_t... Is>(std::index_sequence<Is...>) {
          return mesh::prod{std::get<Positions[Is]>(m)...};
        }(std::make_index_sequence<Positions.size()>{});
      }
    }

    // Compute the positions of the first L set entries of a boolean filter.
    template <size_t L> constexpr std::array<int, L> compute_position(auto const &filter) {
      std::array<int, L> r{};
      int ii = 0;
      for (int i = 0; i < filter.size() and ii < L; ++i) {
        r[ii] = i;
        ii += int(filter[i]);
      }
      return r;
    }

    // Check whether a Green's function evaluation is known to vanish at the given arguments.
    template <Mesh M, typename... XS> bool eval_to_zero(M const &m, XS const &...xs) {
      if constexpr (sizeof...(XS) > 1) {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
          return (eval_to_zero(std::get<Is>(m), xs) or ... or false);
        }(std::make_index_sequence<sizeof...(XS)>());
      } else if constexpr (requires { m.eval_to_zero(xs...); }) {
        return m.eval_to_zero(xs...);
      }
      return false;
    }

  } // namespace detail

} // namespace triqs::gfs
