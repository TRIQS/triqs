// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <cstddef>
#include <triqs/mc_tools/mc_generic.hpp>
#include <gmock/gmock.h>
#include <triqs/test_tools/arrays.hpp>

#include <mpi/mpi.hpp>

#include <chrono>
#include <cstdint>
#include <functional>
#include <numbers>
#include <stdexcept>
#include <thread>

// MC configuration for integrating a sin(x) using naive MC integration.
struct mc_config {
  double a{0};
  double b{std::numbers::pi};
  double x{0};
  [[nodiscard]] double eval() const { return std::sin(x); }
};

// Draw a new x from a uniform distribution.
struct move_x {
  using rng_type = triqs::mc_tools::random_generator;
  std::reference_wrapper<mc_config> config;
  std::reference_wrapper<rng_type> rng;
  double new_x{0};
  move_x(mc_config &config, rng_type &rng) : config(config), rng(rng) {}
  double attempt() {
    new_x = rng.get()(config.get().a, config.get().b);
    return 1.0;
  }
  double accept() {
    config.get().x = new_x;
    return 1.0;
  }
};

// Measure the integral.
struct integral_1d {
  static constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
  std::reference_wrapper<mc_config> config;
  double sum_fx{0.0};
  double sum_fx_sq{0.0};
  std::int64_t count{0};
  integral_1d(mc_config &config) : config(config) {}
  void accumulate(double) {
    auto fx = config.get().eval();
    sum_fx += fx;
    sum_fx_sq += fx * fx;
    ++count;
  }
  void collect_results(mpi::communicator c) {
    auto sum_fx_red    = mpi::reduce(sum_fx, c);
    auto sum_fx_sq_red = mpi::reduce(sum_fx_sq, c);
    auto count_red     = static_cast<double>(mpi::reduce(count, c));
    if (c.rank() == 0) {
      auto ba   = config.get().b - config.get().a;
      auto mean = nan;
      auto err  = nan;
      if (count_red > 0) {
        mean = sum_fx_red / count_red * ba;
        err  = 0.0;
        if (count_red > 1) {
          auto var = (sum_fx_sq_red - sum_fx_red / count_red) / (count_red - 1) * ba * ba;
          err      = std::sqrt(var / count_red);
        }
      }
      fmt::print("Integral =  {} +/-  {}\n", mean, err);
    }
  }
};

// Fixture for mc_generic tests.
class TRIQSMCTools : public ::testing::Test {
  protected:
  void SetUp() override {
    // add move
    mc.add_move(move_x{config, mc.get_rng()}, "move_x");

    // add measure
    mc.add_measure(integral_1d{config}, "integral_1d");
  }

  mpi::communicator comm{};
  triqs::mc_tools::mc_generic<double> mc{"mt19937", comm.rank() * 0x12af5988, 3};
  mc_config config{};
};

// Test a basic MC simulation with specific parameters.
void test_mc_basic(triqs::mc_tools::mc_generic<double> &mc, std::int64_t ncycles, std::int64_t cycle_length) {
  auto params                        = mc.get_run_params();
  params.ncycles                     = ncycles;
  params.cycle_length                = cycle_length;
  params.continue_after_ncycles_done = false; // assert exact cycle counts
  EXPECT_EQ(mc.accumulate(params), 0);
  mc.collect_results(params.comm);
  EXPECT_DOUBLE_EQ(mc.get_percent(), 100);
  EXPECT_EQ(mc.get_current_cycle_number(), params.ncycles);
  EXPECT_EQ(mc.get_nmeasures(), ncycles);
  EXPECT_EQ(mc.get_config_id(), params.ncycles * params.cycle_length);
  if (ncycles > 0) EXPECT_DOUBLE_EQ(mc.get_acceptance_rates().at("move_x"), 1);
}

TEST_F(TRIQSMCTools, MCGenericBasicRunNCycles0) { test_mc_basic(mc, 0, 1); }

TEST_F(TRIQSMCTools, MCGenericBasicRunCycleLength1) { test_mc_basic(mc, 1000000, 1); }

TEST_F(TRIQSMCTools, MCGenericBasicRunCycleLength27) { test_mc_basic(mc, 10000, 27); }

TEST_F(TRIQSMCTools, MCGenericStopCallback) {
  auto params          = mc.get_run_params();
  params.stop_callback = [this]() {
    if (mc.get_config_id() == 100) return true;
    return false;
  };
  EXPECT_EQ(mc.accumulate(params), 1);
  mc.collect_results(params.comm);
  EXPECT_LT(mc.get_percent(), 0);
  EXPECT_EQ(mc.get_current_cycle_number(), 100);
  EXPECT_EQ(mc.get_nmeasures(), 100);
  EXPECT_EQ(mc.get_config_id(), 100);
  EXPECT_DOUBLE_EQ(mc.get_acceptance_rates().at("move_x"), 1);
}

TEST_F(TRIQSMCTools, MCGenericAfterCycleDuty) {
  int counter                        = 0;
  auto params                        = mc.get_run_params();
  params.ncycles                     = 100;
  params.continue_after_ncycles_done = false; // assert exact cycle counts
  params.after_cycle_duty            = [&counter]() { ++counter; };
  EXPECT_EQ(mc.accumulate(params), 0);
  mc.collect_results(params.comm);
  EXPECT_DOUBLE_EQ(mc.get_percent(), 100);
  EXPECT_EQ(mc.get_current_cycle_number(), params.ncycles);
  EXPECT_EQ(mc.get_nmeasures(), params.ncycles);
  EXPECT_EQ(mc.get_config_id(), params.ncycles * params.cycle_length);
  EXPECT_DOUBLE_EQ(mc.get_acceptance_rates().at("move_x"), 1);
}

