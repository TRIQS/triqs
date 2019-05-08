#include <itertools/itertools.hpp>
#include <triqs/gfs.hpp>
#include <triqs/statistics/accumulator.hpp>
#include <triqs/statistics/jackknife.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>

using namespace triqs::stat;
using namespace triqs::gfs;
using namespace triqs;

mpi::communicator world;

// *****************************************************************************
// Linear Binning:

TEST(Stat, Accumulator_LinearBins_InitTest) {
  {
    triqs::stat::details::lin_binning<float> linbins{0.0, 2, -1, true};
    EXPECT_FLOAT_EQ(linbins.acc, 0.0);

    EXPECT_NE(typeid(linbins.acc), typeid(static_cast<double>(0.0)));
    EXPECT_EQ(typeid(linbins.acc), typeid(static_cast<float>(0.0)));
  }
  {
    triqs::stat::details::lin_binning<double> linbins{1.0, 2, -1, false};
    EXPECT_DOUBLE_EQ(linbins.acc, 1.0);
  }

    typedef triqs::arrays::array<double, 2> double_array_2d;
    auto a = double_array_2d{{1., 2., 3., 4.}, {1., 2., 3., 4.}};
  {
    triqs::stat::details::lin_binning<double_array_2d> linbins{a, 1, -1, false};
    EXPECT_EQ(linbins.acc.shape(), a.shape());
    auto it1 = linbins.acc.begin();
    auto it2 = linbins.acc.begin();
    for (; it1; ++it1, ++it2) {
      EXPECT_DOUBLE_EQ(*it1, *it2);
    }
  }
  {
    triqs::stat::details::lin_binning<double_array_2d> linbins{a, 1, -1, true};
    EXPECT_EQ(linbins.acc.shape(), a.shape());
    for (auto it = linbins.acc.begin(); it; ++it) {
      EXPECT_DOUBLE_EQ(*it, 0.0);
    }
  }  
}

TEST(Stat, Accumulator_LinearBins_AdvanceAndCompressManually){
  triqs::stat::details::lin_binning<double> linbins{0.0, 1, -1, true};
  int data_limit = 100;
  for(int k = 0; k < data_limit; k++){
    linbins.acc += k + 1.0;
    linbins.advance();
  }
  EXPECT_EQ(linbins.bins.size(), data_limit);
  EXPECT_EQ(linbins.bin_size, 1);
  EXPECT_DOUBLE_EQ(linbins.acc, 0.0);
  EXPECT_EQ(linbins.count, 0);
  for(int k = 0; k < data_limit; k++){
    EXPECT_DOUBLE_EQ(linbins.bins[k], k + 1.0);
  }

  linbins.compress();

  EXPECT_EQ(linbins.bins.size(), data_limit / 2);
  EXPECT_EQ(linbins.bin_size, 2);
  EXPECT_DOUBLE_EQ(linbins.acc, 0.0);
  EXPECT_EQ(linbins.count, 0);
  for(int k = 0; k < data_limit / 2; k += 1){
    EXPECT_DOUBLE_EQ(linbins.bins[k], 2 * k + 1.5);
  } 
}

TEST(Stat, Accumulator_LinearBins_AdvanceWithCompression) {
  triqs::stat::details::lin_binning<double> linbins{0, 1, 2, true};
  EXPECT_EQ(linbins.bin_size, 1);
  std::vector<int> data = {0,1,2};
  for (auto x : data){
    linbins.acc += x;
    linbins.advance();
  }

  EXPECT_EQ(linbins.bins.size(), 1);
  EXPECT_DOUBLE_EQ(linbins.bins[0], 0.5);
  EXPECT_DOUBLE_EQ(linbins.acc, 2.0);
  EXPECT_EQ(linbins.bin_size, 2);

  linbins.acc += 3.0;
  linbins.advance();

  EXPECT_EQ(linbins.bins.size(), 2);
  EXPECT_DOUBLE_EQ(linbins.bins[0], 0.5);
  EXPECT_DOUBLE_EQ(linbins.bins[1], 2.5);
  EXPECT_DOUBLE_EQ(linbins.acc, 0.0);
  EXPECT_EQ(linbins.bin_size, 2);

  linbins.acc += 4.0;
  linbins.advance();
  linbins.acc += 5.0;
  linbins.advance();

  EXPECT_EQ(linbins.bins.size(), 1);
  EXPECT_DOUBLE_EQ(linbins.bins[0], 1.5);
  EXPECT_DOUBLE_EQ(linbins.acc, 9.0);
  EXPECT_EQ(linbins.bin_size, 4);

}

