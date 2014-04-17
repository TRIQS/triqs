#include <iostream>
#include <triqs/utility/callbacks.hpp>
#include <triqs/mc_tools/mc_generic.hpp>
#include <triqs/statistics.hpp>
#include <triqs/utility/complex_ops.hpp>
using namespace triqs::statistics;

//We want to calculate 1/Z * \int_0^\pi (exp(I*x)+1/2)*(pi^2-x^2) dx
//with Z = \int_0^\pi (exp(I*x)+1/2) dx
// = 1/2 (pi+4*I)
// ~ 1.5708+2*i

//The exact result is 8.325393906-1.770556351*I

// integral_0^pi (exp(i x)+1/2) (pi^2-x^2) dx 
// = 4*I+2 pi+I*pi^2+pi^3/3
// ~ 16.6186+13.8696 i


// the configuration : a position x, its complex weight exp(ix)+1/2
struct configuration {
  
  double x;
  std::complex<double> weight;
  observable<std::complex<double>> M_stack;
  observable<std::complex<double>> sign_stack;
  
  //constructor
  configuration(double x_=1.5):x(x_), weight(exp(x_*std::complex<double>(0,1.0))+0.5){}
  
};

//a move: the position changes by a random number between -pi/6 and pi/6
struct move_slide {
  
  triqs::mc_tools::random_generator &RND;
  configuration *config;
  
  std::complex<double> I;
  double pi;
  
  double new_x;
  std::complex<double> new_weight;
  
  // constructor
  move_slide(configuration &config_, triqs::mc_tools::random_generator &RND_):
   RND(RND_), 
   I(0,1.0), 
   config(&config_), 
   pi(acos(-1.0)) {
  }
  
  std::complex<double> attempt() {
    double delta_x = pi / 3 * (RND(1.0)-0.5);
    new_x = config->x + delta_x;
    if(new_x<0 or new_x>pi)
      return 0.0; 
    else{
      new_weight = exp(I*(new_x))+0.5;
      return new_weight / config->weight;
    }
  }
  
  std::complex<double> accept() {
    config->x = new_x; 
    config->weight = new_weight; 
    return 1.0;
  }
  
  void reject() {}
  
};

// a measurement
struct compute_int{

  configuration *config;
  std::complex<double> Z, M;
  std::complex<double> I;
  double pi;

  compute_int(configuration &config_) : config(&config_), I(0,1.0), pi(acos(-1.0)), Z(0), M(0) {}

  void accumulate(std::complex<double> sign) {
    Z+=sign; 
    std::complex<double> temp=sign*(pow(pi,2)-pow(config->x,2));
    M+=temp; 
    config->M_stack << temp;
    config->sign_stack << sign;
  }

  void collect_results(boost::mpi::communicator const &c) {
    std::complex<double> sum_Z, sum_M;
    boost::mpi::reduce(c, Z, sum_Z, std::plus<std::complex<double>>(), 0);
    boost::mpi::reduce(c, M, sum_M, std::plus<std::complex<double>>(), 0);
    if (c.rank() == 0) {
      std::complex<double> intg_sim=sum_M/sum_Z;
      std::complex<double> intg_exact(8.325393906,-1.770556351);
      std::cout<<"Integrale: "<<intg_sim <<std::endl;
      std::cout<<"Wanted   : "<<intg_exact <<std::endl;
      std::cout<<"Error: "<<abs((intg_sim-intg_exact)/(2.0*intg_exact))*100 <<"%"<< std::endl<< std::endl;
      int binsize=1;
      for(int i=0;i<25;i++){
        std::cout << binsize<<"\t average_and_error(M/sign) = " << average_and_error(config->M_stack/config->sign_stack,binsize)<< std::endl;
        binsize*=2;
      }
      std::cout << "\tAutocorr_time:\t" << autocorrelation_time_from_binning(config->M_stack) << std::endl;
    }
  }
};

int main(int argc, char* argv[]) {

  // initialize mpi
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;

  // greeting
  if (world.rank() == 0) std::cout << "Integral calculation" << std::endl;

  // prepare the MC parameters
  int N_Cycles = 500000;
  int Length_Cycle = 3;
  int N_Warmup_Cycles = 10;
  std::string Random_Name = "";
  int Random_Seed = 374982 + world.rank() * 273894;
  int Verbosity = (world.rank() == 0 ? 2 : 0);

  // construct a Monte Carlo loop
  triqs::mc_tools::mc_generic<std::complex<double>> IntMC(N_Cycles, Length_Cycle, N_Warmup_Cycles,
                                             Random_Name, Random_Seed, Verbosity);

  // construct configuration
  configuration config;

  // add moves and measures
  IntMC.add_move(move_slide(config, IntMC.rng()), "position move");
  IntMC.add_measure(compute_int(config), "magnetization measure");

  // Run and collect results
  IntMC.start(1.0, triqs::utility::clock_callback(600));
  IntMC.collect_results(world);

  return 0;

}