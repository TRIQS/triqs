
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

TEST(Array, AssignVectorArray) {

  vector<double> V;
  array<double, 1> Va(5);
  for (int i = 0; i < 5; ++i) Va(i) = i + 2;

  V = Va / 2.0;
  EXPECT_ARRAY_NEAR(V, array<double, 1>{1.0, 1.5, 2.0, 2.5, 3.0});
  EXPECT_ARRAY_NEAR(Va, array<double, 1>{2, 3, 4, 5, 6});

  V = Va;
  EXPECT_ARRAY_NEAR(V, array<double, 1>{2, 3, 4, 5, 6});
  EXPECT_ARRAY_NEAR(Va, array<double, 1>{2, 3, 4, 5, 6});
}
MAKE_MAIN;
