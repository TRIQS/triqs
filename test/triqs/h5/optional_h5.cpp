#include <triqs/test_tools/arrays.hpp>

using namespace triqs;

TEST(H5, Optional) {

  std::optional<int> a(18), b;

  if (a) std::cout << *a << std::endl;
  {
    h5::file file("opt.h5", 'w');
    h5_write(file, "A", a);
  }
  {
    h5::file file("opt.h5", 'r');
    h5_read(file, "A", b);
  }

  EXPECT_TRUE(bool(b));
  EXPECT_EQ(a, b);
}
MAKE_MAIN;
