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

template <typename T> void test(T val = 1) {
  matrix<T> A(3, 3, FORTRAN_LAYOUT), B(3, 3, FORTRAN_LAYOUT);
  typedef array_view<T, 2> V;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      A(i, j) = (i + 2 * j + 1);
      B(i, j) = (i - j);
    }

  A *= val;
  B *= val;
  T s = 10;
  TEST(A);

  TEST(make_matrix(pow(V(A), 2)));
  TEST(make_matrix(cosh(V(A))));
  TEST(B);
  TEST(abs(B));
  TEST(make_matrix(abs(B)));
  TEST(make_matrix(abs(B + B)));
  TEST(make_matrix(A + s * B));
  TEST(make_matrix(abs(A + s * B)));

  TEST(make_matrix(real(B)));
  TEST(make_matrix(imag(B)));

  auto aa = array<T, 2>{{1, 2}, {3, 4}};
  TEST(make_matrix(exp(aa)));

  TEST(make_matrix(conj(A)));

  // does not compile, since exp is only element wise at the moment
  // to do : implement it for matrix...
  //TEST(make_matrix(exp( matrix<double>{{ 1,2}, {3,4}} )));

  //array<double,3> a(2,2,2);
  //auto b = real(a);
}

int main(int argc, char **argv) {

  test<int>();
  test<long>();
  test<double>();
  test<std::complex<double>>();
  test<std::complex<double>>(std::complex<double>(-2, 3));

  return 0;
}
