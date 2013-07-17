TRIQS in a nutshell
===================

TRIQS is a toolbox containing **ready-to-use applications**, **python modules** as well as **C++ libraries** aimed at physicists in the field of quantum interacting systems.

Applications
------------

Interface to Wien2k for LDA+DMFT calculation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TRIQS allows you to turn band-structure calculations obtained from the Wien2k package to inputs to full-fledged LDA+DMFT calculations in a few lines!



[example here]

To learn more, see <link>

Solvers for impurity models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TRIQS comes with powerful numerical solvers for quantum impurity models.


[example here]

To learn more, see <link>

Python modules
--------------

Green's functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With TRIQS, the manipulation of Green's functions is made easy: construction of Green's functions in frequency and time domains (imaginary and real), Fourier transforms, visualization, tail computation...



.. runblock:: python
  
  # Import the Green's functions 
  from pytriqs.gf.local import GfImFreq, iOmega_n, inverse

  # Create the Matsubara-frequency Green's function and initialize it
  gw = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")
  gw <<= inverse( iOmega_n + 0.5 )


  # Create an imaginary-time Green's function and plot it
  gt = GFBloc_ImTime(Indices = [1], Beta = 50)
  gt <<= InverseFourier(gw)
  
  #from pytriqs.plot.mpl_interface import oplot
  #oplot(g, '-o',  x_window  = (0,10))

  print gt(0.5)


To learn more, see <link>

Lattice tools
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With TRIQS, build a tight-binding model on any lattice in a few lines, and extract its density of states, dispersion...


[example here]

.. runblock:: python

   print 2+2  # this will give output


To learn more, see <link>

C++ libraries
-------------


Monte-Carlo library
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set up a Monte-Carlo simulation in a few lines: you write the configuration, moves and measures, while TRIQS takes care of the Metropolis algorithm and parallelization of the code.


.. compileblock:: 

  #include <iostream>
  #include <triqs/utility/callbacks.hpp>
  #include <triqs/mc_tools/mc_generic.hpp>

  // the configuration: a spin, the inverse temperature, the external field
  struct configuration {
    int spin; double beta, h;
    configuration(double beta_, double h_) : spin(-1), beta(beta_), h(h_) {}
  };

  // a move: flip the spin
  struct flip {
    configuration & config;

    flip(configuration & config_) : config(config_) {}

    double attempt() { return std::exp(-2*config.spin*config.h*config.beta); }
    double accept() { config.spin*= -1; return 1.0; }
    void reject() {}
  };

  // a measurement: the magnetization
  struct compute_m {
    configuration & config;
    double Z, M;

    compute_m(configuration & config_) : config(config_), Z(0), M(0) {}

    void accumulate(double sign) { Z += sign; M += sign * config.spin; }

    void collect_results(boost::mpi::communicator const &c) {
      double sum_Z, sum_M;
      boost::mpi::reduce(c, Z, sum_Z, std::plus<double>(), 0);
      boost::mpi::reduce(c, M, sum_M, std::plus<double>(), 0);

      if (c.rank() == 0) {
        std::cout << "Magnetization: " << sum_M / sum_Z << std::endl << std::endl;
      }
    }
  };

  int main(int argc, char* argv[]) {

    // initialize mpi
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    // greeting
    if (world.rank() == 0) std::cout << "Isolated spin" << std::endl;

    // prepare the MC parameters
    int N_Cycles = 500000;
    int Length_Cycle = 10;
    int N_Warmup_Cycles = 1000;
    std::string Random_Name = "";
    int Random_Seed = 374982 + world.rank() * 273894;
    int Verbosity = (world.rank() == 0 ? 2 : 0);

    // construct a Monte Carlo loop
    triqs::mc_tools::mc_generic<double> SpinMC(N_Cycles, Length_Cycle, N_Warmup_Cycles,
                                               Random_Name, Random_Seed, Verbosity);

    // parameters of the model
    double beta = 0.3;
    double field = 0.5;

    // construct configuration
    configuration config(beta, field);

    // add moves and measures
    SpinMC.add_move(flip(config), "flip move");
    SpinMC.add_measure(compute_m(config), "magnetization measure");

    // Run and collect results
    SpinMC.start(1.0, triqs::utility::clock_callback(600));
    SpinMC.collect_results(world);

    return 0;
  }

To learn more, see <link>

Array library
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create, manipulate and store powerful multi-dimensional arrays: 


.. highlight:: c


.. compileblock:: 

    #include <triqs/arrays.hpp>
    using triqs::arrays::array;
    int main(){
       array<double,1> A(20);
    }

To learn more, see <link>

Expression library: CLEF
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Write mathematical expressions in a seamless and computationally efficient way:


 .. compileblock::
 
   #include <triqs/clef.hpp>
   int main () { 
    triqs::clef::placeholder <1> x_; 

    auto e1 = cos(2*x_+1);
    auto e2 = abs(2*x_-1);
    auto e3 = floor(2*x_-1);
    auto e4 = pow(2*x_+1,2);
   }


To learn more, see <link>
