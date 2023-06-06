// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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

#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

TEST(Gf, r_k) {

  int N = 10;
  int S = 2;
  placeholder<0> k_;

  auto bz = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};
  auto gk = gf<brzone>{{bz, N}, {S, S}};

  gk(k_) << -2 * (cos(k_[0]) + cos(k_[1]));

  auto gr = gf<cyclat>{{N, N}, {S, S}};

  gr() = fourier(gk); // ADD Security if gf =

  for (auto r : gr.mesh())
    if (max_element(abs(gr[r])) > 1.e-10) std::cerr << r << gr[r] << std::endl;

  // reverse transfo
  auto gk2 = gk;
  gk2()    = fourier(gr);
  EXPECT_ARRAY_NEAR(gk.data(), gk2.data());

  // check gr
  auto gr_test                              = gf<cyclat>{{N, N}, {S, S}};
  gr_test[std::array<long, 3>{1, 0, 0}]     = -1;
  gr_test[std::array<long, 3>{N - 1, 0, 0}] = -1;
  gr_test[std::array<long, 3>{0, 1, 0}]     = -1;
  gr_test[std::array<long, 3>{0, N - 1, 0}] = -1;

  EXPECT_ARRAY_NEAR(gr.data(), gr_test.data());

  rw_h5(gk, "gk");
  rw_h5(gr, "gr");
}
MAKE_MAIN;
