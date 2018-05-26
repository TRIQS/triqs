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

template <typename T, typename O1, typename O2, typename O3> void test(O1 o1, O2 o2, O3 o3, bool all = false) {
  matrix<T> M1(2, 3, o1);
  matrix<T> M2(3, 4, o2);
  matrix<T> M3(o3), M4;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) { M1(i, j) = i + j; }
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 4; ++j) { M2(i, j) = 1 + i - j; }

  M3 = M1 * M2; //matmul(M1,M2);

  M4   = M3;
  M4() = 0;
  for (int i = 0; i < 2; ++i)
    for (int k = 0; k < 3; ++k)
      for (int j = 0; j < 4; ++j) M4(i, j) += M1(i, k) * M2(k, j);
  assert_all_close(M4, M3, 1.e-13, false);

  // recheck gemm_generic
  blas::gemm_generic(1, M1, M2, 0, M4);
  assert_all_close(M4, M3, 1.e-13, false);

  if (all) {
    std::cerr << "M1 = " << M1 << std::endl;
    std::cerr << "M2 = " << M2 << std::endl;
    std::cerr << "M3 = " << M3 << std::endl;
    std::cerr << "M4 = " << M1 * M2 << std::endl;
    std::cerr << "M5 = " << M1 * M2 << std::endl;
  }
}

template <typename T> void all_tests() {
  test<T>(C_LAYOUT, C_LAYOUT, C_LAYOUT, true);
  test<T>(C_LAYOUT, C_LAYOUT, FORTRAN_LAYOUT);
  test<T>(C_LAYOUT, FORTRAN_LAYOUT, FORTRAN_LAYOUT);
  test<T>(C_LAYOUT, FORTRAN_LAYOUT, C_LAYOUT);
  test<T>(FORTRAN_LAYOUT, FORTRAN_LAYOUT, FORTRAN_LAYOUT);
  test<T>(FORTRAN_LAYOUT, C_LAYOUT, FORTRAN_LAYOUT);
  test<T>(FORTRAN_LAYOUT, FORTRAN_LAYOUT, C_LAYOUT);
  test<T>(FORTRAN_LAYOUT, C_LAYOUT, C_LAYOUT);
}

int main(int argc, char **argv) {
  all_tests<double>();
  all_tests<std::complex<double>>();
  all_tests<long>();
  return 0;
}
