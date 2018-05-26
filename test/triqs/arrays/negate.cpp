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
using namespace indexmaps;
using namespace storages;

int main(int argc, char **argv) {

  {
    array<double, 1> A(3), B(3);
    for (int i = 0; i < 3; ++i) {
      A(i) = 2.6 * i;
      B(i) = 0;
    }

    B = -A;
    if (max_element(abs(A + B)) > 1.e-15) TRIQS_RUNTIME_ERROR << " array: A+B";
  }

  {
    matrix<double> A(3, 3), B(3, 3);
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j) A(i, j) = 2.6 * i + 9 * j;
    B() = 0;

    B = -A;
    std::cerr << A << std::endl;
    std::cerr << B << std::endl;
    if (max_element(abs(A + B)) > 1.e-15) TRIQS_RUNTIME_ERROR << " matrix : A+B";
  }

  {
    triqs::arrays::vector<double> A(3), B(3);
    for (int i = 0; i < 3; ++i) {
      A(i) = 2.6 * i;
      B(i) = 0;
    }

    B = -A;
    if (max_element(abs(A + B)) > 1.e-15) TRIQS_RUNTIME_ERROR << " vector :  A+B";
  }
}
