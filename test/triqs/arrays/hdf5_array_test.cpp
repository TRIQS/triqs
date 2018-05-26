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
#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
using namespace triqs::arrays;
namespace h5 = triqs::h5;

TEST(Array, H5) {

  array<long, 2> A(2, 3), B, Af(FORTRAN_LAYOUT), Bf(FORTRAN_LAYOUT);
  array<double, 2> D(2, 3), D2;
  array<dcomplex, 1> C(5), C2;
  dcomplex z(1, 2);

  for (int i = 0; i < 5; ++i) C(i) = dcomplex(i, i);

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
    h5_write(top, "A_slice", A(range(), range(1, 3)));
    h5_write(top, "empty", array<double, 2>(0, 10));

    // add some attribute to A
    auto id = top.open_dataset("A");
    h5_write_attribute(id, "AttrOfA1", 12);
    h5_write_attribute(id, "AttrOfA2", 8.9);

    // scalar
    double x = 2.3;
    h5_write(top, "x", x);

    // dcomplex xx(2, 3);
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

    // read the attributes of A
    auto id     = top.open_dataset("A");
    int att1    = h5::h5_read_attribute<int>(id, "AttrOfA1");
    double att2 = h5::h5_read_attribute<double>(id, "AttrOfA2");
    EXPECT_EQ(att1, 12);
    EXPECT_EQ(att2, 8.9);

    h5_read(top, "Af", Bf);
    EXPECT_EQ_ARRAY(Af, Bf);

    h5_read(top, "D", D2);
    EXPECT_ARRAY_NEAR(D, D2);

    h5_read(top, "C", C2);
    EXPECT_ARRAY_NEAR(C, C2);

    array<long, 2> a_sli;
    h5_read(top, "A_slice", a_sli);
    EXPECT_EQ_ARRAY(a_sli, A(range(), range(1, 3)));

    double xxx = 0;
    h5_read(top, "x", xxx);
    EXPECT_DOUBLE_EQ(xxx, 2.3);

    std::string s2("----------------------------------");
    h5_read(top, "s", s2);
    EXPECT_EQ(s2, "a nice chain");

    array<double, 2> empty(5, 5);
    h5_read(top, "empty", empty);
    EXPECT_EQ_ARRAY(empty, (array<double, 2>(0, 10)));
  }
}

// -----------------------------------------------------
// Testing the loading of array of double into complex
// -----------------------------------------------------

TEST(Array, H5RealIntoComplex) {

  array<double, 2> D(2, 3);
  array<dcomplex, 2> C(2, 3);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) { D(i, j) = 10 * i + j; }

  // WRITE the file
  {
    h5::file file("ess_real_complex.h5", H5F_ACC_TRUNC);
    h5::group top(file);
    h5_write(top, "D", D);
  }

  // READ the file
  {
    C() = 89 + 9_j; // put garbage in it
    h5::file file("ess_real_complex.h5", 'r');
    h5::group top(file);
    h5_read(top, "D", C);
    EXPECT_ARRAY_NEAR(C, D);
  }
}

MAKE_MAIN;
