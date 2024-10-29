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
#include <triqs/test_tools/arrays.hpp>

#include <mpi/mpi.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>

#include <complex>
#include <random>

// Test MPI reduction of logarithmic binning accumulators.
template <typename T> void test_mpi(const T &tmp) {
  using namespace triqs::stat;
  log_binning acc_all{tmp, -1};
  log_binning acc_rank{tmp, -1};
  auto rng = std::mt19937{};
  mpi::communicator comm;

  // fill the accumulators
  int const min_bins = 5;
  int const max_bins = min_bins + comm.size() - 1;
  for (int i = 0; i < comm.size(); ++i) {
    // we need to add the most samples first, to be able to make a successful comparison
    int const n = (1 << (min_bins - 1 + comm.size() - 1 - i));
    for (int j = 0; j < n; ++j) {
      auto sample = random_sample(tmp, rng);
      acc_all << sample;
      if (i == comm.rank()) { acc_rank << sample; }
    }
  }
  EXPECT_EQ(acc_rank.n_bins(), min_bins + comm.size() - 1 - comm.rank());

  // mpi reduce the accumulators
  auto [mk_mpi, qk_mpi, nsamples_mpi] = acc_rank.mpi_all_reduce(comm);
  EXPECT_EQ(mk_mpi.size(), max_bins);

  // check the results
  auto nsamples = acc_all.effective_counts();
  for (int i = 0; i < max_bins; ++i) {
    EXPECT_EQ(nsamples_mpi[i], nsamples[i]);
    check_array_or_scalar(mk_mpi[i], acc_all.mean_bins()[i]);
    check_array_or_scalar(qk_mpi[i], acc_all.var_bins()[i]);
  }
}

TEST(TRIQSStat, LogBinningDoubleScalarMPI) { test_mpi(0.0); }
TEST(TRIQSStat, LogBinningComplexDoubleScalarMPI) { test_mpi(std::complex<double>{0.0, 0.0}); }
TEST(TRIQSStat, LogBinningDoubleArrayMPI) { test_mpi(nda::array<double, 1>(7)); }
TEST(TRIQSStat, LogBinningComplexDouble2DArrayMPI) { test_mpi(nda::array<std::complex<double>, 2>(3, 4)); }

MAKE_MAIN;