// auto_corr_variances should give empty vector if no log binning
TEST(Stat, Accumulator_LinBinOnly) {
  int n_log_bins = 0, lin_bin_size = 1, n_lin_bins_max = -1;
  accumulator<double> my_acc{0.0, n_log_bins, n_lin_bins_max, lin_bin_size};

  std::vector<double> data = {0.0, 1.0, 2.0, 3.0};
  for(auto x : data){my_acc << x;}

  EXPECT_EQ(my_acc.n_log_bins(), 0);
  EXPECT_EQ(my_acc.auto_corr_variances(world), std::vector<double>());

}

// *****************************************************************************
// Quasi-RNG Test: Compare to Analytic Results

// Helper class: generates uniform sequence of based on lower Wythoff sequence.
// Calculates in double precision and then casts to floats.
class GoldenRatioQuasiRNGFloat {
public:
  double golden_ratio_m1; // golden ratio minus 1  (Equivalent to golden ratio)
  float get_nth_term(int n){
    // Put in Warning when needed (n too large)
    double output_temp = golden_ratio_m1 * n;
    output_temp -= std::floor(output_temp);
    return static_cast<float>(output_temp);
  }
  GoldenRatioQuasiRNGFloat() {
    golden_ratio_m1 = 2.0 / ( 1.0 + std::sqrt(5) );
  }
};

TEST(Stat, Accumulator_GoldenRatioTest) {
  // Sum up to: Fib[10], Fib[15], Fib[20], Fib[25], Fib[30]
  std::vector<int> nr_samples = {55, 610, 6765, 75025, 832040};
  // From analytic results, evaluated in high percision
  std::vector<float> average_analytic_trunc = {
    0.50495168499705574972843136223786729616865703456136f,
    0.49954749751910563469799921017785479962002820115236f,
    0.50004085146337716717417122805944330471652438858682f,
    0.49999631602503416750963631859590564342443999426869f,
    0.50000033218707454549745407880501207827858000631961f
  };
  std::vector<float> stderr_analytic_f = {
    0.039265840412741919349187939402249012089938885571181f,
    0.011697652089423577816521529549438794957302367097532f,
    0.0035100036188629244571590432844256025990753183026236f,
    0.0010539239387211801847383842584035403509305745267760f,
    0.00031647363494910673051693859572897361990178111789649f
  };

  int n_log_bins = 1, bin_size = 1, n_lin_bins = -1;

  for(int idx = 0; idx < nr_samples.size(); idx++){
    accumulator<float>  my_acc_f{0.0, n_log_bins, n_lin_bins, bin_size};
    accumulator<double> my_acc_d{0.0, n_log_bins, n_lin_bins, bin_size};

    GoldenRatioQuasiRNGFloat generator;

    for(int k = 1; k <= nr_samples[idx]; k++){
      float temp = generator.get_nth_term(k);
      my_acc_f << temp;
      my_acc_d << temp;
    }
    auto [ave_f, stderr_f, ave_f_J, ave_f_real] = 
                              jackknife( [](auto &&x) { return x; }, my_acc_f);

    EXPECT_FLOAT_EQ(ave_f_real, average_analytic_trunc[idx]);
    EXPECT_NEAR(stderr_f / stderr_analytic_f[idx], 1.0, 1E-2);
    EXPECT_NEAR(my_acc_f.auto_corr_variances(world)[0] / stderr_analytic_f[idx],
                1.0, 1E-2);

    auto [ave_d, stderr_d] = mean_and_stderr(my_acc_d);

    EXPECT_EQ(static_cast<float>(ave_d), average_analytic_trunc[idx]);
    EXPECT_EQ(static_cast<float>(stderr_d), stderr_analytic_f[idx]);
    EXPECT_EQ(static_cast<float>(my_acc_d.auto_corr_variances(world)[0]), 
              stderr_analytic_f[idx]);
  }
}

MAKE_MAIN;
