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
#include <triqs/arrays.hpp>
#include <iostream>
using namespace triqs::arrays;

int main(int argc, char **argv) {

  matrix<double> C, A = {{1.0, 2.3}, {3.1, 4.3}};
  matrix<int> B     = {{1, 2}, {3, 4}};
  matrix<double> Bd = {{1, 2}, {3, 4}};

  C = A * B;
  std::cout << C << std::endl;

  C = A * Bd;
  std::cout << C << std::endl;

  // check A*B is indeed in double...
  //static_assert( std::is_same<decltype(A*B), matrix<double>>::value, "oops");

  assert_all_close(A * B, A * Bd, 1.e-13);

  return 0;
}
