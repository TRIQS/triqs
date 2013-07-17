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
#include <iostream>

#include "./src/array.hpp"
#include "./src/vector.hpp"
#include "./src/matrix.hpp"
#include "./src/linalg/matmul.hpp"
#include "./src/linalg/mat_vec_mul.hpp"
#include "./src/linalg/inverse.hpp"
#include "./src/linalg/determinant.hpp"
#include "./src/linalg/a_x_ty.hpp"
#include "./src/blas_lapack/dot.hpp"

using namespace triqs::arrays;

int main(int argc, char **argv) {

 

 triqs::arrays::matrix<double> A(5,5, FORTRAN_LAYOUT);
 //triqs::arrays::matrix<double > A(5,5);
 typedef triqs::arrays::vector<double> vector_type;
 vector_type  MC(5), MB(5);
 A()= 0;

 for (int i =0; i<5; ++i)
  {  MC(i) = i;MB(i)=10*(i+1); }

 range R(1,3);
 std::cout<<" MC(R) = "<<MC(R)<< std::endl<<std::endl;
 std::cout<<" MB(R) = "<<MB(R)<< std::endl<<std::endl;

 A(R,R) += a_x_ty(1.0,MC(R),MB(R));
 std::cout<<" A(R,R) = "<<A(R,R)<< std::endl<<std::endl;

 A(R,R) += a_x_ty(1.0,MB(R),MC(R));

 std::cout<<" A(R,R) = "<<A(R,R)<< std::endl<<std::endl;

 A(R,R) = a_x_ty(1.0,MB(R),MC(R));

 std::cout<<" A(R,R) = "<<A(R,R)<< std::endl<<std::endl;
 
 std::cout<<" full A"<< A<<std::endl<<std::endl;


 std::cout<< " MB, MC, dot "<< MB << MC << dot(MB,MC)<<std::endl;
 std::cout<< " MC, MC, dot "<< MB << MC << dot(MC,MC)<<std::endl;

}



