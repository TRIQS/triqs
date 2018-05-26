/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016 by I. Krivenko
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
#include <triqs/mpi/base.hpp>

using namespace triqs::mpi;

TEST(Comm, split) {

  communicator world;
  int rank = world.rank();

  ASSERT_EQ(4, world.size());

  int colors[] = {0, 1, 1, 2};
  int keys[]   = {5, 13, 18, 7};

  auto comm = world.split(colors[rank], keys[rank]);

  int comm_sizes[] = {1, 2, 2, 1};
  int comm_ranks[] = {0, 0, 1, 0};

  EXPECT_EQ(comm_sizes[rank], comm.size());
  EXPECT_EQ(comm_ranks[rank], comm.rank());
}

MAKE_MAIN;
