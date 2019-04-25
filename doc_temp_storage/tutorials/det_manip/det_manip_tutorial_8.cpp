#include <triqs/det_manip/det_manip.hpp>

struct fun {
  typedef double result_type;
  typedef double argument_type;
  double operator()(double x, double y) const { return (exp(x) - y * y); }
};

int main() {

  triqs::det_manip::det_manip<fun> D(fun(), std::vector<double>{1, 2, 2.5}, std::vector<double>{3, 4, 9});
  std::cout << std::endl << "After construction, D.matrix()=" << D.matrix() << std::endl << std::endl;
  std::cout << "We want to change a row and a column." << std::endl;
  D.change_one_row_and_one_col(0, 1, 2.1, 1.2);
  std::cout << "After change_one_row_and_one_col(0,1, 2.1, 1.2), D.matrix()=" << D.matrix() << std::endl << std::endl;

  triqs::det_manip::det_manip<fun> D2(fun(), std::vector<double>{2, 2.5}, std::vector<double>{4, 9});
  std::cout << std::endl << "After construction, D2.matrix()=" << D2.matrix() << std::endl << std::endl;
  std::cout << "We want to change a row and a column." << std::endl;
  D2.change_one_row_and_one_col(0, 1, 2.1, 1.2);
  std::cout << "After change_one_row_and_one_col(0,1, 2.1, 1.2), D2.matrix()=" << D2.matrix() << std::endl << std::endl;
}
