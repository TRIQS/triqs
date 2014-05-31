#include <triqs/det_manip/det_manip.hpp>

struct fun {
 typedef double result_type;
 typedef double argument_type;
 double operator()(double x, double y) const { return (exp(x) - y * y); }
};

int main() {
 triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1, 2, 2.5}, std::vector<double>{3, 4, 9});
 std::cout << std::endl << "After construction, D.matrix()=" << D.matrix() << std::endl << std::endl;
 int i = 1, j = 0; // number of the removed line and column
 std::cout << "We want to remove a line and a column for i=" << i << ", j=" << j << "." << std::endl;
 // (try of) removal of a line and a column at position (1,0) in the matrix.
 double detratio = D.try_remove(i, j); // the ratio between new and old determinants
 // while the operation is not complete, the matrix stays unchanged
 std::cout << "After try_remove, D.matrix()=" << D.matrix() << std::endl;
 // here we validate the removal: the (inverse) matrix and determinant are updated
 D.complete_operation();
 std::cout << "After complete_operation, D.matrix()=" << D.matrix() << std::endl << std::endl;
}

