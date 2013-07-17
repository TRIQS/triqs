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

#include "./src/array.hpp"
#include "./src/vector.hpp"
#include "./src/matrix.hpp"
#include "./src/linalg/matmul.hpp"
#include <iostream>

using std::cout; using std::endl;
using namespace triqs::arrays;

int main(int argc, char **argv) {

 

 // testing gemv
 triqs::arrays::matrix<double> A(5,5,FORTRAN_LAYOUT);

 for (int i =0; i<5; ++i)
  for (int j=0; j<5; ++j)
   A(i,j) = i+2*j+1; 

 //A *=A;; should be rejected by compiler

 range R(1,3); 

 std::cout<< "A = "<< A<< std::endl;
 std::cout<<A.transpose() << std::endl;
 std::cout<<A(R,R) << std::endl;
 std::cout<<A(R,R).transpose() << std::endl;

 triqs::arrays::matrix_view<double > Acw =  A.transpose();
  
}



