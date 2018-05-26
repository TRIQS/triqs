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
#include "start.hpp"
TEST(Matrix, TransposeDagger) {

  const int N = 5;

  triqs::arrays::matrix<double> A(N, N, FORTRAN_LAYOUT);
  triqs::arrays::matrix<std::complex<double>> B(N, N);

  // A *=A;; should be rejected by compiler

  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j) {
      A(i, j) = i + 2 * j + 1;
      B(i, j) = i + 2.5 * j + (i - 0.8 * j) * 1_j;
    }

  auto at = A.transpose();
  auto ad = dagger(A);
  auto bt = B.transpose();
  auto bd = dagger(B);

  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j) {
      EXPECT_COMPLEX_NEAR(at(i, j), A(j, i));
      EXPECT_COMPLEX_NEAR(ad(i, j), A(j, i));
      EXPECT_COMPLEX_NEAR(bd(i, j), std::conj(B(j, i)));
      EXPECT_COMPLEX_NEAR(bt(i, j), B(j, i));
    }
}
MAKE_MAIN;
