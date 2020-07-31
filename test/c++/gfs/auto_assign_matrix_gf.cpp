// Copyright (c) 2015-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2016 Centre national de la recherche scientifique (CNRS)
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
using triqs::clef::placeholder;
using triqs::clef::placeholder_prime;

TEST(Gf, AutoAssignMatrixGf1) {

  double beta = 2.3;
  auto g      = gf<imfreq, matrix_valued>({beta, Fermion, 10}, {2, 2});
  auto sig    = matrix<double>{{1, -1}, {-1, 1}};

  placeholder_prime<0> om_;
  placeholder<1> u_;
  placeholder<2> v_;

  g[om_](u_, v_) << 1 / (om_ - 3) * sig(u_, v_);

  // CHECK
  for (int om = -10; om < 10; ++om) EXPECT_ARRAY_NEAR(g[om](), sig / ((2 * om + 1) * M_PI * 1i / beta - 3));
}

// ---------------------------------

TEST(Gf, AutoAssignMatrixGf2) {

  double beta = 2.3;
  auto a      = array<gf<imfreq, scalar_valued>, 2>(2, 2);
  auto sig    = matrix<double>{{1, -1}, {-1, 1}};

  placeholder_prime<0> om_;
  placeholder<1> u_;
  placeholder<2> v_;

  auto g0 = gf<imfreq, scalar_valued>({beta, Fermion, 10}, {});
  a(u_, v_) << g0;
  a(u_, v_)[om_] << 1 / (om_ - 3) * sig(u_, v_);

  // CHECK
  for (int u = 0; u < 2; ++u)
    for (int v = 0; v < 2; ++v)
      for (int om = -10; om < 10; ++om) EXPECT_CLOSE(a(u, v)[om], sig(u, v) / ((2 * om + 1) * M_PI * 1i / beta - 3));
}

MAKE_MAIN;
