#include <triqs/arrays.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>

using triqs::arrays::array;
using triqs::arrays::matrix;
using triqs::clef::placeholder;
int main() {
 placeholder<0> i_;
 placeholder<1> j_;
 matrix<double> A(2, 2);
 A(i_, j_) << i_ + j_;

 matrix<double> B = inverse(A);
 double C = determinant(A);

 std::cout << "A^(-1) = " << B << std::endl;
 std::cout << "det(A) = " << C << std::endl;
}

