#include <triqs/arrays.hpp>
#include <triqs/arrays/functional/fold.hpp>
using namespace triqs;

// FIXME change this function for another function that is not implemented?

double frobenius_norm_of_array(arrays::matrix<double> const& a) {
 auto l = [](double r, double x) {
  auto ab = std::abs(x);
  return r + ab * ab;
 };
 return std::sqrt(arrays::fold(l)(a, 0));
}

int main() {
 // declare and init a matrix
 clef::placeholder<0> i_;
 clef::placeholder<1> j_;
 arrays::matrix<double> A(2, 2);
 A(i_, j_) << i_ + j_ / 2.0;

 std::cout << "A = " << A << std::endl;
 std::cout << "||A|| = " << frobenius_norm_of_array(A) << std::endl;
}

