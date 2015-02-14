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
#include "../test_tools.hpp"
#include <triqs/arrays.hpp>
#include <triqs/utility/complex_ops.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mpi;

TEST(Arrays, MPI) { 

 mpi::communicator world;

 // using ARR = array<double,2>;
 using ARR = array<std::complex<double>, 2>;

 ARR A(7, 3), B, AA;
 
auto se = mpi::slice_range(0,6,world.size(), world.rank());

 clef::placeholder<0> i_;
 clef::placeholder<1> j_;

 A(i_, j_) << i_ + 10 * j_;

 B = mpi_scatter(A, world);
 ARR C = mpi_scatter(A, world);

 std::ofstream out("node" + std::to_string(world.rank()));
 out << "  A = " << A << std::endl;
 out << "  B = " << B << std::endl;
 out << "  C = " << C << std::endl;

 EXPECT_ARRAY_EQ( B, A(range(se.first, se.second+1), range()));
 EXPECT_ARRAY_NEAR (C, B);

 B *= -1;
 AA() = 0;

 AA = mpi_gather(B, world);
 if (world.rank()==0) EXPECT_ARRAY_NEAR(AA, -A);

 mpi_broadcast(AA, world);
 EXPECT_ARRAY_NEAR(AA,-A);

 AA() = 0;
 AA = mpi_all_gather(B, world);
 EXPECT_ARRAY_NEAR(AA,-A);

 ARR r1 = mpi_reduce(A, world);
 if (world.rank()==0) EXPECT_ARRAY_NEAR(r1, world.size()* A);

 ARR r2 = mpi_all_reduce(A, world);
 EXPECT_ARRAY_NEAR(r2,world.size()*A);
}
MAKE_MAIN;




