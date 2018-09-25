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

//#include <triqs/mpi.hpp>
#include <triqs/mpi/vector.hpp>

#include <triqs/utility/complex_ops.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mpi;

TEST(Vector, MPI) {

  mpi::communicator world;

  std::vector<double> v1{};
  std::vector<double> v2 = triqs::mpi::mpi_reduce(v1, world);
}

MAKE_MAIN;
