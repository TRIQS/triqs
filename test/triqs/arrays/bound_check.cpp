/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
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
TEST(Array, BoundCheck) {

  array<long, 2> A(2, 3);

  EXPECT_THROW(A(0, 3), key_error);
  EXPECT_THROW(A(range(0, 4), 2), key_error);
  EXPECT_THROW(A(range(10, 14), 2), key_error);
  EXPECT_THROW(A(range(), 5), key_error);
  EXPECT_THROW(A(0, 3), key_error);
}
MAKE_MAIN;
