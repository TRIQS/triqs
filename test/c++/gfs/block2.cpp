// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

TEST(Block2, Base) {

  double beta = 1;
  auto G1     = gf<imfreq>({beta, Fermion}, {2, 2});
  auto G2     = G1;
  auto G3     = G1;

  triqs::clef::placeholder<0> w_;
  G1(w_) << 1. / (w_ + 2.);
  G2(w_) << 2. / (w_ - 2.);

  // Constructors
  auto G_vec    = std::vector{G1, G2, G3};
  auto G_vecvec = std::vector{G_vec, G_vec};

  auto B1 = block2_gf<imfreq>{G_vecvec};
  auto B2 = block2_gf<imfreq>{B1()};
  auto B3 = block2_gf<imfreq>({{"0", "1"}, {"0", "1", "2"}}, G_vecvec);
  auto B4 = make_block2_gf({"0", "1"}, {"0", "1", "2"}, G_vecvec);
  auto B5 = make_block2_gf(2, 1, G1);

  auto view_vec    = std::vector<gf_view<imfreq>>{G1, G2, G3};
  auto view_vecvec = std::vector{view_vec, view_vec};
  auto V1          = make_block2_gf_view(view_vecvec);

  EXPECT_BLOCK2_GF_NEAR(B1, B2);
  EXPECT_BLOCK2_GF_NEAR(B1, B3);
  EXPECT_BLOCK2_GF_NEAR(B1, B4);
  EXPECT_BLOCK2_GF_NEAR(B1, V1);

  // H5 read write
  EXPECT_BLOCK2_GF_NEAR(B1, rw_h5(B1, "block2", "B1"));

  // Data write/read access
  B1(0, 0)[0] = 1.0;
  EXPECT_ARRAY_NEAR(B1(0, 0)(0), matrix<double>{{1, 0}, {0, 1}});

  // Operations
  B1 = B1 / 2.0;
  B1 = B1 * 4.0;
  EXPECT_CLOSE(B1(0, 0)[0](0, 0), 2.0);
  B1 = B1 + B1 * B1;
  EXPECT_CLOSE(B1(0, 0)[0](0, 0), 6.0);
  B1 = B1 + B1() * B1();
  EXPECT_CLOSE(B1(0, 0)[0](0, 0), 42.0);

  // View Access
  V1(0, 0)[0] = 5.0;
  EXPECT_CLOSE(G1[0](0, 0), 5.0);

  // Loops
  for (auto &g : B1) { g[0] = 20; }
  EXPECT_CLOSE(B1(0, 0)[0](0, 0), 20);
  for (auto &g : B1()) { g[0] = 40; }
  EXPECT_CLOSE(B1(0, 0)[0](0, 0), 40);

  // Clef expressions
  clef::placeholder<0> b1_;
  clef::placeholder<1> b2_;
  clef::placeholder<2> iw_;
  B1(b1_, b2_)[iw_] << b1_ * b2_ / (iw_ + 2);
  auto B11 = B1;
  B1(b1_, b2_)(iw_) << B11(b1_, b2_)(iw_) * B1(b1_, b2_)(iw_) * B11(b1_, b2_)(iw_);

  // Reinterpretation (compile checks)
  auto G1_scalar = gf<imfreq, scalar_valued>{{beta, Fermion}};
  auto B1_scalar = make_block2_gf(3, 3, G1_scalar);
  auto B1_interp = reinterpret_scalar_valued_gf_as_matrix_valued(B1_scalar);

  // Inversion
  auto inv_G1 = inverse(G1);
  auto B      = make_block2_gf(3, 3, G1);
  auto inv_B  = inverse(B);
  for (auto &g : inv_B) EXPECT_GF_NEAR(g, inv_G1);
}

TEST(Block2, CopyAssignment) {
  auto g        = gf<imfreq, scalar_valued>{{1, Fermion}};
  auto G_vec    = std::vector{g, g};
  auto G_vecvec = std::vector{G_vec, G_vec};
  auto G        = make_block2_gf({"up", "down"}, {"up", "down"}, G_vecvec);
  block2_gf<imfreq, scalar_valued> G2;
  G2 = G;
  ASSERT_EQ(G2.data().size(), G.data().size());
  ASSERT_EQ(G2.block_names().size(), G.block_names().size());
}

TEST(Block2, Arithmetic) {
  double beta = 1;
  auto G1     = gf<imfreq>({beta, Fermion}, {2, 2});
  auto G2     = gf<imfreq>({beta, Fermion}, {2, 2});
  auto G3     = gf<imfreq>({beta, Fermion}, {2, 2});
  triqs::clef::placeholder<0> w_;
  G1(w_) << 1 / (w_ + 2);
  G2(w_) << 1 / (w_ - 2);
  G3(w_) << 1 / (w_ - 4);

  auto G_vecvec = std::vector<std::vector<gf<imfreq>>>{{G1, G2}, {G2, G3}};
  auto B        = make_block2_gf({"a", "b"}, {"c", "d"}, G_vecvec);

  EXPECT_BLOCK2_GF_NEAR(block2_gf<imfreq>{2 * B}, block2_gf<imfreq>{1.0 * B + B * 1.0});
}

MAKE_MAIN;
