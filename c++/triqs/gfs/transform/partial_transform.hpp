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

    unflatten_2d<N>(make_array_view(g_out.data()), make_array_const_view(g2_flat.data()));
    return std::move(g_out);
  }

} // namespace triqs::gfs