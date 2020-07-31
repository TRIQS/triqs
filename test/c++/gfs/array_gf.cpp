// Copyright (c) 2014-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2015 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2020 Simons Foundation
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

TEST(Gf, ArrayOf) {

  triqs::clef::placeholder<0> w_;
  auto agf = array<gf<imfreq>, 2>{2, 3};
  auto bgf = array<gf<imfreq>, 2>{2, 3};
  agf()    = gf<imfreq>{{10.0, Fermion}, {1, 1}};
  agf(0, 0)(w_) << 1 / (w_ + 2);

  // test H5
  {
    h5::file file("ess_array_gf.h5", 'w');
    h5_write(file, "Agf", agf);
  }
  {
    h5::file file("ess_array_gf.h5", 'r');
    h5_read(file, "Agf", bgf);
  }
  {
    h5::file file("ess_array_gf2.h5", 'w');
    h5_write(file, "Agf", bgf);
  }

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) EXPECT_GF_NEAR(agf(i, j), bgf(i, j));
}
MAKE_MAIN;
