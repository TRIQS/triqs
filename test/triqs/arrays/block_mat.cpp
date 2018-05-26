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

TEST(BlockMatrixH5, S1) {

  array<matrix<double>, 1> V{matrix<double>{{1, 2}, {3, 4}}, matrix<double>{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}};

  auto W = rw_h5(V, "block_mat");

  EXPECT_EQ(first_dim(V), first_dim(W));
  for (int i = 0; i < first_dim(V); ++i) EXPECT_ARRAY_NEAR(V(i), W(i));
}

MAKE_MAIN;
