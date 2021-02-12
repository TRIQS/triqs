#include <triqs/stat/mean_error.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/arrays.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>
#include <vector>

using namespace triqs::stat;
using namespace triqs;
using nda::conj;

// *****************************************************************************
// Quasi-RNG Test: Compare to exact Results
// Author: P. Dumitrescu

// Helper class: generates uniform sequence of based on lower Wythoff sequence.
// Calculates in double precision and then casts to floats.
class GoldenRatioQuasiRNGFloat {
  public:
  double golden_ratio_m1; // golden ratio minus 1  (Equivalent to golden ratio)
  float get_nth_term(int n) {
    // Put in Warning when needed (n too large)
    double output_temp = golden_ratio_m1 * n;
    output_temp -= std::floor(output_temp);
    return static_cast<float>(output_temp);
  }
  GoldenRatioQuasiRNGFloat() { golden_ratio_m1 = 2.0 / (1.0 + std::sqrt(5)); }

  // Exact Results:

  // Sum up to: Fib[10], Fib[15], Fib[20], Fib[25], Fib[30]
  std::vector<int> nr_samples = {55, 610, 6765, 75025, 832040};

  // From exact results, evaluated in high percision
  std::vector<float> ave_exact_f = {0.50495168499705574972843136223786729616865703456136f, 
                                    0.49954749751910563469799921017785479962002820115236f,
                                    0.50004085146337716717417122805944330471652438858682f, 
                                    0.49999631602503416750963631859590564342443999426869f,
                                    0.50000033218707454549745407880501207827858000631961f};

  std::vector<float> err_exact_f = {0.039265840412741919349187939402249012089938885571181f, 
                                    0.011697652089423577816521529549438794957302367097532f,
                                    0.0035100036188629244571590432844256025990753183026236f,
                                    0.0010539239387211801847383842584035403509305745267760f,
                                    0.00031647363494910673051693859572897361990178111789649f};

  // Expected Accuarcy from Welform Summations Currently Implemented:

  float eps = std::numeric_limits<float>::epsilon();
  std::vector<float> tol_err_in_ave_welford{eps, eps, 8 * eps, 15 * eps, 25 * eps};
  std::vector<float> tol_err_in_err_welford{eps, eps, eps, eps, eps};

};

TEST(Stat, Accumulator_GoldenRatioTest) {
  int n_log_bins = 1, lin_bin_size = 1, n_lin_bins = -1;
  GoldenRatioQuasiRNGFloat gen;
  
  for (int idx = 0; idx < gen.nr_samples.size(); idx++) {
    accumulator<float> my_acc_f{0.0, n_log_bins, n_lin_bins, lin_bin_size};
    accumulator<double> my_acc_d{0.0, n_log_bins, n_lin_bins, lin_bin_size};

    for (int n = 1; n <= gen.nr_samples[idx]; n++) {
      float temp = gen.get_nth_term(n);
      my_acc_f << temp;
      my_acc_d << temp;
    }

    // Test log binning
    EXPECT_NEAR(my_acc_f.log_bin_errors()[0] - gen.err_exact_f[idx], 0.0, gen.tol_err_in_ave_welford[idx]);
    EXPECT_EQ(float(my_acc_d.log_bin_errors()[0]), gen.err_exact_f[idx]);

    // Test mean_and_err
    auto [ave_f, err_f] = mean_and_err(my_acc_f.linear_bins());
    EXPECT_NEAR(ave_f - gen.ave_exact_f[idx], 0.0, gen.tol_err_in_ave_welford[idx]);
    EXPECT_NEAR(err_f - gen.err_exact_f[idx], 0.0, gen.tol_err_in_err_welford[idx]);

    auto [ave_d, err_d] = mean_and_err(my_acc_d.linear_bins());
    EXPECT_EQ(float(ave_d), gen.ave_exact_f[idx]);
    EXPECT_EQ(float(err_d), gen.err_exact_f[idx]);
  }
}

MAKE_MAIN;
