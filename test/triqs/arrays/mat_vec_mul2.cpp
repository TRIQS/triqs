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
#include <triqs/arrays/asserts.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <iostream>

using std::cout;
using std::endl;
using namespace triqs::arrays;

int main(int argc, char **argv) {

  matrix<int> Ai   = {{1, 2}, {3, 4}};
  matrix<double> A = {{1, 2}, {3, 4}};
  vector<int> Ci, B     = {1, 1};
  vector<double> Cd, Bd = {1, 1};

  Cd = A * B;
  std::cout << Cd << std::endl;

  Ci = Ai * B;
  std::cout << Ci << std::endl;

  assert_all_close(A * B, A * Bd, 1.e-13);

  return 0;
}
