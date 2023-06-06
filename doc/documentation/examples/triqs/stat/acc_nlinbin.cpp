#include <triqs/stat/accumulator.hpp>
#include <iostream>

using namespace triqs::stat;

int main() {
  // Accumulating a simple scalar
  // Defines linear bins of capacity 2, up to a maximum of 100 bins
  accumulator<double> my_acc(0.0, 0, 100, 2);

  // Always at least one bin for an active accumulatotr
  std::cout << "A. Number of Bins: " << my_acc.n_lin_bins() << std::endl;

  // Add data: fill first bin to full capacity
  my_acc << 0.0;
  my_acc << 1.0;

  std::cout << "B. Number of Bins: " << my_acc.n_lin_bins() << std::endl;

  // Fill up 2nd bin to full capacity and 3rd bin halfway
  my_acc << 2.0;
  my_acc << 3.0;
  my_acc << 4.0;

  std::cout << "C. Number of Bins: " << my_acc.n_lin_bins() << std::endl;

  // ******************

  // accumulator with linear binning turned off
  accumulator<double> my_acc_no_lin(0.0, 1, 0, 1);

  std::cout << "D. Number of Bins: " << my_acc_no_lin.n_lin_bins() << std::endl;
}