
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
using namespace triqs::arrays;

struct S {
  double x = 0, y = 0;
  int i = 0;
  S()   = default;
  S(double x, double y, int k) : x(x), y(y), i(k) {}
};

std::ostream &operator<<(std::ostream &out, S const &s) { return out << "(" << s.x << " " << s.y << " " << s.i << ")"; }

int main(int argc, char **argv) {

  array<S, 2> A(2, 2);

  S s0{1.0, 2.0, 3};
  int p = 0;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j, ++p) A(i, j) = S{double(i), double(j), p};

  std::cout << "  A " << A << A(range(), 0) << std::endl;

  A() = s0;
  std::cout << "  A " << A << A(range(), 0) << std::endl;

  array<array<double, 1>, 2> AA(2, 2);

  array<double, 1> A0(2);
  for (int i = 0; i < 2; ++i) A0(i) = i;

  AA() = A0;

  std::cout << "  AA " << AA << std::endl;

  std::cout << AA(0, 0) << std::endl;

  // even more  : a matrix of functions !
  matrix<std::function<double(double)>> F(2, 2);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) {
      auto s  = i + j;
      F(i, j) = [s](int i) { return i + s; };
    }

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) std::cout << F(i, j)(0) << std::endl;

  return 0;
}
