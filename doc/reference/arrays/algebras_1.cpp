#include <triqs/arrays.hpp>
using namespace triqs::arrays;
int main() {
 array<long, 2> A(2, 2);
 matrix<long, 2> M(2, 2);
 // M + A; // --> ERROR. Rejected by the compiler.
 M + make_matrix_view(A); //--> OK.
}

