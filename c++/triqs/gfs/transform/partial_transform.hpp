// Copyright (c) 2023 Simons Foundation
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
 * @brief Provides a helper to apply a transform to a single component of a product-mesh Green's function.
 */

#pragma once

#include "../gf/flatten.hpp"
#include "../gf/gf.hpp"
#include "../gf/gf_const_view.hpp"

#include "../../mesh/prod.hpp"
#include "../../utility/tuple_tools.hpp"

namespace triqs::gfs {

  /**
   * @ingroup triqs-gfs-fourier
   * @brief Apply a transform to a single mesh component of a product-mesh Green's function.
   *
   * @details The input Green's function is flattened along the component `N` (collapsing all other mesh axes and
   * target indices into a single dimension), the user-supplied `lambda` is applied to the resulting flattened
   * Green's function, and the result is unflattened back onto a product mesh whose `N`-th component is replaced by
   * the mesh produced by `lambda`. This is the common scaffold used by per-axis transforms on product meshes.
   *
   * @tparam N Index of the mesh component to transform (default \f$ 0 \f$).
   * @tparam M Mesh types of the product-mesh components.
   * @tparam Target Target type of the input Green's function.
   * @param gin The input Green's function on a product mesh.
   * @param lambda The transform applied to the flattened Green's function of the selected component.
   * @return A new Green's function whose `N`-th mesh component has been transformed.
   */
  template <int N = 0, typename... M, typename Target> auto partial_transform(gf_const_view<mesh::prod<M...>, Target> gin, auto lambda) {

    // Flatten the gf except for the variable N
    auto gin_flatten = flatten_gf_2d<N>(gin);

    auto g2_flat = lambda(gin_flatten);

    auto mesh_tpl = triqs::tuple::replace<N>(gin.mesh().components(), g2_flat.mesh());
    auto g_out    = gf{mesh::prod{mesh_tpl}, gin.target_shape()};

    unflatten_2d<N>(g_out.data(), g2_flat.data());
    return g_out;
  }

} // namespace triqs::gfs
