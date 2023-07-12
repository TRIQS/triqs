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

#pragma once
#include "gf.hpp"
namespace triqs::gfs {

  /**
   * Given an array a, creates a two-dimensional array whose first dimension is the n-th dimension of a
   * and the second dimension are the flattening of the other dimensions, in the original order
   *
   * @param a : array
   * @tparam N : the dimension to preserve
   *
   * @return : a two-dimensional array
   */
  template <int N = 0, nda::MemoryArray A> auto flatten_2d(A const &v) {
    long nrows = v.extent(N);      // # rows of the result, i.e. n-th dim, which is now at 0.
    long ncols = v.size() / nrows; // # columns of the result. Everything but n-th dim.
    return reshape(typename A::regular_type{nda::transposed_view<0, N>(v)}, nrows, ncols);
  }

  // -------------------------------------------------------

  /// Inverse of flatten_2d
  template <int N = 0> void unflatten_2d(nda::MemoryArray auto &&out, nda::MemoryArray auto const &afl) {
    auto sh_fl = out.shape();
    std::swap(sh_fl[0], sh_fl[N]);
    auto data_fl = reshape(afl, sh_fl);
    out()        = nda::transposed_view<0, N>(data_fl);
  }

  //-------------------------------------

  /**
   * Given a Green-function g, creates a new Green function with a mesh that is the n-th mesh of g
   * and with a target that is tensor_valued<1>, containing the flattening of all other data dimensions
   * in original order.
   *
   * @param g : array
   * @tparam N : the mesh to keep
   *
   * @return : the single-mesh tensor-valued Green-function
   */
  template <int N = 0, MemoryGf G> auto flatten_gf_2d(G const &g) {
    auto const &mesh = [&g]() {
      if constexpr (mesh::is_product<typename G::mesh_t>)
        return std::get<N>(g.mesh());
      else
        return g.mesh();
      ;
    }();
    return gf{mesh, flatten_2d<N>(g.data())};
  }

  //-------------------------------------

  /// Inverse of flatten_gf_2d
  template <int N = 0, MemoryGf Gfl> void unflatten_gf_2d(MemoryGf auto &&g, Gfl const &gfl) {
    static_assert(not mesh::is_product<typename Gfl::mesh_t>, "unflatten_gf_2d: Flattened Green-function must have non-product mesh");
    EXPECTS(std::get<N>(g.mesh()) == gfl.mesh());
    unflatten_2d<N>(g.data(), gfl.data());
  }

} // namespace triqs::gfs
