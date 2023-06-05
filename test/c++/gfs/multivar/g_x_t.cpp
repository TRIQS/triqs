// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell, Thomas Ayral

#include <triqs/test_tools/gfs.hpp>

using namespace triqs::clef;
using namespace triqs::lattice;

double beta  = 1;
int n_freq   = 100;
double t_min = -10, t_max = 10;
int n_times   = n_freq * 2 + 1;
int L         = 16;
int n_bz      = L;
auto _        = all_t{};
auto bz       = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};
using idx_t   = std::array<long, 3>;

TEST(Gf, x_t) {

  auto gkt = gf<prod<brzone, retime>, matrix_valued>{{{bz, n_bz}, {t_min, t_max, n_times}}, {1, 1}};

  placeholder<0> k_;
  placeholder<1> t_;

  auto eps_k = -2 * (cos(k_[0]) + cos(k_[1]));
  gkt(k_, t_) << exp(-1i * eps_k * t_);

  auto gxt = gf<prod<cyclat, retime>, matrix_valued>{{{L, L}, {t_min, t_max, n_times}}, {1, 1}};

  for (auto const &t : std::get<1>(gxt.mesh())) gxt[_, t] = fourier(gkt[_, t]);

  EXPECT_GF_NEAR(gxt, rw_h5(gxt, "ess_g_x_t.h5", "g"));

  EXPECT_ARRAY_NEAR(matrix<dcomplex>{{1}}, gxt(idx_t{0, 0, 0}, 0.0));
  EXPECT_ARRAY_NEAR(matrix<dcomplex>{gxt(idx_t{2, 0, 0}, 0.0)}, gxt(idx_t{1, 0, 0} + idx_t{1, 0, 0}, 0.0));
  EXPECT_ARRAY_NEAR(matrix<dcomplex>{gxt(idx_t{0, 0, 0}, 0.0)}, gxt(idx_t{1, 0, 0} - idx_t{1, 0, 0}, 0.0));
}

// ------------------------------------------------------------------------------------------------------

TEST(Gf, x_tau) {

  auto gkt = gf<prod<brzone, imtime>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, n_times}}, {1, 1}};

  placeholder<0> k_;
  placeholder<1> tau_;

  auto eps_k = -2 * (cos(k_[0]) + cos(k_[1]));
  gkt(k_, tau_) << exp(-eps_k * tau_);

  auto gxt = gf<prod<cyclat, imtime>, matrix_valued>{{{L, L}, {beta, Fermion, n_times}}, {1, 1}};

  for (auto const &t : std::get<1>(gxt.mesh())) gxt[_, t] = fourier(gkt[_, t]);

  auto gg = rw_h5(gxt, "ess_g_x_tau.h5", "g");

  EXPECT_EQ(gxt.mesh(), gg.mesh());
  EXPECT_GF_NEAR(gxt, gg);
}
MAKE_MAIN;
