// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Nils Wentzell

#include <triqs/gfs.hpp>
#include <triqs/test_tools/gfs.hpp>
using namespace triqs::gfs;
using namespace std::complex_literals;

TEST(Functions, RealGf) {

  double beta  = 1;
  int n_iw     = 100;
  auto iw_mesh = gf_mesh<imfreq>{beta, Fermion, n_iw};
  auto shape   = make_shape(1, 1);

  // -- Build Green functions

  auto g_iw    = gf<imfreq, matrix_valued>{iw_mesh, shape};
  auto giw_vec = std::vector{g_iw, g_iw};

  placeholder<0> iw_;
  giw_vec[0](iw_) << 1.0 / (iw_ + 1.0 + 1i);
  giw_vec[1](iw_) << 2.0 / (iw_ - 1.0 + 1i);

  auto block_giw = make_block_gf({"one", "two"}, giw_vec);

  auto giw_vecvec = std::vector{giw_vec, giw_vec};
  auto block2_giw = make_block2_gf({"one", "two"}, {"one", "two"}, giw_vecvec);

  // -- Build the real Block Green function

  std::vector<gf<imfreq, matrix_real_valued>> giw_real_vec;
  for (auto const &bl : block_giw) giw_real_vec.emplace_back(real(bl));

  auto block_giw_real = make_block_gf({"one", "two"}, giw_real_vec);

  // -- Build the real Block2 Green function

  auto giw_real_vecvec = std::vector{giw_real_vec, giw_real_vec};
  auto block2_giw_real = make_block2_gf({"one", "two"}, {"one", "two"}, giw_real_vecvec);

  // -- Asserts

  EXPECT_BLOCK_GF_NEAR(block_giw_real, real(block_giw));
  EXPECT_BLOCK2_GF_NEAR(block2_giw_real, real(block2_giw));

  EXPECT_TRUE(!is_gf_real_in_tau(block_giw[0]));
  EXPECT_TRUE(!is_gf_real_in_tau(block_giw[1]));
  EXPECT_TRUE(!is_gf_real_in_tau(block_giw));
  EXPECT_TRUE(!is_gf_real_in_tau(block2_giw));

  EXPECT_TRUE(is_gf_real_in_tau(make_real_in_tau(block_giw[0])));
  EXPECT_TRUE(is_gf_real_in_tau(make_real_in_tau(block_giw[1])));
  EXPECT_TRUE(is_gf_real_in_tau(make_real_in_tau(block_giw)));
  EXPECT_TRUE(is_gf_real_in_tau(make_real_in_tau(block2_giw)));
}

MAKE_MAIN;
