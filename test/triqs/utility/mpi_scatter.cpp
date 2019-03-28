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

#include <triqs/test_tools/arrays.hpp>
#include <triqs/utility/mpi_scatter.hpp>

#include <numeric>

using namespace triqs::utility;

TEST(MpiScatterRange, Simple) {

  triqs::mpi::communicator comm{};

  for (int N : range(10)) {
    auto scat_V = mpi_scatter(range(N), comm);
    int sum     = std::accumulate(scat_V.begin(), scat_V.end(), 0);
    sum         = triqs::mpi::mpi_all_reduce(sum, comm);
    EXPECT_EQ(N * (N - 1) / 2, sum);
  }
}

TEST(MpiScatterRange, Multi) {

  triqs::mpi::communicator comm{};

  for (int N : range(10)) {
    auto V1 = range(0, N);
    auto V2 = range(N, 2 * N);
    int sum = 0;
    for (auto [v1, v2] : mpi_scatter(zip(V1, V2), comm)) { sum += v1 + v2; }
    sum = triqs::mpi::mpi_all_reduce(sum, comm);
    EXPECT_EQ(N * (2 * N - 1), sum);
  }
}

MAKE_MAIN;
