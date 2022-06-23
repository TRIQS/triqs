#include <nda/nda.hpp>

using nda::array;
using nda::matrix;
using nda::vector;
int main() {
  array<double, 2> A(2, 2), B(2, 2), C;

  A() = 3;
  B() = 1;
  C   = A * B;
  std::cout << "A*B = " << C << std::endl;

  matrix<double> D(2, 2), E(2, 2), F;
  E() = 3;
  E() = 1;
  F   = D * E;
  std::cout << "C*D = " << F << std::endl;

  vector<double> u(2), v(2), w;
  u() = 1;
  v() = 2;
  w   = u + v;

  std::cout << "u+v = " << w << std::endl;
}
