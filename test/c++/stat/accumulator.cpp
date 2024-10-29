// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

#include <itertools/itertools.hpp>
#include <nda/nda.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>

using namespace triqs::stat;

using nda::conj;

using vec_d = std::vector<double>;
using vec_c = std::vector<std::complex<double>>;
using vec_i = std::vector<int>;

// ****************************************************************************
// Linear Binning:

TEST(Stat, Details_LinBins_InitTest_Scalars) {
  for (auto const &nrBins : std::vector<int>{-1, 1, 2}) {
    lin_binning<double> bins_d{1.0, nrBins, 2};
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(bins_d.bins().at(0))>, double>, "EE");
    EXPECT_EQ(bins_d.max_n_bins(), nrBins);
    EXPECT_EQ(bins_d.last_bin_count(), 0);
    EXPECT_EQ(bins_d.bin_capacity(), (nrBins == 1 ? -1 : 2));
    EXPECT_EQ(bins_d.n_bins(), 1);
    EXPECT_EQ(bins_d.bins().at(0), 0.0);

    lin_binning<std::complex<double>> bins_cd{1.0, nrBins, 4};
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(bins_cd.bins().at(0))>, std::complex<double>>, "EE");
    EXPECT_EQ(bins_cd.max_n_bins(), nrBins);
    EXPECT_EQ(bins_cd.last_bin_count(), 0);
    EXPECT_EQ(bins_cd.bin_capacity(), (nrBins == 1 ? -1 : 4));
    EXPECT_EQ(bins_cd.n_bins(), 1);
    EXPECT_EQ(bins_cd.bins().at(0), 0.0);
  }
}

TEST(Stat, Details_LinBins_InitTestArray) {
  typedef nda::array<double, 2> double_array_2d;
  auto a = double_array_2d{{1., 2., 3., 4.}, {1., 2., 3., 4.}};
  // With Zeroing
  lin_binning<double_array_2d> linbins{a, 1, -1};
  EXPECT_EQ(linbins.bins().at(0).shape(), a.shape());
  for (auto const &data : linbins.bins().at(0)) { EXPECT_EQ(data, 0.0); }
}

TEST(Stat, Details_LinBins_InitTestArrayComplex) {
  typedef nda::array<std::complex<double>, 2> cdouble_array_2d;

  auto a = cdouble_array_2d{{0., 0., 0.}, {0., 0., 0.}};
  lin_binning<cdouble_array_2d> bins{a, -1, 1};
  static_assert(std::is_same_v<std::remove_cvref_t<decltype(bins.bins().at(0))>, cdouble_array_2d>, "EE");

  EXPECT_EQ(bins.n_bins(), 1);
  EXPECT_ARRAY_EQ(bins.bins().at(0), a);
}

// ********

TEST(Stat, Details_LinBins_AdvanceOpenEnded_NoBinning) {
  lin_binning<double> linbins{0.0, -1, 1};
  int data_limit = 100;
  for (int k = 0; k < data_limit; k++) { linbins << k + 1.0; }
  EXPECT_EQ(linbins.n_bins(), data_limit);
  EXPECT_EQ(linbins.bins().size(), data_limit);
  EXPECT_EQ(linbins.bin_capacity(), 1);
  EXPECT_EQ(linbins.last_bin_count(), 1);
  for (int k = 0; k < linbins.n_bins(); k++) { EXPECT_EQ(linbins.bins().at(k), k + 1.0); }
}

TEST(Stat, Details_LinBins_AdvanceOpenEnded_WithBinning) {
  lin_binning<double> linbins{0.0, -1, 3};
  int data_limit = 101;
  for (int k = 0; k < data_limit; k++) { linbins << k + 1.0; }
  EXPECT_EQ(linbins.n_bins(), 34);
  EXPECT_EQ(linbins.bin_capacity(), 3);
  EXPECT_EQ(linbins.last_bin_count(), 2);
  for (int k = 0; k < 33; k++) { EXPECT_DOUBLE_EQ(linbins.bins().at(k), 3 * k + 2.0); }
  EXPECT_EQ(linbins.bins().at(33), (99. + 100.) / 2 + 1.0);
}

// ********

