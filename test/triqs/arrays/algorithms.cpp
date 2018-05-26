/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
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

TEST(Array, AlgoStd) {

  matrix<double> A(3, 3, FORTRAN_LAYOUT), B(3, 3, FORTRAN_LAYOUT);

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      A(i, j) = i + 2 * j + 1;
      B(i, j) = i - j;
    }

  EXPECT_EQ(max_element(A), 7);
  EXPECT_EQ(sum(A), 36);
  EXPECT_EQ(min_element(B), -2);
  EXPECT_EQ(sum(B), 0);
  EXPECT_EQ(make_matrix(A + 10 * B), (matrix<double>{{1, -7, -15}, {12, 4, -4}, {23, 15, 7}}));
  EXPECT_EQ(max_element(A + 10 * B), 23);
}

MAKE_MAIN;
