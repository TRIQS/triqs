

#include <triqs/arrays.hpp>
using triqs::arrays::array;
int main() {
 array<double, 1> A(20);
 std::cout << "A = " << A << std::endl; // arrays are not init by default: this is random
 A() = 2;                               //  assign 2 to a complete view of A.
 std::cout << "A = " << A << std::endl;
 A(4) = 5;
 std::cout << "A = " << A << std::endl;
}

