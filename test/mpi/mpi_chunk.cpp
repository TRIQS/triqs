/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017-2018 by O. Parcollet, N. Wentzell
 * Copyright (C) 2019 The Simons Foundation
 *    author : N. Wentzell
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

#include <mpi/mpi.hpp>

#include <gtest.h>
#include <numeric>

using namespace itertools;

TEST(MpiChunk, Simple) {

  mpi::communicator comm{};

  for (int N : range(10)) {
    auto chunk_V = mpi::chunk(range(N), comm);
    int sum      = std::accumulate(chunk_V.begin(), chunk_V.end(), 0);
    sum          = mpi::all_reduce(sum, comm);
    EXPECT_EQ(N * (N - 1) / 2, sum);
  }
}

TEST(MpiChunk, Multi) {

  mpi::communicator comm{};

  for (int N : range(10)) {
    auto V1 = range(0, N);
    auto V2 = range(N, 2 * N);
    int sum = 0;
    for (auto [v1, v2] : mpi::chunk(zip(V1, V2), comm)) { sum += v1 + v2; }
    sum = mpi::all_reduce(sum, comm);
    EXPECT_EQ(N * (2 * N - 1), sum);
  }
}

#include <itertools/omp_chunk.hpp>

TEST(MpiChunk, OMPHybrid) {

  int const N = 10;

  mpi::communicator comm{};
  int sum = 0;

#pragma omp parallel
  for (auto i : omp_chunk(mpi::chunk(range(N)))) {
#pragma omp critical
    {
      std::cout << "mpi_rank " << comm.rank() << "  omp_thread " << omp_get_thread_num() << "  i " << i << std::endl;
      sum += i;
    }
  }

  sum = mpi::all_reduce(sum, comm);
  EXPECT_EQ(N * (N - 1) / 2, sum);
}

MPI_TEST_MAIN;
