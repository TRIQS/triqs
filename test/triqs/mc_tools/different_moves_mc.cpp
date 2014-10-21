// The goal of this program is to give an example of the use of the probability for proposing a move (here, pl and pr)
// The walker try to go towards its left with probability 2*pl/(pl+pr) (pl is given to the move_left constructor), 
//    or towards the right with probability 2*pr/(pl+pr) (pr is given to the move_right constructor). 
// The left move is accepted with probability proba=pr/pl and the right move with probability proba=pl/pr (in reality, each time with probability min(1,proba) ). 
// A python notebook is associated to this example, with the comparison to the theoretical gaussian histogram. 

#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/utility/callbacks.hpp>
#include <triqs/arrays.hpp>
#include <triqs/mc_tools/mc_generic.hpp>
namespace h5 = triqs::h5;

triqs::arrays::array<std::complex<double>,1> make_array( std::complex<double> c){return {c}; };

struct configuration {
  int x;                  //position of the walker
  configuration():x(0){}  //constructor
};

//------------------------------------------------------------
// MOVES
//------------------------------------------------------------
struct move_left {
  configuration *config;
  double proba;
  triqs::mc_tools::random_generator &RND;
  move_left(configuration &config_, double pl, double pr, triqs::mc_tools::random_generator &RND_):config(&config_), proba(pr/pl), RND(RND_){} //constructor
  double attempt() {return proba; }
  double accept()  {config->x += -1; return 1; }
  void reject()    {}
};
struct move_right {
  configuration *config;
  double proba;
  triqs::mc_tools::random_generator &RND;
  move_right( configuration &config_, double pl, double pr, triqs::mc_tools::random_generator &RND_):config(&config_), proba(pl/pr), RND(RND_){} //constructor
  double attempt() {return proba; }
  double accept()  {config->x += 1; return 1; }
  void reject()    {}
};

//------------------------------------------------------------
// MEASUREMENTS
//------------------------------------------------------------
struct compute_histo{
  configuration *config;
  triqs::arrays::array<double,1> H;  // an histogram
  int xmax, tot;
  compute_histo(configuration &config_, triqs::arrays::array<double,1> &H_, int xmax_) : config(&config_), H(H_), xmax(xmax_), tot(0) {}
  void accumulate(double sign) {
   if( config->x+xmax>=0 && config->x-xmax<0){
    H( floor( (config->x+xmax) ) ) += 1; 
    tot += 1; 
   }
   config->x=0;
  }
  void collect_results(boost::mpi::communicator const &c) {
   H/=tot;
   h5::file file("histo.h5",H5F_ACC_TRUNC);
   h5_write(file,"H",H);
  }
};

//------------------------------------------------------------
// MAIN
//------------------------------------------------------------
int main(int argc, char* argv[]) {

  // initialize mpi
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;

  // greeting
  if (world.rank() == 0) std::cout << "Random walk calculation" << std::endl;

  // prepare the MC parameters
  int N_Cycles = 100000;
  int Length_Cycle = 100;
  int N_Warmup_Cycles = 0;
  std::string Random_Name = "";
  int Random_Seed = 374982 + world.rank() * 273894;
  int Verbosity = (world.rank() == 0 ? 3 : 0);
  int xmax=floor(4*sqrt(Length_Cycle) ); //max of the position registered in the histogram
  double pl=1.5, pr=1;  //non normalized probabilities for proposing a left or right move

  h5::file file("params.h5",H5F_ACC_TRUNC);
  h5_write(file,"pr",make_array(pr));
  h5_write(file,"pl",make_array(pl));
  h5_write(file,"xmax",make_array(xmax));
  h5_write(file,"N_Cycles",make_array(N_Cycles));
  h5_write(file,"Length_Cycle",make_array(Length_Cycle));
  
  // construct a Monte Carlo loop
  triqs::mc_tools::mc_generic<double> IntMC(N_Cycles, Length_Cycle, N_Warmup_Cycles, Random_Name, Random_Seed, Verbosity);

  // construct configuration
  configuration config;
  triqs::arrays::array<double,1> H(2*xmax);
  H()=0;

  // add moves and measures
  IntMC.add_move(move_left (config, pl, pr, IntMC.rng()  ), "left move",  pl);
  IntMC.add_move(move_right(config, pl, pr, IntMC.rng()  ), "right move", pr);
  IntMC.add_measure(compute_histo(config, H, xmax), "histogramn measure");

  // Run and collect results
  IntMC.start(1.0, triqs::utility::clock_callback(600));
  IntMC.collect_results(world);

  return 0;

}
