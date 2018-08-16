#include <random>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics/accumulator.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::stat;
using namespace triqs::utility;
using namespace triqs::gfs;
using namespace triqs;

triqs::mpi::communicator world;

// ----- TESTS ------------------

TEST(accumulator, g_tau_binning) {

  using T     = gf<imtime>;
  using obs_t = mc_value<T>;
  using acc_t = accumulator<T>;

  auto zero  = T{{1, Fermion, 10}, {1, 1}};
  auto tmesh = zero.mesh();

  zero = 0;

  bool average = false, variance = true;
  int auto_correlation_n_bins = 10;
  accumulator_cargs acc_args{average, variance, auto_correlation_n_bins};
  acc_t acc{zero, acc_args};

  int seed = 1337;
  std::mt19937 gen(seed);
  std::uniform_real_distribution<double> distr;

  double step_size = 0.1;

  auto g = T{{1, Fermion, 10}, {1, 1}};

  for (auto iter : range(100)) {

    //g = 0;
    std::cout << "before :" << g[0] << "\n";
    
    for (auto t : tmesh) {
      //auto & g = acc.get_work_var();
      g[t] += step_size * (2. * distr(gen) - 1.);
    }

    std::cout << "after  :" <<  g[0] << "\n";
    
    acc << g;
  }

  auto auto_correlation_time_vector = reduce(acc.auto_correlation());
  
  for( auto [n, tau] : enumerate(auto_correlation_time_vector) ) {
    std::cout << n << "\n";
    std::cout << tau.data() << "\n";
  }
}

MAKE_MAIN;
