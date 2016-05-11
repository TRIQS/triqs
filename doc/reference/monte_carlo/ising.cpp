#include <iostream>
#include <triqs/mc_tools/mc_generic.hpp>
#include <triqs/utility/callbacks.hpp>

#include "moves.hpp"
#include "configuration.hpp"
#include "measures.hpp"

int main(int argc, char* argv[]) {

 // initialize mpi
 triqs::mpi::environment env(argc, argv);
 triqs::mpi::communicator world;

 // greeting
 if (world.rank() == 0) std::cout << "Ising chain" << std::endl;

 // Prepare the MC parameters
 int n_cycles = 500000;
 int length_cycle = 50;
 int n_warmup_cycles = 100000;
 std::string random_name = "";
 int random_seed = 374982 + world.rank() * 273894;
 int verbosity = (world.rank() == 0 ? 2 : 0);

 // Construct a Monte Carlo loop
 triqs::mc_tools::mc_generic<double> IsingMC(random_name, random_seed, 1.0, verbosity);

 // parameters of the model
 int length = 100;
 double J = -1.0;
 double field = 0.5;
 double beta = 0.5;

 // construct configuration
 configuration config(length, beta, J, field);

 // add moves and measures
 IsingMC.add_move(flip(&config, IsingMC.get_rng()), "spin flip");
 IsingMC.add_measure(compute_m(&config), "measure magnetization");

 // Run and collect results
 IsingMC.warmup_and_accumulate(n_warmup_cycles, n_cycles, length_cycle, triqs::utility::clock_callback(-1));
 IsingMC.collect_results(world);
}

