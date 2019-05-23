#include <itertools/itertools.hpp>
#include <triqs/arrays.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>

using namespace triqs::stat;
using triqs::arrays::conj_r;

// ****************************************************************************
// Linear Binning:

using linbin_d = triqs::stat::details::lin_binning<double>;

TEST(Stat, Details_LinearBins_InitTest_Float) {
  triqs::stat::details::lin_binning<float> linbins{0.0, -1, 2};
  EXPECT_EQ(linbins.max_n_bins, -1);
  EXPECT_EQ(linbins.last_bin_count, 0);
  EXPECT_EQ(linbins.bin_capacity, 2);
  EXPECT_EQ(linbins.n_bins(), 1);
  EXPECT_EQ(linbins.bins.at(0), 0.0);
  static_assert(std::is_same_v<std::remove_reference_t<decltype(linbins.bins.at(0))>, float>, "EE");
}

TEST(Stat, Details_LinearBins_InitTest_Double) {
  linbin_d linbins{0.0, -1, 2};
  EXPECT_EQ(linbins.n_bins(), 1);
  EXPECT_EQ(linbins.bins.at(0), 0.0);
}

TEST(Stat, Details_LinearBins_InitTestArray) {
  typedef triqs::arrays::array<double, 2> double_array_2d;
  auto a = double_array_2d{{1., 2., 3., 4.}, {1., 2., 3., 4.}};
  // With Zeroing
  triqs::stat::details::lin_binning<double_array_2d> linbins{a, 1, -1};
  EXPECT_EQ(linbins.bins.at(0).shape(), a.shape());
  for (auto const &data : linbins.bins.at(0)) { EXPECT_EQ(data, 0.0); }
  
}

TEST(Stat, Details_LinearBins_InitTestArrayComplex) {
  typedef triqs::arrays::array<std::complex<double>, 2> cdouble_array_2d;

  auto a = cdouble_array_2d{{0., 0., 0.}, {0., 0., 0.}};
  triqs::stat::details::lin_binning<cdouble_array_2d> bins{a, -1, 1};
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.bins.at(0))>, cdouble_array_2d>, "EE");

  EXPECT_EQ(bins.n_bins(), 1);
  EXPECT_ARRAY_EQ(bins.bins.at(0), a);
}


// ********

TEST(Stat, Details_LinearBins_AdvanceOpenEnded_NoBinning) {
  linbin_d linbins{0.0, -1, 1};
  int data_limit = 100;
  for (int k = 0; k < data_limit; k++) { linbins << k + 1.0; }
  EXPECT_EQ(linbins.n_bins(), data_limit);
  EXPECT_EQ(linbins.bins.size(), data_limit);
  EXPECT_EQ(linbins.bin_capacity, 1);
  EXPECT_EQ(linbins.last_bin_count, 1);
  for (int k = 0; k < linbins.n_bins(); k++) { EXPECT_EQ(linbins.bins.at(k), k + 1.0); }
}

TEST(Stat, Details_LinearBins_AdvanceOpenEnded_WithBinning) {
  linbin_d linbins{0.0, -1, 3};
  int data_limit = 101;
  for (int k = 0; k < data_limit; k++) { linbins << k + 1.0; }
  EXPECT_EQ(linbins.n_bins(), 34);
  EXPECT_EQ(linbins.bin_capacity, 3);
  EXPECT_EQ(linbins.last_bin_count, 2);
  for (int k = 0; k < 33; k++) { EXPECT_DOUBLE_EQ(linbins.bins.at(k), 3 * k + 1.0 + 1.0); }
  EXPECT_EQ(linbins.bins.at(33), (99. + 100.) / 2 + 1.0);
}

// ********

