// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
 * @brief Provides test helpers that assert two (block) Green's functions are numerically close.
 */

#pragma once

#include "../utility/exceptions.hpp"

namespace triqs::gfs {

  /**
   * @addtogroup triqs-test-tools
   * @{
   */

  /**
   * @brief Assert that two Green's functions are numerically close.
   *
   * @details Throws a runtime error if the meshes of the two Green's functions differ, or if the maximum
   * element-wise absolute difference of their data exceeds `precision`.
   *
   * @tparam X The type of the first Green's function.
   * @tparam Y The type of the second Green's function.
   * @param x The first Green's function.
   * @param y The second Green's function.
   * @param precision The maximum allowed element-wise absolute difference.
   */
  template <typename X, typename Y> void assert_gfs_are_close(X const &x, Y const &y, double precision) {

    // meshes must be identical and data close
    if (x.mesh() != y.mesh()) TRIQS_RUNTIME_ERROR << "GFs have different meshes\n -- 1st Mesh \n" << x.mesh() << "\n -- 2nd Mesh \n" << y.mesh();
    if (max_element(abs(x.data() - y.data())) > precision)
      TRIQS_RUNTIME_ERROR << "Gfs have different data. max(abs(x-y)) = " << max_element(abs(x.data() - y.data()));
  }

  /**
   * @brief Assert that two block Green's functions are numerically close.
   *
   * @details Throws a runtime error if the number of blocks or the block names differ, then checks each block
   * pair with assert_gfs_are_close().
   *
   * @tparam X The type of the first block Green's function.
   * @tparam Y The type of the second block Green's function.
   * @param x The first block Green's function.
   * @param y The second block Green's function.
   * @param precision The maximum allowed element-wise absolute difference per block.
   */
  template <typename X, typename Y> void assert_block_gfs_are_close(X const &x, Y const &y, double precision) {

    if (x.size() != y.size()) TRIQS_RUNTIME_ERROR << "Block GFs have different number of blocks";
    if (x.block_names() != y.block_names()) TRIQS_RUNTIME_ERROR << "Block GFs have different block_names";
    for (int u = 0; u < x.size(); ++u) assert_gfs_are_close(x[u], y[u], precision);
  }

  /**
   * @brief Assert that two doubly-indexed block Green's functions are numerically close.
   *
   * @details Throws a runtime error if the number of blocks or the block names differ, then checks each
   * \f$ (i, j) \f$ block pair with assert_gfs_are_close().
   *
   * @tparam X The type of the first block2 Green's function.
   * @tparam Y The type of the second block2 Green's function.
   * @param x The first block2 Green's function.
   * @param y The second block2 Green's function.
   * @param precision The maximum allowed element-wise absolute difference per block.
   */
  template <typename X, typename Y> void assert_block2_gfs_are_close(X const &x, Y const &y, double precision) {

    if (x.size() != y.size()) TRIQS_RUNTIME_ERROR << "Block2 GFs have different number of blocks";
    if (x.block_names() != y.block_names()) TRIQS_RUNTIME_ERROR << "Block2 GFs have different block_names";
    for (int i = 0; i < x.size1(); ++i)
      for (int j = 0; j < x.size2(); ++j) assert_gfs_are_close(x(i, j), y(i, j), precision);
  }

  /** @} */

} // namespace triqs::gfs
