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

template <typename A> std::string print_in_indexmap_order(A const &a) {
  std::stringstream out;
  for (typename A::indexmap_type::iterator it(a.indexmap()); it; ++it) {
    auto i = it.indices();
    out << a(i[0], i[1], i[2]) << " ";
  }
  return out.str();
}

template <class AA> void f(AA &A) {

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 4; ++k) A(i, j, k) = 100 * (i + 1) + 10 * (j + 1) + (k + 1);

  auto _ = range{};

  EXPECT_EQ_ARRAY(A(0, _, _), (array<long, 2>{{111, 112, 113, 114}, {121, 122, 123, 124}, {131, 132, 133, 134}}));
  EXPECT_EQ_ARRAY(A(1, _, _), (array<long, 2>{{211, 212, 213, 214}, {221, 222, 223, 224}, {231, 232, 233, 234}}));
  EXPECT_EQ_ARRAY(A(_, 0, _), (array<long, 2>{{111, 112, 113, 114}, {211, 212, 213, 214}}));
  EXPECT_EQ_ARRAY(A(_, _, 1), (array<long, 2>{{112, 122, 132}, {212, 222, 232}}));
  EXPECT_EQ_ARRAY(A(_, 0, 1), (array<long, 1>{112, 212}));
}

TEST(Array, View3) {

  // permutation in triqs::arrays
  array<long, 3> A0(2, 3, 4);
  array<long, 3, _traversal_dynamical> A1(2, 3, 4, make_memory_layout(2, 1, 0));
  array<long, 3, _traversal_dynamical> A2(2, 3, 4, make_memory_layout(2, 0, 1));
  array<long, 3> A3(2, 3, 4, make_memory_layout(0, 1, 2));
  array<long, 3, _traversal_fortran> A4(2, 3, 4, FORTRAN_LAYOUT);

  f(A0);
  f(A1);
  f(A2);
  f(A3);
  f(A4);

  EXPECT_EQ(print_in_indexmap_order(A0), "111 112 113 114 121 122 123 124 131 132 133 134 211 212 213 214 221 222 223 224 231 232 233 234 ");
  EXPECT_EQ(print_in_indexmap_order(A1), "111 211 121 221 131 231 112 212 122 222 132 232 113 213 123 223 133 233 114 214 124 224 134 234 ");
  EXPECT_EQ(print_in_indexmap_order(A2), "111 121 131 211 221 231 112 122 132 212 222 232 113 123 133 213 223 233 114 124 134 214 224 234 ");
  EXPECT_EQ(print_in_indexmap_order(A3), "111 112 113 114 121 122 123 124 131 132 133 134 211 212 213 214 221 222 223 224 231 232 233 234 ");
  EXPECT_EQ(print_in_indexmap_order(A4), "111 211 121 221 131 231 112 212 122 222 132 232 113 213 123 223 133 233 114 214 124 224 134 234 ");
}
MAKE_MAIN;
