#include <nda/nda.hpp>
using nda::array;
int main() {
  array<double, 1> A(10), B(10);
  A()                = 2;
  B()                = 3;
  array<double, 1> C = A + B;
  std::cout << "C = " << C << std::endl;
}
