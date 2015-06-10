#include <triqs/det_manip/det_manip.hpp>

struct fun {
 typedef double result_type;
 typedef double argument_type;
 double operator()(double x, double y) const { return (exp(x) - y * y); }
};

int main() {
  double detratio;
  triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1, 2, 2.5}, std::vector<double>{3, 4, 9});
  std::cout << std::endl << "After construction, D.matrix()=" << D.matrix() << std::endl << std::endl;
  int i0 = 2, i1 = 1, j0 = 0, j1 = 1; // number of the removed lines and columns
  std::cout << "We want to add two lines and columns at the end." << std::endl;
  // (try of) removal of a line and a column at position (1,0) in the matrix.
  detratio = D.insert2_at_end(2.3, 5.4, 4.6, 7.5); // the ratio between new and old determinants
  std::cout << "After insert2_at_end, D.matrix()=" << D.matrix() << std::endl << std::endl;
  std::cout << "We want to remove two lines and columns at the end." << std::endl;
  detratio = D.remove2_at_end(); // the ratio between new and old determinants
  std::cout << "After remove2_at_end, D.matrix()=" << D.matrix() << std::endl << std::endl;
  std::cout << "We want to add one line and column at the end." << std::endl;
  detratio = D.insert_at_end(1.1, 3.3); // the ratio between new and old determinants
  std::cout << "After remove_at_end, D.matrix()=" << D.matrix() << std::endl << std::endl;
  std::cout << "We want to remove one line and column at the end." << std::endl;
  detratio = D.remove_at_end(); // the ratio between new and old determinants
  std::cout << "After remove_at_end, D.matrix()=" << D.matrix() << std::endl;
}
