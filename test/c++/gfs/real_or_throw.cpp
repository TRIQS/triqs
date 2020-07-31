// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Base) {

  triqs::clef::placeholder<0> t_;
  double beta = 1;

  auto Gt = gf<imtime>{{beta, Fermion, 100}, {2, 2}};

  Gt(t_) << 8 * t_;
  auto gtr           = real(Gt);
  array<double, 3> D = real(Gt.data());
  EXPECT_ARRAY_NEAR(D, gtr.data());
  EXPECT_TRUE(is_gf_real(Gt));

  Gt(t_) << 8i * t_;
  EXPECT_FALSE(is_gf_real(Gt));
}
MAKE_MAIN;
