#include <triqs/arrays.hpp>
using triqs::arrays::array;
using triqs::arrays::matrix;
using triqs::arrays::vector;
using triqs::arrays::permutation;
int main() {

 // A 3d array of long, C ordering, no option
 array<long, 3> A3(1, 2, 3);

 // A 2d array of double, C ordering, with explicit Bound Checking
 array<double, 2> B(1, 2);

 // a matrix of long
 matrix<long> M(2, 2);

 // a vector of double
 vector<double> V(10);

 // arrays with custom TraversalOrder

 // C-style
 array<long, 3, 0, permutation(2, 1, 0)> A0(2, 3, 4);
 array<long, 3, 0> A0b; // same type but empty

 // Fortran-style
 array<long, 3, TRAVERSAL_ORDER_FORTRAN> A4(2, 3, 4);
 array<long, 3, 0, permutation(0, 1, 2)> A1b; // same type but empty

 // custom :  (i,j,k)  : index j is fastest, then k, then i
 array<long, 3, 0, permutation(1, 0, 2)> A2(2, 3, 4);
}

