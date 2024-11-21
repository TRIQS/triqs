// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
// Authors: Laura Messio, Olivier Parcollet, Priyanka Seth, Nils Wentzell

// The goal of this program is to give an example of the use of the probability for proposing a move (here, pl and pr)
// The walker try to go towards its left with probability 2*pl/(pl+pr) (pl is given to the move_left constructor),
//    or towards the right with probability 2*pr/(pl+pr) (pr is given to the move_right constructor).
// The left move is accepted with probability proba = pr/pl and the right move with probability proba=pl/pr (in reality, each time with probability min(1,proba) ).
// Two h5 files are produced:
//  - histo.h5
//  - params.h5
// A python notebook is associated to this example, with the comparison to the theoretical gaussian histogram.

#ifndef NDA_DEBUG
#define NDA_DEBUG
#endif

#include <triqs/utility/callbacks.hpp>
#include <nda/nda.hpp>
#include <triqs/mc_tools/mc_generic.hpp>
#include <triqs/test_tools/arrays.hpp>
nda::array<std::complex<double>, 1> make_array(std::complex<double> c) { return {c}; };

struct configuration {
  int x;                    //position of the walker
  configuration() : x(0) {} //constructor
};

long node_that_will_raise_exception = 1;

//------------------------------------------------------------
// MOVES
//------------------------------------------------------------
struct move_left {
  configuration *config;
  double proba               = -9;
  inline static long counter = 0;
  mpi::communicator c; // for error test only
  triqs::mc_tools::random_generator &RND;
  move_left(configuration &config_, double pl, double pr, triqs::mc_tools::random_generator &RND_)
     : config(&config_), proba(pr / pl), RND(RND_) {} //constructor
  double attempt() {
    if ((c.rank() == node_that_will_raise_exception) and counter++ == 2000) TRIQS_RUNTIME_ERROR << "A bad ERROR";
    return proba;
  }
  double accept() {
    config->x += -1;
    return 1;
  }
  void reject() {}
  move_left(move_left const &x) : config(x.config), proba(x.proba), RND(x.RND) { std::cout << "copy move_left" << std::endl; }
  move_left(move_left &&x) = default; //: config(x.config), proba(x.proba), RND(x.RND) { std::cout << "copy move_left"<<std::endl;}
  //  ~move_left() { std::cout << "desctution move_left" << std::endl; }
};
struct move_right {
  configuration *config;
  double proba;
  triqs::mc_tools::random_generator &RND;
  move_right(configuration &config_, double pl, double pr, triqs::mc_tools::random_generator &RND_)
     : config(&config_), proba(pl / pr), RND(RND_) {} //constructor
  double attempt() { return proba; }
  double accept() {
    config->x += 1;
    return 1;
  }
  void reject() {}
};

//------------------------------------------------------------
// MEASUREMENTS
//------------------------------------------------------------
struct compute_histo {
  configuration *config;
  nda::array<double, 1> H; // an histogram of the positions of the walker
  long xmax;               //maximal position registered in the histo
  mpi::communicator world;
  long tot; //number of pointsregistered inthe histogram
  compute_histo(configuration &config_, nda::array<double, 1> &H_, int xmax_) : config(&config_), H(H_), xmax(xmax_), tot(0) {}
  void accumulate(double) {
    auto x = config->x;
    if (x + xmax >= 0 && x - xmax < 0) {
      H(lround(floor((x + xmax)))) += 1;
      tot += 1;
    }
    config->x = 0; // the walker returns to zero.
  }
  void collect_results(mpi::communicator) {
    H /= tot;
    if (world.rank() == 0) {
      h5::file file("histo.h5", 'w');
      h5_write(file, "H", H);
    }
  }
};

mpi::communicator world;

//------------------------------------------------------------
// MAIN
//------------------------------------------------------------

bool test_mc(bool recover_from_exception) {

  // prepare the MC parameters
  int N_Cycles            = 100000; // number of different walks.
  int Length_Cycle        = 100;    // the walker makes Length_Cycle steps during each walk
  int N_Warmup_Cycles     = 0;
  std::string Random_Name = "";
  int Random_Seed         = 374982 + world.rank() * 273894;
  int Verbosity           = (world.rank() == 0 ? 3 : 0);
  int xmax                = static_cast<int>(floor(4 * sqrt(Length_Cycle))); // typical length of a walk
  double pl = 2.5, pr = 1;                                 //non normalized probabilities for proposing a left or right move

  if (world.rank() == 0) {
    h5::file file("params.h5", 'w');
    h5_write(file, "pr", pr);
    h5_write(file, "pl", pl);
    h5_write(file, "xmax", xmax);
    h5_write(file, "N_Cycles", N_Cycles);
    h5_write(file, "Length_Cycle", Length_Cycle);
  }

  // construct a Monte Carlo loop
  triqs::mc_tools::mc_generic<double> IntMC(Random_Name, Random_Seed, Verbosity);

  // construct configuration
  configuration config;
  nda::array<double, 1> H(2 * xmax);
  H() = 0;

  // add moves and measures
  IntMC.add_move(move_left(config, pl, pr, IntMC.get_rng()), "left move", pl);
  IntMC.add_move(move_right(config, pl, pr, IntMC.get_rng()), "right move", pr);
  IntMC.add_measure(compute_histo(config, H, xmax), "histogramn measure");

  bool stopped_by_exception = false;

  try {
    // Run and collect results
    IntMC.warmup_and_accumulate(N_Warmup_Cycles, N_Cycles, Length_Cycle, triqs::utility::clock_callback(600));
    IntMC.collect_results(world);
  } catch (std::exception const &e) {
    std::cerr << "TEST : Exception occurred. Node " << world.rank() << " is stopping cleanly" << std::endl;
    stopped_by_exception = true;
  }

  return stopped_by_exception;
}

TEST(mc_generic, WithException) {
  node_that_will_raise_exception = 1;
  bool stopped_by_exception      = test_mc(true);
  if (world.size() == 1)
    EXPECT_FALSE(stopped_by_exception); // one node only. Should work
  else
    EXPECT_TRUE(stopped_by_exception);
}

TEST(mc_generic, WithOutException) {
  node_that_will_raise_exception = -1;
  bool stopped_by_exception      = test_mc(true);
  EXPECT_FALSE(stopped_by_exception); // one node only. Should work
}

MAKE_MAIN;
