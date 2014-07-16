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

template <typename T> 
std::ostream & operator << (std::ostream & out, std::vector<T> const & v) { 
for (size_t i =0; i<v.size(); ++i) out<< v[i];
return out;
}

int main(int argc, char* argv[]) {

 mpi::environment env(argc, argv);
 mpi::communicator world;

 // using ARR = array<double,2>;
 using VEC = std::vector<std::complex<double>>;

 VEC A(7), B(7), AA(7);

 clef::placeholder<0> i_;
 clef::placeholder<1> j_;

 triqs::clef::make_expr(A)[i_] << i_+1;

 B = mpi::scatter(A, world);

 VEC C = mpi::scatter(A, world);

 std::ofstream out("node" + std::to_string(world.rank()));
 out << "  A = " << A << std::endl;
 out << "  B = " << B << std::endl;
 out << "  C = " << C << std::endl;

 for(auto &x: B) x *= -1;
 for(auto &x: AA) x = 0;

 AA = mpi::gather(B, world);
 out << " AA = " << AA << std::endl;

 mpi::broadcast(AA, world);
 out << " cast AA = " << AA << std::endl;

 for (auto &x : AA) x = 0;

 AA = mpi::allgather(B, world);
 out << " AA = " << AA << std::endl;

}

