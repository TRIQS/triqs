/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include <triqs/utility/buffered_function.hpp>
#include <triqs/test_tools/arrays.hpp>

TEST(BufferedFunction, All) {
 auto f = [x = 0]() mutable {
  auto res = x * x;
  x++;
  return res;
 };
 auto gen = triqs::utility::buffered_function<double>(f, 5);
 for (int u = 0; u < 22; ++u) EXPECT_EQ(gen(), u * u);
}
MAKE_MAIN;
