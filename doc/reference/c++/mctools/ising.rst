.. highlight:: c

.. _ising_solution:

The Ising chain in a magnetic field
-----------------------------------

Here is the a simple Monte-Carlo for a one-dimensional Ising chain.  The
problem is described in detail in this section about :ref:`the Ising model
<isingex>`.

The configuration
*****************

We start by defining a configuration class on which the move and measure
classes will act. We write this class in a file :file:`configuration.hpp`::

    #ifndef configuration_hpp
    #define configuration_hpp

    // The configuration of the system
    struct configuration {

      // N is the length of the chain, M the total magnetization
      // beta the inverse temperature, J the coupling , field the magnetic field and energy the energy of the configuration
      // field the magnetic field and energy the energy of the configuration
      int N, M;
      double beta, J, field, energy;

      // the chain of spins: true means "up", false means "down"
      std::vector<bool> chain;

      // constructor
      configuration(int N_, double beta_, double J_, double field_):
        N(N_), M(-N), beta(beta_), J(J_), field(field_), energy(-N*(J-field)), chain(N,false) {}

    };

    #endif

The move
********

The move class should have three methods: `attempt()`, `accept()` and `reject()`::

    #ifndef moves_hpp
    #define moves_hpp

    #include <triqs/mc_tools/random_generator.hpp>
    #include <vector>
    #include "configuration.hpp"

    // A move flipping a random spin
    struct flip {

      configuration * config;
      triqs::mc_tools::random_generator &RNG;

      int site;
      double delta_energy;

      // constructor
      flip(configuration & config_, triqs::mc_tools::random_generator & RNG_) :
         config(&config_), RNG(RNG_) {}

      double attempt() {
        // pick a random site
        site = RNG(config->N);

        // find the neighbors with periodicity
        int left = (site==0 ? config->N-1 : site-1);
        int right = (site==config->N-1 ? 0 : site+1);

        // compute energy difference from field
        delta_energy = (config->chain[site] ? 2 : -2) * config->field;

        // compute energy difference from J
        if(config->chain[left] == config->chain[right]) {
          delta_energy += (config->chain[left] == config->chain[site] ? 4 : -4) * config->J;
        }

        // return Metroplis ratio
        return std::exp(-config->beta * delta_energy);
      }

      // if move accepted just flip site and update energy and magnetization
      double accept() {
        config->M += (config->chain[site] ? -2 : 2);
        config->chain[site] = !config->chain[site];
        config->energy += delta_energy;

        return 1.0;
      }

      // nothing to do if the move is rejected
      void reject() {}
    };

    #endif


Measure
*******

The measure class has two methods, `accumulate` and `collect_results`::


      #ifndef MEASURES_HPP
      #define MEASURES_HPP

      #include "configuration.hpp"

      // The measure of the magnetization
      struct compute_m {

        configuration * config;
        double Z, M;

        compute_m(configuration & config_) : config(&config_), Z(0), M(0) {}

        // accumulate Z and magnetization
        void accumulate(int sign) {

          Z += sign;
          M += config->M;

        }

        // get final answer M / (Z*N)
        void collect_results(boost::mpi::communicator const &c) {

          double sum_Z, sum_M;
          boost::mpi::reduce(c, Z, sum_Z, std::plus<double>(), 0);
          boost::mpi::reduce(c, M, sum_M, std::plus<double>(), 0);

          if (c.rank() == 0) {
            std::cout << "Magnetization: " << sum_M / (sum_Z*config->N) << std::endl << std::endl;
          }

        }

      };

      #endif


Main program
************

The Monte-Carlo itself can now be written::

    #include <iostream>
    #include <boost/python.hpp>
    #include <triqs/mc_tools/mc_generic.hpp>
    #include <triqs/utility/callbacks.hpp>

    #include "moves.hpp"
    #include "configuration.hpp"
    #include "measures.hpp"

    int main(int argc, char* argv[]) {

      // initialize mpi
      boost::mpi::environment env(argc, argv);
      boost::mpi::communicator world;

      // greeting
      if (world.rank() == 0) std::cout << "Ising chain" << std::endl;

      // Prepare the MC parameters
      int N_Cycles = 500000;
      int Length_Cycle = 50;
      int N_Warmup_Cycles = 100000;
      std::string Random_Name = "";
      int Random_Seed = 374982 + world.rank() * 273894;
      int Verbosity = (world.rank() == 0 ? 2 : 0);

      // Construct a Monte Carlo loop
      triqs::mc_tools::mc_generic<double> IsingMC(N_Cycles, Length_Cycle, N_Warmup_Cycles,
                                                  Random_Name, Random_Seed, Verbosity);

      // parameters of the model
      int length = 100;
      double J = -1.0;
      double field = 0.5;
      double beta = 0.5;

      // construct configuration
      configuration config(length, beta, J, field);

      // add moves and measures
      IsingMC.add_move(flip(config, IsingMC.RandomGenerator), "spin flip");
      IsingMC.add_measure(compute_m(config), "measure magnetization");

      // Run and collect results
      IsingMC.start(1.0, triqs::utility::clock_callback(-1));
      IsingMC.collect_results(world);

      return 0;
    }


This yields::

    Ising chain

    1%; 2%; 3%; 4%; 5%; 6%; 7%; 8%; 9%; 10%; 11%; 12%; 13%; 14%; 15%; 16%; 17%;
    18%; 19%; 20%; 21%; 22%; 23%; 24%; 25%; 26%; 27%; 28%; 29%; 30%; 31%; 32%; 33%;
    34%; 35%; 36%; 37%; 38%; 39%; 40%; 41%; 42%; 43%; 44%; 45%; 46%; 47%; 48%; 49%;
    50%; 51%; 52%; 53%; 54%; 55%; 56%; 57%; 58%; 59%; 60%; 61%; 62%; 63%; 64%; 65%;
    66%; 67%; 68%; 69%; 70%; 71%; 72%; 73%; 74%; 75%; 76%; 77%; 78%; 79%; 80%; 81%;
    82%; 83%; 84%; 85%; 86%; 87%; 88%; 89%; 90%; 91%; 92%; 93%; 94%; 95%; 96%; 97%;
    98%; 99%; 100%; 

    Total number of measures: 500000
    Average sign: 1

    Magnetization: 0.0927603

