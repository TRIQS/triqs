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
#include <triqs/arrays/vector.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace triqs::arrays;

int main(int argc, char **argv) {
  triqs::arrays::vector<double> V(5);
  for (int i = 0; i < 5; ++i) V[i] = i + 1;
  V *= 2;
  std::cout << V << std::endl;
  V[range(2, 4)] /= 2.0;
  std::cout << V << std::endl;
  V[range(0, 5, 2)] *= 10;
  std::cout << V << std::endl;
  triqs::arrays::vector<double> V2(5, -2.3);
  std::cout << V2 << std::endl;
  return 0;
}
