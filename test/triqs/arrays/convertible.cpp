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
#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
using namespace triqs::arrays;
namespace h5 = triqs::h5;

TEST(Array, ConvertibleCR) {

  array<double, 2> A(2, 2);
  array<dcomplex, 2> B(2, 2);

  //A = B; // should not compile
  B = A;

  // can convert an array of double to an array of complex
  static_assert(std::is_convertible<array<double, 2>, array<dcomplex, 2>>::value, "oops");

  // can not do the reverse !
  static_assert(!std::is_convertible<array<dcomplex, 2>, array<double, 2>>::value, "oops");
}

MAKE_MAIN;