TEST_F(TRIQSMCTools, MCGenericWarmuup) {
  auto params                        = mc.get_run_params();
  params.ncycles                     = 100;
  params.continue_after_ncycles_done = false; // assert exact cycle counts
  EXPECT_EQ(mc.warmup(params), 0);
  EXPECT_DOUBLE_EQ(mc.get_percent(), 100);
  EXPECT_EQ(mc.get_current_cycle_number(), params.ncycles);
  EXPECT_EQ(mc.get_nmeasures(), 0);
  EXPECT_EQ(mc.get_config_id(), params.ncycles * params.cycle_length);
}

TEST_F(TRIQSMCTools, MCGenericStopCallbackOnOneRank) {
  // a rank that stops early must not keep the other ranks polling forever in overtime, whether it is the root rank or not
  if (comm.size() == 1) GTEST_SKIP();
  for (int stop_rank : {0, 1}) {
    int ncycles_run              = 0;
    auto params                  = mc.get_run_params();
    params.ncycles               = 100;
    params.check_cycles_interval = 0.01;
    params.stop_callback         = [&]() { return ++ncycles_run == 10 and comm.rank() == stop_rank; };
    auto const ncycles_before    = mc.get_current_cycle_number();
    EXPECT_EQ(mc.accumulate(params), comm.rank() == stop_rank ? 1 : 0);
    if (comm.rank() == stop_rank) {
      EXPECT_EQ(mc.get_current_cycle_number() - ncycles_before, 10);
      EXPECT_LT(mc.get_percent(), 100);
    } else {
      EXPECT_GE(mc.get_current_cycle_number() - ncycles_before, params.ncycles);
      EXPECT_GE(mc.get_percent(), 100);
    }
    mc.collect_results(params.comm);
  }
}

TEST_F(TRIQSMCTools, MCGenericMismatchedCycleParameters) {
  if (comm.size() == 1) GTEST_SKIP();
  auto params    = mc.get_run_params();
  params.ncycles = 100 * (comm.rank() + 1);
  EXPECT_THROW(mc.accumulate(params), std::runtime_error);
  params.ncycles                     = 100;
  params.cycle_length                = comm.rank() + 1;
  params.continue_after_ncycles_done = false;
  EXPECT_THROW(mc.accumulate(params), std::runtime_error);
}

TEST_F(TRIQSMCTools, MCGenericOvertimeDutyLockstep) {
  // Overtime cycles (continue_after_ncycles_done with ranks finishing at different times) must not run
  // the after-cycle duty: it executes exactly ncycles times on EVERY rank, so duties and move calibrates
  // that communicate stay in lockstep (a rank-dependent call count deadlocks collective duties).
  std::int64_t duty_count      = 0;
  auto params                  = mc.get_run_params();
  params.ncycles               = 100;
  params.check_cycles_interval = 0.01;
  params.after_cycle_duty      = [this, &duty_count]() {
    ++duty_count;
    // slow down rank 0 so all other ranks reach ncycles first and cycle in overtime
    if (comm.rank() == 0) std::this_thread::sleep_for(std::chrono::milliseconds{2});
  };
  EXPECT_EQ(mc.accumulate(params), 0);
  EXPECT_EQ(duty_count, params.ncycles);
  EXPECT_GE(mc.get_current_cycle_number(), params.ncycles);
  // ranks that finished early kept measuring during overtime
  if (mpi::has_env && comm.size() > 1 && comm.rank() != 0) EXPECT_GT(mc.get_nmeasures(), params.ncycles);
  mc.collect_results(params.comm);
}

TEST_F(TRIQSMCTools, MCGenericPropagateExceptionInOvertime) {
  // rank 0 throws once every other rank has finished its cycles and polls in overtime
  if (comm.size() == 1) GTEST_SKIP();
  auto params                     = mc.get_run_params();
  params.ncycles                  = 100;
  params.check_cycles_interval    = 0.01;
  params.check_exception_interval = 0.01;
  int ncycles_run                 = 0;
  params.after_cycle_duty         = [&]() {
    if (comm.rank() == 0) {
      comm.barrier();
      throw std::runtime_error("Some exception on rank 0!");
    } else if (++ncycles_run == params.ncycles) {
      comm.barrier();
    }
  };
  EXPECT_THROW(mc.accumulate(params), std::runtime_error);
  if (comm.rank() != 0) EXPECT_GE(mc.get_percent(), 100);
}

TEST_F(TRIQSMCTools, MCGenericRethrowException) {
  // the original exception is rethrown on the rank where it occurred, the other ranks throw a generic one
  auto params                     = mc.get_run_params();
  params.ncycles                  = -1;
  params.check_exception_interval = 0.01;
  bool const throwing_rank        = comm.rank() == comm.size() - 1;
  params.after_cycle_duty         = [&]() {
    if (throwing_rank and mc.get_config_id() == 10) throw std::runtime_error("Some exception on this rank!");
  };
  auto const expected_msg = throwing_rank ? "Some exception on this rank!" : "an exception occurred on one of the MPI ranks";
  EXPECT_THAT([&]() { mc.accumulate(params); }, testing::ThrowsMessage<std::runtime_error>(testing::HasSubstr(expected_msg)));
  // the simulation is cleanly stopped and can be run again
  params.ncycles          = 10;
  params.after_cycle_duty = []() {};
  EXPECT_EQ(mc.accumulate(params), 0);
}

MAKE_MAIN;
