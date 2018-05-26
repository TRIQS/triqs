/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 20185 by Simons Foundation
 * author : O. Parcollet
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

TEST(Array, SwapIndex) {

  array<double, 4> A(1, 2, 3, 4);

  triqs::clef::placeholder<0> i_;
  triqs::clef::placeholder<1> j_;
  triqs::clef::placeholder<2> k_;
  triqs::clef::placeholder<3> l_;

  A(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  auto S = swap_index_view(A, 0, 2);

  array<double, 4> B(3, 2, 1, 4);
  B(k_, j_, i_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  EXPECT_EQ(S, B());
  EXPECT_EQ(first_dim(A), third_dim(S));
  EXPECT_EQ(first_dim(S), third_dim(A));
}

MAKE_MAIN;
