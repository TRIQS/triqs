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
#include <triqs/arrays/vector.hpp>
#include <triqs/arrays/matrix.hpp>
#include <triqs/arrays/linalg/eigenelements.hpp>

#include <iostream>

using namespace triqs::arrays;
using namespace triqs::arrays::linalg;

int main(int argc, char **argv) {

 

 matrix<double> A(3,3);

 for (int i =0; i<3; ++i)
  for (int j=0; j<=i; ++j)
  { A(i,j) = (i>j ? i+2*j : i-j); A(j,i) = A(i,j);}

 std::cerr<<"A = "<<A<<std::endl;
 eigenelements_worker< matrix_view <double>, true> w (A());
 w.invoke();
 std::cout<<"A = "<<A<<std::endl;
 std::cout<<" vectors = "<< w.values()<<std::endl;
 std::cout<<" values = "<< w.vectors()<<std::endl;

 A() =0;
 A(0,1) = 1;
 A(1,0) = 1;
 A(2,2) = 8;
 A(0,2) = 2;
 A(2,0) = 2;

 std::cout<<"A = "<<A<<std::endl;
 std::cout<<" values = "<< eigenelements(A(),true).first<<std::endl;
 std::cout<<" vectors = "<< eigenelements(A(),true).second<<std::endl;

 A() =0;
 A(0,1) = 1;
 A(1,0) = 1;
 A(2,2) = 8;

 std::cout<<"A = "<<A<<std::endl;
 std::cout<<" vectors = "<< eigenelements(A(),true).second<<std::endl;
 std::cout<<" values = "<< eigenelements(A(),true).first<<std::endl;
 std::cout<<"A = "<<A<<std::endl;

 return 0;

}



