/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#include "./common.hpp"
// order of include in horrible : on mac + clang, array (in fact boost::tuple) must come before boost::mpi
#include "./src/array.hpp"
#include <boost/mpi.hpp>
#include <iostream>
#include <sstream>
#include <triqs/utility/c14.hpp>

using std::cout; using std::endl;
using namespace triqs::arrays;
namespace mpi=boost::mpi;

int main(int argc, char* argv[]) 
{
 

 mpi::environment env(argc, argv);
 mpi::communicator world;

 array<long,2> A (2,2), B(2,2),C(2,2);
 for (int i =0; i<2; ++i)
  for (int j=0; j<2; ++j) 
  { A(i,j) = (1+world.rank())*(10*i+ j);}

 if (world.rank() ==0) std::cout<<" A = "<<A<<std::endl;

 boost::mpi::reduce (world, A,C, std::c14::plus<>(),0);

 int s= world.size();
 if (world.rank() ==0) std::cout<<" C = "<<C<< "  should be "<< array<long,2>( (s*(s+1)/2) * A) <<std::endl;

 return 0;
}

/*
 if (0 && (world.size()>1)) { 
  if (world.rank() == 0) {
   world.send(1, 0, A);
  } else {          
   world.recv(0, 0, C);
  }                    
  world.barrier();
  if (world.rank() ==1) std::cerr<<" node "<<world.rank()<<" C = "<<C<<std::endl;
 }

 */ 
