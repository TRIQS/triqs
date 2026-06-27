// Copyright (c) 2020 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet

/**
 * @file
 * @brief Provides the block-wise lazy Fourier transform for block Green's functions.
 */

#pragma once

#include "./block_gf.hpp"

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-map
   * @{
   */

  /**
   * @brief Lazily apply the Fourier transform block by block to a block Green's function.
   *
   * @details Returns a lazy transform object that applies `fourier` to each block when assigned into a block Green's
   * function.
   *
   * @tparam V Mesh type. @tparam T Target type. @tparam L Layout type. @tparam A Block arity.
   * @param g Block Green's function.
   * @return A lazy transform applying `fourier` block by block.
   */
  template <typename V, typename T, typename L, int A> auto fourier(block_gf<V, T, L, A> const &g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  }

  /// Lvalue overload of the block-wise lazy Fourier transform (see the const overload).
  template <typename V, typename T, typename L, int A> auto fourier(block_gf<V, T, L, A> &g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  }

  /// View overload of the block-wise lazy Fourier transform (see the const overload).
  template <typename V, typename T, typename L, int A, bool C> auto fourier(block_gf_view<V, T, L, A, C> g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  }

  /** @} */

} // namespace triqs::gfs
