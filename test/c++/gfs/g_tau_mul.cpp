// Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2023 Simons Foundation
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

#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;

TEST(GfMul, Statistic) {
  double beta = 10.0;

  auto f = gf<imtime>{{beta, Fermion, 100}, {1, 1}};
  auto b = gf<imtime>{{beta, Boson, 100}, {1, 1}};

  EXPECT_THROW(gf<imtime>{f + b}, triqs::runtime_error);

  {
    gf<imtime> p = gf<imtime>{f * f};
    EXPECT_TRUE(p.mesh().statistic() == Boson);
  }
  {
    gf<imtime> p = gf<imtime>{f * b};
    EXPECT_TRUE(p.mesh().statistic() == Fermion);
  }
  {
    gf<imtime> p = gf<imtime>{b * f};
    EXPECT_TRUE(p.mesh().statistic() == Fermion);
  }
  {
    gf<imtime> p = gf<imtime>{b * b};
    EXPECT_TRUE(p.mesh().statistic() == Boson);
  }
}

// -------------------------------------

TEST(GfMul, Statistic2Imtime) {
  double beta = 10.0;
  using g_t   = gf<prod<imtime, imtime>>;

  auto ff = g_t{{{beta, Fermion, 100}, {beta, Fermion, 100}}, {1, 1}};
  auto fb = g_t{{{beta, Fermion, 100}, {beta, Boson, 100}}, {1, 1}};

  EXPECT_THROW(g_t{ff + fb}, triqs::runtime_error);

  {
    g_t p = g_t{ff * ff};
    EXPECT_TRUE(std::get<0>(p.mesh()).statistic() == Boson);
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Boson);
  }
  {
    g_t p = g_t{ff * fb};
    EXPECT_TRUE(std::get<0>(p.mesh()).statistic() == Boson);
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Fermion);
  }
  {
    g_t p = g_t{fb * ff};
    EXPECT_TRUE(std::get<0>(p.mesh()).statistic() == Boson);
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Fermion);
  }
  {
    g_t p = g_t{fb * fb};
    EXPECT_TRUE(std::get<0>(p.mesh()).statistic() == Boson);
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Boson);
  }
}

// -------------------------------------

TEST(GfMul, Statistic_k_tau) {
  using g_t = gf<prod<brzone, imtime>, matrix_valued>;

  double beta = 10.0;
  int n_bz    = 20;
  auto bz     = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};
  auto g_eps  = gf<brzone>{{bz, n_bz}, {1, 1}};

  auto f = g_t{{{bz, n_bz}, {beta, Fermion, 100}}, {1, 1}};
  auto b = g_t{{{bz, n_bz}, {beta, Boson, 100}}, {1, 1}};

  EXPECT_THROW(g_t{f + b}, triqs::runtime_error);
  EXPECT_THROW(g_t{f - b}, triqs::runtime_error);

  {
    g_t p = g_t{f + f};
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Fermion);
  }
  {
    g_t p = g_t{b + b};
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Boson);
  }
  {
    g_t p = g_t{f * f};
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Boson);
  }
  {
    g_t p = g_t{f * b};
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Fermion);
  }
  {
    g_t p = g_t{b * f};
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Fermion);
  }
  {
    g_t p = g_t{b * b};
    EXPECT_TRUE(std::get<1>(p.mesh()).statistic() == Boson);
  }
}

MAKE_MAIN;
