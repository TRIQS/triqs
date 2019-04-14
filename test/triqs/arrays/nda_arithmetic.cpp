#include "./nda_test_common.hpp"

placeholder<0> i_;
placeholder<1> j_;

TEST(NDA, compound_ops) {

  array<long, 2> A(2, 3), Af(2, 3, FORTRAN_LAYOUT);
  Af(i_, j_) << 10 * i_ + j_;
  A = Af;

  A *= 2.0;
  EXPECT_ARRAY_NEAR(A, array<long, 2>{{0, 2, 4}, {20, 22, 24}});

  Af /= 2.0;
  EXPECT_ARRAY_NEAR(Af, array<long, 2>{{0, 0, 1}, {5, 5, 6}});

  array<double, 2> B(A);
  B /= 4;
  EXPECT_ARRAY_NEAR(B, array<double, 2>{{0.0, 0.5, 1.0}, {5.0, 5.5, 6.0}});
}
MAKE_MAIN;
