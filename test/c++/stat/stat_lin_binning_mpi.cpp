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

#include <mpi/mpi.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>

#include <complex>
#include <random>

// Test MPI reduction of logarithmic binning accumulators.
template <typename T> void test_mpi(const T &tmp) {
  using namespace triqs::stat;
  lin_binning acc_all{tmp, -1, 5};
  lin_binning acc_rank{tmp, -1, 5};
  auto rng = std::mt19937{};
  mpi::communicator comm;

  // fill the accumulators
  int const min_bins = 5;
  for (int i = 0; i < comm.size(); ++i) {
    int const n = 5 * (min_bins + comm.size());
    for (int j = 0; j < n; ++j) {
      auto sample = random_sample(tmp, rng);
      acc_all << sample;
      if (i == comm.rank()) { acc_rank << sample; }
    }
  }
  EXPECT_EQ(acc_rank.n_bins(), min_bins + comm.size());

  // mpi reduce the overall mean, variance and count
  auto [mean, var0, count] = acc_rank.mpi_all_reduce(comm);
  check_array_or_scalar(mean, acc_all.mean());
  check_array_or_scalar(var0, acc_all.var_data());
  EXPECT_EQ(count, acc_all.count());

  // mpi gather the full bins
  auto bins     = acc_rank.mpi_all_gather(comm);
  auto all_bins = acc_all.full_bins();
  EXPECT_EQ(bins.size(), all_bins.size());
  for (int i = 0; i < bins.size(); ++i) check_array_or_scalar(bins[i], all_bins[i]);
}

TEST(TRIQSStat, LinBinningDoubleScalarMPI) { test_mpi(0.0); }
TEST(TRIQSStat, LinBinningComplexDoubleScalarMPI) { test_mpi(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LinBinningDoubleArrayMPI) { test_mpi(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LinBinningComplexDouble2DArrayMPI) { test_mpi(nda::array<std::complex<double>, 2>(3, 4)); }

// Test the mpi_all_gather method of a linear binning accumulator with enforcing the same capacity.
TEST(TRIQSStat, LinBinningDoubleScalarMPISameCapacity) {
  using namespace triqs::stat;
  mpi::communicator comm;
  int const bin_cap = 1 << comm.rank();
  int const max_cap = 1 << (comm.size() - 1);
  lin_binning acc_all{0.0, -1, max_cap};
  lin_binning acc_rank{0.0, -1, bin_cap};
  auto rng = std::mt19937{};

  // fill the accumulator
  int const nbins = 1 << (comm.size() - 1);
  for (int i = 0; i < comm.size(); ++i) {
    int const n = 1 << i;
    for (int j = 0; j < nbins * n; ++j) {
      auto sample = random_sample(0.0, rng);
      acc_all << sample;
      if (i == comm.rank()) { acc_rank << sample; }
    }
  }
  EXPECT_EQ(acc_rank.n_full_bins(), nbins);
  EXPECT_EQ(acc_rank.bin_capacity(), bin_cap);
  EXPECT_EQ(acc_all.n_full_bins(), (1 << comm.size()) - 1);
  EXPECT_EQ(acc_all.bin_capacity(), max_cap);

  // mpi gather the full bins with the same capacity
  auto bins     = acc_rank.mpi_all_gather(comm, true);
  auto all_bins = acc_all.full_bins();
  EXPECT_EQ(bins.size(), all_bins.size());
  for (int i = 0; i < bins.size(); ++i) check_array_or_scalar(bins[i], all_bins[i]);
}

MAKE_MAIN;
