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
#include <triqs/arrays/algorithms.hpp>
#include <triqs/arrays/matrix.hpp>
#include <iostream>

namespace tqa = triqs::arrays;

int main(int argc, char **argv) {
 
 tqa::matrix<double> A(3,3, FORTRAN_LAYOUT),B(3,3,FORTRAN_LAYOUT);

 A() = -2;

 for (int i =0; i<3; ++i)
  for (int j=0; j<3; ++j)
  { A(i,j) = i+2*j+1; B(i,j) = i-j;}

 TEST(A);
 TEST(max_element(A));
 TEST(sum(A));
 TEST(B);
 TEST(min_element(B));
 TEST(sum(B));
 TEST(make_matrix(A+10*B));
 TEST(max_element(A+10*B));

 return 0;
}
