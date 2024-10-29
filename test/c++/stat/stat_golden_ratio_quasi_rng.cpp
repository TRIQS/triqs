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
// Authors: Philipp Dumitrescu, Nils Wentzell

#include <triqs/stat/lin_binning.hpp>
#include <triqs/stat/log_binning.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <vector>

// anonymous namespace
namespace {
  // number of samples to collect (equals {Fib[10], Fib[15], Fib[20], Fib[25], Fib[30]})
  std::vector<int> const nr_samples = {55, 610, 6765, 75025, 832040};

  // expected mean in high precision
  std::vector<float> const mean_exp = {0.50495168499705574972843136223786729616865703456136f, 0.49954749751910563469799921017785479962002820115236f,
                                       0.50004085146337716717417122805944330471652438858682f, 0.49999631602503416750963631859590564342443999426869f,
                                       0.50000033218707454549745407880501207827858000631961f};

  // expected error in high precision
  std::vector<float> const err_exp = {0.039265840412741919349187939402249012089938885571181f, 0.011697652089423577816521529549438794957302367097532f,
                                      0.0035100036188629244571590432844256025990753183026236f,
                                      0.0010539239387211801847383842584035403509305745267760f,
                                      0.00031647363494910673051693859572897361990178111789649f};

  // expected accuracy of the Welford algorithm used in the binning accumulators
  constexpr float eps = std::numeric_limits<float>::epsilon();
  std::vector<float> const tol_mean{eps, eps, 8 * eps, 15 * eps, 25 * eps};
  std::vector<float> const tol_err{eps, eps, eps, eps, eps};
} // namespace

// Quasi RNG to generates a uniform sequence based on the lower Wythoff sequence.
struct golden_ratio_quasi_rng {
  double golden_ratio_m1 = 2.0 / (1.0 + std::sqrt(5));

  float get(int n) {
    double output_temp = golden_ratio_m1 * n;
    output_temp -= std::floor(output_temp);
    return static_cast<float>(output_temp);
  }
};

TEST(TRIQSStat, GoldenRatioQuasiRNGTest) {
  using namespace triqs::stat;
  golden_ratio_quasi_rng gen;

  for (int idx = 0; idx < nr_samples.size(); idx++) {
    log_binning log_acc{0.0, 1};
    lin_binning lin_acc{0.0, -1, 1};

    for (int n = 1; n <= nr_samples[idx]; n++) {
      float temp = gen.get(n);
      log_acc << temp;
      lin_acc << temp;
    }

    // test log binning
    auto [m_log, err_log, tau_log, count_log] = log_acc.mean_errors_and_taus();
    EXPECT_EQ(float(m_log), mean_exp[idx]);
    EXPECT_EQ(float(err_log[0]), err_exp[idx]);

    // test linear binning
    auto [m_lin, err_lin, tau_lin] = lin_acc.mean_error_and_tau();
    EXPECT_EQ(float(m_lin), mean_exp[idx]);
    EXPECT_EQ(float(err_lin), err_exp[idx]);
  }
}

MAKE_MAIN;
