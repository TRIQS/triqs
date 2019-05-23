#include <triqs/stat/accumulator.hpp>
#include <iostream>

using namespace triqs::stat;

int main() {
  // Accumulating a simple scalar
  // Defines linear bins of capacity 1, up to a maximum of 100 bins
  accumulator<double> my_acc(0.0, 0, 100, 1);

  // Add data
  my_acc << 0.0 << 1.0 << 2.0 << 3.0 << 4.0;

  std::vector<double> output = my_acc.linear_bins();

  for (auto &x : output) {
    std::cout << x << std::endl; // 0.0, 1.0, 2.0, 3.0, 4.0
  }
}
