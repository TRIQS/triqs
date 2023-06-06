#include <triqs/stat/accumulator.hpp>
#include <iostream>

using namespace triqs::stat;

int main() {
  accumulator<double> my_acc(0.0, 0, 100, 1);
  my_acc << 0.0 << 1.0 << 2.0 << 3.0 << 4.0;

  std::vector<double> output = my_acc.linear_bins();

  // Print data of linear bins
  for (auto &x : output) { std::cout << x << std::endl; }
}
