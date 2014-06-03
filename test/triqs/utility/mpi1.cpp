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
#include <triqs/utility/mpi1.hpp>
#include <iostream>
#include <sstream>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mpi;

// a struct containing arrays. I only allow here boost serialization form, no mpi stuff here.
struct S { 
 array<long,2> x; array<double,1> y;
 template<typename Ar> friend void serialize( Ar & ar, S & s) { ar & s.x; ar & s.y;}
};

int main(int argc, char* argv[]) {
 
 mpi::environment env(argc, argv);
 mpi::communicator world;

 array<long,2> A {{1,2}, {3,4}}, C(2,2);

 // boost mpi
 boost::mpi::reduce (world, A,C, std::c14::plus<>(),0);
 int s= world.size();
 if (world.rank() ==0) std::cout<<" C = "<<C<< "  should be "<< std::endl << array<long,2>(s*A) <<std::endl;

 // triqs mpi
 C = A;
 mpi::reduce_in_place (world, C);
 if (world.rank() ==0) std::cout<<" C = "<<C<< "  should be "<< std::endl << array<long,2>(s*A) <<std::endl;

 // test rvalue views
 C = A; 
 mpi::reduce_in_place (world, C());
 if (world.rank() ==0) std::cout<<" C = "<<C<< "  should be "<< std::endl << array<long,2>(s*A) <<std::endl;
 
 // more complex class
 auto x = S { { {1,2},{3,4}}, { 1,2,3,4}};
 mpi::reduce_in_place (world, x);
 if (world.rank() ==0) std::cout<<" S.x = "<<x.x<<" S.y = "<<x.y<<std::endl;

 // a simple number
 double y = 1+world.rank(), z=0;
 mpi::reduce(world,y,z);
 if (world.rank() ==0) std::cout<<" y = "<<y<< "  should be "<< 1+world.rank()<<std::endl;
 if (world.rank() ==0) std::cout<<" z = "<<z<< "  should be "<< s*(s+1)/2 <<std::endl;
 mpi::reduce_in_place(world,y);
 if (world.rank() ==0) std::cout<<" y = "<<y<< "  should be "<< s*(s+1)/2 <<std::endl;

 mpi::broadcast(world,C);

 // reduced x,y,C, .... a variadic form
 mpi::reduce_in_place_v (world, x,y,C);

 // more complex object
 auto ca = array< array<int,1>, 1 > { array<int,1>{1,2}, array<int,1>{3,4}};
 auto cC = ca; 
 mpi::reduce_in_place (world, cC);
 if (world.rank() ==0) std::cout<<" cC = "<<cC<< std::endl;

 return 0;
}

