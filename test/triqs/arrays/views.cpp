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

TEST(Array, View) {
  {
    array<int, 2> *p = new array<int, 2>(2, 3); // create an array p
    for (int i = 0; i < 2; ++i)
      for (int j = 0; j < 3; ++j) (*p)(i, j) = 10 * i + j;
    array_view<int, 2> B(*p); // making a view
    delete p;                 // p is gone...
    B(0, 0) = 314;
    EXPECT_EQ(B(0, 0), 314); // ok, but B (and the data) is still alive
  }

  array<long, 2> A(2, 3);
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) A(i, j) = 10 * i + j;

  array_view<long, 2> AA(A);

  std::stringstream fs1, fs2;
  fs1 << A;
  fs2 << AA;
  EXPECT_EQ(fs1.str(), fs2.str());
  EXPECT_EQ(AA(0, 0), 0);

  array_view<long, 1> SL1(A(0, range(0, 3)));
  array_view<long, 1> SL2(A(1, range(0, 2)));
  array_view<long, 1> SL3(A(1, range(1, 3)));
  array_view<long, 1> SL4(A(range(0, 2), 0));
  array_view<long, 1> SL5(A(range(0, 2), 1));

  EXPECT_EQ_ARRAY(SL1, (array<long, 1>{0, 1, 2}));
  EXPECT_EQ_ARRAY(SL2, (array<long, 1>{10, 11}));
  EXPECT_EQ_ARRAY(SL3, (array<long, 1>{11, 12}));
  EXPECT_EQ_ARRAY(SL4, (array<long, 1>{0, 10}));
  EXPECT_EQ_ARRAY(SL5, (array<long, 1>{1, 11}));
}
MAKE_MAIN;
