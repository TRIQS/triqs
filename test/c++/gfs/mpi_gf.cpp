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

#ifndef NDA_DEBUG
#define NDA_DEBUG
#endif

#include <triqs/test_tools/arrays.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>

#include <fmt/ranges.h>

using namespace triqs;
using namespace nda;
using namespace triqs::gfs;
using namespace nda::clef;

class TRIQSGF : public ::testing::Test {
  protected:
  using imfreq_cube_mesh_t = prod<imfreq, imfreq, imfreq>;
  using gf1_t              = gf<imfreq>;
  using gf2_t              = gf<imfreq_cube_mesh_t, tensor_valued<4>>;
  using block_gf1_t        = block_gf<imfreq>;
  using block_gf2_t        = block2_gf<imfreq_cube_mesh_t, tensor_valued<4>>;

  void SetUp() override {
    placeholder<0> w_;
    auto m = imfreq{beta, Fermion, Nfreq};
    g1     = gf1_t{m, {1, 1}};
    g1(w_) << 1 / (w_ + 1);

    placeholder<1> w1_;
    placeholder<2> w2_;
    placeholder<3> w3_;
    auto m3 = imfreq_cube_mesh_t{m, m, m};
    g2      = gf2_t{m3, {2, 2, 2, 2}};
    g2(w1_, w2_, w3_) << w1_ * w1_ * w1_ + w2_ * w2_ + w3_;
    bgf1 = make_block_gf(3, g1);
    bgf2 = make_block2_gf(2, 2, g2);
  }

  int root    = 0;
  double beta = 10;
  int Nfreq   = 8;
  gf1_t g1;
  gf2_t g2;
  block_gf1_t bgf1;
  block_gf2_t bgf2;
  mpi::communicator world;
};

// Check if two (block) Green's functions are close.
template <typename G1, typename G2> void check_gfs(G1 const &g1, G2 const &g2, double eps = 1e-6) {
  if constexpr (BlockGreenFunction_v<G1>) {
    if constexpr (G1::arity == 1) {
      EXPECT_BLOCK_GF_NEAR(g1, g2, eps);
    } else {
      EXPECT_BLOCK2_GF_NEAR(g1, g2, eps);
    }
  } else {
    EXPECT_GF_NEAR(g1, g2, eps);
  }
}

// Create empty (block) Green's function.
template <typename G> G make_empty_gf(G const &g) {
  if constexpr (BlockGreenFunction_v<G>) {
    auto g_tmp = typename G::g_t{(*g.begin()).mesh(), (*g.begin()).target_shape()};
    if constexpr (G::arity == 1)
      return G{g.size(), g_tmp};
    else
      return G{g.size1(), g.size2(), g_tmp};
  } else {
    return G{g.mesh(), g.target_shape()};
  }
}

// Check if a (block) Green's function is empty.
template <typename G> void check_empty_gf(G const &g) {
  if constexpr (BlockGreenFunction_v<G>) {
    if constexpr (G::arity == 1) {
      EXPECT_EQ(g.size(), 0);
    } else {
      EXPECT_EQ(g.size1(), 0);
    }
    EXPECT_EQ(g.block_names().size(), 0);
  } else {
    EXPECT_EQ(g.data().size(), 0);
  }
}

// Test MPI broadcast.
template <typename G> void test_bcast(G const &g, mpi::communicator world, int root) {
  // broadcast
  auto g_bcast = G{};
  if (world.rank() == root) g_bcast = g;
  mpi::broadcast(g_bcast, world, root);
  check_gfs(g_bcast, g, 1e-12);

  // broadcast view
  g_bcast = g;
  if (world.rank() != root) g_bcast *= 0;
  mpi::broadcast(g_bcast(), world, root);
  check_gfs(g_bcast, g, 1e-12);
}

// Test MPI reduce and allreduce into existing (block) Green's functions.
template <typename G> void test_reduce_into(G const &g, mpi::communicator world, int root) {
  // reduce
  auto g_red = G{};
  mpi::reduce_into(g, g_red, world, root);
  if (world.rank() == root) {
    check_gfs(g_red, G{world.size() * g});
  } else {
    check_empty_gf(g_red);
  }

  // allreduce
  g_red = G{};
  mpi::all_reduce_into(g, g_red, world);
  check_gfs(g_red, G{world.size() * g});

  // allreduce const view
  auto g_red_v = make_empty_gf(g);
  mpi::all_reduce_into(typename G::const_view_type{g}, g_red_v(), world);
  check_gfs(g_red_v, G{world.size() * g});
}

// Test in place MPI reduce and allreduce.
template <typename G> void test_reduce_in_place(G const &g, mpi::communicator world, int root) {
  // in place reduce
  auto g_red = g;
  mpi::reduce_in_place(g_red, world, root);
  if (world.rank() == root) {
    check_gfs(g_red, G{world.size() * g});
  } else {
    check_gfs(g_red, g);
  }

  // in place allreduce
  g_red = g;
  mpi::all_reduce_in_place(g_red, world);
  check_gfs(g_red, G{world.size() * g});

  // in place reduce view
  g_red = g;
  mpi::reduce_in_place(g_red(), world, root);
  if (world.rank() == root) {
    check_gfs(g_red, G{world.size() * g});
  } else {
    check_gfs(g_red, g);
  }

  // in place allreduce view
  g_red = g;
  mpi::all_reduce_in_place(g_red(), world);
  check_gfs(g_red, G{world.size() * g});
}

