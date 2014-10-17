#include <triqs/arrays.hpp>
using triqs::arrays::array;
using triqs::clef::placeholder;
int main() {
 // init
 placeholder<0> i_;
 placeholder<1> j_;
 array<long, 2> A(2, 2), B(2, 2);
 A(i_, j_) << i_ + j_;
 B(i_, j_) << i_ - j_;

 // use expressions
 array<long, 2> C = A + 2 * B;
 array<double, 2> D = 0.5 * A; // Type promotion is automatic
 std::cout << "A= " << A << std::endl;
 std::cout << "B= " << B << std::endl;
 std::cout << "C= " << C << std::endl;
 std::cout << "D= " << D << std::endl;
}

