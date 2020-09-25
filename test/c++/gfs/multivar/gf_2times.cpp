// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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

TEST(Gf, TwoTimes) {

  auto m  = mesh::retime{0, 10, 99};
  auto G  = gf<prod<retime, retime>>{{m, m}, {2, 2}};
  auto gg = gf<retime>{{m}, {2, 2}};

  triqs::clef::placeholder<0> t_;
  triqs::clef::placeholder<1> tp_;

  double beta   = 1;
  int n_im_freq = 100;

  auto G_w_wn2 =
     gf<prod<imfreq, imfreq>>({mesh::imfreq(beta, Fermion, n_im_freq), mesh::imfreq(beta, Fermion, n_im_freq)}, {2, 2});

  std::cout << G_w_wn2.mesh();
}
MAKE_MAIN;
