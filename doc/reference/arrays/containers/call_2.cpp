#include <triqs/arrays.hpp>
using triqs::arrays::array;
using triqs::clef::placeholder;

int main() {
  placeholder<0> i_;
  placeholder<1> j_;
  array<double, 2> A(2, 2), B(2, 2);

  A(i_, j_) << i_ + 2 * j_;
  B(i_, j_) << A(j_, i_) / 2;

  std::cout << "A = " << A << std::endl;
  std::cout << "B = " << B << std::endl;
}
