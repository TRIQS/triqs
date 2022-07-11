// Copyright (c) 2014-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2016 Centre national de la recherche scientifique (CNRS)
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
using namespace triqs::gfs;
using namespace triqs::arrays;

TEST(Gf, PlaceHolder) {

  double wmax = 10;
  int N       = 10;
  auto G1     = gf<refreq>{{-wmax, wmax, N}, {1, 1}};
  auto G2     = gf<refreq>{{-wmax, wmax, N * 2}, {1, 1}};

  triqs::clef::placeholder<0> om_;

  G1(om_) << om_ + 0.1i;
  G2(om_) << om_ + 0.1i;

  auto g1 = G1;

  //for (auto w : g1.mesh()) {  
    //g1[w] = G1(w);
  //}

  g1(om_) << g1(om_);

  EXPECT_GF_NEAR(G1, g1);

  // the placeholder is evaluated -> mesh_point_t -> domain::value_t -> evaluated ...
  G1(om_) << om_ + 0.1i - G2(om_);
  EXPECT_ARRAY_ZERO(G1.data());

  auto g = G1;
  g(om_) << g(om_);
  EXPECT_GF_NEAR(G1, g);
}
MAKE_MAIN;
