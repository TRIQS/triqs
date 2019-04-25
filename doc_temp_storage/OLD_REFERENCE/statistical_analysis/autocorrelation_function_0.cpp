#include <triqs/statistics.hpp>
#include <triqs/mc_tools/random_generator.hpp>
using namespace triqs::statistics;
int main() {
  observable<double> A;
  triqs::mc_tools::random_generator RND;
  for (int i = 0; i < 20; i++) A << RND(1.0);
  std::cout << make_normalized_autocorrelation(A) << std::endl;
  return 0;
}
