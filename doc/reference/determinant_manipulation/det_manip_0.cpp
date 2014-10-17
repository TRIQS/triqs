#include <triqs/det_manip/det_manip.hpp>

struct fun {

 typedef double result_type;
 typedef double argument_type;

 double operator()(double x, double y) const {
  const double pi = acos(-1.);
  const double beta = 10.0;
  const double epsi = 0.1;
  double tau = x - y;
  bool s = (tau > 0);
  tau = (s ? tau : beta + tau);
  double r = epsi + tau / beta * (1 - 2 * epsi);
  return -2 * (pi / beta) / std::sin(pi * r);
 }
};

int main() {

 fun f;
 triqs::det_manip::det_manip<fun> D(f, 100);

 /// insertions of 3 lines and 3 columns
 double x = 2., y = 9., detratio;
 std::cout << D.size() << std::endl;
 detratio = D.try_insert(0, 0, x, y);
 std::cout << D.size() << std::endl;
 D.complete_operation();
 std::cout << D.size() << std::endl;
 detratio = D.try_insert(0, 1, 2., 3.);
 D.complete_operation();
 detratio = D.try_insert(0, 0, 4., 5.);
 D.complete_operation();

 /// removal of a line (the 3rd) and a column (the 2nd)
 detratio = D.try_remove(2, 1);
 D.complete_operation();
}

