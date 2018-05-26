#include <iostream>
#include <triqs/utility/callbacks.hpp>
#include <triqs/mc_tools/mc_generic.hpp>

// --------------- the configuration: a spin, the inverse temperature, the external field ---
struct configuration {
  int spin;
  double beta, h;
  configuration(double beta_, double h_) : spin(-1), beta(beta_), h(h_) {}
};

// --------------- a move: flip the spin ---------------
struct flip {
  configuration *config;

  flip(configuration *config_) : config(config_) {}

  double attempt() { return std::exp(-2 * config->spin * config->h * config->beta); }
  double accept() {
    config->spin *= -1;
    return 1.0;
  }
  void reject() {}
};

//  ----------------- a measurement: the magnetization ------------
struct compute_m {
  configuration *config;
  double Z = 0, M = 0;

  compute_m(configuration *config_) : config(config_) {}

  void accumulate(double sign) {
    Z += sign;
    M += sign * config->spin;
  }

  void collect_results(triqs::mpi::communicator c) {
    double sum_Z = triqs::mpi::reduce(Z, c);
    double sum_M = triqs::mpi::reduce(M, c);
    if (c.rank() == 0) std::cout << "Magnetization: " << sum_M / sum_Z << std::endl;
  }
};

//  ----------------- main ------------
int main(int argc, char *argv[]) {

  // initialize mpi
  triqs::mpi::environment env(argc, argv);
  triqs::mpi::communicator world;

  // greeting
  if (world.rank() == 0) std::cout << "Isolated spin" << std::endl;

  // prepare the MC parameters
  int n_cycles            = 5000000;
  int length_cycle        = 10;
  int n_warmup_cycles     = 10000;
  std::string random_name = "";
  int random_seed         = 374982 + world.rank() * 273894;
  int verbosity           = (world.rank() == 0 ? 2 : 0);

  // construct a Monte Carlo loop
  triqs::mc_tools::mc_generic<double> SpinMC(random_name, random_seed, 1.0, verbosity);

  // parameters of the model
  double beta  = 0.3;
  double field = 0.5;

  // construct configuration
  configuration config(beta, field);

  // add moves and measures
  SpinMC.add_move(flip(&config), "flip move");
  SpinMC.add_measure(compute_m(&config), "magnetization measure");

  // Run and collect results
  SpinMC.warmup_and_accumulate(n_warmup_cycles, n_cycles, length_cycle, triqs::utility::clock_callback(600));

  SpinMC.collect_results(world);
}
