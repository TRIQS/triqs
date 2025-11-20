#include <nda/nda.hpp>
#include <nda/linalg.hpp>

using nda::array;
using nda::matrix;
using nda::clef::placeholder;
int main() {
  placeholder<0> i_;
  placeholder<1> j_;
  matrix<double> A(2, 2);
  A(i_, j_) << i_ + j_;

  matrix<double> B = nda::linalg::inv(A);
  double C         = nda::linalg::det(A);

  std::cout << "A^(-1) = " << B << std::endl;
  std::cout << "det(A) = " << C << std::endl;
}
