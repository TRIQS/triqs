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
#include <triqs/utility/real_or_complex.hpp>
#include "../test_tools.hpp"

using triqs::utility::real_or_complex;

TEST(RealOrComplex, All) {

 auto x= real_or_complex(1);
 auto y= real_or_complex(3);

 {
  auto z = x + y;
  EXPECT_EQ(double(z), 4);
  EXPECT_EQ(z.is_real(), true);
 }

 { 
  auto z = x + 1_j;
  EXPECT_EQ(z.is_real(), false);
  EXPECT_THROW(double y = double(z), std::exception);
 }

 {
  auto z = 2 + x;
  EXPECT_EQ(double(z), 3);
 }
}
MAKE_MAIN;
