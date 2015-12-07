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
#include <triqs/utility/variant_int_string.hpp>
#include <triqs/test_tools/arrays.hpp>

using triqs::utility::variant_int_string;

struct visitor {
 std::string operator()(int i) { return "int = " + std::to_string(i); }
 std::string operator()(std::string const &s) { return "string = " + s; }
};

TEST(Variant, Base) {

 auto v = variant_int_string(3);
 EXPECT_EQ(apply_visitor(visitor(), v), "int = 3");

 auto v2 = variant_int_string("some_string");
 EXPECT_EQ(apply_visitor(visitor(), v2), "string = some_string");

 auto v3 = v2;
 EXPECT_EQ(apply_visitor(visitor(), v3), "string = some_string");

 v = v2;
 EXPECT_EQ(apply_visitor(visitor(), v), "string = some_string");

 {
  auto x = variant_int_string(3);
  auto y = variant_int_string(5);
  EXPECT_TRUE(x < y);
 }

 {
  auto x = variant_int_string("ab");
  auto y = variant_int_string("bc");
  EXPECT_TRUE(x < y);
 }
}

MAKE_MAIN;

