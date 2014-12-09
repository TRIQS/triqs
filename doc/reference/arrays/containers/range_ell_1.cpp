#include <triqs/arrays.hpp>
using triqs::arrays::array;
using triqs::arrays::ellipsis;

// a generic function that sum array, array_view or in fact anything
// with the right concept on its first dimension
template <typename ArrayType> array<typename ArrayType::value_type, ArrayType::rank - 1> sum0(ArrayType const& A) {
 array<typename ArrayType::value_type, ArrayType::rank - 1> res = A(0, ellipsis());
 for (size_t u = 1; u < first_dim(A); ++u) res += A(u, ellipsis());
 return res;
}

// test
int main() {
 array<double, 2> A(5, 2);
 A() = 2;
 array<double, 3> B(5, 2, 3);
 B() = 1;
 std::cout << sum0(A) << sum0(B) << std::endl;
}

