// Copyright (c) 2023 Simons Foundation
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

#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;
using triqs::clef::placeholder;

constexpr auto _ = all_t{}; // NOLINT
using std::exp;

double eps_k(auto const &k) { return -2 * (cos(k[0]) + cos(k[1])); }
double onefermion(double tau, double eps, double beta) { return -exp(-eps * tau) / (1 + exp(-beta * eps)); }

// ------------------------------------------------------------

TEST(Gf, Gk_dlr_mat) {

  double beta  = 5;
  int n_bz     = 10;
  double w_max = 100.0;
  double eps   = 1e-10;

  auto bz = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};

  auto g1 = gf<prod<brzone, dlr_imtime>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, w_max, eps}}, {1, 1}};

  for (auto const &[k, tau] : g1.mesh()) g1[k, tau] = onefermion(tau, eps_k(k), beta);

  // do nothing
  auto g1b = partial_transform<1>(make_const_view(g1), [](auto &&gg) { return gg; });
  EXPECT_GF_NEAR(g1, g1b);

  auto g2 = partial_transform<1>(make_const_view(g1), [](auto &&gg) { return make_gf_dlr(gg); });
  auto g3 = partial_transform<1>(make_const_view(g2), [](auto &&gg) { return make_gf_dlr_imfreq(gg); });

  auto g3_check = gf<prod<brzone, dlr_imfreq>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, w_max, eps}}, {1, 1}};

  for (auto const &[k, w] : g3_check.mesh()) g3_check[k, w] = 1 / (w - eps_k(k));

  EXPECT_GF_NEAR(g3, g3_check);
}

// ------------------------------------------------------------
// same with other orders

TEST(Gf, G_dlr_k_mat) {

  double beta  = 5;
  int n_bz     = 10;
  double w_max = 100.0;
  double eps   = 1e-10;

  auto bz = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};

  auto g1 = gf<prod<dlr_imtime, brzone>, matrix_valued>{{{beta, Fermion, w_max, eps}, {bz, n_bz}}, {1, 1}};

  for (auto const &[tau, k] : g1.mesh()) g1[tau, k] = onefermion(tau, eps_k(k), beta);

  // do nothing
  auto g1b = partial_transform<0>(make_const_view(g1), [](auto &&gg) { return gg; });
  EXPECT_GF_NEAR(g1, g1b);

  auto g2 = partial_transform<0>(make_const_view(g1), [](auto &&gg) { return make_gf_dlr(gg); });
  auto g3 = partial_transform<0>(make_const_view(g2), [](auto &&gg) { return make_gf_dlr_imfreq(gg); });

  auto g3_check = gf<prod<dlr_imfreq, brzone>, matrix_valued>{{{beta, Fermion, w_max, eps}, {bz, n_bz}}, {1, 1}};

  for (auto const &[w, k] : g3_check.mesh()) g3_check[w, k] = 1 / (w - eps_k(k));

  EXPECT_GF_NEAR(g3, g3_check);
}

MAKE_MAIN;
