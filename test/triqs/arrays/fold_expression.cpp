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

#include <triqs/arrays/matrix.hpp>
#include <triqs/arrays/functional/fold.hpp>
#include <triqs/arrays/functional/map.hpp>
#include <triqs/arrays/algorithms.hpp>
#include <iostream>

using namespace triqs::arrays;

auto Abs = map(std::function<double(double)>(static_cast<double (*)(double)>(std::abs)));

int main(int argc, char **argv) {

  triqs::arrays::matrix<double> A(3, 3, FORTRAN_LAYOUT), B(3, 3, FORTRAN_LAYOUT), C(FORTRAN_LAYOUT);
  triqs::arrays::matrix<double> Ac(3, 3);

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      A(i, j) = i + 2 * j + 1;
      B(i, j) = i - 3 * j;
    }

  C = A + B;
  std::cout << " A " << A << std::endl;
  std::cout << " B " << B << std::endl;
  std::cout << " A+B " << C << std::endl;

  std::cout << " max A : " << triqs::arrays::max_element(A) << std::endl;
  std::cout << " max B : " << triqs::arrays::max_element(B) << std::endl;
  std::cout << " max abs(B) : " << triqs::arrays::max_element(Abs(B)) << std::endl;
  std::cout << " max A+B : " << triqs::arrays::max_element(A + B) << std::endl;

  std::cout << " sum(A) " << sum(A) << std::endl;
  std::cout << " prod(A) " << prod(A) << std::endl;
  return 0;
}
