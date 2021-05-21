#include <nda/clef.hpp>
#include <triqs/stat.hpp>
using namespace triqs::stat;
int main() {
  observable<double> X;
  X << 1.0;
  X << -1.0;
  X << .5;
  X << .0;
  std::cout << average_and_error(X) << std::endl;
  std::cout << average_and_error(X * X) << std::endl;
  return 0;
}
