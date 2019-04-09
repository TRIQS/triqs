/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include <triqs/arrays.hpp>
#include <triqs/utility/view_tools.hpp>

using namespace triqs;
using namespace triqs::arrays;

template <typename T> struct check {
  static_assert(std::is_same<typename T::view_type, view_or_type_t<T>>::value, "err");
  static_assert(std::is_same<typename T::regular_type, regular_t<T>>::value, "err");
};

int main(int argc, char **argv) {
  check<array<int, 1>>();
  check<array_view<int, 1>>();
  check<matrix<int>>();
  check<matrix_view<int>>();
  return 0;
}
