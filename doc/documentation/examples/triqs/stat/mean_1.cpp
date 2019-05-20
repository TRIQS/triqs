#include <triqs/stat/mean_error.hpp>

using namespace triqs::stat;

int main() {
  std::vector<double> data{1., 2., 3., 4.};
  auto ave = mean(data);
  std::cout << "Average: " << ave << std::endl; // = 2.5
}
