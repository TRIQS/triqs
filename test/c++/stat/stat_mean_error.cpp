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

#include <triqs/stat/mean_error.hpp>
#include <triqs/stat/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <mpi/mpi.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>

#include <complex>
#include <random>
#include <tuple>

// anonymous namespace with some parameters
namespace {
  constexpr int nsamples = 100;
} // namespace

// Generate random data and calculate its mean and error.
template <typename T> auto generate_data(T const &tmp, mpi::communicator comm) {
  using namespace triqs::stat;
  auto rng       = std::mt19937{};
  auto mean_rank = zeroed_sample(tmp);
  auto err_rank  = make_real(zeroed_sample(tmp));
  auto mean_all  = zeroed_sample(tmp);
  auto err_all   = make_real(zeroed_sample(tmp));
  nda::array<T, 1> data_rank(nsamples);
  nda::array<T, 1> data_all(nsamples * comm.size());
  for (int i = 0; i < nsamples * comm.size(); ++i) {
    // generate a random sample
    T sample = random_sample(tmp, rng);

    // add the sample to the data and update the mean and error
    auto const ns = static_cast<double>(i + 1);
    data_all(i)   = sample;
    err_all += ((ns - 1) / ns) * abs_square(sample - mean_all);
    mean_all += (sample - mean_all) / ns;

    // do the same with rank specific data
    if (int j = i - comm.rank() * nsamples; j >= 0 && j < nsamples) {
      auto const ns_rank = static_cast<double>(j + 1);
      data_rank(j)       = sample;
      err_rank += ((ns_rank - 1) / ns_rank) * abs_square(sample - mean_rank);
      mean_rank += (sample - mean_rank) / ns_rank;
    }
  }

  // calculate the standard error
  err_rank = nda::sqrt(err_rank / (nsamples * (nsamples - 1)));
  err_all  = nda::sqrt(err_all / (nsamples * comm.size() * (nsamples * comm.size() - 1)));
  return std::make_tuple(mean_rank, err_rank, mean_all, err_all, data_rank, data_all);
}

// Test mean, mean_mpi, mean_and_err and mean_and_err_mpi functions.
template <typename T> void test_mean_and_error(T const &tmp) {
  mpi::communicator comm;
  auto [mean_rank, err_rank, mean_all, err_all, data_rank, data_all] = generate_data<T>(tmp, comm);
  auto const mpi_samples                                             = nsamples * comm.size();

  // mean
  auto m1 = triqs::stat::mean(data_rank);
  check_array_or_scalar(m1, mean_rank);
  m1 = triqs::stat::mean<triqs::stat::mean_tag::sum>(data_rank);
  check_array_or_scalar(m1, mean_rank * nsamples);

  // mean_mpi
  auto m1_mpi = triqs::stat::mean_mpi(comm, data_rank);
  check_array_or_scalar(m1_mpi, mean_all);
  m1_mpi = triqs::stat::mean_mpi<triqs::stat::mean_tag::sum>(comm, data_rank);
  check_array_or_scalar(m1_mpi, mean_all * mpi_samples);

  // mean_and_err
  auto [m2, e2] = triqs::stat::mean_and_err(data_rank);
  check_array_or_scalar(m2, mean_rank);
  check_array_or_scalar(e2, err_rank);
  std::tie(m2, e2) = triqs::stat::mean_and_err<triqs::stat::error_tag::sum, triqs::stat::mean_tag::sum>(data_rank);
  check_array_or_scalar(m2, mean_rank * nsamples);
  check_array_or_scalar(e2, triqs::stat::abs_square(err_rank) * nsamples * (nsamples - 1));
  std::tie(m2, e2) = triqs::stat::mean_and_err<triqs::stat::error_tag::jk_err>(data_rank);
  check_array_or_scalar(m2, mean_rank);
  check_array_or_scalar(e2, err_rank * std::sqrt((nsamples - 1) * (nsamples - 1)));

  // mean_and_err_mpi
  auto [m2_mpi, e2_mpi] = triqs::stat::mean_and_err_mpi(comm, data_rank);
  check_array_or_scalar(m2_mpi, mean_all);
  check_array_or_scalar(e2_mpi, err_all);
  std::tie(m2_mpi, e2_mpi) = triqs::stat::mean_and_err_mpi<triqs::stat::error_tag::sum, triqs::stat::mean_tag::sum>(comm, data_rank);
  check_array_or_scalar(m2_mpi, mean_all * mpi_samples);
  check_array_or_scalar(e2_mpi, triqs::stat::abs_square(err_all) * mpi_samples * (mpi_samples - 1));
  std::tie(m2_mpi, e2_mpi) = triqs::stat::mean_and_err_mpi<triqs::stat::error_tag::jk_err>(comm, data_rank);
  check_array_or_scalar(m2_mpi, mean_all);
  check_array_or_scalar(e2_mpi, err_all * std::sqrt((mpi_samples - 1) * (mpi_samples - 1)));
}

TEST(TRIQSStat, MeanErrorWithDoubleScalar) { test_mean_and_error(0.0); }

TEST(TRIQSStat, MeanErrorWithComplexDoubleScalar) { test_mean_and_error(std::complex<double>{0.0, 0.0}); }

TEST(TRIQSStat, MeanErrorWithDoubleArray) { test_mean_and_error(nda::array<double, 1>(5)); }

TEST(TRIQSStat, MeanErrorWithComplexDouble2DArray) { test_mean_and_error(nda::array<std::complex<double>, 2>(3, 4)); }

TEST(TRIQSStat, MeanErrorWithTransformedViewsOfArrays) {
  using namespace triqs::stat;
  using arr_t    = nda::array<double, 1>;
  auto data      = std::vector<arr_t>{{1.0}, {2.0}, {3.0}, {4.0}, {5.0}};
  auto data_mean = arr_t{3.0};

  // mean of data
  auto m1 = mean(data);
  check_array_or_scalar(m1, data_mean);

  // identity transform
  auto m2 = mean(std::ranges::transform_view(data, [](auto const &x) { return x; }));
  check_array_or_scalar(m2, data_mean);

  // constant scale transform
  auto m3 = mean(std::ranges::transform_view(data, [](auto const &x) { return x * 2; }));
  check_array_or_scalar(m3, data_mean * 2.0);

  // constant shift transform
  auto m4 = mean(std::ranges::transform_view(data, [](auto const &x) { return x + 1.0; }));
  check_array_or_scalar(m4, data_mean + 1.0);

  // constant scale and shift transform
  auto scale_view = std::ranges::transform_view(data, [](auto const &x) { return x * 2; });
  auto m5         = mean(std::ranges::transform_view(scale_view, [](auto const &x) { return nda::make_regular(x + 1.0); }));
  check_array_or_scalar(m5, data_mean * 2.0 + 1.0);
}

MAKE_MAIN;
