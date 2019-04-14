#include "./nda_test_common.hpp"

// ==============================================================

TEST(NDA, empty_iteration) {
  array<int, 1> arr(0);
  int s = 0;
  for (auto i : arr) s += i;
  EXPECT_EQ(s, 0);
}
MAKE_MAIN
