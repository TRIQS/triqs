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
#include <triqs/arrays/linalg/eigenelements.hpp>
#include <triqs/utility/complex_ops.hpp>

#include <iostream>

using namespace triqs::arrays;
using namespace triqs::arrays::linalg;
using dcomplex = std::complex<double>;

template <typename T> void check_eig(matrix<T> M, matrix<T> vectors, array<double, 1> values) {
  auto _ = range();
  for (auto i : range(0, first_dim(M))) {
    std::cerr << "check " << i << std::endl;
    std::cerr << (M - values(i)) * vectors(i, _) << std::endl;
    assert_all_close(M * vectors(i, _), values(i) * vectors(i, _), 1.e-14);
  }
}

template <typename M> void test(M A) {
  auto w = eigenelements(make_clone(A));
  std::cerr << "A = " << A << std::endl;
  std::cerr << " values = " << w.first << std::endl;
  std::cerr << " vectors = " << w.second << std::endl;
  check_eig(A, w.second, w.first);
}

int main(int argc, char **argv) {

  {
    matrix<double> A(3, 3);

    for (int i = 0; i < 3; ++i)
      for (int j = 0; j <= i; ++j) {
        A(i, j) = (i > j ? i + 2 * j : i - j);
        A(j, i) = A(i, j);
      }
    test(A);

    A()     = 0;
    A(0, 1) = 1;
    A(1, 0) = 1;
    A(2, 2) = 8;
    A(0, 2) = 2;
    A(2, 0) = 2;

    test(A);

    A()     = 0;
    A(0, 1) = 1;
    A(1, 0) = 1;
    A(2, 2) = 8;

    test(A);
  }
  { // the complex case

    matrix<dcomplex> M(2, 2);

    M(0, 0) = 1;
    M(0, 1) = 1.0_j;
    M(1, 0) = -1.0_j;
    M(1, 1) = 2;

    test(M);
  }

  { // the complex case

    matrix<dcomplex> M(2, 2, FORTRAN_LAYOUT);

    M(0, 0) = 1;
    M(0, 1) = 1.0_j;
    M(1, 0) = -1.0_j;
    M(1, 1) = 2;

    test(M);
  }

  return 0;
}
