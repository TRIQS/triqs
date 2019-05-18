#include <itertools/itertools.hpp>
#include <triqs/arrays.hpp>
#include <triqs/gfs.hpp>
#include <triqs/stat/mean_error.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>

using namespace triqs::stat;
using namespace triqs::gfs;
using namespace triqs;

mpi::communicator world;

// *****************************************************************************
// Quasi-RNG Test: Compare to Analytic Results

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
};

TEST(Stat, Accumulator_GoldenRatioTest) {
  // Sum up to: Fib[10], Fib[15], Fib[20], Fib[25], Fib[30]
  std::vector<int> nr_samples = {55, 610, 6765, 75025, 832040};
  // From analytic results, evaluated in high percision
  std::vector<float> average_analytic_trunc = {
     0.50495168499705574972843136223786729616865703456136f, 0.49954749751910563469799921017785479962002820115236f,
     0.50004085146337716717417122805944330471652438858682f, 0.49999631602503416750963631859590564342443999426869f,
     0.50000033218707454549745407880501207827858000631961f};
  std::vector<float> stderr_analytic_f = {
     0.039265840412741919349187939402249012089938885571181f, 0.011697652089423577816521529549438794957302367097532f,
     0.0035100036188629244571590432844256025990753183026236f, 0.0010539239387211801847383842584035403509305745267760f,
     0.00031647363494910673051693859572897361990178111789649f};

  int n_log_bins = 1, lin_bin_size = 1, n_lin_bins = -1;

  for (int idx = 0; idx < nr_samples.size(); idx++) {
    accumulator<float> my_acc_f{0.0, n_log_bins, n_lin_bins, lin_bin_size};
    accumulator<double> my_acc_d{0.0, n_log_bins, n_lin_bins, lin_bin_size};

    GoldenRatioQuasiRNGFloat generator;

    for (int k = 1; k <= nr_samples[idx]; k++) {
      float temp = generator.get_nth_term(k);
      my_acc_f << temp;
      my_acc_d << temp;
    }
    // auto [ave_f, stderr_f, ave_f_J, ave_f_real] = jackknife([](auto &&x) { return x; }, my_acc_f);

    auto [ave_f, stderr_f] = mean_and_err(my_acc_f.linear_bins());

    // EXPECT_FLOAT_EQ();
    EXPECT_NEAR(ave_f / average_analytic_trunc[idx], 1.0, 1E-3);
    EXPECT_NEAR(stderr_f / stderr_analytic_f[idx], 1.0, 1E-3);
    EXPECT_NEAR(my_acc_f.auto_corr_variances()[0] / stderr_analytic_f[idx], 1.0, 1E-2);

    auto [ave_d, stderr_d] = mean_and_err(my_acc_d.linear_bins());

    EXPECT_EQ(static_cast<float>(ave_d), average_analytic_trunc[idx]);
    EXPECT_EQ(static_cast<float>(stderr_d), stderr_analytic_f[idx]);
    EXPECT_EQ(static_cast<float>(my_acc_d.auto_corr_variances()[0]), stderr_analytic_f[idx]);
  }
}

MAKE_MAIN;
