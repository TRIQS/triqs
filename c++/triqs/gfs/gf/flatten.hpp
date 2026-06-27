// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
 * @brief Provides utilities to flatten the data of arrays and Green's functions into a two-dimensional form.
 */

#pragma once

#include "./gf.hpp"
#include "../../utility/macros.hpp"

#include <utility>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-reshape
   * @{
   */

  /**
   * @brief Flatten an array into two dimensions, keeping one dimension and collapsing the rest.
   *
   * @details The result is a two-dimensional array whose first dimension is the \f$ N \f$-th dimension of the input
   * and whose second dimension is the flattening of all other dimensions, kept in their original order.
   *
   * @tparam N Index of the dimension to preserve.
   * @tparam A `nda::MemoryArray` type of the input.
   * @param v Input array.
   * @return A two-dimensional array.
   */
  template <int N = 0, nda::MemoryArray A> auto flatten_2d(A const &v) {
    long nrows = v.extent(N);      // # rows of the result, i.e. n-th dim, which is now at 0.
    long ncols = v.size() / nrows; // # columns of the result. Everything but n-th dim.
    return reshape(typename A::regular_type{nda::transposed_view<0, N>(v)}, nrows, ncols);
  }

  // -------------------------------------------------------

  /**
   * @brief Inverse of triqs::gfs::flatten_2d: scatter a two-dimensional array back into a higher-rank array.
   *
   * @tparam N Index of the dimension that was preserved by the flattening.
   * @param out Output array, whose shape determines how the flattened data is reshaped (written into).
   * @param afl Flattened (two-dimensional) input array.
   */
  template <int N = 0> void unflatten_2d(nda::MemoryArray auto &&out, nda::MemoryArray auto const &afl) {
    auto sh_fl = out.shape();
    std::swap(sh_fl[0], sh_fl[N]);
    auto data_fl = reshape(afl, sh_fl);
    out()        = nda::transposed_view<0, N>(data_fl);
  }

  //-------------------------------------

  /**
   * @brief Flatten a Green's function into a single-mesh, tensor-valued Green's function.
   *
   * @details The result keeps the \f$ N \f$-th mesh of the input Green's function and has a `tensor_valued<1>` target
   * containing the flattening of all other data dimensions in their original order.
   *
   * @tparam N Index of the mesh to keep.
   * @tparam G triqs::gfs::MemoryGf type of the input.
   * @param g Input Green's function.
   * @return The single-mesh, tensor-valued Green's function.
   */
  template <int N = 0, MemoryGf G> auto flatten_gf_2d(G const &g) {
    auto const &mesh = get_mesh<N>(g);
    return gf{mesh, flatten_2d<N>(g.data())};
  }

  //-------------------------------------

  /**
   * @brief Inverse of triqs::gfs::flatten_gf_2d: scatter a flattened Green's function back into a higher-rank one.
   *
   * @tparam N Index of the mesh that was kept by the flattening.
   * @tparam Gfl triqs::gfs::MemoryGf type of the flattened Green's function (must have a non-product mesh).
   * @param g Output Green's function (written into).
   * @param gfl Flattened input Green's function.
   */
  template <int N = 0, MemoryGf Gfl> void unflatten_gf_2d(MemoryGf auto &g, Gfl const &gfl) {
    static_assert(not mesh::is_product<typename Gfl::mesh_t>, "unflatten_gf_2d: Flattened Green-function must have non-product mesh");
    EXPECTS(get_mesh<N>(g) == gfl.mesh());
    unflatten_2d<N>(g.data(), gfl.data());
  }

  /** @} */

} // namespace triqs::gfs
