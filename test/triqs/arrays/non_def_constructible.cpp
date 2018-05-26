/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016 by O. Parcollet
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

// a little non copyable object
struct A {
  int i        = 2;
  A(A const &) = delete;
  A(A &&)      = default;
  A &operator=(A const &) = delete;
  A &operator=(A &&) = default;

  A(int j) : i(j) { std::cout << " constructing A : " << i << std::endl; }
};

TEST(Array, NonDefaultConstructible) {

  array<A, 2> a(make_shape(2, 2), [](int i, int j) { return i + 10 * j; });
  // array<A,2> a( {2,2}, [](int i, int j) { return i+ 10*j;});
  array<A, 1> a1(make_shape(2), [](int i) { return i; });

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) { EXPECT_EQ(a(i, j).i, i + 10 * j); }

  // copy fails to compile
  //auto b= a;

  // view is ok
  auto b = a();

  //a.resize(3,3); // does not compile
}

MAKE_MAIN;
