/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#include "./common.hpp"
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/make_immutable_array.hpp>
#include <iostream>

namespace tqa = triqs::arrays;
namespace tql = triqs::clef;
using tqa::range;

int main(int argc, char **argv) {

  tql::placeholder<0> i_;
  tql::placeholder<1> j_;

  TEST(make_immutable_array(i_ + j_, i_ = range(0, 2), j_ = range(0, 2)));

  TEST((tqa::array<int, 2>(tqa::make_immutable_array(i_ + j_, i_ = range(0, 2), j_ = range(0, 2)))));

  // or if you prefer using a lambda...
  auto b = make_immutable_array([](int i, int j) { return i - j; }, range(0, 2), range(0, 2));

  std::cerr << "b = " << b << std::endl;
  std::cerr << "b = " << tqa::array<double, 2>(b) << std::endl;

  return 0;
}