TEST(Stat, Details_LinearBins_ManualCompression_FullBinLvl2) {
  linbin_d linbins{0.0, -1, 1};
  int data_limit = 100;
  for (int k = 0; k < data_limit; k++) { linbins << k + 1.0; }
  EXPECT_EQ(linbins.n_bins(), data_limit);

  linbins.compress(2);
  EXPECT_EQ(linbins.n_bins(), data_limit / 2);
  EXPECT_EQ(linbins.bin_capacity, 2);
  EXPECT_EQ(linbins.last_bin_count, 2);
  for (int k = 0; k < data_limit / 2; k += 1) { EXPECT_EQ(linbins.bins.at(k), 2 * k + 1.5); }

  linbins << 3.14;
  EXPECT_EQ(linbins.n_bins(), data_limit / 2 + 1);
  EXPECT_EQ(linbins.bin_capacity, 2);
  EXPECT_EQ(linbins.last_bin_count, 1);
}

TEST(Stat, Details_LinearBins_ManualCompression_FullBinLvl3) {
  {
    linbin_d linbinsA{0.0, -1, 1};
    for (int k = 0; k < 101; k++) { linbinsA << k + 1.0; }
    EXPECT_EQ(linbinsA.n_bins(), 101);
    EXPECT_EQ(linbinsA.last_bin_count, 1);
    EXPECT_EQ(linbinsA.bin_capacity, 1);

    linbinsA.compress(3);
    EXPECT_EQ(linbinsA.n_bins(), 34);
    EXPECT_EQ(linbinsA.bin_capacity, 3);
    EXPECT_EQ(linbinsA.last_bin_count, 2);
    for (int k = 0; k < 33; k++) { EXPECT_DOUBLE_EQ(linbinsA.bins.at(k), 3 * k + 1.0 + 1.0); }
    EXPECT_EQ(linbinsA.bins.at(33), (99. + 100.) / 2 + 1.0);
  }
  {
    linbin_d linbinsB{0.0, -1, 2};
    for (int k = 0; k < 100; k++) { linbinsB << k + 1.0; }
    EXPECT_EQ(linbinsB.n_bins(), 50);
    EXPECT_EQ(linbinsB.last_bin_count, 2);
    EXPECT_EQ(linbinsB.bin_capacity, 2);

    linbinsB.compress(3);
    EXPECT_EQ(linbinsB.n_bins(), 17);
    EXPECT_EQ(linbinsB.bin_capacity, 6);
    EXPECT_EQ(linbinsB.last_bin_count, 4);
    for (int k = 0; k < 16; k++) { EXPECT_EQ(linbinsB.bins.at(k), 6 * k + 5.0 / 2.0 + 1.0); } // n * k + n (n - 1) / (2 n) + n / n
    EXPECT_EQ(linbinsB.bins.at(16), (96. + 97. + 98. + 99.) / 4 + 1.0);
  }
}

TEST(Stat, Details_LinearBins_ManualCompression_PartialFullEndLvl2) {
  linbin_d linbinsB{0.0, -1, 2};
  for (int k = 0; k < 99; k++) { linbinsB << k + 1.0; }
  EXPECT_EQ(linbinsB.n_bins(), 50);
  EXPECT_EQ(linbinsB.last_bin_count, 1);
  EXPECT_EQ(linbinsB.bin_capacity, 2);

  linbinsB.compress(3);
  EXPECT_EQ(linbinsB.n_bins(), 17);
  EXPECT_EQ(linbinsB.bin_capacity, 6);
  EXPECT_EQ(linbinsB.last_bin_count, 3);
  for (int k = 0; k < 16; k++) { EXPECT_EQ(linbinsB.bins.at(k), 6 * k + 5.0 / 2.0 + 1.0); } // n * k + n (n - 1) / (2 n) + n / n
  EXPECT_EQ(linbinsB.bins.at(16), (96. + 97. + 98.) / 3 + 1.0);

  linbinsB.compress(5);
  EXPECT_EQ(linbinsB.n_bins(), 4);
  EXPECT_EQ(linbinsB.bin_capacity, 30);
  EXPECT_EQ(linbinsB.last_bin_count, 9);
  for (int k = 0; k < 3; k++) { EXPECT_EQ(linbinsB.bins.at(k), 30 * k + 29. / 2.0 + 1.0); } // n * k + n (n - 1) / (2 n) + n / n
  EXPECT_EQ(linbinsB.bins.at(3), 90. + 4. + 1.0);

  linbinsB.compress(3);
  EXPECT_EQ(linbinsB.n_bins(), 2);
  EXPECT_EQ(linbinsB.bin_capacity, 90);
  EXPECT_EQ(linbinsB.last_bin_count, 9);
  EXPECT_EQ(linbinsB.bins.at(0), 89. / 2.0 + 1.0); // n * k + n (n - 1) / (2 n) + n / n
  EXPECT_EQ(linbinsB.bins.at(1), 90. + 4. + 1.0);
}
// ********

