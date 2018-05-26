/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2015 by O. Parcollet
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

TEST(Array, Algebra) {

  array<int, 1> A(3), B(3), C;
  array<double, 1> D;
  B() = 2;
  A() = 3;

  C = A + B;
  EXPECT_ARRAY_NEAR(C, array<int, 1>{5, 5, 5});

  C = A * B;
  EXPECT_ARRAY_NEAR(C, array<int, 1>{6, 6, 6});

  C = 2 * B;
  EXPECT_ARRAY_NEAR(C, array<int, 1>{4, 4, 4});

  C = 2 * B;
  EXPECT_ARRAY_NEAR(C, array<int, 1>{4, 4, 4});

  D = 2.3 * B;
  EXPECT_ARRAY_NEAR(D, array<double, 1>{4.6, 4.6, 4.6});

  D = A + B / 1.2;
  EXPECT_ARRAY_NEAR(D, array<double, 1>{4.66666666666667, 4.66666666666667, 4.66666666666667});

  auto x = A + B + 2 * A;
  EXPECT_PRINT("(([3,3,3] + [2,2,2]) + (2 * [3,3,3]))", x);

  C = A + 2 * A + 3 * A - 2 * A + A - A + A + A * 3 + A + A + A + A + A + A + A + A + A + A + A + A + A;
  EXPECT_ARRAY_NEAR(C, array<int, 1>{63, 63, 63});
}
MAKE_MAIN;
