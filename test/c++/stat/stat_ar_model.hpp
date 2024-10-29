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

#pragma once

#include <fmt/core.h>

#include <cmath>
#include <random>

// A simple autoregressive model to test some triqs::stat functionalities.
//
// The model is defined as X_t = \phi * X_{t-1} + \epsilon_t, where \phi is the autoregressive parameter with |\phi| < 1
// and \epsilon_t is a white noise with E[\epsilon_t] = 0 and Var[\epsilon_t] = \sigma^2.
//
// It has the following properties:
// - E[X_t] = \mu = 0
// - Var[X_t] = E[X_t^2] - \mu^2 = \sigma^2 / (1 - \phi^2)
// - C_n = Cov[X_{t+n}, X_t] = E[X_{t+n}X_t] - \mu^2 = \phi^{|n|} * \sigma^2 / (1 - \phi^2)
// - \tau_n = \sum_{i=1}^{n} (1 - i / n) \frac{C_i}{C_0} = \sum_{i=1}^{n} (1 - i / n) \phi^i
//          = \frac{\phi - \phi^{n+1} - \phi^n}{1 - \phi} + \frac{1}{n} \frac{\phi - \phi^{n+1}}{(1 - \phi)^2}
// - \tau_{\infty} = \frac{2 \phi - \phi^2}{(1 - \phi)^2}
struct ar_model {
  double x_t{0.0};
  double phi{0.1};
  double sigma{1.0};
  std::mt19937_64 rng{};
  std::normal_distribution<double> dist{0.0, sigma};

  // Constructor.
  ar_model(double phi, double sigma, int rank = 0) : phi(phi), sigma(sigma), dist(0.0, sigma) {
    rng.seed(0x6dd9ebbab1cfad36 + 0x2544382c71ac491b * rank);
  }

  // Generate a sample of the autoregressive process.
  [[nodiscard]] double sample() {
    x_t = phi * x_t + dist(rng);
    return x_t;
  }

  // Calculate the mean of the process.
  [[nodiscard]] double mean() const { return 0.0; }

  // Calculate the variance of the process.
  [[nodiscard]] double variance() const { return sigma * sigma / (1 - phi * phi); }

  // Calculate the covariance between X_t and X_{t+n}.
  [[nodiscard]] double covariance(int n) const { return std::pow(phi, std::abs(n)) * variance(); }

  // Calculate the integrated autocorrelation time of the process for a finite number of samples.
  [[nodiscard]] double autocorrelation_time(int n) const {
    return (phi - std::pow(phi, n + 1) - std::pow(phi, n)) / (1 - phi) + (phi - std::pow(phi, n + 1)) / (n * (1 - phi) * (1 - phi));
  }

  // Calculate the integrated autocorrelation time of the process for inifinite samples.
  [[nodiscard]] double autocorrelation_time() const { return phi / (1 - phi); }

  // Print analytic properties of the process.
  void print_analytic_results() const {
    fmt::println("AR model:");
    fmt::println("{:-<25}", "");
    fmt::println("phi = {}", phi);
    fmt::println("sigma = {}", sigma);
    fmt::println("mean = {}", mean());
    fmt::println("variance = {:.8f}", variance());
    fmt::println("tau = {:.8f}", autocorrelation_time());
    fmt::println("");
  }
};
