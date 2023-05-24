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
// Authors: Hugo U. R. Strand, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

TEST(Gf, DensityFermion) {

  double beta   = 1;
  int n_im_freq = 400;
  auto G        = gf<imfreq, scalar_valued>{{beta, Fermion, n_im_freq}};

  EXPECT_EQ(G.mesh().positive_only(), 0);

  triqs::clef::placeholder<0> om_;
  G(om_) << 1 / (om_ + 2.3);

  auto n = triqs::gfs::density(G);

  EXPECT_COMPLEX_NEAR(n, 1 / (1 + std::exp(-beta * 2.3)), 1.e-9);

  rw_h5(G, "DensFerm");
}

TEST(Gf, DensityBoson) {

  double beta   = 1;
  int n_im_freq = 400;
  auto G        = gf<imfreq, scalar_valued>{{beta, Boson, n_im_freq}};

  EXPECT_EQ(G.mesh().positive_only(), 0);

  triqs::clef::placeholder<0> om_;
  G(om_) << 1 / (om_ - 2.3); // nb! need positive energy for a free boson

  auto n = triqs::gfs::density(G);

  EXPECT_COMPLEX_NEAR(n, 1 / (-1 + std::exp(beta * 2.3)), 1.e-9);

  rw_h5(G, "DensBos");
}

TEST(Gf, Density_with_not_all_moments) {

  double beta   = 1;
  int n_im_freq = 400;
  auto G        = gf<imfreq, scalar_valued>{{beta, Fermion, n_im_freq}};

  EXPECT_EQ(G.mesh().positive_only(), 0);

  triqs::clef::placeholder<0> om_;

  G(om_) << 1.0;
  EXPECT_THROW(triqs::gfs::density(G), triqs::runtime_error);

  G(om_) << om_;
  EXPECT_THROW(triqs::gfs::density(G), triqs::runtime_error);
}

TEST(Gf, DensityFermionReFreq) {

  double wmax = 10;
  int N       = 1000;
  auto h      = matrix<dcomplex>{{{1 + 0i, 1i}, {-1i, 2 + 0i}}};
  auto G      = gf<refreq>{{-wmax, wmax, N}, {2, 2}};

  //G(iw_) << inverse(w_ - h + eta); // FIXME
  for (auto const &w : G.mesh()) G[w] = inverse(w - h + 1e-8i);

  auto n = triqs::gfs::density(G);
  EXPECT_ARRAY_EQ(n, dagger(n));
}

MAKE_MAIN;
