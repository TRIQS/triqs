#include <triqs/gfs.hpp>
#include <iostream>
using namespace triqs::gfs;

int main() {
  int N1 = 1, N2 = 1;

  // Create a tail with moments -2 .. 8 (intialized to 0) and a given shape
  triqs::gfs::tail t(make_shape(N1, N2));

  t.reset(6); // Set moments >= 6 to unknown (NaN)
  std::cout << t(0) << std::endl;
  t(2) = .5; // Set the 2nd moment
  std::cout << t << std::endl;
}
