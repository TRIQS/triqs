#include <triqs/arrays.hpp>
#include <triqs/arrays/linalg/eigenelements.hpp>

using triqs::arrays::array;
using triqs::arrays::matrix;
using triqs::arrays::linalg::eigenvalues;
using triqs::clef::placeholder;
int main() {
  placeholder<0> i_;
  placeholder<1> j_;
  matrix<double> A(2, 2);
  A(i_, j_) << i_ + j_;

  auto v = eigenvalues(A);

  std::cout << "v = " << v << std::endl;
}
