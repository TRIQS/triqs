// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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

#include <triqs/utility/lazy_bracket.hpp>
#include <triqs/test_tools/arrays.hpp>

struct A {

  std::vector<int> d;

  // 4x3x2 array
  A() : d(24, 0) {
    int s = 0;
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        for (int k = 0; k < 2; ++k) { d[s++] = k + 10 * j + 100 * i; }
  }

  int &_call(int i, int j, int k) { return d[6 * i + 2 * j + k]; }

  /// Warning : the lambda MUST return a decltype(auto). By default, it would be auto, and loose the ref !!
  template <typename T> decltype(auto) operator[](T &&x) {
    return triqs::utility::make_lazy_bracket<3>([this](auto &&... y) -> decltype(auto) { return this->_call(y...); }, x);
  }
};

TEST(LazyBra, ThreeD) {
  A a;
  EXPECT_EQ(a[3][2][1], 321);

  static_assert(std::is_reference<decltype(a[3][2][1])>::value, "oop");

  a[3][2][1] *= 10;
  EXPECT_EQ(a[3][2][1], 3210);
}

// Test in 1d
struct B {

  int _call(int i) { return 2 * i; }

  template <typename T> decltype(auto) operator[](T &&x) {
    return triqs::utility::make_lazy_bracket<1>([this](auto &&... y) -> decltype(auto) { return this->_call(y...); }, x);
  }
};

TEST(LazyBra, OneD) {
  B b;
  int r = b[1];
  EXPECT_EQ(r, 2);
}

MAKE_MAIN;
