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
#include "./src/linalg/mat_vec_mul.hpp"
#include "./src/linalg/inverse.hpp"
#include "./src/linalg/determinant.hpp"

#include "./src/blas_lapack/gemm.hpp"
#include "./src/blas_lapack/gemv.hpp"
#include "./src/blas_lapack/ger.hpp"
#include "./src/blas_lapack/axpy.hpp"

#include <iostream>
using namespace triqs::arrays;

int main(int argc, char **argv) {

 
 
 	typedef std::complex<double> T;
 triqs::arrays::vector<T>  V(5),V2(5);

 for (int i =0; i<5; ++i) {V(i) = i; V2(i) = -1;}

 std::cout<<"V = "<<V<<std::endl;
 std::cout<<"V2 = "<<V2<<std::endl;

 std::cout <<"starting blas test"<<std::endl;

 blas::axpy(2.0,V,V2);

 std::cout<<"V = "<<V<<std::endl;
 std::cout<<"V2 = "<<V2<<std::endl;

 triqs::arrays::vector <double> V3(2);
 for (int i =0; i<2; ++i) {V3(i) = i+1;}
 

  triqs::arrays::matrix<double> M1(2,2,FORTRAN_LAYOUT), M2(2,2,FORTRAN_LAYOUT), M3(2,2,FORTRAN_LAYOUT);
 for (int i =0; i<2; ++i)
  for (int j=0; j<2; ++j)
  { M1(i,j) = i+j; M2(i,j) = 1; M3(i,j)=0;}

 // try to multiply
 blas::gemm(1.0,M1, M2, 1.0, M3);
 std::cout<<"M1 = "<<M1<<std::endl;
 std::cout<<"M2 = "<<M2<<std::endl;
 std::cout<<"M3 = "<<M3<<std::endl;
 
 triqs::arrays::matrix<double> Mc1(2,2), Mc2(2,2), Mc3(2,2);
 for (int i =0; i<2; ++i)
  for (int j=0; j<2; ++j)
  { Mc1(i,j) = i+j; Mc2(i,j) = 1; Mc3(i,j)=0;}

 // try to multiply
 blas::gemm(1.0,Mc1, Mc2, 1.0, Mc3);
 std::cout<<"Mc1 = "<<Mc1<<std::endl;
 std::cout<<"Mc2 = "<<Mc2<<std::endl;
 std::cout<<"Mc3 = "<<Mc3<<std::endl;

 std::cout<<"V3 = "<<V3<<std::endl;
 blas::ger(1.0,V3,V3,M2);
 std::cout<<"M2 = "<<M2<<std::endl;

 // try to invert
 triqs::arrays::vector <int> ipiv(2);
 lapack::getrf(M1, ipiv);
 lapack::getri(M1, ipiv);
 std::cout<<"inv M1 = "<<M1<<std::endl;


}



