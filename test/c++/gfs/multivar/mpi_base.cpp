// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Thomas Ayral, Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

using gf3_s = gf<prod<imfreq, imfreq, imfreq>, scalar_valued>;

TEST(Gfs, MPI_multivar) {

  mpi::communicator world;

  int nw = 2, nbw = 10;
  double beta = 10;
  clef::placeholder<3> iw_;
  clef::placeholder<4> inu_;
  clef::placeholder<5> inup_;

  auto g = gf3_s{{{beta, Boson, nbw}, {beta, Fermion, nw}, {beta, Fermion, nw}}};
  g(iw_, inu_, inup_) << inu_ + 10 * inup_ + 100 * iw_;

  auto g2 = g;
  g2      = mpi::reduce(g, world);
  if (world.rank() == 0) { EXPECT_ARRAY_NEAR(g2.data(), g.data() * world.size()); }

  mpi::broadcast(g2, world);
  if (world.rank() == 1) { EXPECT_ARRAY_NEAR(g2.data(), g.data() * world.size()); }

  gf3_s g3 = mpi::all_reduce(g, world);
  EXPECT_ARRAY_NEAR(g3.data(), g.data() * world.size());

  //gf3_s g4 = mpi::scatter(g);
  //g2(iw_, inu_, inup_) << g2(iw_, inu_, inup_) * (1 + world.rank());
  //g4 = mpi_gather(g2);
  // Test the result ?

  auto G  = make_block_gf({g});
  auto g0 = gf<imfreq, scalar_valued>{{beta, Boson, nbw}};
  auto G2 = make_block_gf({g0});

  mpi::broadcast(G, world);
  mpi::broadcast(G2, world);
}
MAKE_MAIN;