TEST(Stat, Details_LinearBins_AdvanceAndCompress_EvenNrBins) {
  linbin_d linbins{0, 2, 1};
  EXPECT_EQ(linbins.bin_capacity, 1);

  std::vector<int> data = {0, 1, 2};
  for (auto x : data) { linbins << x; }
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.last_bin_count, 1);
  EXPECT_EQ(linbins.bins.at(0), 0.5);
  EXPECT_EQ(linbins.bins.at(1), 2.0);
  EXPECT_EQ(linbins.bin_capacity, 2);

  linbins << 3.0;
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.last_bin_count, 2);
  EXPECT_EQ(linbins.bins.at(0), 0.5);
  EXPECT_EQ(linbins.bins.at(1), 2.5);
  EXPECT_EQ(linbins.bin_capacity, 2);

  linbins << 4.0;
  linbins << 5.0;
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.bins.at(0), 1.5);
  EXPECT_EQ(linbins.bins.at(1), 4.5);
  EXPECT_EQ(linbins.last_bin_count, 2);
  EXPECT_EQ(linbins.bin_capacity, 4);
}

TEST(Stat, Details_LinearBins_AdvanceAndCompress_OddNrBins) {
  linbin_d linbins{0, 3, 1};
  EXPECT_EQ(linbins.bin_capacity, 1);

  std::vector<int> data = {0, 1, 2, 3};
  for (auto x : data) { linbins << x; }
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.bin_capacity, 2);
  EXPECT_EQ(linbins.last_bin_count, 2);
  EXPECT_EQ(linbins.bins.at(0), 0.5);
  EXPECT_EQ(linbins.bins.at(1), 2.5);

  linbins << 3.0;
  EXPECT_EQ(linbins.n_bins(), 3);
  EXPECT_EQ(linbins.bin_capacity, 2);
  EXPECT_EQ(linbins.last_bin_count, 1);
  EXPECT_EQ(linbins.bins.at(0), 0.5);
  EXPECT_EQ(linbins.bins.at(1), 2.5);
  EXPECT_EQ(linbins.bins.at(2), 3.0);

  linbins << 4.0;
  linbins << 5.0;
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.bin_capacity, 4);
  EXPECT_EQ(linbins.bins.at(0), 1.5);
  EXPECT_EQ(linbins.bins.at(1), 4.0);
  EXPECT_EQ(linbins.last_bin_count, 3);
}
// *****************************************************************************
// Logbin Tests

using logbin_d = triqs::stat::details::log_binning<double>;

TEST(Stat, Details_LogBins_InitTest_Float) {
  triqs::stat::details::log_binning<float> bins{0.0, -1};
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Mk.at(0))>, float>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Qk.at(0))>, float>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.acc.at(0))>, float>, "EE");
  EXPECT_EQ(bins.max_n_bins, -1);
  EXPECT_EQ(bins.n_bins(), 1);
  EXPECT_EQ(bins.Mk.at(0), 0.0);
  EXPECT_EQ(bins.Qk.at(0), 0.0);
  EXPECT_EQ(bins.acc.at(0), 0.0);
  EXPECT_EQ(bins.acc_count.at(0), 0);
}

