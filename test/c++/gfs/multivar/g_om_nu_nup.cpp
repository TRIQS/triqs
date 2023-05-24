// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
using namespace triqs::clef;

TEST(Gf, G_om_nu_nup) {

  double beta = 1;

  auto G =
     gf<prod<imfreq, imfreq, imfreq>, tensor_valued<4>>{{{beta, Boson, 3}, {beta, Fermion, 10}, {beta, Fermion, 10}}, {2, 2, 2, 2}};

  // try to assign to expression
  placeholder<2> om_;
  placeholder<0> nu_;
  placeholder<1> nup_;

  G(om_, nu_, nup_) << 1 / (nu_ - 2.1 * nup_ + 1.6 * om_ + 1);

  dcomplex res1 = G[2, 3, 5](0, 0, 0, 0);
  dcomplex res  = 1 / (matsubara_freq{3, beta, Fermion} - 2.1 * matsubara_freq{5, beta, Fermion} + 1.6 * matsubara_freq{2, beta, Boson} + 1);
  EXPECT_CLOSE(res1, res);
  rw_h5(G, "ess_g_om_nu_nup.h5", "g");
}
MAKE_MAIN;
