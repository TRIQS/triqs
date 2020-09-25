// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

TEST(GfCartesian, H5_RW_EvaluatorS) {
  double beta = 1;
  auto g      = gf<prod<imfreq, imfreq>, scalar_valued>{{{beta, Fermion, 5}, {beta, Boson, 5}}};
  g()         = 2;

  h5::file file("g_nu_nuph5", 'w');
  h5_write(file, "g", g);
  gf<prod<imfreq, imfreq>, scalar_valued> g2{};
  h5_read(file, "g", g2);

  EXPECT_ARRAY_NEAR(g.data(), g2.data());
  //EXPECT_GF_NEAR(g, g2);

  auto w0  = matsubara_freq(0, beta, Fermion);
  auto W10 = matsubara_freq(10, beta, Boson);
  auto W0  = matsubara_freq(0, beta, Boson);

  EXPECT_COMPLEX_NEAR(g(w0, W0), g2(w0, W0), 1.e-13);
  EXPECT_COMPLEX_NEAR(g(w0, W10), g2(w0, W10), 1.e-13);
}

TEST(GfCartesian, H5_RW_EvaluatorM) {
  double beta = 1;
  auto g      = gf<prod<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 5}, {beta, Boson, 5}}, {1, 1}};
  g()         = 2;

  h5::file file("g_nu_nuph5", 'w');
  h5_write(file, "g", g);
  gf<prod<imfreq, imfreq>, matrix_valued> g2{};
  h5_read(file, "g", g2);

  EXPECT_ARRAY_NEAR(g.data(), g2.data());
  //EXPECT_GF_NEAR(g, g2);

  auto w0  = matsubara_freq(0, beta, Fermion);
  auto W10 = matsubara_freq(10, beta, Boson);
  auto W0  = matsubara_freq(0, beta, Boson);

  EXPECT_ARRAY_NEAR(g(w0, W0), g2(w0, W0));
  EXPECT_ARRAY_NEAR(g(w0, W10), g2(w0, W10));
}

MAKE_MAIN;
