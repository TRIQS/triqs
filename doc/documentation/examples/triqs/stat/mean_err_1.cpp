#include <triqs/stat/mean_error.hpp>

using namespace triqs::stat;

int main() {
  std::vector<double> data{1., 2., 3., 4.};
  auto [ave, err] = mean_and_err(data);
  std::cout << "Average: " << ave << "\n"; // = 2.5
  std::cout << "Standard Error: " << err << std::endl; // = sqrt(5. / 12)
}
