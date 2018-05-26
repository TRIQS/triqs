#include "start.hpp"
TEST(Array, Bug2) {

  array<double, 3> A(10, 2, 2);
  A() = 0;

  A(4, range(), range()) = 1;
  A(5, range(), range()) = 2;

  matrix_view<double> M1 = A(4, range(), range());
  matrix_view<double> M2 = A(5, range(), range());

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{1, 1}, {1, 1}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{2, 2}, {2, 2}});

  M1 = M2;

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{2, 2}, {2, 2}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{2, 2}, {2, 2}});
}
MAKE_MAIN;
