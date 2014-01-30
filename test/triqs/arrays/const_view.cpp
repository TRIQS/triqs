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
#include "./src/array.hpp"
#include <iostream>

using namespace triqs::arrays;


template <typename U, ull_t opt, ull_t to, bool B> void f(array_view<U, 2, opt, to, B, true> const &a) {
 std::cout << a << std::endl;
}

void f2(array_const_view<long, 2> const &a) { std::cout << a << std::endl; }

int main(int argc, char **argv) {

 {
  array<long, 2> A(2, 3);
  A() = 3;
  f2(A());


  array_const_view<long,2> Vc = A();
  //array_view<long,2> V = Vc;
 }

//#define SHOULD_NOT_COMPILE
#ifdef SHOULD_NOT_COMPILE
 {
  const array<long, 1> A = {1, 2, 3, 4};

  // None of this should compile
  A(0) = 2;
  A()(0) = 2;
  A(range(0,2))(0) = 10;
 }
#endif
 return 0;
}

