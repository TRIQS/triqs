/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include <triqs/arrays.hpp>
#include <triqs/utility/complex_ops.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace triqs;
using namespace triqs::arrays;
using namespace mpi;

TEST(Arrays, MPI) {

  mpi::communicator world;

  // using arr_t = array<double,2>;
  using arr_t = array<std::complex<double>, 2>;

  arr_t A(7, 3), B, AA;

  auto se = itertools::chunk_range(0, 7, world.size(), world.rank());

  clef::placeholder<0> i_;
  clef::placeholder<1> j_;

  A(i_, j_) << i_ + 10 * j_;

  B       = mpi::scatter(A, world);
  arr_t C = mpi::scatter(A, world);

  std::ofstream out("node" + std::to_string(world.rank()));
  out << "  A = " << A << std::endl;
  out << "  B = " << B << std::endl;
  out << "  C = " << C << std::endl;

  EXPECT_ARRAY_EQ(B, A(range(se.first, se.second), range()));
  EXPECT_ARRAY_NEAR(C, B);

  B *= -1;
  AA() = 0;

  AA = mpi_gather(B, world);
  if (world.rank() == 0) EXPECT_ARRAY_NEAR(AA, -A);

  mpi::broadcast(AA, world);
  EXPECT_ARRAY_NEAR(AA, -A);

  AA() = 0;
  AA   = mpi::all_gather(B, world);
  EXPECT_ARRAY_NEAR(AA, -A);

  arr_t r1 = mpi::reduce(A, world);
  if (world.rank() == 0) EXPECT_ARRAY_NEAR(r1, world.size() * A);

  arr_t r2 = mpi::all_reduce(A, world);
  EXPECT_ARRAY_NEAR(r2, world.size() * A);
}

// test reduce MAX, MIN
TEST(Arrays, MPIReduceMAX) {

  mpi::communicator world;
  using arr_t = array<int, 1>;
  clef::placeholder<0> i_;
  clef::placeholder<1> r_;
  auto r = world.rank();
  auto s = world.size();

  arr_t a(7);
  a(i_) << (i_ - r + 2) * (i_ - r + 2);

  auto b1 = a, b2 = a;
  for (int i = 0; i < 7; ++i) {
    arr_t c(s);
    c(r_) << (i - r_ + 2) * (i - r_ + 2);
    b1(i) = min_element(c);
    b2(i) = max_element(c);
  }

  arr_t r1 = mpi::reduce(a, world, 0, true, MPI_MIN);
  arr_t r2 = mpi::reduce(a, world, 0, true, MPI_MAX);

  std::cerr << " a = " << r << a << std::endl;
  std::cerr << "r1 = " << r << r1 << std::endl;
  std::cerr << "r2 = " << r << r2 << std::endl;

  EXPECT_ARRAY_EQ(r1, b1);
  EXPECT_ARRAY_EQ(r2, b2);
}

// test transposed matrix broadcast
TEST(Arrays, matrix_transpose_bcast) {

  mpi::communicator world;

  matrix<dcomplex> A  = {{1, 2, 3}, {4, 5, 6}};
  matrix<dcomplex> At = A.transpose();

  matrix<dcomplex> B;
  if (world.rank() == 0) B = At;

  mpi::broadcast(B, world, 0);

  EXPECT_ARRAY_EQ(At, B);
}

// test transposed array broadcast
TEST(Arrays, array_transpose_bcast) {

  mpi::communicator world;

  array<dcomplex, 2> A  = {{1, 2, 3}, {4, 5, 6}};
  array<dcomplex, 2> At = transposed_view(A, 0, 1);

  array<dcomplex, 2> B(2, 3);
  if (world.rank() == 0) B = At;

  mpi::broadcast(B, world, 0);

  EXPECT_ARRAY_EQ(At, B);
}

MAKE_MAIN;
