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
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <iostream>

using std::cout;
using std::endl;
using namespace triqs::arrays;

int main(int argc, char **argv) {

  triqs::arrays::matrix<double> A(5, 5, FORTRAN_LAYOUT);
  triqs::arrays::matrix<double> Ac(5, 5);
  typedef triqs::arrays::vector<double> vector_type;
  vector_type MC(5), MB(5);

  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j) A(i, j) = i + 2 * j + 1;

  Ac = A;

  MC() = 1;
  MB() = 0;
  range R(1, 3);
  std::cout << "A = " << A << std::endl << std::endl;

  //MB(R) = 2* mat_vec_mul(A(R,R),MC(R));
  MB(R) = A(R, R) * MC(R);
  std::cout << "A(R,R) = " << A(R, R) << std::endl << std::endl;
  std::cout << " MC(R) = " << MC(R) << std::endl << std::endl;
  std::cout << " MB = " << A(R, R) * MC(R) << " = " << MB << std::endl << std::endl;

  std::cout << "testing infix" << std::endl << std::endl;

  //MB(R) += mat_vec_mul(A(R,R),MC(R));
  MB(R) += A(R, R) * MC(R);
  std::cout << "A = " << A(R, R) << std::endl << std::endl;
  std::cout << " MC = " << MC << std::endl << std::endl;
  std::cout << " MB(R) += A(R,R) * MC(R) = " << MB << std::endl << std::endl;

  std::cout << " explicit blas call" << std::endl << std::endl;

  triqs::arrays::matrix_view<double> Acw = A.transpose();

  vector_type::view_type MB_w(MB(R));

  std::cout << " MC = " << MC << std::endl << std::endl;

  blas::gemv(1, A(R, R), MC(R), 0, MB_w);
  std::cout << "A = " << A(R, R) << std::endl << std::endl;
  std::cout << " MB = " << MB << std::endl << std::endl;

  blas::gemv(1, Ac(R, R), MC(R), 0, MB_w);
  std::cout << "A = " << Ac(R, R) << std::endl << std::endl;
  std::cout << " MB = " << MB << std::endl << std::endl;

  blas::gemv(1, Acw(R, R), MC(R), 0, MB_w);
  std::cout << "A = " << Acw(R, R) << std::endl << std::endl;
  std::cout << " MB = " << MB << std::endl << std::endl;

  //std::cout<< A(R,R) * A(R,R) << " = " << triqs::arrays::matrix<double,Option::Fortran > (A(R,R) * A(R,R)) <<std::endl<<std::endl;
}
