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

#include <triqs/stat/mean_error.hpp>
#include <triqs/stat/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <complex>
#include <random>
#include <span>
#include <utility>
#include <vector>

// Generate a random sample.
template <typename T, typename RNG> [[nodiscard]] auto random_sample([[maybe_unused]] T const &tmp, RNG &rng) {
  // lambda to generate a random float, double or complex double
  auto single_value = [&]() {
    auto dist = std::uniform_real_distribution<double>{};
    if constexpr (nda::is_complex_v<nda::get_value_t<T>>)
      return std::complex<double>{dist(rng), dist(rng)};
    else
      return dist(rng);
  };

  // fill the sample with random values
  if constexpr (nda::Scalar<T>) {
    return single_value();
  } else {
    T sample = tmp;
    for (auto &x : sample) x = single_value();
    return sample;
  }
}

// Check arrays/scalars etc. for near equality.
template <typename A, typename B> auto check_array_or_scalar(A const &a, B const &b, double tol = 1e-10) {
  if constexpr (nda::Scalar<A>) {
    EXPECT_COMPLEX_NEAR(a, b, tol);
  } else {
    EXPECT_ARRAY_NEAR(a, b, tol);
  }
};

// Compare the elements of two vectors.
template <typename T> void check_vectors(std::vector<T> const &vec1, std::vector<T> const &vec2, double tol = 1e-10) {
  EXPECT_EQ(vec1.size(), vec2.size());
  for (auto [e1, e2] : itertools::zip(vec1, vec2)) check_array_or_scalar(e1, e2, tol);
}

// Calculate the mean and sum of squared deviations from the mean of a given number of adjacent data points.
template <typename T> auto bin_data(std::vector<T> const &data, int n) {
  using namespace triqs::stat;
  std::vector<T> m_data;
  std::vector<get_real_t<T>> v_data;

  // group n data points together and calculate their mean and sum of squared deviations from the mean
  auto const nbins = data.size() / n;
  for (int i = 0; i < nbins; ++i) {
    auto [m, v] = mean_and_err(std::span(data.data() + i * n, n));
    m_data.push_back(m);
    v_data.push_back(abs_square(v) * n * (n - 1));
  }

  // handle any left over data points
  auto const left_over = data.size() % n;
  if (left_over != 0) {
    auto [m, v] = mean_and_err(std::span{data.data() + nbins * n, left_over});
    m_data.push_back(m);
    v_data.push_back(abs_square(v) * left_over * (left_over - 1));
  }

  return std::make_pair(m_data, v_data);
}
