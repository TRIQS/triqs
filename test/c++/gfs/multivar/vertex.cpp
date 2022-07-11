// Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;

using triqs::clef::placeholder;
// scalar valued gf_vertex
using gf_vertex_t        = gf<prod<imfreq, imfreq, imfreq>, scalar_valued>;
using gf_vertex_tensor_t = gf<prod<imfreq, imfreq, imfreq>, tensor_valued<3>>;

// -----------------------------------------------------

TEST(Gf, VertexScalar) {

  double beta   = 10.0;
  int n_im_freq = 10;

  auto m = mesh::imfreq{beta, Fermion, n_im_freq};

  auto vertex = gf_vertex_t{{m, m, m}};

  placeholder<0> w0_;
  placeholder<1> w1_;
  placeholder<2> w2_;

  vertex(w0_, w1_, w2_) << w0_ + 2.3 * w1_ + 3.1 * w2_;

  vertex[0, 0, 0] = 10;

  EXPECT_CLOSE((vertex[1, 6, 3]), (M_PI * (2 * 1 + 1) / 10.0 + 2.3 * M_PI * (2 * 6 + 1) / 10.0 + 3.1 * M_PI * (2 * 3 + 1) / 10.0) * 1i);

  vertex[0, 0, 0] *= 2;

  EXPECT_CLOSE(vertex(0, 0, 0), 20);

  rw_h5(vertex, "vertexS", "v");
}

// -----------------------------------------------------

TEST(Gf, VertexTensor) {

  double beta   = 10.0;
  int n_im_freq = 10;

  auto m = mesh::imfreq{beta, Fermion, n_im_freq};

  // now with indices
  auto vertex = gf_vertex_tensor_t{{m, m, m}, {2, 2, 2}};

  vertex[0, 0, 0](0, 0, 0) = 10;
  EXPECT_CLOSE((vertex[0, 0, 0](0, 0, 0)), 10);

  rw_h5(vertex, "vertexT", "v");
}
MAKE_MAIN;
