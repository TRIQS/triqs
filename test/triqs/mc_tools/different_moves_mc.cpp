// The goal of this program is to give an example of the use of the probability for proposing a move (here, pl and pr)
// The walker try to go towards its left with probability 2*pl/(pl+pr) (pl is given to the move_left constructor),
//    or towards the right with probability 2*pr/(pl+pr) (pr is given to the move_right constructor).
// The left move is accepted with probability proba = pr/pl and the right move with probability proba=pl/pr (in reality, each time with probability min(1,proba) ).
// Two h5 files are produced:
//  - histo.h5
//  - params.h5
// A python notebook is associated to this example, with the comparison to the theoretical gaussian histogram.

#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/utility/callbacks.hpp>
#include <triqs/arrays.hpp>
#include <triqs/mc_tools/mc_generic.hpp>
namespace h5  = triqs::h5;
namespace mpi = triqs::mpi;
triqs::arrays::array<std::complex<double>, 1> make_array(std::complex<double> c) { return {c}; };

struct configuration {
  int x;                    //position of the walker
  configuration() : x(0) {} //constructor
};

//------------------------------------------------------------
// MOVES
//------------------------------------------------------------
struct move_left {
  configuration *config;
  double proba = -9;
  triqs::mc_tools::random_generator &RND;
  move_left(configuration &config_, double pl, double pr, triqs::mc_tools::random_generator &RND_)
     : config(&config_), proba(pr / pl), RND(RND_) {} //constructor
  double attempt() { return proba; }
  double accept() {
    config->x += -1;
    return 1;
  }
  void reject() {}
  move_left(move_left const &x) : config(x.config), proba(x.proba), RND(x.RND) { std::cout << "copy move_left" << std::endl; }
  move_left(move_left &&x) = default; //: config(x.config), proba(x.proba), RND(x.RND) { std::cout << "copy move_left"<<std::endl;}
  ~move_left() { std::cout << "desctution move_left" << std::endl; }
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
  triqs::arrays::array<double, 1> H; // an histogram of the positions of the walker
  long xmax;                         //maximal position registered in the histo
  long tot;                          //number of pointsregistered inthe histogram
  compute_histo(configuration &config_, triqs::arrays::array<double, 1> &H_, int xmax_) : config(&config_), H(H_), xmax(xmax_), tot(0) {}
  void accumulate(double sign) {
    if (config->x + xmax >= 0 && config->x - xmax < 0) {
      H(floor((config->x + xmax))) += 1;
      tot += 1;
    }
    config->x = 0; // the walker returns to zero.
  }
  void collect_results(triqs::mpi::communicator c) {
    H /= tot;
    h5::file file("histo.h5", H5F_ACC_TRUNC);
    h5_write(file, "H", H);
  }
};

//------------------------------------------------------------
// MAIN
//------------------------------------------------------------
int main(int argc, char *argv[]) {

  // initialize mpi
  mpi::environment env(argc, argv);
  mpi::communicator world;

  // greeting
  if (world.rank() == 0) std::cout << "Random walk calculation" << std::endl;

  // prepare the MC parameters
  int N_Cycles            = 100000; // number of different walks.
  int Length_Cycle        = 100;    // the walker makes Length_Cycle steps during each walk
  int N_Warmup_Cycles     = 0;
  std::string Random_Name = "";
  int Random_Seed         = 374982 + world.rank() * 273894;
  int Verbosity           = (world.rank() == 0 ? 3 : 0);
  int xmax                = floor(4 * sqrt(Length_Cycle)); // typical length of a walk
  double pl = 2.5, pr = 1;                                 //non normalized probabilities for proposing a left or right move

  h5::file file("params.h5", H5F_ACC_TRUNC);
  h5_write(file, "pr", pr);
  h5_write(file, "pl", pl);
  h5_write(file, "xmax", xmax);
  h5_write(file, "N_Cycles", N_Cycles);
  h5_write(file, "Length_Cycle", Length_Cycle);

  // construct a Monte Carlo loop
  triqs::mc_tools::mc_generic<double> IntMC(Random_Name, Random_Seed, 1.0, Verbosity);

  // construct configuration
  configuration config;
  triqs::arrays::array<double, 1> H(2 * xmax);
  H() = 0;

  // add moves and measures
  IntMC.add_move(move_left(config, pl, pr, IntMC.get_rng()), "left move", pl);
  IntMC.add_move(move_right(config, pl, pr, IntMC.get_rng()), "right move", pr);
  IntMC.add_measure(compute_histo(config, H, xmax), "histogramn measure");

  // Run and collect results
  IntMC.warmup_and_accumulate(N_Warmup_Cycles, N_Cycles, Length_Cycle, triqs::utility::clock_callback(600));
  IntMC.collect_results(world);

  h5_write(file, "Stats", IntMC.get_acceptance_rates());
  //std::cout  << IntMC.average_sign() << std::endl;
}
