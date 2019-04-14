#include "./nda_test_common.hpp"
// ==============================================================

TEST(NDA, Algo1) {
  matrix<double> A(3, 3, FORTRAN_LAYOUT), B(3, 3, FORTRAN_LAYOUT);

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      A(i, j) = i + 2 * j + 1;
      B(i, j) = i - j;
    }

  EXPECT_EQ(max_element(A), 7);
  EXPECT_EQ(sum(A), 36);
  EXPECT_EQ(min_element(B), -2);
  EXPECT_EQ(sum(B), 0);
  EXPECT_EQ(make_matrix(A + 10 * B), (matrix<double>{{1, -7, -15}, {12, 4, -4}, {23, 15, 7}}));
  EXPECT_EQ(max_element(A + 10 * B), 23);
}

// ==================== ANY ALL ==========================================

TEST(NDA, any_all) {
  auto nan = std::numeric_limits<double>::quiet_NaN();

  array<double, 2> A(2, 3);
  A() = 98;

  EXPECT_FALSE(any(isnan(A)));

  A() = nan;
  EXPECT_TRUE(all(isnan(A)));

  A()     = 0;
  A(0, 0) = nan;

  EXPECT_FALSE(all(isnan(A)));
  EXPECT_TRUE(any(isnan(A)));
}

// -----------------------------------------------------

TEST(NDA, any_all_c) {
  auto nan = std::numeric_limits<double>::quiet_NaN();

  array<std::complex<double>, 2> A(2, 3);
  A() = 98;

  EXPECT_FALSE(any(isnan(A)));

  A() = nan;
  EXPECT_TRUE(all(isnan(A)));

  A()     = 0;
  A(0, 0) = nan;

  EXPECT_FALSE(all(isnan(A)));
  EXPECT_TRUE(any(isnan(A)));
}
MAKE_MAIN
