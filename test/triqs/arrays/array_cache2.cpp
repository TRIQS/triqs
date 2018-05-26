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
#define TRIQS_ARRAYS_CACHE_COPY_VERBOSE
#include <triqs/arrays/cache.hpp>
#include <iostream>

using namespace triqs::arrays;

void f(array_view<long, 2> A, long u) { A(0, 0) = u; }

int main(int argc, char **argv) {

  array<long, 2> A(2, 3, FORTRAN_LAYOUT);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) A(i, j) = 10 * i + j;

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) std::cout << "A " << A(i, j) << std::endl;

  array<long, 2> Ac(A);

  std::cout << "A = " << A << std::endl;
  std::cout << "Ac = " << Ac << std::endl;

  std::cout << "----------" << std::endl;
  std::cout << "Expr = " << make_const_cache(2 * A).view() << std::endl;

  std::cout << "----------" << std::endl;
  std::cout << "A F->C = " << make_const_cache(A).view() << std::endl;

  std::cout << "----------" << std::endl;
  std::cout << "A C->C = " << make_const_cache(Ac).view() << std::endl;

  std::cout << "----------" << std::endl;
  f(make_cache(Ac), 287);
  std::cout << "Ac = " << Ac << std::endl;

  std::cout << "----------" << std::endl;
  f(make_cache(A), 287);
  std::cout << "A = " << A << std::endl;

  std::cout << "----------" << std::endl;
  std::cout << "A = " << A << std::endl;
  std::cout << "A(range(0,1),range(1,2) ) = " << A(range(0, 1), range(1, 2)) << std::endl;

  array_view<long, 2> V(A(range(0, 1), range(1, 2)));
  array_view<long, 2> Vc(Ac(range(0, 1), range(1, 2)));

  Vc(0, 0) = 300;

  V = Vc;
  std::cout << "A = " << A << std::endl;
  std::cout << "Ac = " << Ac << std::endl;

  f(make_cache(V), 156);
  std::cout << "A = " << A << std::endl;
  std::cout << "Ac = " << Ac << std::endl;

  return 0;
}
