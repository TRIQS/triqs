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

#include <triqs/stat/jackknife.hpp>
#include <triqs/stat/mean_error.hpp>
#include <triqs/stat/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <mpi/mpi.hpp>
#include <nda/nda.hpp>

#include <random>
#include <ranges>
#include <utility>

// anonymous namespace with some parameters
namespace {
  constexpr int nsamples = 100;
} // namespace

// Generate random data.
template <typename T> auto generate_data(T const &tmp, mpi::communicator comm) {
  using namespace triqs::stat;
  auto rng = std::mt19937{};
  nda::array<T, 1> data_rank(nsamples);
  nda::array<T, 1> data_all(nsamples * comm.size());
  for (int i = 0; i < nsamples * comm.size(); ++i) {
    T sample    = random_sample(tmp, rng);
    data_all(i) = sample;
    if (int j = i - comm.rank() * nsamples; j >= 0 && j < nsamples) data_rank(j) = sample;
  }
  return std::make_pair(data_rank, data_all);
}

// Test jackknife resampling.
template <typename T> void test_jackknife(T const &tmp) {
  using namespace triqs::stat;
  mpi::communicator comm;
  auto [data_rank, data_all] = generate_data(tmp, comm);

  // without mpi and identity function
  auto exp_naive_x = mean(data_rank);
  auto exp_jk_samples =
     std::ranges::transform_view(data_rank, [exp_naive_x](auto const &x) { return (nsamples * exp_naive_x - x) / (nsamples - 1); });
  auto [exp_jk_x, exp_jk_xerr]          = mean_and_err<error_tag::jk_err>(exp_jk_samples);
  T exp_corr_x                          = nsamples * exp_naive_x - (nsamples - 1) * exp_jk_x;
  auto [corr_x, jk_xerr, jk_x, naive_x] = jackknife([](auto x) { return x; }, data_rank);
  check_array_or_scalar(exp_jk_x, jk_x);
  check_array_or_scalar(exp_jk_xerr, jk_xerr);
  check_array_or_scalar(exp_naive_x, naive_x);
  check_array_or_scalar(exp_corr_x, corr_x);

  // without mpi and quadratic function
  auto f                                = [](auto const &x) { return x * x; };
  auto f_wrapped                        = [f](auto const &x) { return nda::make_regular(f(x)); };
  T exp_naive_f                         = f(exp_naive_x);
  auto [exp_jk_f, exp_jk_ferr]          = mean_and_err<error_tag::jk_err>(std::ranges::transform_view(exp_jk_samples, f_wrapped));
  T exp_corr_f                          = nsamples * exp_naive_f - (nsamples - 1) * exp_jk_f;
  auto [corr_f, jk_ferr, jk_f, naive_f] = jackknife(f, data_rank);
  check_array_or_scalar(exp_jk_f, jk_f);
  check_array_or_scalar(exp_jk_ferr, jk_ferr);
  check_array_or_scalar(exp_naive_f, naive_f);
  check_array_or_scalar(exp_corr_f, corr_f);

  // without mpi and a linear function of more than one variable
  auto g                       = [](auto const &x, auto const &y) { return 2 * x + 3 * y; };
  auto g_wrapped               = [g](auto const &x, auto const &y) { return nda::make_regular(g(x, y)); };
  auto zipped_exp_jk_samples   = std::views::zip(exp_jk_samples, exp_jk_samples);
  T exp_naive_g                = g(exp_naive_x, exp_naive_x);
  auto [exp_jk_g, exp_jk_gerr] = mean_and_err<error_tag::jk_err>(
     std::ranges::transform_view(zipped_exp_jk_samples, [g_wrapped](auto const &tup) { return g_wrapped(std::get<0>(tup), std::get<1>(tup)); }));
  T exp_corr_g                          = nsamples * exp_naive_g - (nsamples - 1) * exp_jk_g;
  auto [corr_g, jk_gerr, jk_g, naive_g] = jackknife(g, data_rank, data_rank);
  check_array_or_scalar(exp_jk_g, jk_g);
  check_array_or_scalar(exp_jk_gerr, jk_gerr);
  check_array_or_scalar(exp_naive_g, naive_g);
  check_array_or_scalar(exp_corr_g, corr_g);

  // with mpi and identity function
  auto const mpi_nsamples = nsamples * comm.size();
  exp_naive_x             = mean(data_all);
  auto exp_jk_samples_all = std::ranges::transform_view(
     data_all, [exp_naive_x, mpi_nsamples](auto const &x) { return (mpi_nsamples * exp_naive_x - x) / (mpi_nsamples - 1); });
  std::tie(exp_jk_x, exp_jk_xerr)          = mean_and_err<error_tag::jk_err>(exp_jk_samples_all);
  exp_corr_x                               = mpi_nsamples * exp_naive_x - (mpi_nsamples - 1) * exp_jk_x;
  std::tie(corr_x, jk_xerr, jk_x, naive_x) = jackknife_mpi(comm, [](auto x) { return x; }, data_rank);
  check_array_or_scalar(exp_jk_x, jk_x);
  check_array_or_scalar(exp_jk_xerr, jk_xerr);
  check_array_or_scalar(exp_naive_x, naive_x);
  check_array_or_scalar(exp_corr_x, corr_x);

  // with mpi and quadratic function
  exp_naive_f                              = f(exp_naive_x);
  std::tie(exp_jk_f, exp_jk_ferr)          = mean_and_err<error_tag::jk_err>(std::ranges::transform_view(exp_jk_samples_all, f_wrapped));
  exp_corr_f                               = mpi_nsamples * exp_naive_f - (mpi_nsamples - 1) * exp_jk_f;
  std::tie(corr_f, jk_ferr, jk_f, naive_f) = jackknife_mpi(comm, f, data_rank);
  check_array_or_scalar(exp_jk_f, jk_f);
  check_array_or_scalar(exp_jk_ferr, jk_ferr);
  check_array_or_scalar(exp_naive_f, naive_f);
  check_array_or_scalar(exp_corr_f, corr_f);

  // with mpi and a linear function of more than one variable
  exp_naive_g                     = g(exp_naive_x, exp_naive_x);
  auto zipped_exp_jk_samples_all  = std::views::zip(exp_jk_samples_all, exp_jk_samples_all);
  std::tie(exp_jk_g, exp_jk_gerr) = mean_and_err<error_tag::jk_err>(
     std::ranges::transform_view(zipped_exp_jk_samples_all, [g_wrapped](auto const &tup) { return g_wrapped(std::get<0>(tup), std::get<1>(tup)); }));
  exp_corr_g                               = mpi_nsamples * exp_naive_g - (mpi_nsamples - 1) * exp_jk_g;
  std::tie(corr_g, jk_gerr, jk_g, naive_g) = jackknife_mpi(comm, g, data_rank, data_rank);
  check_array_or_scalar(exp_jk_g, jk_g);
  check_array_or_scalar(exp_jk_gerr, jk_gerr);
  check_array_or_scalar(exp_naive_g, naive_g);
  check_array_or_scalar(exp_corr_g, corr_g);
}

TEST(TRIQSStat, JackknifeBasicsWithDoubleScalar) { test_jackknife(0.0); }

TEST(TRIQSStat, JackknifeBasicsWithComplexDoubleScalar) { test_jackknife(std::complex<double>{0.0, 0.0}); }

TEST(TRIQSStat, JackknifeBasicsWithDoubleArray) { test_jackknife(nda::array<double, 1>(5)); }

TEST(TRIQSStat, JackknifeBasicsWithComplexDouble2DArray) { test_jackknife(nda::array<std::complex<double>, 2>(3, 4)); }

MAKE_MAIN;
