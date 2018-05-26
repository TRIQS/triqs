
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
#include <triqs/arrays/matrix.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <iostream>
#include <triqs/arrays/asserts.hpp>

using namespace triqs::arrays;
namespace tqa = triqs::arrays;
using namespace indexmaps;
using namespace storages;
#include "triqs/utility/typeid_name.hpp"

int main(int argc, char **argv) {

  matrix<long> A(2, 2), B(2, 2), C(2, 2);
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) {
      A(i, j) = 10 * i + j;
      B(i, j) = i + 2 * j;
    }
  std::cout << " A = " << A << std::endl;
  std::cout << " B = " << B << std::endl;

  std::cout << (A + 2 * B) << std::endl;
  std::cout << "-------" << std::endl;

  std::cout << (A + 2 * B).domain() << std::endl;
  std::cout << "----EVAL ---" << std::endl;

  C = A + 2 * B;
  std::cout << " C = A + 2*B = " << C << std::endl;

  C = std::plus<matrix<long>>()(A, B);
  std::cout << " C = A+B =  " << C << std::endl;

  // matrix multiplication
  matrix<double> Af(2, 2), Bf(2, 2), Cf(2, 2), id(2, 2);
  Af       = A;
  Bf       = B;
  Bf(0, 0) = 1;
  Cf()     = 0;
  id()     = 1;
  std::cout << " Af = " << Af << std::endl;
  std::cout << " Bf = " << Bf << std::endl;

  std::cout << (Af * Bf).domain() << std::endl;

  std::cout << " computing  Cf = Af * Bf " << std::endl;
  Cf = Af * Bf;
  std::cout << " Cf  = " << Cf << std::endl;

  std::cout << " matrix( Af * Bf )" << matrix<double>(Af * Bf) << std::endl;

  std::cout << " matrix( Af * (Bf + Cf) )" << matrix<double>(Af * (Bf + Cf)) << std::endl;

  TEST(A);
  TEST(make_matrix(2 * A));
  TEST(A + 2);
  TEST(make_matrix(A + 2));
  TEST(make_matrix(1 + A));

  //  test the vector ops : scalar * vector, vector + vector, ...
  tqa::vector<double> V(3);
  V(0) = 1;
  V(1) = 2;
  V(2) = 3;
  tqa::vector<double> V2(3);
  V2(0) = 10;
  V2(1) = 20;
  V2(2) = 30;
  TEST(make_vector(V2 + 2.0 * V));

  // test the division by a matrix
  TEST(Af);
  TEST(1 / Af);
  TEST(make_matrix(2 / Af));
  TEST(make_matrix(Af / 2));

  assert_all_close(make_matrix(Af / Af), id, 1.e-14);
  //TEST(make_matrix(Af/Af));

  // matrix <long> AA(3,3); TEST( tqa::make_matrix(A+ 2* AA)); // exception because of size...
  return 0;
}
