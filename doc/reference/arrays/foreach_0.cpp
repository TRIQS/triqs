#include <triqs/arrays.hpp>
using triqs::arrays::array;
int main() {
 array<long, 2> A(2, 3);
 foreach(A, [&A](size_t i, size_t j) { A(i, j) = i + j; });
 std::cout << " A " << A << std::endl;
}

