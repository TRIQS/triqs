// Copyright (c) 2021 Simons Foundation
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
// Authors: Philipp Dumitrescu

#include <itertools/itertools.hpp>
#include <numeric>
#include <triqs/arrays.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>

#include <cmath>
#include <random>

// #include <format>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::stat;
using namespace triqs::stat::details;

using nda::conj;

using vec_d = std::vector<double>;
using vec_c = std::vector<std::complex<double>>;
using vec_i = std::vector<int>;

mpi::communicator c;

TEST(Stat, LogBinErrorsMPI_EqualSize) {
  // c.size() should be power of 2

  accumulator<double> acc_i{0.0, -1};
  accumulator<double> acc_all{0.0, -1};

  int N = 1024;

  std::mt19937 gen(123);
  std::normal_distribution<double> distr{1.0, 2.0};

  for (long i = 0; i < N; ++i) {
    auto x = distr(gen);
    acc_all << double(x);
    if (c.size() * i / N == c.rank()) { acc_i << x; }
  }

  auto [errs, counts] = acc_i.log_bin_errors_all_reduce(c);

  if (c.rank() == 0) {
    auto [errs_all, counts_all] = acc_all.log_bin_errors();
    for (int i = 0; i < errs.size(); i++) { EXPECT_NEAR(errs[i], errs_all[i], std::numeric_limits<double>::epsilon() * 120); }
  }
}

TEST(Stat, LogBinErrorsMPI_UnEqualSize) {

  accumulator<double> acc_i{0.0, -1};
  accumulator<double> acc_all{0.0, -1};

  int N1 = 1024;
  int N2 = 16;
  int N0 = 8;

  int log_errs_expected = 11; // = log2(N1) + 1
  if (c.size() == 1) {
    log_errs_expected = 4; // = log2(N0) + 1
  }

  std::mt19937 gen(123);
  std::normal_distribution<double> distr{1.0, 2.0};

  if (c.size() > 1) {
    for (long i = 0; i < N1; ++i) {
      auto x = distr(gen);
      acc_all << double(x);
      if (c.rank() == 1) { acc_i << x; }
    }
  }

  if (c.size() > 2) {
    for (long i = 0; i < N2; ++i) {
      auto x = distr(gen);
      acc_all << double(x);
      if (c.rank() == 2) { acc_i << x; }
    }
  }

  for (long i = 0; i < N0; ++i) {
    auto x = distr(gen);
    acc_all << double(x);
    if (c.rank() == 0) { acc_i << x; }
  }

  auto [errs, counts]         = acc_i.log_bin_errors_all_reduce(c);
  auto [errs_all, counts_all] = acc_all.log_bin_errors();

  EXPECT_EQ(errs.size(), log_errs_expected);

  for (int i = 0; i < errs.size(); i++) { EXPECT_NEAR(errs[i], errs_all[i], errs[i] * std::numeric_limits<double>::epsilon() * 10); }
}

MAKE_MAIN;
