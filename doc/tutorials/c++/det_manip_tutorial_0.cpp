#include <triqs/det_manip/det_manip.hpp>

struct fun {

 typedef double result_type;
 typedef double argument_type;

 // gives the coefficients of the matrix (function F of the documentation)
 double operator()(double x, double y) const { return (x - y); }
};

int main() {

 fun f;
 int init_size = 100; // maximum size of the matrix before a resize

 // creation of a class det_manip
 triqs::det_manip::det_manip<fun> D(f, init_size);

 // the initial matrix is empty:
 std::cout << std::endl << "After construction: D.matrix()=" << D.matrix() << std::endl << std::endl;
}

