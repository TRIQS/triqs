// Copyright (c) 2023 Simons Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0.txt
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors: Nils Wentzell

#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/test_tools/gfs.hpp>

#include <iostream>
#include <triqs/utility/callbacks.hpp>
#include <triqs/mc_tools/mc_generic.hpp>

// --------------- the configuration a spin, the inverse temperature, the external field ---
struct config_t {
  double beta, h;
  int spin = -1;
};

// --------------- a move: flip the spin ---------------
struct flip {
  config_t &config;
  double attempt() { return std::exp(-2 * config.spin * config.h * config.beta); }
  double accept() {
    config.spin *= -1;
    return 1.0;
  }
  void reject() {}
};

//  ----------------- a measurement: the magnetization ------------
struct compute_m {
  config_t &config;
  double &Z;
  double M = 0;

  void accumulate(double sign) {
    Z += sign;
    M += sign * config.spin;
  }

  void collect_results(mpi::communicator c) {
    auto Z_sum = mpi::reduce(Z, c);
    auto M_sum = mpi::reduce(M, c);
    if (c.rank() == 0) std::cout << "Magnetization: " << M_sum / Z_sum << std::endl;
  }
};

TEST(mc_generic, callback) {

  // initialize mpi
  mpi::communicator world;

  // prepare the MC parameters
  int n_warmup_cycles     = 0;
  int n_cycles            = 1000;
  int length_cycle        = 10;
  std::string random_name = "";
  int random_seed         = 374982 + world.rank() * 273894;
  int verbosity           = (world.rank() == 0 ? 2 : 0);

  // construct a Monte Carlo loop
  triqs::mc_tools::mc_generic<double> SpinMC(random_name, random_seed, verbosity);

  // parameters of the model
  double beta  = 0.3;
  double field = 0.5;

  // construct config_t
  config_t config{beta, field};

  // add moves
  SpinMC.add_move(flip{config}, "flip move");

  // Add measures
  double Z = 0.0;
  SpinMC.add_measure(compute_m{config, Z}, "magnetization measure");

  // Run and collect results
  // We are using the callback to stop the simulation after n_cycles
  auto call_back_cycles = [n_cycles, n = 0]() mutable { return ++n >= n_cycles; };
  SpinMC.warmup_and_accumulate(n_warmup_cycles, -1, length_cycle, call_back_cycles);
  SpinMC.collect_results(world);

  EXPECT_EQ(Z, double(n_cycles));
}

MAKE_MAIN;
