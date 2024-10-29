// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
// Authors: Philipp D, Igor Krivenko, Nils Wentzell

#include <triqs/stat/histograms.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <mpi/mpi.hpp>

#include <vector>
#include <utility>

// Create histogram.
auto make_histogram() {
  triqs::stat::histogram h{0, 10, 11};
  std::vector<double> data{1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.1, 12.1, 32.2};
  for (auto i : data) h << i;
  return h;
}

TEST(TRIQSStat, HistogramMPIBroadcast) {
  mpi::communicator world;
  nda::vector<double> exp_h = {0, 1, 2, 2, 0, 1, 0, 0, 0, 1, 1};

  // create and broadcast histogram
  auto h = world.rank() ? triqs::stat::histogram{0, 20} : make_histogram();
  mpi::broadcast(h, world, 0);
  EXPECT_EQ(11, h.size());
  EXPECT_EQ(0, h.mesh_point(0));
  EXPECT_EQ(1, h.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), h.limits());
  EXPECT_ARRAY_NEAR(exp_h, h.data());
  EXPECT_EQ(8, h.n_data_pts());
  EXPECT_EQ(3, h.n_lost_pts());
}

TEST(TRIQSStat, HistogramMPIReduce) {
  mpi::communicator world;
  nda::vector<double> exp_h = {0, 1, 2, 2, 0, 1, 0, 0, 0, 1, 1};
  exp_h *= world.size();

  // create and reduce histogram
  auto h  = make_histogram();
  auto h2 = mpi::reduce(h, world, 0);
  if (world.rank() == 0) {
    EXPECT_EQ(11, h2.size());
    EXPECT_EQ(0, h2.mesh_point(0));
    EXPECT_EQ(1, h2.mesh_point(1));
    EXPECT_EQ(std::make_pair(.0, 10.), h2.limits());
    EXPECT_ARRAY_NEAR(exp_h, h2.data());
    EXPECT_EQ(8 * world.size(), h2.n_data_pts());
    EXPECT_EQ(3 * world.size(), h2.n_lost_pts());
  }

  // all_reduce histogram
  auto h3 = mpi::all_reduce(h, world);
  EXPECT_EQ(11, h3.size());
  EXPECT_EQ(0, h3.mesh_point(0));
  EXPECT_EQ(1, h3.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), h3.limits());
  EXPECT_ARRAY_NEAR(exp_h, h3.data());
  EXPECT_EQ(8 * world.size(), h3.n_data_pts());
  EXPECT_EQ(3 * world.size(), h3.n_lost_pts());
}

MAKE_MAIN;
