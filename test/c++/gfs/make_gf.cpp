// Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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
// Authors: Nils Wentzell

#include <triqs/test_tools/gfs.hpp>
#include <iostream>
using namespace triqs::gfs;
using namespace triqs::clef;

placeholder<0> iw;
double beta = 1.0;

//-----------------------------

TEST(make_gf, from_view) {

  // Create and intialize G1
  gf<imfreq> G1{{beta, Fermion, 5}, {2, 2}};
  G1[iw] << 1.0 / iw;

  // Create G2 from view of G1
  auto G2 = make_gf(G1());

  EXPECT_GF_NEAR(G1, G2);
}

//-----------------------------

TEST(make_gf, from_mesh_and_target) {

  // Create and intialize G1
  gf<imfreq> G1{{beta, Fermion, 5}, {2, 2}};
  G1[iw] << 1.0 / iw;

  // Create G2 with same mesh and target as G1
  auto G2 = make_gf(G1.mesh(), G1.target());

  // Fill G2 with same values as G1
  G2() = G1;

  EXPECT_GF_NEAR(G1, G2);
}

//-----------------------------

TEST(make_gf, new_layout) {

  // Create and intialize G1
  gf<imfreq, matrix_valued> G1{{beta, Fermion, 5}, {2, 2}};
  gf<imfreq, matrix_valued, nda::basic_layout<0, nda::encode(std::array{1, 0, 2}), layout_prop_e::contiguous>> G2{{beta, Fermion, 5}, {2, 2}};
  G1[iw] << 1.0 / iw;

  // Create G2 from G1 with new memory layout
  G2 = G1;

  nda::range_all _;

  for (int w = 0; w < 5; ++w) { EXPECT_ARRAY_NEAR(G2.data()(w, _, _), G1.data()(w, _, _)); }

  for (auto const &w : G1.mesh()) { EXPECT_ARRAY_NEAR(G2[w], G1[w]); }

  EXPECT_EQ(G1.data().indexmap().stride_order, (std::array{0, 1, 2}));
  EXPECT_EQ(G2.data().indexmap().stride_order, (std::array{1, 0, 2}));

  for (int w = 0; w < 5; ++w)
    for (int i = 0; i < 2; ++i)
      for (int j = 0; j < 2; ++j) EXPECT_CLOSE((G1.data()(w, i, j)), (G2.data()(w, i, j)));

  EXPECT_ARRAY_NEAR(G1.data(), G2.data());
  EXPECT_GF_NEAR(G1, G2);
}

MAKE_MAIN;
