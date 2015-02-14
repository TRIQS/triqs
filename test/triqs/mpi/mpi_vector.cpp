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
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include "./test_tools.hpp"
#include <triqs/mpi/vector.hpp>
#include <triqs/arrays.hpp>
#include <iostream>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mpi;

TEST(MPI, vector) {

 mpi::communicator world;

 using VEC = std::vector<std::complex<double>>;

 VEC A(7), B(7), AA(7);

 clef::placeholder<0> i_;

 triqs::clef::make_expr(A)[i_] << i_ + 1;

 B = mpi_scatter(A, world);

 VEC C = mpi_scatter(A, world);

 for (auto &x : B) x *= -1;
 for (auto &x : AA) x = 0;
 for (auto &x : A) x *= -1;

 AA = mpi_all_gather(B, world);

 EXPECT_EQ(A,AA);

}
MAKE_MAIN;


