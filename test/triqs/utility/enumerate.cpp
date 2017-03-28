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
#include <triqs/utility/enumerate.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <vector>
TEST(Enumerate, All) {

 std::vector<int> V{1, 2, 3, 4, 5, 6};

 for (auto [i, x] : triqs::utility::enumerate(V)) {
  //static_assert(std::is_reference<decltype(i)>::value, "oops");
  static_assert(std::is_reference<decltype(x)>::value, "oops");
  std::cerr << i << " " << x << "\n";
  //x *= 10;
 }

 for (auto const & [i, x] : triqs::utility::enumerate(V)) {
  std::cerr << i << " " << x << "\n";
 }
}
MAKE_MAIN;
