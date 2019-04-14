#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/arrays.hpp>
#include <cmath>
#include <limits>
#include <triqs/arrays.hpp>
#include <triqs/arrays/asserts.hpp>
#include <triqs/arrays/blas_lapack/dot.hpp>

// ==============================================================

TEST(Vector, Ops) {

  vector<double> V{1, 2, 3, 4, 5};

  V *= 2;
  EXPECT_EQ(V, (array<double, 1>{2, 4, 6, 8, 10}));

  V[range(2, 4)] /= 2.0;
  EXPECT_EQ(V, (array<double, 1>{2, 4, 3, 4, 10}));

  V[range(0, 5, 2)] *= 10;
  EXPECT_EQ(V, (array<double, 1>{20, 4, 30, 4, 100}));
}

// ==============================================================

TEST(Vector, Ops2) {

  vector<double> V{1, 2, 3, 4, 5};
  auto W = V;

  W += V;
  EXPECT_EQ(V, (array<double, 1>{1, 2, 3, 4, 5}));
  EXPECT_EQ(W, (array<double, 1>{2, 4, 6, 8, 10}));

  W -= V;
  EXPECT_EQ(V, (array<double, 1>{1, 2, 3, 4, 5}));
  EXPECT_EQ(W, (array<double, 1>{1, 2, 3, 4, 5}));
}

// ==============================================================

TEST(Vector, Dot) {
  vector<double> a(2), aa(2), c(2);
  a() = 2.0;
  c() = 1;
  vector<int> b(2);
  b() = 3;
  aa  = 2 * a;

  EXPECT_DOUBLE_EQ(dot(a, b), 12);
  EXPECT_DOUBLE_EQ(dot(aa, a), 16);
  EXPECT_DOUBLE_EQ(dot(aa, b), 24);
  EXPECT_DOUBLE_EQ(dot(aa - a, b), 12);
}

// ==============================================================

TEST(Vector, Dot2) {

  /// Added by I. Krivenko, #122
  /// test the complex version, specially with the zdotu workaround on Os X.
  vector<std::complex<double>> v(2);
  v(0) = 0;
  v(1) = {0, 1};

  EXPECT_COMPLEX_NEAR(blas::dot<false>(v, v), -1);
  EXPECT_COMPLEX_NEAR(blas::dot<true>(v, v), 1);
  EXPECT_COMPLEX_NEAR(dot(v, v), -1);
  EXPECT_COMPLEX_NEAR(dotc(v, v), 1);
}

MAKE_MAIN

