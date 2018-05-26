#include <triqs/det_manip/det_manip.hpp>

struct fun {
  typedef double result_type;
  typedef double argument_type;
  double operator()(double x, double y) const { return (exp(x) - y * y); }
};

int main() {
  triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1, 2, 2.5}, std::vector<double>{3, 4, 9});
  std::cout << std::endl << "After construction, D.matrix()=" << D.matrix() << std::endl << std::endl;
  double x0 = 2.1, y0 = 7, x1 = 3.5, y1 = 5;
  int i0 = 2, i1 = 1, j0 = 0, j1 = 3; // number of the added lines and columns
  std::cout << "We want to add a line and a column for i0=" << i0 << ", j0=" << j0 << ", i1=" << i1 << ", j1=" << j1 << ", x0=" << x0 << ", y0=" << y0
            << ", x1=" << x1 << ", y1=" << y1 << ")." << std::endl;
  // (try of) insertion of 2 lines and 2 columns in the matrix
  double detratio = D.try_insert2(i0, i1, j0, j1, x0, x1, y0, y1); // the ratio between new and old determinants
  // while the operation is not complete, the matrix stays unchanged
  std::cout << "After try_insert2, D.matrix()=" << D.matrix() << std::endl;
  // here we validate the insertion: the (inverse) matrix and determinant are updated
  D.complete_operation();
  std::cout << "After complete_operation, D.matrix()=" << D.matrix() << std::endl << std::endl;
}
