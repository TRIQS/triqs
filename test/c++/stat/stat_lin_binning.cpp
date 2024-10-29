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

#include <triqs/stat/lin_binning.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <complex>
#include <random>
#include <tuple>

// Check the state of a linear binning accumulator.
template <typename T>
void check_state(T const &acc, long max_n_bins, long last_bin_count, long bin_capacity, long n_bins, long n_full_bins, long count) {
  using namespace triqs::stat;
  EXPECT_EQ(acc.max_n_bins(), max_n_bins);
  EXPECT_EQ(acc.last_bin_count(), last_bin_count);
  EXPECT_EQ(acc.bin_capacity(), bin_capacity);
  EXPECT_EQ(acc.n_bins(), n_bins);
  EXPECT_EQ(acc.n_full_bins(), n_full_bins);
  EXPECT_EQ(acc.full_bins().size(), n_full_bins);
  EXPECT_EQ(acc.count(), count);
}

// Compare two linear binning accumulators.
template <typename T> void check_acc(T const &acc1, T const &acc2) {
  check_state(acc2, acc1.max_n_bins(), acc1.last_bin_count(), acc1.bin_capacity(), acc1.n_bins(), acc1.n_full_bins(), acc1.count());
  check_vectors(acc1.bins(), acc2.bins());
  check_array_or_scalar(acc1.mean(), acc2.mean());
  check_array_or_scalar(acc1.var_data(), acc2.var_data());
}

// Test linear binning with 0 bins.
template <typename T> void test_zero_bins(const T &tmp) {
  using namespace triqs::stat;
  lin_binning acc{tmp, 0, 1};
  auto rng = std::mt19937{};

  // empty accumulator
  check_state(acc, 0, 0, 0, 0, 0, 0);

  // add some random samples
  for (int i = 0; i < 10; ++i) acc << random_sample(tmp, rng);
  check_state(acc, 0, 0, 0, 0, 0, 10);
}

// Test linear binning with 1 bin.
template <typename T> void test_one_bin(const T &tmp) {
  using namespace triqs::stat;
  lin_binning acc{tmp, 1, 1};
  auto rng = std::mt19937{};
  std::vector<T> data{};

  // empty accumulators
  check_state(acc, 1, 0, -1, 1, 0, 0);

  // add some random samples
  for (int i = 0; i < 10; ++i) {
    auto sample = random_sample(tmp, rng);
    acc << sample;
    data.push_back(sample);
  }
  check_state(acc, 1, 10, -1, 1, 0, 10);
  auto [m1, v1] = bin_data(data, 10);
  check_vectors(acc.bins(), m1);
  check_array_or_scalar(acc.mean(), m1[0]);
  check_array_or_scalar(acc.var_data(), v1[0]);

  // compress bins manually
  acc.compress(2);
  check_state(acc, 1, 10, -1, 1, 0, 10);

  // add some more random samples
  for (int i = 0; i < 10; ++i) {
    auto sample = random_sample(tmp, rng);
    acc << sample;
    data.push_back(sample);
  }
  check_state(acc, 1, 20, -1, 1, 0, 20);
  auto [m2, v2] = bin_data(data, 20);
  check_vectors(acc.bins(), m2);
  check_array_or_scalar(acc.mean(), m2[0]);
  check_array_or_scalar(acc.var_data(), v2[0]);
}

