/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014-2015 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/arrays.hpp>
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
      auto se = mpi::slice_range(0, 2 * Nfreq - 1, world.size(), u);
      d(range(se.first, se.second + 1), 0, 0) *= (1 + u);
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
  gf<imfreq> g2 = mpi_reduce(g1, world);
  // out << g2.data()<<std::endl;
  if (world.rank() == 0) EXPECT_ARRAY_NEAR(g2.data(), world.size() * g1.data());
}

//----------------------------------------------

TEST_F(MpiGf, AllReduce) {
  // all reduction
  gf<imfreq> g2 = mpi_all_reduce(g1, world);
  EXPECT_ARRAY_NEAR(g2.data(), world.size() * g1.data());
}

//----------------------------------------------

TEST_F(MpiGf, ReduceView) { // all reduction of gf_view
  gf<imfreq> g2 = g1;
  g2()          = mpi_all_reduce(g1(), world);
  EXPECT_ARRAY_NEAR(g2.data(), world.size() * g1.data());
}

//----------------------------------------------

//TEST_F(MpiGf, ScatterGather) {
//// scatter-gather test with ="
//auto g2     = g1;
//g2.data()() = 0.0;
//auto g2b    = g1;
//g2          = mpi_scatter(g1);
//g2(w_) << g2(w_) * (1 + world.rank());
//g2b = mpi_gather(g2);
//if (world.rank() == 0) EXPECT_ARRAY_NEAR(d, g2b.data());
//}

//----------------------------------------------

//TEST_F(MpiGf, ScatterGatherWithConstruction) {
//// scatter-allgather test with construction

//gf<imfreq> g2 = mpi_scatter(g1);
//g1.data()()   = 0;
//g2(w_) << g2(w_) * (1 + world.rank());

//g1 = mpi_all_gather(g2);
//EXPECT_EQ(g1.mesh().first_index_window(), -Nfreq);
//EXPECT_EQ(g1.mesh().last_index_window(), Nfreq - 1);
//EXPECT_ARRAY_NEAR(d, g1.data());
//}

//----------------------------------------------

TEST_F(MpiGf, ReduceBlock) {
  // reduce with block Green's function
  block_gf<imfreq> bgf = make_block_gf({g1, g1, g1});
  EXPECT_ARRAY_NEAR(bgf[0].data(), g1.data());

  block_gf<imfreq> bgf2;
  auto bgf3 = bgf;

  bgf2 = mpi_reduce(bgf);
  if (world.rank() == 0) EXPECT_ARRAY_NEAR(bgf2[0].data(), world.size() * g1.data());

  bgf3 = mpi_all_reduce(bgf);
  EXPECT_ARRAY_NEAR(bgf3[0].data(), world.size() * g1.data());
}

//----------------------------------------------

TEST_F(MpiGf, ReduceBlockView) {
  block_gf<imfreq> bgf = make_block_gf({g1, g1, g1});

  auto bgf2 = bgf;

  bgf2() = mpi_reduce(bgf);
  if (world.rank() == 0) EXPECT_ARRAY_NEAR(bgf2[0].data(), world.size() * g1.data());

  bgf2() = mpi_all_reduce(bgf);
  EXPECT_ARRAY_NEAR(bgf2[0].data(), world.size() * g1.data());
}

//----------------------------------------------

//TEST_F(MpiGf, final) {
//auto g10 = gf<imfreq>{{beta, Fermion, Nfreq}, {1, 1}};
//g10(w_) << 1 / (w_ + 1);

//auto m  = mpi_scatter(gf_mesh<imfreq>{beta, Fermion, Nfreq}, world, 0);
//auto g3 = gf<imfreq>{m, {1, 1}};
//auto g4 = gf<imfreq>{m, {1, 1}};
//g3(w_) << 1 / (w_ + 1);
//g1.data()() = -9;
//g1(w_) << 1 / (w_ + 1);
//g4 = mpi_all_gather(g3);
//EXPECT_ARRAY_NEAR(g4.data(), g10.data());
//EXPECT_ARRAY_NEAR(g1.data(), g10.data());
//}
MAKE_MAIN;
