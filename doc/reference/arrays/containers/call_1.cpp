#include <triqs/arrays.hpp>
using namespace triqs::arrays;
int main() {
 array<double, 2> A(4, 4);
 for (int i = 0; i < 4; ++i)
  for (int j = 0; j < 4; ++j) A(i, j) = i + 10 * j;
 array_view<double, 2> V = A(range(0, 2), range(0, 2));
 std::cout << "V = " << V << std::endl;
 V = -V;
 std::cout << "A = " << A << std::endl;
}

