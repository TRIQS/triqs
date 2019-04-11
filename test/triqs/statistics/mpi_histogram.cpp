/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017 by I. Krivenko
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
#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics/histograms.hpp>

using namespace triqs::statistics;
using namespace triqs;

histogram make_hd() {
  histogram hd{0, 10, 11};
  std::vector<double> data{1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.1, 12.1, 32.2};
  for (auto i : data) hd << i;
  return hd;
}

TEST(histogram, mpi_broadcast) {

  mpi::communicator world;
  auto h = world.rank() ? histogram{0, 20} : make_hd();

  arrays::vector<double> true_h = {0, 1, 2, 2, 0, 1, 0, 0, 0, 1, 1};
  mpi::broadcast(h, world, 0);

  EXPECT_EQ(11, h.size());
  EXPECT_EQ(0, h.mesh_point(0));
  EXPECT_EQ(1, h.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), h.limits());
  EXPECT_ARRAY_NEAR(true_h, h.data());
  EXPECT_EQ(8, h.n_data_pts());
  EXPECT_EQ(3, h.n_lost_pts());
}

TEST(histogram, mpi_reduce) {

  mpi::communicator world;
  auto h                            = make_hd();
  arrays::vector<double> true_h_rec = {0, 1, 2, 2, 0, 1, 0, 0, 0, 1, 1};
  true_h_rec *= world.size();

  // reduce

  auto h2 = mpi::reduce(h, world, 0);

  if (world.rank() == 0) {
    EXPECT_EQ(11, h2.size());
    EXPECT_EQ(0, h2.mesh_point(0));
    EXPECT_EQ(1, h2.mesh_point(1));
    EXPECT_EQ(std::make_pair(.0, 10.), h2.limits());
    EXPECT_ARRAY_NEAR(true_h_rec, h2.data());
    EXPECT_EQ(8 * world.size(), h2.n_data_pts());
    EXPECT_EQ(3 * world.size(), h2.n_lost_pts());
  }

  // all_reduce

  auto h3 = mpi::all_reduce(h, world);

  EXPECT_EQ(11, h3.size());
  EXPECT_EQ(0, h3.mesh_point(0));
  EXPECT_EQ(1, h3.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), h3.limits());
  EXPECT_ARRAY_NEAR(true_h_rec, h3.data());
  EXPECT_EQ(8 * world.size(), h3.n_data_pts());
  EXPECT_EQ(3 * world.size(), h3.n_lost_pts());
}

MAKE_MAIN;
