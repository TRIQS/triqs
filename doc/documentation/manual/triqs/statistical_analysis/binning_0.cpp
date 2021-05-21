#include <triqs/stat.hpp>
using namespace triqs::stat;
int main() {
  observable<double> A;
  A << 1.;
  A << 1.5;
  A << .2;
  A << 1.1;
  auto A_b = make_binned_series(A, 2);
  std::cout << A_b << std::endl;
  return 0;
}
