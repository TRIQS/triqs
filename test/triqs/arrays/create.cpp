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
#include <type_traits>
static_assert(!std::is_pod<triqs::arrays::array<long, 2>>::value, "POD pb");

TEST(Array, Create) {

  array<long, 2> A;
  A.resize(make_shape(3, 3));
  EXPECT_EQ(A.shape(), (mini_vector<size_t, 2>{3, 3}));

  matrix<double> M;
  M.resize(3, 3);

  EXPECT_EQ(M.shape(), (mini_vector<size_t, 2>{3, 3}));

  vector<double> V;
  V.resize(10);

  EXPECT_EQ(V.shape(), (mini_vector<size_t, 1>{10}));
}
MAKE_MAIN;
