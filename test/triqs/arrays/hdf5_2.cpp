/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include <triqs/arrays.hpp>
#include <iostream>
//#include <triqs/arrays/h5/simple_read_write.hpp>

using namespace triqs::arrays;
using namespace triqs;

int main(int argc, char **argv) {
 
 try { 

 array<long,2> A (2,3),B,vc;
 array<double,2> D (2,3), D2;
 array<long,2> Af(FORTRAN_LAYOUT),Bf(FORTRAN_LAYOUT),vf(FORTRAN_LAYOUT);

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
    A(i,j) = 10*i+ j;

 Af = A;

 std::cout<<" A= "<<A<<std::endl;
 std::cout<<" Arange(0,1),range(1,3)  = "<< A(range(),range(1,3))<<std::endl;

 h5::file file("ess2.h5", H5F_ACC_TRUNC);
 h5::group top(file);

 h5_write(top,"A",A);
 h5_write(top,"A_slice",A(range(),range(1,3)));
 
 //h5_read (top, "A",B);   std::cout<< "B = "<< B<<std::endl;
 //h5_read (top, "Af",Bf); std::cout<< "Bf = "<< Bf<<std::endl;
 //h5_read (top, "D",D2);  std::cout<< "D = "<< D2<<std::endl;
 } 
 catch(std::exception const&  err) { std::cout<<err.what()<<std::endl;}
}


