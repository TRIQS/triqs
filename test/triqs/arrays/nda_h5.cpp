#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/arrays.hpp>
#include <cmath>
#include <limits>
#include <triqs/arrays.hpp>
#include <triqs/arrays/asserts.hpp>

using namespace triqs::arrays;
namespace h5 = triqs::h5;

// ==============================================================

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

// ==============================================================

TEST(Array, H5ArrayString) {

  // array of string
  array<std::string, 1> A(2), B;
  A(0) = "Nice String";
  A(1) = "another";

  // vector of string
  std::vector<std::string> V1, V2;
  V1.push_back("abcd");
  V1.push_back("de");

  // writing
  h5::file file("test_array_string.h5", H5F_ACC_TRUNC);
  h5::group top(file);

  h5_write(top, "A", A);
  h5_write(top, "V", V1);

  // rereading
  h5_read(top, "A", B);
  h5_read(top, "V", V2);

  //comparing
  for (int i = 0; i < 2; ++i) {
    EXPECT_EQ(V1[i], V2[i]);
    EXPECT_EQ(A(i), B(i));
  }
}
// ==============================================================

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

// ==============================================================

// -----------------------------------------------------
// Testing h5 for std vector
// -----------------------------------------------------

TEST(Array, H5StdVector) {

  std::vector<double> v{1.1, 2.2, 3.3, 4.5};
  std::vector<std::complex<double>> vc{1.1, 2.2, 3.3, 4.5};

  std::vector<double> v2;
  std::vector<std::complex<double>> vc2;

  {
    h5::file file1("test_std_vector.h5", H5F_ACC_TRUNC);
    // do we need this top ?
    h5::group top(file1);
    h5_write(top, "vdouble", v);
    h5_write(top, "vcomplex", vc);
  }

  {
    h5::file file2("test_std_vector.h5", H5F_ACC_RDONLY);
    h5::group top2(file2);
    h5_read(top2, "vdouble", v2);
    h5_read(top2, "vcomplex", vc2);
  }

  for (size_t i = 0; i < v.size(); ++i) EXPECT_EQ(v[i], v2[i]);
  for (size_t i = 0; i < vc.size(); ++i) EXPECT_EQ(vc[i], vc2[i]);
}

// ==============================================================

// -----------------------------------------------------
// Testing h5 for an array of matrix
// -----------------------------------------------------

TEST(BlockMatrixH5, S1) {

  array<matrix<double>, 1> V{matrix<double>{{1, 2}, {3, 4}}, matrix<double>{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}};

  auto W = rw_h5(V, "block_mat");

  EXPECT_EQ(first_dim(V), first_dim(W));
  for (int i = 0; i < first_dim(V); ++i) EXPECT_ARRAY_NEAR(V(i), W(i));
}
MAKE_MAIN
