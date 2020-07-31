// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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

#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/arrays.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>
#include <iostream>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::gfs;
using namespace triqs::clef;

mpi::communicator world;

class MpiGf : public ::testing::Test {
  protected:
  virtual void SetUp() {
    g1 = gf<imfreq>{{beta, Fermion, Nfreq}, {1, 1}};
    g1(w_) << 1 / (w_ + 1);

    d = make_clone(g1.data());
    for (int u = 0; u < world.size(); ++u) {
      auto se = itertools::chunk_range(0, 2 * Nfreq, world.size(), u);
      d(range(se.first, se.second), 0, 0) *= (1 + u);
    }
  }

  double beta = 10;
  int Nfreq   = 8;
  placeholder<0> w_;
  gf<imfreq> g1;
  array<dcomplex, 3> d;
};

//----------------------------------------------

TEST_F(MpiGf, Reduce) {
  // reduction
  gf<imfreq> g2 = mpi::reduce(g1, world);
  // out << g2.data()<<std::endl;
  if (world.rank() == 0) test_gfs_are_close(g2, gf{world.size() * g1});
}

//----------------------------------------------

TEST_F(MpiGf, AllReduce) {
  // all reduction
  gf<imfreq> g2 = mpi::all_reduce(g1, world);
  test_gfs_are_close(g2, gf{world.size() * g1});
}

//----------------------------------------------

TEST_F(MpiGf, ReduceView) { // all reduction of gf_view
  gf<imfreq> g2 = g1;
  g2()          = mpi::all_reduce(g1(), world);
  test_gfs_are_close(g2, gf{world.size() * g1});
}

//----------------------------------------------

//TEST_F(MpiGf, ScatterGather) {
//// scatter-gather test with ="
//auto g2     = g1;
//g2.data()() = 0.0;
//auto g2b    = g1;
//g2          = mpi::scatter(g1);
//g2(w_) << g2(w_) * (1 + world.rank());
//g2b = mpi::gather(g2);
//if (world.rank() == 0) EXPECT_ARRAY_NEAR(d, g2b.data());
//}

//----------------------------------------------

//TEST_F(MpiGf, ScatterGatherWithConstruction) {
//// scatter-allgather test with construction

//gf<imfreq> g2 = mpi::scatter(g1);
//g1.data()()   = 0;
//g2(w_) << g2(w_) * (1 + world.rank());

//g1 = mpi::all_gather(g2);
//EXPECT_EQ(g1.mesh().first_index_window(), -Nfreq);
//EXPECT_EQ(g1.mesh().last_index_window(), Nfreq - 1);
//EXPECT_ARRAY_NEAR(d, g1.data());
//}

//----------------------------------------------

TEST_F(MpiGf, ReduceBlock) {
  // reduce with block Green's function
  block_gf<imfreq> bgf = make_block_gf({g1, g1, g1});
  test_gfs_are_close(bgf[0], g1);

  block_gf<imfreq> bgf2;
  auto bgf3 = bgf;

  bgf2 = mpi::reduce(bgf);
  if (world.rank() == 0) test_gfs_are_close(bgf2[0], gf{world.size() * g1});

  bgf3 = mpi::all_reduce(bgf);
  test_gfs_are_close(bgf3[0], gf{world.size() * g1});
}

//----------------------------------------------

TEST_F(MpiGf, ReduceBlockView) {
  block_gf<imfreq> bgf = make_block_gf({g1, g1, g1});

  auto bgf2 = bgf;

  bgf2() = mpi::reduce(bgf);
  if (world.rank() == 0) test_gfs_are_close(bgf2[0], gf{world.size() * g1});

  bgf2() = mpi::all_reduce(bgf);
  test_gfs_are_close(bgf2[0], gf{world.size() * g1});
}

//----------------------------------------------

//TEST_F(MpiGf, final) {
//auto g10 = gf<imfreq>{{beta, Fermion, Nfreq}, {1, 1}};
//g10(w_) << 1 / (w_ + 1);

//auto m  = mpi::scatter(gf_mesh<imfreq>{beta, Fermion, Nfreq}, world, 0);
//auto g3 = gf<imfreq>{m, {1, 1}};
//auto g4 = gf<imfreq>{m, {1, 1}};
//g3(w_) << 1 / (w_ + 1);
//g1.data()() = -9;
//g1(w_) << 1 / (w_ + 1);
//g4 = mpi::all_gather(g3);
//EXPECT_ARRAY_NEAR(g4.data(), g10.data());
//EXPECT_ARRAY_NEAR(g1.data(), g10.data());
//}
MAKE_MAIN;
