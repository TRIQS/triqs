#include <triqs/arrays.hpp>
#include <triqs/arrays/algorithms.hpp>
using triqs::arrays::matrix;
using triqs::clef::placeholder;
int main() {
 // declare and init a matrix
 placeholder<0> i_;
 placeholder<1> j_;
 matrix<int> A(2, 2);
 A(i_, j_) << i_ - j_;

 std::cout << "A = " << A << std::endl;
 std::cout << "abs(A)= " << matrix<double>(abs(A)) << std::endl;
 std::cout << "sum(A) = " << sum(A) << std::endl;
 std::cout << "max_element(A) = " << max_element(A) << std::endl;
 std::cout << "min_element(A) = " << min_element(A) << std::endl;
 std::cout << "min_element(abs(A)) = " << min_element(abs(A)) << std::endl;
}
