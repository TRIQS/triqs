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

#pragma once
#include "../gf/gf.hpp"
//#include "../gf/gf_view.hpp"
#include "../gf/gf_const_view.hpp"
#include "../gf/flatten.hpp"

/* *-----------------------------------------------------------------------------------------------------
   *
   * make_gf_from_fourier (g, mesh, options)  -> fourier_transform of g
   *
   * *-----------------------------------------------------------------------------------------------------*/
namespace triqs::gfs {

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
