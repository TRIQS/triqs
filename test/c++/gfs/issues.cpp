// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

#define TRIQS_DEBUG
#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Issue276) {

  auto g  = gf<imfreq, matrix_valued>{{1, Fermion, 6}, {1, 1}};
  auto g2 = gf<imfreq, matrix_valued>{{1, Fermion, 6}, {1, 1}};

  triqs::clef::placeholder<0> om_;
  g(om_) << 1 / (om_ + 3);
  g2(om_) << 1 / (om_ - 3);

  auto gc  = g;
  auto gc2 = g2;

  {
    gf_const_view<imfreq> gv  = g;
    gf_const_view<imfreq> gv2 = g2;

    EXPECT_GF_NEAR(g, gv);
    EXPECT_GF_NEAR(g2, gv2);

    swap(gv, gv2);

    EXPECT_GF_NEAR(g, gv2);
    EXPECT_GF_NEAR(g2, gv);

    EXPECT_GF_NEAR(g, gc);
    EXPECT_GF_NEAR(g2, gc2);
  }

  {
    g(om_) << 1 / (om_ + 3);
    g2(om_) << 1 / (om_ - 3);

    gf_view<imfreq> gv  = g;
    gf_view<imfreq> gv2 = g2;

    EXPECT_GF_NEAR(g, gv);
    EXPECT_GF_NEAR(g2, gv2);

    swap(gv, gv2);

    EXPECT_GF_NEAR(g, gv2);
    EXPECT_GF_NEAR(g2, gv);
  }
}

// ----------------------------------------------------------

// TEST(Gf, Issue319) {
//   double beta = 20;
//   int n_iw    = 100;
//   int dim     = 2;
//   int rank    = 0; //world.rank();

//   gf<prod<imfreq, imfreq, imfreq>> g4_w{{{beta, Fermion, n_iw}, {beta, Fermion, n_iw}, {beta, Boson, 25}}, {dim, dim}};

//   auto G4_w  = make_block_gf(1, g4_w);
//   auto mesh  = G4_w[0].mesh();
//   auto N     = mesh.size();
//   auto slice = itertools::chunk_range(0, N, 1, 0); // world.size(), world.rank());
//   auto mpg   = triqs::mesh::mesh_pt_generator<prod<imfreq, imfreq, imfreq>>(&G4_w[0].mesh());

//   mpg += slice.first;
//   mpg.to_point();
// }

MAKE_MAIN;
