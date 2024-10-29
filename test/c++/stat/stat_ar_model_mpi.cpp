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

#include "./stat_ar_model.hpp"
#include "./stat_test_utils.hpp"

#include <triqs/stat/lin_binning.hpp>
#include <triqs/stat/log_binning.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <mpi/mpi.hpp>

// anonymous namespace with some parameters
namespace {
  constexpr int nsamples = 10000000;
} // namespace

void print_log_binning_results(triqs::stat::log_binning<double> const &acc, mpi::communicator const &comm) {
  auto [m, errs, taus, effs] = acc.mean_errors_and_taus(comm, comm.size() * 32);
  if (comm.rank() == 0) {
    fmt::println("{}", "Log binning");
    fmt::println("{:-<25}", "");
    fmt::println("mean = {:<15.8f}", m);
    fmt::println("variance = {:<15.8f}", errs[0] * errs[0] * static_cast<double>(effs[0]));
    fmt::println("{:<15}{:<15}{:<15}", "N", "error", "tau");
    for (std::size_t i = 0; i < errs.size(); ++i) { fmt::println("{:<15}{:<15.8f}{:<15.8f}", effs[i], errs[i], taus[i]); }
    fmt::println("");
  }
}

void print_lin_binning_results(triqs::stat::lin_binning<double> const &acc, mpi::communicator const &comm) {
  auto [m, err, tau] = acc.mean_error_and_tau(comm);
  if (comm.rank() == 0) {
    fmt::println("{}", "Lin binning");
    fmt::println("{:-<25}", "");
    fmt::println("mean = {:<15.8f}", m);
    fmt::println("variance = {:<15.8f}", acc.var_data() / static_cast<double>(acc.count() - 1));
    fmt::println("mean error = {:<15.8f}", err);
    fmt::println("tau = {:<15.8f}", tau);
    fmt::println("");
  }
}

TEST(TRIQSStat, LogBinningARModel) {
  using namespace triqs::stat;
  mpi::communicator comm;
  auto model = ar_model(0.9, 1.0, comm.rank());
  auto acc1  = log_binning{0.0, -1};
  auto acc2  = log_binning{0.0, -1};
  auto acc4  = log_binning{0.0, -1};

  // generate some samples
  for (int i = 0; i < nsamples; ++i) {
    auto s = model.sample();
    acc1 << s;
    if (i % 2 == 0) acc2 << s;
    if (i % 4 == 0) acc4 << s;
  }

  // print results
  if (comm.rank() == 0) model.print_analytic_results();
  print_log_binning_results(acc1, comm);
  print_log_binning_results(acc2, comm);
  print_log_binning_results(acc4, comm);
}

TEST(TRIQSStat, LinBinningARModel) {
  using namespace triqs::stat;
  mpi::communicator comm;
  auto model = ar_model(0.9, 1.0, comm.rank());
  auto acc1  = lin_binning{0.0, 256, 1};

  // generate some samples
  for (int i = 0; i < nsamples; ++i) {
    auto s = model.sample();
    acc1 << s;
  }

  // print results
  if (comm.rank() == 0) model.print_analytic_results();
  print_lin_binning_results(acc1, comm);
}

TEST(TRIQSStat, ARModelCompareLinAndLogBinning) {
  using namespace triqs::stat;
  mpi::communicator comm;
  auto model   = ar_model(0.9, 1.0, comm.rank());
  auto log_acc = log_binning{0.0, -1};
  auto lin_acc = lin_binning{0.0, -1, 1};

  // generate some samples
  for (int i = 0; i < 1000; ++i) {
    auto s = model.sample();
    log_acc << s;
    lin_acc << s;
  }

  // compare results for bin size 1
  auto [m_log, errs_log, taus_log, effs_log] = log_acc.mean_errors_and_taus(comm);
  auto [m_lin, err_lin, tau_lin]             = lin_acc.mean_error_and_tau(comm);
  check_array_or_scalar(m_log, m_lin);
  check_array_or_scalar(errs_log[0], err_lin);
  check_array_or_scalar(taus_log[0], tau_lin);

  // compare results for larger bin sizes
  for (int i = 1; i < 9; ++i) {
    lin_acc.compress(2);
    std::tie(m_lin, err_lin, tau_lin) = lin_acc.mean_error_and_tau(comm);
    check_array_or_scalar(errs_log[i], err_lin);
    check_array_or_scalar(taus_log[i], tau_lin);
  }
}

MAKE_MAIN;
