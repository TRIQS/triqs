/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017 by O. Parcollet
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
#include <triqs/utility/zip.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <vector>
TEST(Enumerate, All) {

 using triqs::utility::zip;

 std::vector<int> V1{1, 2, 3, 4, 5, 6};
 std::vector<int> V2{-1, -2, -3, -4, -5, -6};

 for (auto[x,y] : zip(V1,V2)) {
  static_assert(std::is_reference<decltype(x)>::value, "oops");
  static_assert(std::is_reference<decltype(y)>::value, "oops");
  std::cerr << x << " " << y << "\n";
  x *= 10;
 }

 for (auto[x,y,z] : zip(V1,V2, V2)) {
  std::cerr << x << " " << y << " " << z << "\n";
 }
}
MAKE_MAIN;
