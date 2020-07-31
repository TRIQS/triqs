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

using triqs::clef::placeholder;

// ==============================================================

TEST(NDA, LazyFill) {

  placeholder<0> i_;
  placeholder<1> j_;

  array<double, 2> A(2, 2);
  A(i_, j_) << i_ * 8.1 + 2.31 * j_;

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) EXPECT_EQ(A(i, j), i * 8.1 + 2.31 * j);
}

// ==============================================================

TEST(NDA, MakeImmutable) {

  auto A = make_immutable_array([](int i, int j) { return 2 * i - j; }, range(0, 2), range(0, 2));

  array<int, 2> B = A;

  EXPECT_ARRAY_NEAR(B, array<int, 2>{{0, -1}, {2, 1}}, 1.e-15);
}

// ==============================================================

TEST(NDA, MakeImmutableLazy) {

  placeholder<0> i_;
  placeholder<1> j_;

  auto A = make_immutable_array(i_ + j_, i_ = range(0, 2), j_ = range(0, 2));

  array<int, 2> B = A;

  EXPECT_ARRAY_NEAR(B, array<int, 2>{{0, 1}, {1, 2}}, 1.e-15);
}

MAKE_MAIN
