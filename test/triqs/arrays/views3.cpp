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
#define ARRAY_DEBUG_SLICE

#include <triqs/arrays/array.hpp>
#include <iostream>

using std::cout;
using std::endl;
using namespace triqs::arrays;

///
template <typename IM, typename A> void print_in_indexmap_order(std::ostream &out, IM const &im, A const &a) {
 out << "[";
 for (typename IM::iterator it(im); it; ++it) {
  auto i = it.indices();
  out << a(i[0], i[1], i[2]) << " ";
 }
 out << "]";
}

template <class AA> void f(AA &A) {

 for (int i = 0; i < 2; ++i)
  for (int j = 0; j < 3; ++j)
   for (int k = 0; k < 4; ++k) A(i, j, k) = 100 * (i + 1) + 10 * (j + 1) + (k + 1);

 print_in_indexmap_order(std::cout, A.indexmap(), A);
 std::cout << std::endl;

 std::cout << A(0, range(), range()) << std::endl;
 std::cout << A(1, range(), range()) << std::endl;
 std::cout << A(range(), 0, range()) << std::endl;
 std::cout << A(range(), range(), 1) << std::endl;
 std::cout << A(range(), 0, 1) << std::endl;
 std::cout << "-------------------------------------------------- " << std::endl;
}

int main(int argc, char **argv) {

 try {

  // permutation in triqs::arrays
  array<long, 3> A0(2, 3, 4);
  array<long, 3, _traversal_dynamical> A1(2, 3, 4, make_memory_layout(2, 1, 0));
  array<long, 3, _traversal_dynamical> A2(2, 3, 4, make_memory_layout(2, 0, 1));
  array<long, 3> A3(2, 3, 4, make_memory_layout(0, 1, 2));
  array<long, 3, _traversal_fortran> A4(2, 3, 4, FORTRAN_LAYOUT);

  f(A0);
  f(A1);
  f(A2);
  f(A3);
  f(A4);
 }

 catch (const char *ERR) {

  std::cout << "Error is " << ERR << std::endl;
 }


 return 0;
}

