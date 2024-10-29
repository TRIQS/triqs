// Copyright (c) 2019-2023 Simons Foundation
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
// Authors: Philipp D, Olivier Parcollet, Nils Wentzell

#include "./stat_test_utils.hpp"

#include <triqs/stat/log_binning.hpp>
#include <triqs/stat/mean_error.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/h5.hpp>
#include <nda/nda.hpp>

#include <bit>
#include <complex>
#include <random>
#include <span>
#include <vector>

// Check the state of a logarithmic binning accumulator.
template <typename T> void check_state(T const &acc, long max_n_bins, long count) {
  EXPECT_EQ(acc.max_n_bins(), max_n_bins);
  EXPECT_EQ(acc.count(), count);
  long exp_nbins = 0;
  long exp_naccs = 0;
  if (max_n_bins == 1) {
    exp_nbins = 1;
  } else if (max_n_bins != 0) {
    exp_nbins = (count <= 1 ? 1 : std::bit_width(static_cast<unsigned long>(count)));
    exp_naccs = exp_nbins;
    if (max_n_bins > 1 && exp_nbins >= max_n_bins) {
      exp_nbins = max_n_bins;
      exp_naccs = max_n_bins - 1;
    }
  }
  EXPECT_EQ(acc.mean_bins().size(), exp_nbins);
  EXPECT_EQ(acc.var_bins().size(), exp_nbins);
  EXPECT_EQ(acc.bare_bins().size(), exp_naccs);
  EXPECT_EQ(acc.bare_counts().size(), exp_naccs);
}

// Compare two logarithmic binning accumulators.
template <typename T> void check_acc(T const &acc1, T const &acc2) {
  check_state(acc2, acc1.max_n_bins(), acc1.count());
  check_vectors(acc1.mean_bins(), acc2.mean_bins());
  check_vectors(acc1.var_bins(), acc2.var_bins());
  check_vectors(acc1.bare_bins(), acc2.bare_bins());
  EXPECT_EQ(acc1.bare_counts(), acc2.bare_counts());
}

// Check the bins of a logarithmic binning accumulator.
template <typename T> void check_bins(T const &acc, std::vector<typename T::value_t> const &data) {
  using namespace triqs::stat;
  for (int i = 0; i < acc.n_bins(); ++i) {
    auto const bin_size = (1 << i);
    auto [m, v]         = bin_data(data, bin_size);
    auto const nsamples = (data.size() % bin_size == 0 ? m.size() : m.size() - 1);
    auto [mean, err]    = mean_and_err(std::span{m.data(), nsamples});
    err                 = (nsamples < 2 ? zeroed_sample(err) : abs_square(err) * nsamples * (nsamples - 1));
    check_array_or_scalar(acc.mean_bins()[i], mean);
    check_array_or_scalar(acc.var_bins()[i], err);
  }
}

// Test logarithmic binning with 0 bins.
template <typename T> void test_zero_bins(const T &tmp) {
  using namespace triqs::stat;
  log_binning acc{tmp, 0};
  auto rng = std::mt19937{};

  // empty accumulator
  check_state(acc, 0, 0);

  // add some random samples
  for (int i = 0; i < 10; ++i) acc << random_sample(tmp, rng);
  check_state(acc, 0, 10);

  // check sizes of calculated errors and taus
  auto [m, errs, taus, effs] = acc.mean_errors_and_taus();
  EXPECT_EQ(errs.size(), 0);
}

// Test logarithmic binning with 1 bin.
template <typename T> void test_one_bin(const T &tmp) {
  using namespace triqs::stat;
  log_binning acc{tmp, 1};
  auto rng = std::mt19937{};
  std::vector<T> data{};

  // empty accumulators
  check_state(acc, 1, 0);

  // add some random samples
  for (int i = 0; i < 10; ++i) {
    auto sample = random_sample(tmp, rng);
    acc << sample;
    data.push_back(sample);
  }
  check_state(acc, 1, 10);
  check_bins(acc, data);

  // add some more random samples
  for (int i = 0; i < 10; ++i) {
    auto sample = random_sample(tmp, rng);
    acc << sample;
    data.push_back(sample);
  }
  check_state(acc, 1, 20);
  check_bins(acc, data);

  // check sizes of calculated errors and taus
  auto [m, errs, taus, effs] = acc.mean_errors_and_taus();
  EXPECT_EQ(errs.size(), 1);
  std::tie(m, errs, taus, effs) = acc.mean_errors_and_taus(30);
  EXPECT_EQ(errs.size(), 0);
}

