#include <nda/nda.hpp>
using nda::array;
using nda::array_view;
using itertools::range;
int main() {
  array<double, 2> A(3, 3);
  A() = 2.5;
  std::cout << A << std::endl;

  array_view<double, 1> B = A(1, range()); // select the first line of the matrix
  std::cout << "B = " << B << std::endl;
  B(0) = 1;

  std::cout << "A = " << A << std::endl;
}
