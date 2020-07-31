// Copyright (c) 2014-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2017 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2020 Simons Foundation
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
placeholder<2> k_;
placeholder<3> l_;

TEST(Array, c_ordered_transposed_view1) {

  array<int, 4> a(1, 2, 3, 4, make_memory_layout(0, 1, 2, 3));
  a(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  auto v = c_ordered_transposed_view(a());

  EXPECT_EQ(a.shape(), (mini_vector<size_t, 4>{1, 2, 3, 4}));
  EXPECT_EQ(v.shape(), (mini_vector<size_t, 4>{1, 2, 3, 4}));
  EXPECT_EQ(a.indexmap().memory_layout(), (memory_layout_t<4>{0, 1, 2, 3}));
  EXPECT_EQ(v.indexmap().memory_layout(), (memory_layout_t<4>{0, 1, 2, 3}));
}

TEST(Array, c_ordered_transposed_view2) {

  array<int, 4> a(1, 2, 3, 4, make_memory_layout(1, 0, 2, 3));
  a(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  auto v = c_ordered_transposed_view(a());

  EXPECT_EQ(a.shape(), (mini_vector<size_t, 4>{1, 2, 3, 4}));
  EXPECT_EQ(v.shape(), (mini_vector<size_t, 4>{2, 1, 3, 4}));
  EXPECT_EQ(a.indexmap().memory_layout(), (memory_layout_t<4>{1, 0, 2, 3}));
  EXPECT_EQ(v.indexmap().memory_layout(), (memory_layout_t<4>{0, 1, 2, 3}));
}

TEST(Array, c_ordered_transposed_view3) {

  array<int, 4> a(1, 2, 3, 4, make_memory_layout(2, 0, 3, 1));
  a(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  auto v = c_ordered_transposed_view(a());

  EXPECT_EQ(a.shape(), (mini_vector<size_t, 4>{1, 2, 3, 4}));
  EXPECT_EQ(v.shape(), (mini_vector<size_t, 4>{3, 1, 4, 2}));
  EXPECT_EQ(a.indexmap().memory_layout(), (memory_layout_t<4>{2, 0, 3, 1}));
  EXPECT_EQ(v.indexmap().memory_layout(), (memory_layout_t<4>{0, 1, 2, 3}));
}

MAKE_MAIN;
