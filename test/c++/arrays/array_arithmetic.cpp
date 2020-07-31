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

placeholder<0> i_;
placeholder<1> j_;

TEST(NDA, compound_ops) {

  array<long, 2> A(2, 3), Af(2, 3, FORTRAN_LAYOUT);
  Af(i_, j_) << 10 * i_ + j_;
  A = Af;

  A *= 2.0;
  EXPECT_ARRAY_NEAR(A, array<long, 2>{{0, 2, 4}, {20, 22, 24}});

  Af /= 2.0;
  EXPECT_ARRAY_NEAR(Af, array<long, 2>{{0, 0, 1}, {5, 5, 6}});

  array<double, 2> B(A);
  B /= 4;
  EXPECT_ARRAY_NEAR(B, array<double, 2>{{0.0, 0.5, 1.0}, {5.0, 5.5, 6.0}});
}
MAKE_MAIN;
