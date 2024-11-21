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
#include <triqs/test_tools/arrays.hpp>

#include <mpi/mpi.hpp>

#include <cstdint>
#include <functional>
#include <numbers>
#include <stdexcept>

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
      auto mean = sum_fx_red / count_red * ba;
      auto var  = (sum_fx_sq_red - sum_fx_red / count_red) / (count_red - 1) * ba * ba;
      fmt::print("Integral =  {} +/-  {}\n", mean, std::sqrt(var / count_red));
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
  auto params         = mc.get_run_params();
  params.ncycles      = ncycles;
  params.cycle_length = cycle_length;
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
  int counter             = 0;
  auto params             = mc.get_run_params();
  params.ncycles          = 100;
  params.after_cycle_duty = [&counter]() { ++counter; };
  EXPECT_EQ(mc.accumulate(params), 0);
  mc.collect_results(params.comm);
  EXPECT_DOUBLE_EQ(mc.get_percent(), 100);
  EXPECT_EQ(mc.get_current_cycle_number(), params.ncycles);
  EXPECT_EQ(mc.get_nmeasures(), params.ncycles);
  EXPECT_EQ(mc.get_config_id(), params.ncycles * params.cycle_length);
  EXPECT_DOUBLE_EQ(mc.get_acceptance_rates().at("move_x"), 1);
}

TEST_F(TRIQSMCTools, MCGenericWarmuup) {
  auto params    = mc.get_run_params();
  params.ncycles = 100;
  EXPECT_EQ(mc.warmup(params), 0);
  EXPECT_DOUBLE_EQ(mc.get_percent(), 100);
  EXPECT_EQ(mc.get_current_cycle_number(), params.ncycles);
  EXPECT_EQ(mc.get_nmeasures(), 0);
  EXPECT_EQ(mc.get_config_id(), params.ncycles * params.cycle_length);
}

TEST_F(TRIQSMCTools, MCGenericContinueAfterNCyclesDone) {
  auto params                        = mc.get_run_params();
  params.ncycles                     = static_cast<std::int64_t>(1000000) * (comm.rank() + 1);
  params.continue_after_ncycles_done = true;
  params.check_cycles_interval       = 0.01;
  EXPECT_EQ(mc.accumulate(params), 0);
  mc.collect_results(params.comm);
  EXPECT_GE(mc.get_percent(), 100);
  EXPECT_GE(mc.get_current_cycle_number(), params.ncycles);
  EXPECT_GE(mc.get_nmeasures(), params.ncycles);
  EXPECT_GE(mc.get_config_id(), params.ncycles * params.cycle_length);
  EXPECT_DOUBLE_EQ(mc.get_acceptance_rates().at("move_x"), 1);
}

TEST_F(TRIQSMCTools, MCGenericPropagateException) {
  if (comm.size() != 1) {
    auto params                     = mc.get_run_params();
    params.ncycles                  = -1;
    params.propagate_exception      = true;
    params.check_exception_interval = 0.1;
    params.after_cycle_duty         = [this]() {
      if (comm.rank() == 0 && mc.get_config_id() == 10) throw std::runtime_error("Some exception on rank 0!");
    };
    bool exc_caught = false;
    try {
      mc.accumulate(params);
    } catch (std::exception const &e) {
      fmt::print("[Rank {}] Exception caught: {}\n", comm.rank(), e.what());
      exc_caught = true;
    }
    EXPECT_TRUE(exc_caught);
  }
}

MAKE_MAIN;