TEST(Stat, Details_LinBins_ManualCompression_FullBinLvl2) {
  lin_binning<double> linbins{0.0, -1, 1};
  int data_limit = 100;
  for (int k = 0; k < data_limit; k++) { linbins << k + 1.0; }
  EXPECT_EQ(linbins.n_bins(), data_limit);

  linbins.compress(2);
  EXPECT_EQ(linbins.n_bins(), data_limit / 2);
  EXPECT_EQ(linbins.bin_capacity(), 2);
  EXPECT_EQ(linbins.last_bin_count(), 2);
  for (int k = 0; k < data_limit / 2; k += 1) { EXPECT_EQ(linbins.bins().at(k), 2 * k + 1.5); }

  linbins << 3.14;
  EXPECT_EQ(linbins.n_bins(), data_limit / 2 + 1);
  EXPECT_EQ(linbins.bin_capacity(), 2);
  EXPECT_EQ(linbins.last_bin_count(), 1);
}

TEST(Stat, Details_LinBins_ManualCompression_FullBinLvl3) {
  {
    lin_binning<double> linbinsA{0.0, -1, 1};
    for (int k = 0; k < 101; k++) { linbinsA << k + 1.0; }
    EXPECT_EQ(linbinsA.n_bins(), 101);
    EXPECT_EQ(linbinsA.last_bin_count(), 1);
    EXPECT_EQ(linbinsA.bin_capacity(), 1);

    linbinsA.compress(3);
    EXPECT_EQ(linbinsA.n_bins(), 34);
    EXPECT_EQ(linbinsA.bin_capacity(), 3);
    EXPECT_EQ(linbinsA.last_bin_count(), 2);
    for (int k = 0; k < 33; k++) { EXPECT_DOUBLE_EQ(linbinsA.bins().at(k), 3 * k + 2.0); }
    EXPECT_EQ(linbinsA.bins().at(33), (99. + 100.) / 2 + 1.0);
  }
  {
    lin_binning<double> linbinsB{0.0, -1, 2};
    for (int k = 0; k < 100; k++) { linbinsB << k + 1.0; }
    EXPECT_EQ(linbinsB.n_bins(), 50);
    EXPECT_EQ(linbinsB.last_bin_count(), 2);
    EXPECT_EQ(linbinsB.bin_capacity(), 2);

    linbinsB.compress(3);
    EXPECT_EQ(linbinsB.n_bins(), 17);
    EXPECT_EQ(linbinsB.bin_capacity(), 6);
    EXPECT_EQ(linbinsB.last_bin_count(), 4);
    for (int k = 0; k < 16; k++) { EXPECT_EQ(linbinsB.bins().at(k), 6 * k + 3.5); } // n * k + n (n - 1) / (2 n) + n / n
    EXPECT_EQ(linbinsB.bins().at(16), (96. + 97. + 98. + 99.) / 4 + 1.0);
  }
}

TEST(Stat, Details_LinBins_ManualCompression_PartialFullEndLvl2) {
  lin_binning<double> linbinsB{0.0, -1, 2};
  for (int k = 0; k < 99; k++) { linbinsB << k + 1.0; }
  EXPECT_EQ(linbinsB.n_bins(), 50);
  EXPECT_EQ(linbinsB.last_bin_count(), 1);
  EXPECT_EQ(linbinsB.bin_capacity(), 2);

  linbinsB.compress(3);
  EXPECT_EQ(linbinsB.n_bins(), 17);
  EXPECT_EQ(linbinsB.bin_capacity(), 6);
  EXPECT_EQ(linbinsB.last_bin_count(), 3);
  for (int k = 0; k < 16; k++) { EXPECT_EQ(linbinsB.bins().at(k), 6 * k + 5.0 / 2.0 + 1.0); } // n * k + n (n - 1) / (2 n) + n / n
  EXPECT_EQ(linbinsB.bins().at(16), (96. + 97. + 98.) / 3 + 1.0);

  linbinsB.compress(5);
  EXPECT_EQ(linbinsB.n_bins(), 4);
  EXPECT_EQ(linbinsB.bin_capacity(), 30);
  EXPECT_EQ(linbinsB.last_bin_count(), 9);
  for (int k = 0; k < 3; k++) { EXPECT_EQ(linbinsB.bins().at(k), 30 * k + 29. / 2.0 + 1.0); } // n * k + n (n - 1) / (2 n) + n / n
  EXPECT_EQ(linbinsB.bins().at(3), 90. + 4. + 1.0);

  linbinsB.compress(3);
  EXPECT_EQ(linbinsB.n_bins(), 2);
  EXPECT_EQ(linbinsB.bin_capacity(), 90);
  EXPECT_EQ(linbinsB.last_bin_count(), 9);
  EXPECT_EQ(linbinsB.bins().at(0), 89. / 2.0 + 1.0); // n * k + n (n - 1) / (2 n) + n / n
  EXPECT_EQ(linbinsB.bins().at(1), 90. + 4. + 1.0);
}
// ********

