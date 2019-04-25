#include <triqs/arrays.hpp>
using namespace triqs::arrays;
int main() {
  array<double, 2> A(2, 3);
  A()     = 0; //  assign 0 to A
  A(0, 0) = 5;
  A(1, 1) = 2 * A(0, 0);
  std::cout << "A = " << A << std::endl;
}
