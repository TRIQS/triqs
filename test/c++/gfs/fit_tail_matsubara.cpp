// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#define NDA_DEBUG

#include <triqs/test_tools/gfs.hpp>

using namespace triqs::arrays;

TEST(FitTailMatsubara, Basic) { // NOLINT

  triqs::clef::placeholder<0> iw_;
  double beta = 10;
  int N       = 100;

  auto iw_mesh = mesh::imfreq{beta, Fermion, N};

  // Set the fraction of mesh points to use for the tail fit
  double tail_fraction = 0.3;
  iw_mesh.get_tail_fitter(tail_fraction);

  auto gw   = gf<imfreq>{iw_mesh, {1, 1}};
  auto gw_s = gf<imfreq, scalar_valued>{iw_mesh, {}};

  // Initialize the Green functions
  array<dcomplex, 1> c{0, 1, 3, 5};
  gw(iw_) << c(0) + c(1) / iw_ + c(2) / iw_ / iw_ + c(3) / iw_ / iw_ / iw_;
  gw_s(iw_) << c(0) + c(1) / iw_ + c(2) / iw_ / iw_ + c(3) / iw_ / iw_ / iw_;

  // ==== Fix only the 0th moment to 0
  {
    auto known_moments   = array<dcomplex, 3>{{{0.0}}};
    auto known_moments_s = array<dcomplex, 1>{0.0};

    // Get the tail using least square fitting
    auto [tail, err]     = fit_tail(gw, known_moments);
    auto [tail_s, err_s] = fit_tail(gw_s, known_moments_s);

    EXPECT_ARRAY_NEAR(c, tail(range(0, 4), 0, 0), 1e-8);
    EXPECT_ARRAY_NEAR(c, tail_s(range(0, 4)), 1e-8);
  }

  // ==== Now fix both the 0th and 1st moment
  {
    auto known_moments   = array<dcomplex, 3>{{{0.0}}, {{1.0}}};
    auto known_moments_s = array<dcomplex, 1>{0.0, 1.0};

    // Get the tail using least square fitting
    auto [tail, err]     = fit_tail(gw, known_moments);
    auto [tail_s, err_s] = fit_tail(gw_s, known_moments_s);

    EXPECT_ARRAY_NEAR(c, tail(range(4), 0, 0), 1e-8);
    EXPECT_ARRAY_NEAR(c, tail_s(range(4)), 1e-8);
  }
}

// ------------------------------------------------------------------------------

TEST(FitTailMatsubara, Real_F_and_B) { // NOLINT

  triqs::clef::placeholder<0> iw_;
  triqs::clef::placeholder<0> iW_;
  double beta = 10;
  int N       = 100;

  auto gw_f = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
  auto gw_b = gf<imfreq>{{beta, Boson, N - 1}, {1, 1}};

  // Set the fraction of mesh points to use for the tail fit
  double tail_fraction = 0.3;
  gw_f.mesh().get_tail_fitter(tail_fraction);
  gw_b.mesh().get_tail_fitter(tail_fraction);

  // Initialize the Green functions
  gw_f(iw_) << 1 / (iw_ - 1);
  gw_b(iW_) << 1 / (iW_ - 1);

  // Fix both the 0th and 1st moment
  auto known_moments = array<dcomplex, 3>{{{0.0}}, {{1.0}}};

  auto [tail_f, err_r] = fit_tail(gw_f, known_moments);
  auto [tail_b, err_b] = fit_tail(gw_b, known_moments);

  auto tail_exact = array<dcomplex, 1>{0.0, 1.0, 1.0, 1.0, 1.0};

  EXPECT_ARRAY_NEAR(tail_exact, tail_f(range(5), 0, 0), 1e-9);
  EXPECT_ARRAY_NEAR(tail_exact, tail_b(range(5), 0, 0), 1e-9);
}

// ------------------------------------------------------------------------------

TEST(FitTailMatsubara, Complex) { // NOLINT

  triqs::clef::placeholder<0> iw_;
  double beta = 10;
  size_t N       = 200;

  auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};

  // Initialize the Green functions
  auto a = dcomplex(1.0, 0.4);
  gw(iw_) << 1 / (iw_ - a);

  // Fix both the 0th and 1st moment
  auto known_moments = array<dcomplex, 3>{{{0.0}}, {{1.0}}};

  auto [tail, err] = fit_tail(gw, known_moments);

  auto tail_exact = array<dcomplex, 1>{dcomplex(0, 0), dcomplex(1, 0), a, std::pow(a, 2), std::pow(a, 3)};

  EXPECT_ARRAY_NEAR(tail_exact, tail(range(5), 0, 0), 1e-7);
}

// ------------------------------------------------------------------------------

TEST(FitTailMatsubara, Multivar) { // NOLINT

  triqs::clef::placeholder<0> k_;
  triqs::clef::placeholder<1> iW_;
  triqs::clef::placeholder<2> iw_;

  int N_k     = 4;
  double beta = 10;
  size_t N_iW    = 4;
  size_t N_iw    = 100;

  auto BL        = bravais_lattice{matrix<double>{{1, 0}, {0, 1}}};
  auto BZ        = brillouin_zone{BL};
  auto k_mesh    = mesh::brzone(BZ, N_k);
  auto iW_mesh   = mesh::imfreq{beta, Boson, N_iW};
  auto iw_mesh   = mesh::imfreq{beta, Fermion, N_iw};
  auto prod_mesh = k_mesh * iW_mesh * iw_mesh;

  auto g = gf{prod_mesh, {1, 1}};

  // Initialize the Multivariable Green functions
  g(k_, iW_, iw_) << 1 / (iw_ + iW_ - cos(k_[0]) * cos(k_[1]));

  // Fix both the 0th and 1st moment
  auto known_moments                       = array<dcomplex, 5>(2, N_k * N_k, 2 * N_iW - 1, 1, 1);
  known_moments(0, range(), range(), 0, 0) = 0.0;
  known_moments(1, range(), range(), 0, 0) = 1.0;

  // Fit for all k-points. Resulting shape is (N_orders, N_k * N_k, 1, 1)
  auto [tail, err] = fit_tail<2>(g, known_moments);

  // Calculate the exact tail
  auto tail_exact = array<dcomplex, 3>(5, N_k * N_k, 2 * N_iW - 1);
  for (auto [k, iW] : itertools::product(k_mesh, iW_mesh)) {
    dcomplex pole = cos(k[0]) * cos(k[1]) - iW;
    tail_exact(range(), k.linear_index(), iW.linear_index()) =
       array<dcomplex, 1>{dcomplex(0.0, 0.0), dcomplex(1.0, 0.0), pole, std::pow(pole, 2), std::pow(pole, 3)};
  }

  EXPECT_ARRAY_NEAR(tail_exact, tail(range(5), range(), range(), 0, 0), 1e-6);
}

MAKE_MAIN;
