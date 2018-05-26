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

TEST(Array, AliasMatmul) {

  auto _ = range{};

  array<dcomplex, 3> A(10, 2, 2);
  A() = -1;

  A(4, _, _) = 1;
  A(5, _, _) = 2;

  matrix_view<dcomplex> M1 = A(4, _, _);
  matrix_view<dcomplex> M2 = A(5, _, _);

  M1 = M1 * M2;

  EXPECT_ARRAY_NEAR(M1, matrix<dcomplex>{{4, 4}, {4, 4}});
  EXPECT_ARRAY_NEAR(M2, matrix<dcomplex>{{2, 2}, {2, 2}});

  matrix<double> B1(2, 2), B2(2, 2);
  B1() = 2;
  B2() = 3;

  B1 = make_clone(B1) * B2;
  EXPECT_ARRAY_NEAR(B1, matrix<double>{{6, 0}, {0, 6}});
}

MAKE_MAIN;
