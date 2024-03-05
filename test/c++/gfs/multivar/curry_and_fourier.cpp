// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#ifndef NDA_DEBUG
#define NDA_DEBUG
#endif

#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

TEST(GfM, CurryFourier) {
  double beta = 1;
  auto bz     = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};

  int n_freq  = 100;
  int n_times = n_freq * 2 + 1;
  int n_bz    = 10;
  auto gkw    = gf<prod<brzone, imfreq>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, n_freq}}, {1, 1}};
  auto gkt    = gf<prod<brzone, imtime>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, n_times}}, {1, 1}};

  auto _ = all_t{};

  rw_h5(gkw, "curr_g_k_om.h5", "g");
  rw_h5(gkt, "curr_g_k_t.h5", "g");

  placeholder<0> k_;
  placeholder<1> w_;

  auto eps_k = -2 * (cos(k_[0]) + cos(k_[1]));
  gkw(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));

  auto const &mk = std::get<0>(gkt.mesh());

  for (auto const &k : mk) gkt[k, _] = fourier(gkw[k, _]);

  // works also, but uses the evaluator which return to the same point
  for (auto k : mk) { EXPECT_EQ(k.data_index(), mk.to_data_index(mk.to_index(closest_mesh_pt(k.value())))); }

  /// Testing the result
  auto gk_w_test = gf<imfreq>{{beta, Fermion, n_freq}, {1, 1}};
  auto gk_t_test = gf<imtime>{{beta, Fermion, n_times}, {1, 1}};

  for (auto k : std::get<0>(gkw.mesh())) {
    gk_w_test(w_) << 1 / (w_ - eval(eps_k, k_ = k) - 1 / (w_ + 2));
    gk_t_test() = fourier(gk_w_test);
    EXPECT_ARRAY_NEAR(gk_w_test.data(), gkw[k, _].data());
    EXPECT_ARRAY_NEAR(gk_t_test.data(), gkt[k, _].data());
  }
}
MAKE_MAIN;
