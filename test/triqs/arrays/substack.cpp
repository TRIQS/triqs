
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018 by N. Wentzell
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
#include <triqs/test_tools/gfs.hpp>

using namespace triqs::arrays;

TEST(matrix, vstack) {

  matrix<double> A1 = { { 1, 2 }, { 3, 4 }}; 
  matrix<double> A2 = { { 5, 6 }, { 7, 8 }}; 

  auto B = vstack(A1, A2);

  EXPECT_EQ(B(2, 0), 5);
  EXPECT_EQ(B(3, 1), 8);
  
  EXPECT_EQ(B(range(0, 2), range(0, 2)), A1);
  EXPECT_EQ(B(range(2, 4), range(0, 2)), A2);
  
  EXPECT_EQ(B.shape(), make_shape(4, 2));
}

MAKE_MAIN;
