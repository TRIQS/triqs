#include <random>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics/accumulator.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::stat;
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
  accumulator_cargs acc_args{average, variance};
  acc_t acc{zero, acc_args};

  int seed = 1337;
  std::mt19937 gen(seed);
  std::uniform_real_distribution<double> distr;

  double step_size = 0.1;

  auto g = T{{1, Fermion, 10}, {1, 1}};

  for (auto iter : range(100)) {

    g = 0;
    
    for (auto t : tmesh) {
      auto & g = acc.get_work_var();
      g[t] += step_size * (2. * distr(gen) - 1.);
      acc << g;
    }
  }
}

MAKE_MAIN;
