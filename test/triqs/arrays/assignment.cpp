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
#include <triqs/arrays/matrix.hpp>
#include <iostream>

using namespace triqs::arrays;

int main(int argc, char **argv) {
 
 

 array<long,2> A (2,3);
 //array<long,2> Af (2,3, "F" );
 array<long,2> Af (2,3, FORTRAN_LAYOUT );
 //array<long,2> Af (2,3, memory_layout<2>('F'));

 std::cout <<"Filling Af...."<<std::endl;

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   Af(i,j) = 10*i+ j;

 // assign 
 A = Af;
 std::cout <<"A= Af --- > A =  "<<A<<std::endl;

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   std::cout<<" i,j = "<<i<<"    "<<j<<" A="<< A(i,j)<< " should be "<< 10*i+ j<<std::endl;

 std::cout<<" copy construction B(A)"<<std::endl;
 array<long,2> B(Af);

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   std::cout<<"B "<<B(i,j)<<std::endl;

 std::cout<<"  B2 = A"<<std::endl;
 array<long,2> B2;
 B2 = A;
 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   std::cout<<"B2 "<<B2(i,j)<<std::endl;

 array<double,2> F = A;
 std::cout<<"  F = A"<<F<<std::endl;

 matrix<std::complex<double> > M1(2,2), M2(2,2);
 M1() = 0;
 A()=0;
 std::cerr<< " is scalar "<< is_scalar_for< int, matrix<std::complex<double> > >::type::value<< std::endl; 

 return 0;
}

