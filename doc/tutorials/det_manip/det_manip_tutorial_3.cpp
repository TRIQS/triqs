#include <triqs/det_manip/det_manip.hpp>

struct fun {
 typedef double result_type;
 typedef double argument_type;
 double operator()(double x, double y) const { return (exp(x) - y * y); }
};

int main() {
 triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1, 2, 2.5}, std::vector<double>{3, 4, 9});
 std::cout << std::endl << "After construction, D.matrix()=" << D.matrix() << std::endl << std::endl;
 double x0 = 2.1, y0 = 7;
 int i = 2, j = 0; // number of the added line and column
 std::cout << "We want to add a line and a column for i=" << i << ", j=" << j << ", x=" << x0 << ", y=" << y0 << "." << std::endl;
 // (try of) insertion of a line and a column at position (3,1) in the matrix
 // with x[i]=x0, y[j]=y0.
 double detratio = D.try_insert(i, j, x0, y0); // the ratio between new and old determinants
 // while the operation is not complete, the matrix stays unchanged
 std::cout << "After try_insert, D.matrix()=" << D.matrix() << std::endl;
 // here we validate the insertion: the (inverse) matrix and determinant are updated
 D.complete_operation();
 std::cout << "After complete_operation, D.matrix()=" << D.matrix() << std::endl << std::endl;
}

