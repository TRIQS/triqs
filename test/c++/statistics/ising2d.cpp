// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell, tayral

#include <triqs/mc_tools/random_generator.hpp>
#include <triqs/mc_tools/mc_generic.hpp>
#include <triqs/utility/callbacks.hpp>
#include <triqs/arrays.hpp>
#include <triqs/statistics.hpp>
#include <vector>
#include <iostream>
#include <fstream>
//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
// H = -J \sum_<ij> s_i s_j - h \sum_i s_i
// theoretical T_c = 2/log(1+sqrt(2)) for J = 1.0
using namespace triqs::statistics;
/**************
 * config
 **************/

struct configuration {
  // N is the linear size of spin matrix, M the total magnetization,
  // beta the inverse temperature, J the coupling,
  // field the magnetic field and energy the energy of the configuration
  int N, M;
  double beta, J, field, energy;
  // the chain of spins: true means "up", false means "down"
  triqs::arrays::array<bool, 2> chain;
  observable<double> M_stack;

  // constructor
  configuration(int N_, double beta_, double J_, double field_)
     : N(N_), M(-N * N), beta(beta_), J(J_), field(field_), energy(-J * 4 * N / 2 + N * field), chain(N, N), M_stack() {
    chain() = false;
  }
};

/**************
 * move
 **************/

// A move flipping a random spin
struct flip {
  configuration *config;
  triqs::mc_tools::random_generator &RNG;

  struct site {
    int i, j;
  }; //small struct storing indices of a given site
  site s;
  double delta_energy;

  // constructor
  flip(configuration &config_, triqs::mc_tools::random_generator &RNG_) : config(&config_), RNG(RNG_) {}

  // find the neighbours with periodicity
  std::vector<site> neighbors(site s, int N) {
    std::vector<site> nns(4);
    int counter = 0;
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if ((i == 0) != (j == 0)) //xor
          nns[counter++] = site{(s.i + i + N) % N, (s.j + j + N) % N};
      }
    }
    return nns;
  }
  double attempt() {
    // pick a random site
    int index = RNG(config->N * config->N);
    s         = {index % config->N, index / config->N};

    // compute energy difference from field
    delta_energy         = (config->chain(s.i, s.j) ? 2 : -2) * config->field;
    auto nns             = neighbors(s, config->N); //nearest-neighbors
    double sum_neighbors = 0.0;
    for (auto &x : nns) sum_neighbors += ((config->chain(x.i, x.j)) ? 1 : -1);
    // compute energy difference from J
    delta_energy += -sum_neighbors * config->J * (config->chain(s.i, s.j) ? -2 : 2);

    // return Metroplis ratio
    return std::exp(-config->beta * delta_energy);
  }

  // if move accepted just flip site and update energy and magnetization
  double accept() {
    config->M += (config->chain(s.i, s.j) ? -2 : 2);
    config->chain(s.i, s.j) = !config->chain(s.i, s.j);
    config->energy += delta_energy;
    return 1.0;
  }

  // nothing to do if the move is rejected
  void reject() {}
};

/**************
 * measure
 **************/
struct compute_m {

  configuration *config;
  double Z, M;

  compute_m(configuration &config_) : config(&config_), Z(0), M(0) {}

  // accumulate Z and magnetization
  void accumulate(int sign) {

    Z += sign;
    M += config->M;
    //config->M_stack << double(config->M/(config->N*config->N));
    config->M_stack << config->M;
  }

  // get final answer M / (Z*N)
  void collect_results(mpi::communicator const &c) {

    double sum_Z = mpi::reduce(Z, c);
    double sum_M = mpi::reduce(M, c);

    if (c.rank() == 0) {
      std::cout << "#Beta:\t" << config->beta << "\tMagnetization:\t" << sum_M / (sum_Z * (config->N * config->N)) << std::endl;
      std::cout << "average_and_error(M) = " << average_and_error(config->M_stack) << std::endl;
      std::cout << "#Beta:\t" << config->beta << "\tAutocorr_time:\t" << autocorrelation_time_from_binning(config->M_stack) << std::endl;
      std::ofstream outfile("magnetization_series.dat");
      for (int i = 0; i < config->M_stack.size(); i++) outfile << config->M_stack[i] << std::endl;
      outfile.close();
    }
  }
};

int main(int argc, char *argv[]) {

  // initialize mpi
  mpi::environment env(argc, argv);
  mpi::communicator world;

  double H = 0.0, B = 0.5;
  int N  = 20;
  int nc = 100000;
  if (argc == 4) {
    H  = atof(argv[1]); //field
    B  = atof(argv[2]); //inverse temp
    N  = atoi(argv[3]); //size along one dimension
    nc = 1000000;
  }
  if (world.rank() == 0) std::cout << "2D Ising with field = " << H << ", beta = " << B << ", N = " << N << std::endl;

  // Prepare the MC parameters
  int n_cycles            = nc;
  int length_cycle        = 100;
  int n_warmup_cycles     = 100000;
  std::string random_name = "";
  int random_seed         = 374982 + world.rank() * 273894;
  int verbosity           = 0;

  // Construct a Monte Carlo loop
  triqs::mc_tools::mc_generic<double> IsingMC(random_name, random_seed, verbosity);

  // parameters of the model
  int length   = N;
  double J     = 1.0;
  double field = H;
  double beta  = B;

  // construct configuration
  configuration config(length, beta, J, field);

  // add moves and measures
  IsingMC.add_move(flip(config, IsingMC.get_rng()), "spin flip");
  IsingMC.add_measure(compute_m(config), "measure magnetization");

  // Run and collect results

  IsingMC.warmup_and_accumulate(n_warmup_cycles, n_cycles, length_cycle, triqs::utility::clock_callback(-1));
  IsingMC.collect_results(world);

  return 0;
}
