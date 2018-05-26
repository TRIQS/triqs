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
#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
using namespace triqs::arrays;

TEST(Array, ScalarAssign) {

  int N = 5;
  matrix<int> A(N, N, FORTRAN_LAYOUT);
  A() = 2;
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j) EXPECT_EQ(A(i, j), (i == j ? 2 : 0));

  array_view<int, 2> AA(A);
  AA = 2;
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j) EXPECT_EQ(A(i, j), 2);
}
MAKE_MAIN;
