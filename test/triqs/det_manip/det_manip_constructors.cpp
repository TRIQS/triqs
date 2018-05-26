#include <triqs/det_manip/det_manip.hpp>

struct fun {

  typedef double result_type;
  typedef double argument_type;

  //gives the coefficients of the matrix (function F of the documentation)
  double operator()(double x, double y) const { return (x - y + 1) * (x - y); }
};

int main() {
  try {
    fun f;
    int init_size = 10;
    std::vector<double> initial_x{1, 2, 2.5}, initial_y{3, 4, 9};

    //creation of an empty class det_manip
    triqs::det_manip::det_manip<fun> D1(f, init_size);

    //creation of a class det_manip with a 3 by 3 matrix
    triqs::det_manip::det_manip<fun> D2(f, initial_x, initial_y);

    //the initial matrix:
    std::cout << std::endl << "After construction: D.matrix()=" << D1.matrix() << std::endl;
    std::cout << std::endl << "After construction: D.matrix()=" << D2.matrix() << std::endl;
  } catch (std::exception const &e) { std::cout << "error " << e.what() << std::endl; }
}