// Test logarithmic binning with unlimited number of bins.
template <typename T> void test_unlimited_bins(const T &tmp) {
  using namespace triqs::stat;
  log_binning acc{tmp, -1};
  std::vector<T> data{};
  auto rng = std::mt19937{};

  // empty accumulators
  check_state(acc, -1, 0);

  // add some random samples
  for (int i = 0; i < 10; ++i) {
    auto sample = random_sample(tmp, rng);
    acc << sample;
    data.push_back(sample);
  }
  check_state(acc, -1, 10);
  check_bins(acc, data);

  // add some more random samples
  for (int i = 0; i < 10; ++i) {
    auto sample = random_sample(tmp, rng);
    acc << sample;
    data.push_back(sample);
  }
  check_state(acc, -1, 20);

  // check sizes of calculated errors and taus
  auto [m, errs, taus, effs] = acc.mean_errors_and_taus();
  EXPECT_EQ(errs.size(), 5);
  std::tie(m, errs, taus, effs) = acc.mean_errors_and_taus(5);
  EXPECT_EQ(errs.size(), 3);
  std::tie(m, errs, taus, effs) = acc.mean_errors_and_taus(30);
  EXPECT_EQ(errs.size(), 0);
}

// Test logarithmic binning with some finite number of bins > 1.
template <typename T> void test_finite_bins(const T &tmp) {
  using namespace triqs::stat;
  log_binning acc2{tmp, 2};
  log_binning acc4{tmp, 4};
  log_binning acc5{tmp, 5};
  log_binning acc10{tmp, 10};
  std::vector<T> data{};
  auto rng = std::mt19937{};

  // empty accumulators
  check_state(acc2, 2, 0);
  check_state(acc4, 4, 0);
  check_state(acc5, 5, 0);
  check_state(acc10, 10, 0);

  // add some random samples
  for (int i = 0; i < 10; ++i) {
    auto sample = random_sample(tmp, rng);
    acc2 << sample;
    acc4 << sample;
    acc5 << sample;
    acc10 << sample;
    data.push_back(sample);
  }
  check_state(acc2, 2, 10);
  check_bins(acc2, data);
  check_state(acc4, 4, 10);
  check_bins(acc4, data);
  check_state(acc5, 5, 10);
  check_bins(acc5, data);
  check_state(acc10, 10, 10);
  check_bins(acc10, data);

  // add some more random samples
  for (int i = 0; i < 33; ++i) {
    auto sample = random_sample(tmp, rng);
    acc2 << sample;
    acc4 << sample;
    acc5 << sample;
    acc10 << sample;
    data.push_back(sample);
  }
  check_state(acc2, 2, 43);
  check_bins(acc2, data);
  check_state(acc4, 4, 43);
  check_bins(acc4, data);
  check_state(acc5, 5, 43);
  check_bins(acc5, data);
  check_state(acc10, 10, 43);
  check_bins(acc10, data);
}

// zero bins
TEST(TRIQSStat, LogBinningDoubleScalarMaxNBinsZero) { test_zero_bins(0.0); }
TEST(TRIQSStat, LogBinningComplexDoubleScalarMaxNBinsZero) { test_zero_bins(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LogBinningDoubleArrayMaxNBinsZero) { test_zero_bins(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LogBinningComplexDouble2DArrayMaxNBinsZero) { test_zero_bins(nda::array<std::complex<double>, 2>(3, 4)); }

// one bin
TEST(TRIQSStat, LogBinningDoubleScalarMaxNBinsOne) { test_one_bin(0.0); }
TEST(TRIQSStat, LogBinningComplexDoubleScalarMaxNBinsOne) { test_one_bin(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LogBinningDoubleArrayMaxNBinsOne) { test_one_bin(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LogBinningComplexDouble2DArrayMaxNBinsOne) { test_one_bin(nda::array<std::complex<double>, 2>(3, 4)); }

// unlimited bins
TEST(TRIQSStat, LogBinningDoubleScalarMaxNBinsUnlimited) { test_unlimited_bins(0.0); }
TEST(TRIQSStat, LogBinningComplexDoubleScalarMaxNBinsUnlimited) { test_unlimited_bins(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LogBinningDoubleArrayMaxNBinsUnlimited) { test_unlimited_bins(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LogBinningComplexDouble2DArrayMaxNBinsUnlimited) { test_unlimited_bins(nda::array<std::complex<double>, 2>(3, 4)); }

// finite bins
TEST(TRIQSStat, LogBinningDoubleScalarMaxNBinsFinite) { test_finite_bins(0.0); }
TEST(TRIQSStat, LogBinningComplexDoubleScalarMaxNBinsFinite) { test_finite_bins(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LogBinningDoubleArrayMaxNBinsFinite) { test_finite_bins(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LogBinningComplexDouble2DArrayMaxNBinsFinite) { test_finite_bins(nda::array<std::complex<double>, 2>(3, 4)); }

// HDF5
TEST(TRIQSStat, LogBinningHDF5) {
  using namespace triqs::stat;
  auto tmp = nda::array<std::complex<double>, 2>(3, 4);
  auto acc = log_binning(tmp, 7);
  auto rng = std::mt19937{};

  // fill the accumulator
  for (int i = 0; i < 100; ++i) acc << random_sample(tmp, rng);

  // write and read to/from HDF5
  auto acc_h5 = rw_h5(acc, "triqs_stat_log_binning", "acc");

  // check accumulator read from HDF5
  check_acc(acc, acc_h5);
}

MAKE_MAIN;