// Test linear binning with unlimited number of bins.
template <typename T> void test_unlimited_bins(const T &tmp) {
  using namespace triqs::stat;
  lin_binning acc1{tmp, -1, 1};
  lin_binning acc2{tmp, -1, 2};
  lin_binning acc6{tmp, -1, 6};
  lin_binning acc30{tmp, -1, 30};
  std::vector<T> data{};
  auto rng = std::mt19937{};

  // empty accumulators
  check_state(acc1, -1, 0, 1, 1, 0, 0);
  check_state(acc2, -1, 0, 2, 1, 0, 0);
  check_state(acc6, -1, 0, 6, 1, 0, 0);
  check_state(acc30, -1, 0, 30, 1, 0, 0);

  // add some random samples
  for (int i = 0; i < 20; ++i) {
    auto sample = random_sample(tmp, rng);
    acc1 << sample;
    acc2 << sample;
    acc6 << sample;
    acc30 << sample;
    data.push_back(sample);
  }
  auto [m, v] = bin_data(data, 20);
  check_state(acc1, -1, 1, 1, 20, 20, 20);
  check_vectors(acc1.bins(), bin_data(data, 1).first);
  check_array_or_scalar(acc1.mean(), m[0]);
  check_array_or_scalar(acc1.var_data(), v[0]);
  check_state(acc2, -1, 2, 2, 10, 10, 20);
  check_vectors(acc2.bins(), bin_data(data, 2).first);
  check_array_or_scalar(acc2.mean(), m[0]);
  check_array_or_scalar(acc2.var_data(), v[0]);
  check_state(acc6, -1, 2, 6, 4, 3, 20);
  check_vectors(acc6.bins(), bin_data(data, 6).first);
  check_array_or_scalar(acc6.mean(), m[0]);
  check_array_or_scalar(acc6.var_data(), v[0]);
  check_state(acc30, -1, 20, 30, 1, 0, 20);
  check_vectors(acc30.bins(), m);
  check_array_or_scalar(acc30.mean(), m[0]);
  check_array_or_scalar(acc30.var_data(), v[0]);

  // compress bins manually
  acc1.compress(2);
  check_acc(acc1, acc2);

  // compress them again
  acc1.compress(3);
  check_acc(acc1, acc6);

  // compress them all
  acc1.compress(5);
  check_acc(acc1, acc30);
}

// Test linear binning with some finite number of bins > 1.
template <typename T> void test_finite_bins(const T &tmp) {
  using namespace triqs::stat;
  lin_binning acc2{tmp, 2, 4};
  lin_binning acc4{tmp, 4, 2};
  lin_binning acc5{tmp, 5, 2};
  lin_binning acc10{tmp, 10, 1};
  std::vector<T> data{};
  auto rng = std::mt19937{};

  // empty accumulators
  check_state(acc2, 2, 0, 4, 1, 0, 0);
  check_state(acc4, 4, 0, 2, 1, 0, 0);
  check_state(acc5, 5, 0, 2, 1, 0, 0);
  check_state(acc10, 10, 0, 1, 1, 0, 0);

  // add some random samples
  for (int i = 0; i < 10; ++i) {
    auto sample = random_sample(tmp, rng);
    acc2 << sample;
    acc4 << sample;
    acc5 << sample;
    acc10 << sample;
    data.push_back(sample);
  }
  auto [m, v] = bin_data(data, 10);
  check_state(acc2, 2, 2, 8, 2, 1, 10);
  check_vectors(acc2.bins(), bin_data(data, 8).first);
  check_array_or_scalar(acc2.mean(), m[0]);
  check_array_or_scalar(acc2.var_data(), v[0]);
  check_state(acc4, 4, 2, 4, 3, 2, 10);
  check_vectors(acc4.bins(), bin_data(data, 4).first);
  check_array_or_scalar(acc4.mean(), m[0]);
  check_array_or_scalar(acc4.var_data(), v[0]);
  check_state(acc5, 5, 2, 2, 5, 5, 10);
  check_vectors(acc5.bins(), bin_data(data, 2).first);
  check_array_or_scalar(acc5.mean(), m[0]);
  check_array_or_scalar(acc5.var_data(), v[0]);
  check_state(acc10, 10, 1, 1, 10, 10, 10);
  check_vectors(acc10.bins(), bin_data(data, 1).first);
  check_array_or_scalar(acc10.mean(), m[0]);
  check_array_or_scalar(acc10.var_data(), v[0]);

  // add some more random samples
  for (int i = 0; i < 3; ++i) {
    auto sample = random_sample(tmp, rng);
    acc2 << sample;
    acc4 << sample;
    acc5 << sample;
    acc10 << sample;
    data.push_back(sample);
  }
  std::tie(m, v) = bin_data(data, 13);
  check_state(acc2, 2, 5, 8, 2, 1, 13);
  check_vectors(acc2.bins(), bin_data(data, 8).first);
  check_array_or_scalar(acc2.mean(), m[0]);
  check_array_or_scalar(acc2.var_data(), v[0]);
  check_state(acc4, 4, 1, 4, 4, 3, 13);
  check_vectors(acc4.bins(), bin_data(data, 4).first);
  check_array_or_scalar(acc4.mean(), m[0]);
  check_array_or_scalar(acc4.var_data(), v[0]);
  check_state(acc5, 5, 1, 4, 4, 3, 13);
  check_vectors(acc5.bins(), bin_data(data, 4).first);
  check_array_or_scalar(acc5.mean(), m[0]);
  check_array_or_scalar(acc5.var_data(), v[0]);
  check_state(acc10, 10, 1, 2, 7, 6, 13);
  check_vectors(acc10.bins(), bin_data(data, 2).first);
  check_array_or_scalar(acc10.mean(), m[0]);
  check_array_or_scalar(acc10.var_data(), v[0]);

  // compress bins manually
  acc4.compress(2);
  check_vectors(acc4.bins(), acc2.bins());

  acc10.compress(2);
  check_vectors(acc10.bins(), acc5.bins());
}

