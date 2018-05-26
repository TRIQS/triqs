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

using std::cout;
using std::endl;
using namespace triqs::arrays;

int main(int argc, char **argv) {

  array<long, 2> A(2, 3);

  // first print the index generator
  for (array<long, 2>::indexmap_type::domain_type::generator it = A.indexmap().domain().begin(); it; ++it) std::cout << "  " << *it << std::endl;

  std::cout << std::endl;

  for (array<long, 2>::indexmap_type::domain_type::gal_generator<permutations::permutation(0, 1)> it(A.indexmap().domain()); it; ++it)
    std::cout << "  " << *it << std::endl;

  std::cout << std::endl;

  for (array<long, 2>::indexmap_type::domain_type::gal_generator<permutations::permutation(1, 0)> it(A.indexmap().domain()); it; ++it)
    std::cout << "  " << *it << std::endl;

  std::cout << std::endl;

  std::cout << " C order : traversal" << std::endl;

  for (array<long, 2>::iterator it = A.begin(); it; ++it) {
    *it = it.indices()[0] + 10 * it.indices()[1];
    std::cout << " i,j = " << it.indices() << std::endl;
  }
  std::cout << "A = i + 10*j" << A << std::endl;

  int u = 0;
  for (array<long, 2>::iterator it = A.begin(); it; ++it, ++u) {
    *it = u;
    std::cout << " i,j = " << it.indices() << std::endl;
  }
  std::cout << "A = order of visit " << A << std::endl;

  std::cout << " F order : traversal" << std::endl;
  array<long, 2, _traversal_fortran> Af(2, 3, FORTRAN_LAYOUT);

  for (auto it = Af.begin(); it; ++it) {
    *it = it.indices()[0] + 10 * it.indices()[1];
    std::cout << " i,j = " << it.indices() << std::endl;
  }
  std::cout << "A = i + 10*j" << Af << std::endl;

  u = 0;
  for (auto it = Af.begin(); it; ++it, ++u) {
    *it = -u;
    std::cout << " i,j = " << it.indices() << std::endl;
  }
  std::cout << "Af = order of visit " << Af << std::endl;

  return 0;
}
