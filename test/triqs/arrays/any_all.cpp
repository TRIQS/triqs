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
#include "./start.hpp"
#include <cmath>
#include <limits>

TEST(Array, any_all) {

 auto nan = std::numeric_limits<double>::quiet_NaN();

 array<double, 2> A(2, 3);
 A() = 98;

 EXPECT_FALSE(any(isnan(A)));

 A() = nan;
 EXPECT_TRUE(all(isnan(A)));

 A() = 0;
 A(0,0) = nan;

 EXPECT_FALSE(all(isnan(A)));
 EXPECT_TRUE(any(isnan(A)));

}
MAKE_MAIN