// zero bins
TEST(TRIQSStat, LinBinningDoubleScalarMaxNBinsZero) { test_zero_bins(0.0); }
TEST(TRIQSStat, LinBinningComplexDoubleScalarMaxNBinsZero) { test_zero_bins(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LinBinningDoubleArrayMaxNBinsZero) { test_zero_bins(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LinBinningComplexDouble2DArrayMaxNBinsZero) { test_zero_bins(nda::array<std::complex<double>, 2>(3, 4)); }

// one bin
TEST(TRIQSStat, LinBinningDoubleScalarMaxNBinsOne) { test_one_bin(0.0); }
TEST(TRIQSStat, LinBinningComplexDoubleScalarMaxNBinsOne) { test_one_bin(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LinBinningDoubleArrayMaxNBinsOne) { test_one_bin(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LinBinningComplexDouble2DArrayMaxNBinsOne) { test_one_bin(nda::array<std::complex<double>, 2>(3, 4)); }

// unlimited bins
TEST(TRIQSStat, LinBinningDoubleScalarMaxNBinsUnlimited) { test_unlimited_bins(0.0); }
TEST(TRIQSStat, LinBinningComplexDoubleScalarMaxNBinsUnlimited) { test_unlimited_bins(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LinBinningDoubleArrayMaxNBinsUnlimited) { test_unlimited_bins(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LinBinningComplexDouble2DArrayMaxNBinsUnlimited) { test_unlimited_bins(nda::array<std::complex<double>, 2>(3, 4)); }

// finite bins
TEST(TRIQSStat, LinBinningDoubleScalarMaxNBinsFinite) { test_finite_bins(0.0); }
TEST(TRIQSStat, LinBinningComplexDoubleScalarMaxNBinsFinite) { test_finite_bins(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LinBinningDoubleArrayMaxNBinsFinite) { test_finite_bins(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LinBinningComplexDouble2DArrayMaxNBinsFinite) { test_finite_bins(nda::array<std::complex<double>, 2>(3, 4)); }

// HDF5
TEST(TRIQSStat, LinBinningHDF5) {
  using namespace triqs::stat;
  auto tmp = nda::array<std::complex<double>, 2>(3, 4);
  auto acc = lin_binning(tmp, 7, 3);
  auto rng = std::mt19937{};

  // fill the accumulator
  for (int i = 0; i < 100; ++i) acc << random_sample(tmp, rng);

  // write and read to/from HDF5
  auto acc_h5 = rw_h5(acc, "triqs_stat_lin_binning", "acc");

  // check accumulator read from HDF5
  check_acc(acc, acc_h5);
}

MAKE_MAIN;
