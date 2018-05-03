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
#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
#if not H5_VERSION_GE(1, 8, 9)
int main() {}

#else

#include <triqs/h5/serialization.hpp>
using triqs::h5::serialize;
using triqs::h5::deserialize;
using namespace triqs::arrays;

TEST(H5Serialize, All) {

 auto a = array<double, 1>{1, 2, 3, 4, 5};
 auto s = serialize(a);
 auto b = deserialize<array<double, 1>>(s);
 EXPECT_ARRAY_NEAR(a, b);

 auto s2 = serialize(array<double, 1>(10000));
 auto s3 = serialize(array<double, 1>(20000));
 EXPECT_TRUE(first_dim(s3) < first_dim(s2) * 2); // Test scaling in size with overhead
}
MAKE_MAIN;

#endif

