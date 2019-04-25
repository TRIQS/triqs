#include <triqs/statistics.hpp>
using namespace triqs::statistics;

int main() {

  // Histogram with 21 bins over [0;10] range
  histogram hist{0, 10, 21};

  // General information about histogram
  std::cout << "Number of bins = " << hist.size() << std::endl;
  auto limits = hist.limits();
  std::cout << "Histogram range [" << limits.first << ";" << limits.second << "]" << std::endl;

  // Accumulate some value
  for (double x : {-10.0, -0.05, 1.1, 2.0, 2.2, 2.9, 3.4, 5.0, 9.0, 10.0, 10.5, 12.1, 32.2}) hist << x;

  // Print accumulated histogram
  std::cout << "Histogram:\n" << hist << std::endl;

  // Accumulated and lost samples
  std::cout << "Accumulated data points: " << hist.n_data_pts() << std::endl;
  std::cout << "Lost data points: " << hist.n_lost_pts() << std::endl;

  // Direct access to histogram data
  std::cout << "Histogram data: " << hist.data() << std::endl;

  // Make normalized histogram (PDF)
  std::cout << "PDF:\n" << pdf(hist) << std::endl;

  // Make integrated and normalized histogram (CDF)
  std::cout << "CDF:\n" << cdf(hist) << std::endl;

  return 0;
}
