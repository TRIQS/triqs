#include <triqs/arrays.hpp>
using namespace triqs::arrays;
int main() {
 array<double, 2> A(2, 3);
 A.resize(make_shape(5, 5));

 matrix<double> M;
 M.resize(3, 3);

 vector<double> V;
 V.resize(10);
}

