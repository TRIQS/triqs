#include <triqs/stat/accumulator.hpp>
#include <iostream>

using namespace triqs::stat;

int main() {
  // Accumulating a simple scalar
  // Defines log bin with maxmimum of 4
  accumulator<double> my_acc(0.0, 4, 0, 1);

  // Always at least one bin for an active accumulatotr
  std::cout << "A. Number of Bins: " << my_acc.n_log_bins() << std::endl;
  
  my_acc << 0.0;
  my_acc << 1.0;
  my_acc << 2.0;
  my_acc << 3.0;
  my_acc << 4.0;

  // Now there are three bins of binning capacity 2^0, 2^1, 2^2
  std::cout << "B. Number of Bins: " << my_acc.n_log_bins() << std::endl;
  
  my_acc << 5.0;
  my_acc << 6.0;
  my_acc << 7.0;
  my_acc << 8.0;

  // Another bin was created since there is a full 2^3 number of samples
  std::cout << "C. Number of Bins: " << my_acc.n_log_bins() << std::endl;

  // Further adding data will continue to accumulate with the current bin sizes, 
  // but no new bins will be added as we have reached my_acc.n_log_bins_max() == 4.

  // ******************

  // accumulator with log binning turned off
  accumulator<double> my_acc_no_log(0.0, 0, 1, 1);

  std::cout << "D. Number of Bins: " << my_acc_no_log.n_log_bins() << std::endl;
  
}