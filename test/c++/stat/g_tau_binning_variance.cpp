// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Philipp D, Olivier Parcollet

#include <random>

#include <triqs/gfs.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/test_tools/arrays.hpp>

using namespace triqs::stat;
using namespace triqs::utility;
using namespace triqs::gfs;
using namespace triqs;

mpi::communicator world;

/**
 * Use uncorrelated samples from a normal distribution
 * with known mean and standard deviation
 * and compute the mean and standard deviation using
 * the triqs::stat::acc_t
 */

/*
TEST(accumulator, g_tau_binning_mean_stddev) {

  using g_t   = gf<imtime, tensor_valued<2>>;
  // using obs_t = mc_value<g_t>;
  using acc_t = accumulator<g_t>;

  auto zero  = g_t{{1, Fermion, 2}, {1, 1}};
  auto tmesh = zero.mesh();
  zero       = 0;

  int seed = 1337;
  std::mt19937 gen(seed);
  double mean = 1.337, stddev = 0.1337;
  double var = stddev * stddev;
  std::normal_distribution<double> distr(mean, stddev);

  auto g_markow = zero;
  // mpi::communicator world;

  for (auto N_exp : range(10, 13)) {

    int auto_correlation_n_bins = 4;
    int max_n_bins              = 10;
    int bin_size                = 1;

    acc_t acc{zero, auto_correlation_n_bins, max_n_bins, bin_size};

    long N = 1 << N_exp;
    std::cout << "----------\n";
    std::cout << "number of samples N = " << N << "\n";

    for (auto iter : range(N)) {
      auto zero_tmp = g_t{{1, Fermion, 2}, {1, 1}};
      for (auto t : tmesh) zero_tmp[t] += distr(gen);
      acc << zero_tmp;
    }

    // auto correlation
    auto auto_corr = acc.auto_corr_variances(world);

    auto total_meas_std = auto_corr[auto_corr.size() - 1][0](0, 0).real();
    auto total_meas_var = total_meas_std * total_meas_std;

    std::cout << "autocorrelation analysis\n"
              << "n, bin_size=1<<n, var, var*N, tau\n";

    for (auto [n, gf_meas_var] : itertools::enumerate(auto_corr)) {

      auto meas_sterr = gf_meas_var[0](0, 0).real();
      auto meas_var = meas_sterr * meas_sterr;

      auto tau = 0.5 * (meas_var / total_meas_var - 1.);

      std::cout << n << ", " << (1ul << n) << ", " << meas_var << ", " << meas_var * N << ", " << tau << "\n";

      EXPECT_NEAR(var, meas_var * N, 1e-2);
      EXPECT_NEAR(tau, 0., 5e-2); // Uncorrelated samples
    }

    auto [gf_mean, gf_stddev] = mean_and_stderr(world, acc);
    auto meas_mean            = gf_mean[0](0, 0).real();
    auto meas_stddev          = gf_stddev[0](0, 0).real();
    auto meas_var             = meas_stddev * meas_stddev;

    std::cout << "mean = " << mean << "\n";
    std::cout << "meas_mean = " << meas_mean << "\n";

    std::cout << "var = " << var << "\n";
    std::cout << "meas_var = " << meas_var << "\n";
    std::cout << "meas_var * (N-1) = " << meas_var * double(N - 1) << "\n";

    std::cout << "stddev = " << stddev << "\n";
    std::cout << "meas_stddev = " << meas_stddev << "\n";
    std::cout << "meas_stddev * sqrt(N-1) = " << meas_stddev * sqrt(double(N - 1)) << "\n";

    double eps = 1e-2;
    EXPECT_NEAR(mean, meas_mean, 2 * meas_stddev);
    EXPECT_NEAR(var, meas_var * N, eps);
    EXPECT_NEAR(stddev, meas_stddev * sqrt(N), eps);
  }
}
*/

/** 
 * Test auto correlation length estimation 
 * on a modified Gausian correlated random walk.
 *
 * With position updates:
 *
 * x_new = f * x_old + sqrt(1 - f*f) * NormalDistribRNG()
 *
 * where f = exp(-1/tau) and tau is the correlation length.
 * 
 */
/*
TEST(accumulator, g_tau_binning) {

  using T     = double;
  using obs_t = mc_value<T>;
  using acc_t = accumulator<T>;

  double zero = 0;

  int N = 1 << 22;
  std::cout << "number of samples N = " << N << "\n";

  bool average = false, variance = true;
  int auto_correlation_n_bins = 21;
  accumulator_cargs acc_args{average, variance, auto_correlation_n_bins};
  acc_t acc{zero, acc_args};

  int seed = 1337 + 1;
  std::mt19937 gen(seed);

  double mean = 0.0, stddev = 1.0;
  double var = stddev * stddev;
  std::normal_distribution<double> distr(mean, stddev);

  double correlation_length = 20;
  double f                  = exp(-1. / correlation_length);

  auto x_markow(zero);

  for (auto iter : range(N)) {
    x_markow = f * x_markow + sqrt(1 - f * f) * distr(gen);
    acc << x_markow;
  }

  auto [opt_x_mean, opt_x_var, opt_auto_corr] = reduce(acc);

  for (auto [n, meas_var] : enumerate(*opt_auto_corr)) {
    auto tau = tau_estimates(*opt_auto_corr, n);

    std::cout << n << ", " << (1ul << n) << ", " << meas_var << ", " << tau << ", "
              << "\n";
  }

  double meas_corrlen = tau_estimates(*opt_auto_corr, 10);
  std::cout << "correlation_length = " << correlation_length << "\n";
  std::cout << "meas_corrlen       = " << meas_corrlen << "\n";

  EXPECT_NEAR(meas_corrlen, correlation_length, 2e0);

  auto meas_mean   = *opt_x_mean;
  auto meas_var    = *opt_x_var;
  auto meas_stddev = sqrt(meas_var);

  std::cout << "mean = " << mean << "\n";
  std::cout << "stddev = " << stddev << "\n";
  std::cout << "var = " << var << "\n";
  std::cout << "var/N = " << var / N << "\n";

  std::cout << "meas_mean = " << meas_mean << "\n";

  std::cout << "meas_var = " << meas_var << "\n";
  std::cout << "meas_stddev = " << meas_stddev << "\n";

  std::cout << "meas_var * (1 + 2 * correlation_length) = " << meas_var * (1 + 2 * correlation_length) << "\n";
  std::cout << "meas_stddev * sqrt(1 + 2 * correlation_length) = " << meas_stddev * sqrt(1 + 2 * correlation_length) << "\n";

  double two_sigma = 2 * meas_stddev * sqrt(1 + 2 * correlation_length);

  std::cout << "---------------------\n";
  std::cout << "mean = " << mean << "\n"
            << "meas_mean = " << meas_mean << " +/- " << two_sigma << "\n";
  std::cout << "---------------------\n";

  EXPECT_NEAR(mean, meas_mean, two_sigma);
}
*/
MAKE_MAIN;
