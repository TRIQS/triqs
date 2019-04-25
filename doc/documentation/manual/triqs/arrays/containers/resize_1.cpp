#include <triqs/arrays.hpp>
using namespace triqs::arrays;
int main() {
  array<double, 2> A(2, 3);
  A()                     = 9;
  array_view<double, 2> V = A();
  A.resize(make_shape(5, 5));
  A() = 0;
  std::cout << V << std::endl;
}
