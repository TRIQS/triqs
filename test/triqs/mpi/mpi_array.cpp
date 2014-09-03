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
#include <iostream>
#include <type_traits>
#include <triqs/arrays.hpp>
#include <triqs/mpi.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mpi;

int main(int argc, char* argv[]) {

 mpi::environment env(argc, argv);
 mpi::communicator world;

 // using ARR = array<double,2>;
 using ARR = array<std::complex<double>, 2>;

 ARR A(7, 3), B, AA;

 clef::placeholder<0> i_;
 clef::placeholder<1> j_;

 A(i_, j_) << i_ + 10 * j_;

 //std::cerr << "B0 "<< B <<std::endl;
 B = mpi::scatter(A, world);
 std::cerr << "B "<< B <<std::endl;

 ARR C = mpi::scatter(A, world);

 std::ofstream out("node" + std::to_string(world.rank()));
 out << "  A = " << A << std::endl;
 out << "  B = " << B << std::endl;
 out << "  C = " << C << std::endl;

 B *= -1;
 AA() = 0;

 AA = mpi::gather(B, world);
 out << " AA = " << AA << std::endl;

 mpi::broadcast(AA, world);
 out << " cast AA = " << AA << std::endl;

 AA() = 0;

 AA = mpi::allgather(B, world);
 out << " AA = " << AA << std::endl;

 ARR r1 = mpi::reduce(A, world);
 out <<" Reduce "<< std::endl;
 out << " r1 = " << r1 << std::endl;

 ARR r2 = mpi::allreduce(A, world);
 out <<" AllReduce "<< std::endl;
 out << " r2 = " << r2 << std::endl;

}

