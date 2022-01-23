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
// Authors: Olivier Parcollet

#define TRIQS_DEBUG
#include <triqs/test_tools/gfs.hpp>

// ----------------------------------------------------------

using namespace triqs::lattice;

TEST(CtHyb, gf_inverse1) {
  double beta = 100.0;
  size_t n_iw    = 1025;

  auto G_iw = gf<imfreq>{{beta, Fermion, n_iw}, {1, 1}};
  triqs::clef::placeholder<0> om_;
  G_iw(om_) << om_ - 2.8;
  auto G_iw_inv = G_iw;
  G_iw_inv(om_) << 1. / (om_ - 2.8);
  G_iw = inverse(G_iw);
  EXPECT_GF_NEAR(G_iw, G_iw_inv);
}

TEST(CtHyb, gf_inverse2) {
  double beta = 100.0;
  int n_iw    = 100;

  int nk   = 4;
  double t = 1.0;
  auto bz  = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};

  auto G_iw = gf<prod<imfreq, brzone>>{{{beta, Fermion, n_iw}, {bz, nk}}, {1, 1}};

  triqs::clef::placeholder<0> om_;
  triqs::clef::placeholder<1> k_;

  G_iw(om_, k_) << om_ - 2 * t * (cos(k_(0)) + cos(k_(1)));

  auto G_iw_inv = G_iw;
  G_iw_inv(om_, k_) << 1. / (om_ - 2 * t * (cos(k_(0)) + cos(k_(1))));

  G_iw = inverse(G_iw);

  EXPECT_GF_NEAR(G_iw, G_iw_inv);
}

TEST(CtHyb, gf_inverse3) {
  double beta = 100.0;
  int n_iw    = 10;
  int n_t     = 10;

  int nk   = 4;
  double t = 1.0;
  auto bz  = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};

  auto G_iw = gf<prod<imfreq, imtime, brzone>>{{{beta, Fermion, n_iw}, {beta, Fermion, n_t}, {bz, nk}}, {1, 1}};

  triqs::clef::placeholder<0> om_;
  triqs::clef::placeholder<2> t_;
  triqs::clef::placeholder<1> k_;

  G_iw(om_, t_, k_) << om_ - 7 * t_ - 2 * t * (cos(k_(0)) + cos(k_(1)));

  auto G_iw_inv = G_iw;
  G_iw_inv(om_, t_, k_) << 1. / (om_ - 7 * t_ - 2 * t * (cos(k_(0)) + cos(k_(1))));

  G_iw = inverse(G_iw);

  EXPECT_GF_NEAR(G_iw, G_iw_inv);
}

MAKE_MAIN;
