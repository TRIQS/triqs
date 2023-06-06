#include <nda/nda.hpp>
using itertools::range;
using nda::array;
using nda::array_view;
int main() {
  array<double, 2> A(3, 3);
  A() = 2.5;
  std::cout << A << std::endl;

  array_view<double, 1> B = A(1, range::all); // select the first line of the matrix
  std::cout << "B = " << B << std::endl;
  B(0) = 1;

  std::cout << "A = " << A << std::endl;
}
