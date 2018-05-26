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
#include <iostream>
namespace tqa = triqs::arrays;
namespace tql = triqs::clef;

int main(int argc, char **argv) {

  tql::placeholder<0> i_;
  tql::placeholder<1> j_;
  tqa::array<double, 2> A(2, 2);
  A(i_, j_) << i_ + 2 * j_;
  std::cout << "A = " << A << std::endl;

  tqa::array<double, 2> B(2, 2);
  B(i_, j_) << i_ + 2 * j_;
  std::cout << "B = " << B << std::endl;
}
