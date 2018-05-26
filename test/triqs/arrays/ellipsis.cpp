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
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/arrays/matrix.hpp>
#include <iostream>
#include <triqs/arrays/asserts.hpp>

using namespace triqs::arrays;

template <typename ArrayType> array_view<typename ArrayType::value_type, ArrayType::rank - 1> sum0(ArrayType const &A) {
  array<typename ArrayType::value_type, ArrayType::rank - 1> res = A(0, ellipsis());
  for (size_t u = 1; u < A.shape()[0]; ++u) res += A(u, ellipsis());
  return res;
}

int main(int argc, char **argv) {

  {
    array<long, 3> A(2, 3, 4);
    A() = 7;

    std::cerr << A(0, ellipsis()) << std::endl;

    array<long, 4> B(2, 3, 4, 5);
    B() = 8;

    std::cerr << B(0, ellipsis(), 3) << std::endl;
    std::cerr << B(0, ellipsis(), 2, 3) << std::endl;
    std::cerr << B(ellipsis(), 2, 3) << std::endl;

    assert_all_close(B(0, ellipsis(), 3), B(0, range(), range(), 3), 1.e-15);
    assert_all_close(B(0, ellipsis(), 2, 3), B(0, range(), 2, 3), 1.e-15);
    assert_all_close(B(ellipsis(), 2, 3), B(range(), range(), 2, 3), 1.e-15);

    std::cerr << B.shape() << std::endl;
  }
  ///.....

  {
    array<double, 2> A(5, 2);
    A() = 2;
    array<double, 3> B(5, 2, 3);
    B() = 3;
    std::cerr << sum0(A) << sum0(B) << std::endl;
  }

  return 0;
}
