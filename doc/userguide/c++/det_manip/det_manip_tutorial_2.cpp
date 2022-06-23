#include <triqs/det_manip/det_manip.hpp>

struct fun {
  typedef double result_type;
  typedef double argument_type;
  double operator()(double x, double y) const { return (x - y); }
};

int main() {
  fun f;
  int i = 0, j = 1;
  std::vector<double> initial_x{1, 2}, initial_y{3, 4};
  triqs::det_manip::det_manip<fun> D(f, initial_x, initial_y);
  std::cout << std::endl << "D.matrix()=" << D.matrix() << std::endl << std::endl;
  std::cout << "The size of the matrix is " << D.size() << std::endl << std::endl;
  std::cout << "The determinant is " << D.determinant() << std::endl << std::endl;
  std::cout << "The inverse matrix is" << D.inverse_matrix() << std::endl << std::endl;
  std::cout << "The value of the parameters for coefficient (i,j)=(" << i << "," << j << ") is (x,y)=(" << D.get_x(i) << "," << D.get_y(j) << ")"
            << std::endl
            << std::endl;
}
