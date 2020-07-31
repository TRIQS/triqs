// Copyright (c) 2019-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Olivier Parcollet, Nils Wentzell

#include "./array_test_common.hpp"

// ==============================================================

TEST(NDA, AssignFortranOrder) {
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

// ===============================================================

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

MAKE_MAIN
