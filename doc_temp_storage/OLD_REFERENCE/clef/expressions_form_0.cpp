#include <triqs/clef.hpp>
#include <vector>
using namespace triqs::clef;
int main() {
  placeholder<0> i_;
  placeholder<1> x_;
  placeholder<2> y_;
  std::vector<int> V;

  // arithmetic
  auto e = x_ + 2 * y_;

  // simple math function
  auto e1 = cos(2 * x_ + 1);
  auto e2 = abs(2 * x_ - 1);

  // making V lazy
  auto e0 = make_expr(V)[i_];
}
