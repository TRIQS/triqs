
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
#include <triqs/arrays/functional/map.hpp>
#include <triqs/arrays/matrix.hpp>
#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;
using namespace triqs::arrays;

template <class T> T mmax(T const &x, T const &y) { return std::max(x, y); }

int main(int argc, char **argv) {

  triqs::arrays::matrix<double> A(3, 3, FORTRAN_LAYOUT), B(3, 3, FORTRAN_LAYOUT);
  A() = -2;

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      A(i, j) = i + 2 * j + 1;
      B(i, j) = i - j;
    }

  auto Abs = map(std::function<double(double)>(static_cast<double (*)(double)>(std::abs)));
  auto Max = map2(std::function<double(double, double)>(mmax<double>));

  auto sqr = map([](double x) { return x * x; });

  std::cout << " A " << A << std::endl << std::endl;
  std::cout << " B " << B << std::endl << std::endl;
  std::cerr << " Sqr(A) " << make_matrix(sqr(A)) << std::endl << std::endl;
  std::cout << " abs(B+B) = " << (Abs(B + B)) << std::endl << std::endl;
  std::cout << " A+10*B = " << make_matrix(A + 10 * B) << std::endl << std::endl;
  std::cout << " Abs(A+10*B) = " << make_matrix(Abs(A + 10 * B)) << std::endl << std::endl;
  std::cout << " Max(A,10*B)" << make_matrix(Max(A, 10 * B)) << std::endl << std::endl;

  return 0;
}
