#include <triqs/det_manip/det_manip.hpp>

struct fun {

 typedef double result_type;
 typedef double argument_type;

 // gives the coefficients of the matrix (function F of the documentation)
 double operator()(double x, double y) const { return (x - y); }
};

int main() {

 fun f;
 std::vector<double> initial_x{1, 2}, initial_y{3, 4};

 // creation of a class det_manip with a 2 by 2 matrix
 triqs::det_manip::det_manip<fun> D(f, initial_x, initial_y);

 // the initial matrix:
 std::cout << std::endl << "After construction: D.matrix()=" << D.matrix() << std::endl << std::endl;
}

