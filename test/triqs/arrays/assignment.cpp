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

TEST(Array, AssignFortranOrder) {

  static_assert(is_scalar_for<int, matrix<std::complex<double>>>::type::value == 1, "oops");

  array<long, 2> Af(2, 3, FORTRAN_LAYOUT);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) Af(i, j) = 10 * i + j;

  // assign
  array<long, 2> A;
  A = Af;

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) EXPECT_EQ(A(i, j), 10 * i + j);

  // copy construction B(A)
  array<long, 2> B(Af);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) EXPECT_EQ(B(i, j), 10 * i + j);

  // assign C -> C
  array<long, 2> B2;
  B2 = A;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) EXPECT_EQ(B2(i, j), 10 * i + j);

  // double
  array<double, 2> F = A;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) EXPECT_EQ(F(i, j), 10 * i + j);
}
MAKE_MAIN;
