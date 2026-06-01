#pragma once

#include <triqs/gfs.hpp>
#include <triqs/gfs/functions/dlr2d.hpp>
#include <triqs/mesh.hpp>

using namespace triqs::arrays;
using namespace triqs::gfs;
using namespace triqs::mesh;

namespace triqs::gfs {

  // make_gf_dlr2d: dlr2d_imfreq -> dlr2d
  auto make_gf_dlr2d(gf_const_view<dlr2d_imfreq, scalar_valued> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(gf_const_view<dlr2d_imfreq, tensor_valued<1>> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(gf_const_view<dlr2d_imfreq, matrix_valued> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(gf_const_view<dlr2d_imfreq, tensor_valued<3>> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(gf_const_view<dlr2d_imfreq, tensor_valued<4>> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block_gf_const_view<dlr2d_imfreq, scalar_valued> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block_gf_const_view<dlr2d_imfreq, tensor_valued<1>> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block_gf_const_view<dlr2d_imfreq, matrix_valued> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block_gf_const_view<dlr2d_imfreq, tensor_valued<3>> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block_gf_const_view<dlr2d_imfreq, tensor_valued<4>> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block2_gf_const_view<dlr2d_imfreq, scalar_valued> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block2_gf_const_view<dlr2d_imfreq, tensor_valued<1>> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block2_gf_const_view<dlr2d_imfreq, matrix_valued> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block2_gf_const_view<dlr2d_imfreq, tensor_valued<3>> g) { return make_gf_dlr2d<0>(g); }
  auto make_gf_dlr2d(block2_gf_const_view<dlr2d_imfreq, tensor_valued<4>> g) { return make_gf_dlr2d<0>(g); }

  // make_gf_dlr2d_imfreq: dlr2d -> dlr2d_imfreq
  auto make_gf_dlr2d_imfreq(gf_const_view<dlr2d, scalar_valued> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(gf_const_view<dlr2d, tensor_valued<1>> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(gf_const_view<dlr2d, matrix_valued> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(gf_const_view<dlr2d, tensor_valued<3>> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(gf_const_view<dlr2d, tensor_valued<4>> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block_gf_const_view<dlr2d, scalar_valued> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block_gf_const_view<dlr2d, tensor_valued<1>> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block_gf_const_view<dlr2d, matrix_valued> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block_gf_const_view<dlr2d, tensor_valued<3>> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block_gf_const_view<dlr2d, tensor_valued<4>> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block2_gf_const_view<dlr2d, scalar_valued> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block2_gf_const_view<dlr2d, tensor_valued<1>> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block2_gf_const_view<dlr2d, matrix_valued> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block2_gf_const_view<dlr2d, tensor_valued<3>> g) { return make_gf_dlr2d_imfreq<0>(g); }
  auto make_gf_dlr2d_imfreq(block2_gf_const_view<dlr2d, tensor_valued<4>> g) { return make_gf_dlr2d_imfreq<0>(g); }

} // namespace triqs::gfs