// Test in place MPI reduce and allreduce using mpi_reduce_into.
template <typename G> void test_reduce_in_place_into(G const &g, mpi::communicator world, int root) {
  // in place reduce
  auto g_red = g;
  mpi::reduce_into(g_red, g_red, world, root);
  if (world.rank() == root) {
    check_gfs(g_red, G{world.size() * g});
  } else {
    check_gfs(g_red, g);
  }

  // in place allreduce
  g_red = g;
  mpi::all_reduce_into(g_red, g_red, world);
  check_gfs(g_red, G{world.size() * g});

  // in place reduce view
  g_red = g;
  mpi_reduce_into(g_red, g_red(), world, root);
  if (world.rank() == root) {
    check_gfs(g_red, G{world.size() * g});
  } else {
    check_gfs(g_red, g);
  }

  // in place allreduce view
  g_red = g;
  mpi::all_reduce_into(g_red(), g_red(), world);
  check_gfs(g_red, G{world.size() * g});
}

// Test MPI reduce and allreduce.
template <typename G> void test_reduce(G const &g, mpi::communicator world, int root) {
  // reduce
  auto g_red = mpi::reduce(g, world, root);
  if (world.rank() == root) {
    check_gfs(g_red, G{world.size() * g});
  } else {
    check_empty_gf(g_red);
  }

  // allreduce
  auto g_red_all = mpi::all_reduce(g, world);
  check_gfs(g_red_all, G{world.size() * g});

  // allreduce view
  auto g_red_v = mpi::all_reduce(g(), world);
  check_gfs(g_red_v, G{world.size() * g});

  // allreduce const view
  auto g_red_cv = mpi::all_reduce(typename G::const_view_type{g}, world);
  check_gfs(g_red_cv, G{world.size() * g});
}

TEST_F(TRIQSGF, BroadcastGF) {
  test_bcast(g1, world, root);
  test_bcast(g2, world, root);
}

TEST_F(TRIQSGF, BroadcastGFView) {
  // broadcast a GF view into a GF view
  if (world.rank() == root) {
    mpi::broadcast(g2[0, 0, nda::range::all], world, root);
  } else {
    mpi::broadcast(g2[1, 1, nda::range::all], world, root);
    EXPECT_GF_NEAR(g2[0, 0, nda::range::all], g2[1, 1, nda::range::all]);
  }
}

TEST_F(TRIQSGF, ReduceIntoExistingGF) {
  test_reduce_into(g1, world, root);
  test_reduce_into(g2, world, root);
}

TEST_F(TRIQSGF, ReduceIntoExistingGFView) {
  // reduce a GF view into an existing GF view
  mpi::reduce_into(g2[0, 0, nda::range::all], g2[1, 1, nda::range::all], world, root);
  if (world.rank() == root) { EXPECT_GF_NEAR(g2[1, 1, nda::range::all], gf{g2[0, 0, nda::range::all] * world.size()}); }
}

TEST_F(TRIQSGF, ReduceInPlaceGF) {
  test_reduce_in_place(g1, world, root);
  test_reduce_in_place(g2, world, root);
}

TEST_F(TRIQSGF, ReduceInPlaceGFView) {
  // allreduce a GF view in place
  auto g2_copy = g2;
  mpi::all_reduce_in_place(g2[0, 0, nda::range::all], world);
  EXPECT_GF_NEAR(g2[0, 0, nda::range::all], gf{g2_copy[0, 0, nda::range::all] * world.size()});
}

TEST_F(TRIQSGF, ReduceIntoExistingGFInPlace) {
  test_reduce_in_place_into(g1, world, root);
  test_reduce_in_place_into(g2, world, root);
}

TEST_F(TRIQSGF, ReduceIntoExistingGFViewInPlace) {
  // allreduce a non-contiguous GF view in place
  auto g2_copy = g2;
  mpi::all_reduce_into(g2[0, nda::range::all, 0], g2[0, nda::range::all, 0], world);
  EXPECT_GF_NEAR(g2[0, nda::range::all, 0], gf{g2_copy[0, nda::range::all, 0] * world.size()});
}

TEST_F(TRIQSGF, ReduceGF) {
  test_reduce(g1, world, root);
  test_reduce(g2, world, root);
}

TEST_F(TRIQSGF, ReduceGFView) {
  // allreduce a non-contiguous GF view
  auto g_red = mpi::all_reduce(g2[nda::range::all, 1, 0], world);
  EXPECT_GF_NEAR(g_red, gf{g2[nda::range::all, 1, 0] * world.size()});
}

TEST_F(TRIQSGF, BroadcastBlockGF) {
  test_bcast(bgf1, world, root);
  test_bcast(bgf2, world, root);
}

TEST_F(TRIQSGF, ReduceIntoExistingBlockGF) {
  test_reduce_into(bgf1, world, root);
  test_reduce_into(bgf2, world, root);
}

TEST_F(TRIQSGF, ReduceInPlaceBlockGF) {
  test_reduce_in_place(bgf1, world, root);
  test_reduce_in_place(bgf2, world, root);
}

TEST_F(TRIQSGF, ReduceIntoExistingBlockGFInPlace) {
  test_reduce_in_place_into(bgf1, world, root);
  test_reduce_in_place_into(bgf2, world, root);
}

TEST_F(TRIQSGF, ReduceBlockGF) {
  test_reduce(bgf1, world, root);
  test_reduce(bgf2, world, root);
}

MAKE_MAIN;