TEST(Stat, Details_LinBins_AdvanceAndCompress_EvenNrBins) {
  lin_binning<double> linbins{0, 2, 1};
  EXPECT_EQ(linbins.bin_capacity(), 1);

  linbins << 0.0 << 1.0 << 2.0;
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.last_bin_count(), 1);
  EXPECT_EQ(linbins.bins(), vec_d({0.5, 2.0}));
  EXPECT_EQ(linbins.bin_capacity(), 2);

  linbins << 3.0;
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.last_bin_count(), 2);
  EXPECT_EQ(linbins.bins(), vec_d({0.5, 2.5}));
  EXPECT_EQ(linbins.bin_capacity(), 2);

  linbins << 4.0 << 5.0;
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.bins(), vec_d({1.5, 4.5}));
  EXPECT_EQ(linbins.last_bin_count(), 2);
  EXPECT_EQ(linbins.bin_capacity(), 4);
}

TEST(Stat, Details_LinBins_AdvanceAndCompress_OddNrBins) {
  lin_binning<double> linbins{0, 3, 1};
  EXPECT_EQ(linbins.bin_capacity(), 1);

  linbins << 0.0 << 1.0 << 2.0 << 3.0;
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.bin_capacity(), 2);
  EXPECT_EQ(linbins.last_bin_count(), 2);
  EXPECT_EQ(linbins.bins(), vec_d({0.5, 2.5}));

  linbins << 3.0;
  EXPECT_EQ(linbins.n_bins(), 3);
  EXPECT_EQ(linbins.bin_capacity(), 2);
  EXPECT_EQ(linbins.last_bin_count(), 1);
  EXPECT_EQ(linbins.bins(), vec_d({0.5, 2.5, 3.0}));

  linbins << 4.0 << 5.0;
  EXPECT_EQ(linbins.n_bins(), 2);
  EXPECT_EQ(linbins.bin_capacity(), 4);
  EXPECT_EQ(linbins.last_bin_count(), 3);
  EXPECT_EQ(linbins.bins(), vec_d({1.5, 4.0}));
}

// *****************************************************************************
// Logbin Tests

using logbin_d = log_binning<double>;

TEST(Stat, Details_LogBins_InitTest_Float) {
  log_binning<float> bins{0.0, -1};
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
  log_binning<std::complex<float>> bins{0.0, -1};
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
  typedef nda::array<std::complex<double>, 2> cdouble_array_2d;
  auto a = cdouble_array_2d{{0., 0., 0.}, {0., 0., 0.}};
  log_binning<cdouble_array_2d> bins{a, -1};
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Mk.at(0))>, cdouble_array_2d>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Qk.at(0))>, nda::array<double, 2>>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.acc.at(0))>, cdouble_array_2d>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.acc_count.at(0))>, int>, "EE");
  EXPECT_EQ(bins.n_bins(), 1);
  EXPECT_ARRAY_EQ(bins.Mk.at(0), a);
  EXPECT_ARRAY_EQ(bins.Qk.at(0), nda::real(a));
  EXPECT_ARRAY_EQ(bins.acc.at(0), a);
}

