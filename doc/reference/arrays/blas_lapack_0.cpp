#include <triqs/arrays.hpp>
using triqs::arrays::matrix;
using triqs::clef::placeholder;
int main() {
  // declare an init 2 matrices
  placeholder<0> i_;
  placeholder<1> j_;
  matrix<double> A(2, 2), B(2, 2), C;
  A(i_, j_) << i_ + j_;
  B(i_, j_) << 2 * i_ + j_;
  C = A * B;
  std::cout << " C = " << C << std::endl;
}
