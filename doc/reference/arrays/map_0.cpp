#include <triqs/arrays.hpp>
using namespace triqs;
int main() {
  // declare and init a matrix
  clef::placeholder<0> i_;
  clef::placeholder<1> j_;
  arrays::matrix<int> A(2, 2);
  A(i_, j_) << i_ + j_;

  // the mapped function
  auto F = arrays::map([](int i) { return i * 2.5; });

  std::cout << "A = " << A << std::endl;
  std::cout << "F(A) = " << F(A) << std::endl; // oops no computation done
  std::cout << "F(A) = " << make_matrix(F(A)) << std::endl;
  std::cout << "3*F(2*A) = " << make_matrix(3 * F(2 * A)) << std::endl;
}
