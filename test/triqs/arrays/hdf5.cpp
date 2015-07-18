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
#include "test_tools.hpp"
#include <triqs/arrays.hpp>
using namespace triqs::arrays;
namespace h5 = triqs::h5;

/*template<typename T>
class H5Test : ::testing::test { 
 public : 
  T value;
};


TYPED_TEST_P(H5Test, RW) {
   TypeParam n = 0;
 Construction pb...
}
*/



TEST(Array, H5) {

 array<long, 2> A(2, 3), B, Af(FORTRAN_LAYOUT), Bf(FORTRAN_LAYOUT);
 array<double, 2> D(2, 3), D2;
 array<std::complex<double>, 1> C(5), C2;
 std::complex<double> z(1, 2);

 for (int i = 0; i < 5; ++i) C(i) = std::complex<double>(i, i);

 for (int i = 0; i < 2; ++i)
  for (int j = 0; j < 3; ++j) {
   A(i, j) = 10 * i + j;
   D(i, j) = A(i, j) / 10.0;
  }
 Af = A;

 // WRITE the file
 {
  h5::file file("ess.h5", H5F_ACC_TRUNC);
  h5::group top(file);

  h5_write(top, "A", A);
  h5_write(top, "Af", Af);
  h5_write(top, "C", C);
  h5_write(top, "D", D);
  h5_write(top, "S", "");
  h5_write(top,"A_slice",A(range(),range(1,3)));

  // scalar
  double x = 2.3;
  h5_write(top, "x", x);

  // std::complex<double> xx(2, 3);
  // h5_write(top, "xx", xx);

  h5_write(top, "s", std::string("a nice chain"));

  top.create_group("G");
  h5_write(top, "G/A", A);

  auto G = top.open_group("G");
  h5_write(G, "A2", A);
 }

 // READ the file
 {
  h5::file file("ess.h5", 'r');
  h5::group top(file);

  h5_read(top, "A", B);
  EXPECT_EQ_ARRAY(A, B);

  h5_read(top, "Af", Bf);
  EXPECT_EQ_ARRAY(Af, Bf);

  h5_read(top, "D", D2);
  EXPECT_CLOSE_ARRAY(D, D2);

  h5_read(top, "C", C2);
  EXPECT_CLOSE_ARRAY(C, C2);

  array<long,2> a_sli;
  h5_read(top, "A_slice", a_sli);
  EXPECT_EQ_ARRAY(a_sli, A(range(), range(1, 3)));

  double xxx = 0;
  h5_read(top, "x", xxx);
  EXPECT_DOUBLE_EQ(xxx, 2.3);

  std::string s2("----------------------------------");
  h5_read(top, "s", s2);
  EXPECT_EQ(s2, "a nice chain");
 }
}

MAKE_MAIN;