TEST(Stat, Details_LogBins_InitTest_FloatComplex) {
  triqs::stat::details::log_binning<std::complex<float>> bins{0.0, -1};
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Mk.at(0))>, std::complex<float>>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Qk.at(0))>, float>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.acc.at(0))>, std::complex<float>>, "EE");
  EXPECT_EQ(bins.max_n_bins, -1);
  EXPECT_EQ(bins.n_bins(), 1);
  EXPECT_EQ(bins.Mk.at(0), std::complex<float>(0.0, 0.0));
  EXPECT_EQ(bins.Qk.at(0), 0.0);
  EXPECT_EQ(bins.acc.at(0), std::complex<float>(0.0, 0.0));
  EXPECT_EQ(bins.acc_count.at(0), 0);
}

TEST(Stat, Details_LogBins_InitTestArray) {
  typedef triqs::arrays::array<std::complex<double>, 2> cdouble_array_2d;
  auto a = cdouble_array_2d{{0., 0., 0.}, {0., 0., 0.}};
  triqs::stat::details::log_binning<cdouble_array_2d> bins{a, -1};
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Mk.at(0))>, cdouble_array_2d>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Qk.at(0))>, triqs::arrays::array<double, 2>>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.acc.at(0))>, cdouble_array_2d>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.acc_count.at(0))>, int>, "EE");
  EXPECT_EQ(bins.n_bins(), 1);
  EXPECT_ARRAY_EQ(bins.Mk.at(0), a);
  EXPECT_ARRAY_EQ(bins.Qk.at(0), triqs::arrays::real(a));
  EXPECT_ARRAY_EQ(bins.acc.at(0), a);
}

TEST(Stat, Details_LogBins_AddData) {
  // TODO
}



// *****************************************************************************

TEST(Stat, Accumulator_LinBinOnly) {
  int n_log_bins_max = 0, lin_bin_capacity = 1, n_lin_bins_max = -1;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  std::vector<double> data = {0.0, 1.0, 2.0, 3.0};
  for (auto x : data) { my_acc << x; }
  // Nothing in the log bins
  EXPECT_EQ(my_acc.n_log_bins(), 0);
  EXPECT_EQ(my_acc.log_bin_errors(), std::vector<double>());
  // Lin Bins Filled
  EXPECT_EQ(my_acc.n_lin_bins(), 4);
  EXPECT_EQ(my_acc.n_lin_bins_max(), -1);
  for (auto [x_o, x_i] : itertools::zip(my_acc.linear_bins(), data)) { EXPECT_EQ(x_o, x_i); }
}

TEST(Stat, Accumulator_LogBinOnly) {
  int n_log_bins_max = -1, lin_bin_capacity = 1, n_lin_bins_max = 0;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  std::vector<double> data = {0.0, 1.0, 2.0, 3.0};
  for (auto x : data) { my_acc << x; }
  // Nothing in the lin bins
  EXPECT_EQ(my_acc.n_lin_bins(), 0);
  EXPECT_EQ(my_acc.n_lin_bins_max(), 0);
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>());
  // Log Bins Filled
  EXPECT_EQ(my_acc.n_log_bins(), 4);
}

TEST(Stat, Accumulator_Off) {
  int n_log_bins_max = 0, lin_bin_capacity = 1, n_lin_bins_max = 0;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  std::vector<double> data = {0.0, 1.0, 2.0, 3.0};
  for (auto x : data) { my_acc << x; }
  EXPECT_EQ(my_acc.n_lin_bins(), 0);
  EXPECT_EQ(my_acc.n_lin_bins_max(), 0);
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>());
  EXPECT_EQ(my_acc.n_log_bins(), 0);
  EXPECT_EQ(my_acc.log_bin_errors(), std::vector<double>());
}

TEST(Stat, Accumulator_Chaining) {
  int n_log_bins_max = -1, lin_bin_capacity = 1, n_lin_bins_max = -1;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  my_acc << 0.0 << 1.0 << 2.0 << 3.0;
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>({0.0, 1.0, 2.0, 3.0}));
  EXPECT_EQ(my_acc.n_log_bins(), 3);
}



// *****************************************************************************
// tau_estimate_from_errors
// *****************************************************************************

MAKE_MAIN;
