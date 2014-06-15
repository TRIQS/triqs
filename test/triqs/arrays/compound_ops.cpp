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
#include <triqs/arrays/array.hpp>
#include <iostream>

using namespace triqs::arrays;

int main(int argc, char **argv) {
 
 

 array<long,2> A (2,3);
 array<long,2> Af (2,3,FORTRAN_LAYOUT);

 std::cout <<"Filling Af...."<<std::endl;

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   Af(i,j) = 10*i+ j;

 // assign 
 A = Af;
 std::cout <<"A= Af --- > A =  "<<A<<std::endl;

 A *=2.0;

 std::cout<<" 2* A= "<<A<<std::endl;

 Af /= 2.0;

 std::cout<<"  Af/2 =  "<<Af<<std::endl;

 // should this really compile ??
 long i=5; i/=2.0; std::cout <<i<<std::endl;
 
 array<double,2> B (A);

 B /=4;
 
 std::cout<<"  B= A/4 =  "<<B<<std::endl;

 return 0;
}