TEST(Stat, Details_LogBins_InitTest_OneBin) {
  log_binning<std::complex<float>> bins{0.0, 1};
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Mk.at(0))>, std::complex<float>>, "EE");
  static_assert(std::is_same_v<std::remove_reference_t<decltype(bins.Qk.at(0))>, float>, "EE");
  EXPECT_EQ(bins.max_n_bins, 1);
  EXPECT_EQ(bins.Mk.size(), 1);
  EXPECT_EQ(bins.Mk.at(0), std::complex<float>(0.0, 0.0));
  EXPECT_EQ(bins.Qk.size(), 1);
  EXPECT_EQ(bins.Qk.at(0), 0.0);
  EXPECT_EQ(bins.acc.size(), 0);
  EXPECT_EQ(bins.acc_count.size(), 0);
}

// *******
template <typename T> void CheckLogBinSize(log_binning<T> bins, long n_size) {
  EXPECT_EQ(bins.Mk.size(), n_size);
  EXPECT_EQ(bins.Qk.size(), n_size);
  EXPECT_EQ(bins.acc.size(), n_size);
  EXPECT_EQ(bins.acc_count.size(), n_size);
}

TEST(Stat, Details_LogBins_AddDataComplex) {
  using namespace std::complex_literals;

  log_binning<std::complex<double>> bins{0.0, 3};
  EXPECT_EQ(bins.Mk, vec_c({0.0 * 1i}));
  EXPECT_EQ(bins.Qk, vec_d({0.0}));
  EXPECT_EQ(bins.acc, vec_c({0.0 * 1i}));
  EXPECT_EQ(bins.acc_count, vec_i({0}));

  bins << 1i;
  EXPECT_EQ(bins.Mk, vec_c({1i}));
  EXPECT_EQ(bins.Qk, vec_d({0.0}));
  EXPECT_EQ(bins.acc, vec_c({1i}));
  EXPECT_EQ(bins.acc_count, vec_i({1}));

  bins << 2 * 1i;
  EXPECT_EQ(bins.Mk, vec_c({1.5 * 1i, 1.5 * 1i}));
  EXPECT_EQ(bins.Qk, vec_d({0.5, 0.0}));
  EXPECT_EQ(bins.acc, vec_c({0 * 1i, 3 * 1i}));
  EXPECT_EQ(bins.acc_count, vec_i({0, 1}));

  bins << 3 * 1i;
  EXPECT_EQ(bins.Mk, vec_c({2.0 * 1i, 1.5 * 1i}));
  EXPECT_EQ(bins.Qk, vec_d({2.0, 0.0}));
  EXPECT_EQ(bins.acc, vec_c({3 * 1i, 3 * 1i}));
  EXPECT_EQ(bins.acc_count, vec_i({1, 1}));

  bins << 4 * 1i;
  EXPECT_EQ(bins.Mk, vec_c({2.5 * 1i, 2.5 * 1i, 2.5 * 1i}));
  EXPECT_EQ(bins.Qk, vec_d({5.0, 2.0, 0.0}));
  EXPECT_EQ(bins.acc, vec_c({0.0, 0.0}));
  EXPECT_EQ(bins.acc_count, vec_i({0, 0}));

  bins << 5 * 1i << 6 * 1i << 7 * 1i << 8 * 1i;
  EXPECT_EQ(bins.Mk, vec_c({4.5 * 1i, 4.5 * 1i, 4.5 * 1i}));
  EXPECT_EQ(bins.Qk, vec_d({42.0, 20.0, 8.0}));
  EXPECT_EQ(bins.acc, vec_c({0.0, 0.0}));
  EXPECT_EQ(bins.acc_count, vec_i({0, 0}));
}

// *****************************************************************************

TEST(Stat, Accumulator_LinBinOnly) {
  mpi::communicator c;
  int n_log_bins_max = 0, lin_bin_capacity = 1, n_lin_bins_max = -1;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  std::vector<double> data = {0.0, 1.0, 2.0, 3.0};
  for (auto x : data) { my_acc << x; }

  EXPECT_EQ(my_acc.n_lin_bins(), 4);
  EXPECT_EQ(my_acc.n_lin_bins_max(), -1);
  EXPECT_EQ(my_acc.linear_bins(), data);
  EXPECT_EQ(my_acc.lin_bin_capacity(), 1);
  my_acc.compress_linear_bins(2);
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>({0.5, 2.5}));

  EXPECT_EQ(my_acc.n_log_bins(), 0);
  EXPECT_EQ(my_acc.n_log_bins_max(), 0);

  EXPECT_EQ(my_acc.data_input_count(), 4);

  auto [errors_i, count_i] = my_acc.log_bin_errors();
  EXPECT_EQ(errors_i, std::vector<double>());
  EXPECT_EQ(count_i, std::vector<long>());

  auto [errors_all, count_all] = my_acc.log_bin_errors_all_reduce(c);
  EXPECT_EQ(errors_all, std::vector<double>());
  EXPECT_EQ(count_all, std::vector<long>());
}

TEST(Stat, Accumulator_LogBinOnly) {
  int n_log_bins_max = 3, lin_bin_capacity = 1, n_lin_bins_max = 0;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
  for (auto x : data) { my_acc << x; }

  EXPECT_EQ(my_acc.n_lin_bins(), 0);
  EXPECT_EQ(my_acc.n_lin_bins_max(), 0);
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>());
  EXPECT_EQ(my_acc.lin_bin_capacity(), 0);
  my_acc.compress_linear_bins(2);
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>());

  EXPECT_EQ(my_acc.n_log_bins(), 3);
  EXPECT_EQ(my_acc.n_log_bins_max(), 3);
  EXPECT_EQ(my_acc.data_input_count(), 8);
}

TEST(Stat, Accumulator_Off) {
  mpi::communicator c;
  int n_log_bins_max = 0, lin_bin_capacity = 1, n_lin_bins_max = 0;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  std::vector<double> data = {0.0, 1.0, 2.0, 3.0};
  for (auto x : data) { my_acc << x; }

  EXPECT_EQ(my_acc.n_lin_bins(), 0);
  EXPECT_EQ(my_acc.n_lin_bins_max(), 0);
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>());
  my_acc.compress_linear_bins(2);
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>());

  EXPECT_EQ(my_acc.n_log_bins(), 0);
  EXPECT_EQ(my_acc.n_log_bins_max(), 0);
  EXPECT_EQ(my_acc.data_input_count(), 4);

  auto [errors_i, count_i] = my_acc.log_bin_errors();
  EXPECT_EQ(errors_i, std::vector<double>());
  EXPECT_EQ(count_i, std::vector<long>());

  auto [errors_all, count_all] = my_acc.log_bin_errors_all_reduce(c);
  EXPECT_EQ(errors_all, std::vector<double>());
  EXPECT_EQ(count_all, std::vector<long>());
}

// *****************************************************************************

TEST(Stat, Accumulator_LogBinErrors) {
  int n_log_bins_max = 3, lin_bin_capacity = 1, n_lin_bins_max = 3;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
  for (auto x : data) { my_acc << x; }

  EXPECT_EQ(my_acc.n_log_bins(), 3);
  EXPECT_EQ(my_acc.n_log_bins_max(), 3);
  EXPECT_EQ(my_acc.data_input_count(), 8);

  vec_d ans({std::sqrt(3.0 / 4.0), std::sqrt(5.0 / 3.0), 2.0});
  auto [errors, counts] = my_acc.log_bin_errors();
  EXPECT_EQ(errors, ans);
  EXPECT_EQ(counts, std::vector<long>({8, 4, 2}));
}

TEST(Stat, Accumulator_Chaining) {
  int n_log_bins_max = -1, lin_bin_capacity = 1, n_lin_bins_max = -1;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};
  my_acc << 0.0 << 1.0 << 2.0 << 3.0;
  EXPECT_EQ(my_acc.linear_bins(), std::vector<double>({0.0, 1.0, 2.0, 3.0}));
  EXPECT_EQ(my_acc.n_log_bins(), 3);
  EXPECT_EQ(my_acc.data_input_count(), 4);
}

// *****************************************************************************
// tau_estimate_from_errors

TEST(Stat, Tau_From_Errors) {
  typedef nda::array<double, 2> double_array_2d;
  double_array_2d a = double_array_2d{{1., 1., 1., 1.}, {1., 1., 1., 1.}};
  double_array_2d b = 2 * a;
  auto taus         = tau_estimate_from_errors(b, a);
  EXPECT_ARRAY_EQ(taus, 1.5 * a);
}

// *****************************************************************************

MAKE_MAIN;
