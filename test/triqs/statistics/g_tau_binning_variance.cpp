
#include <random>

#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics/accumulator.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::stat;
using namespace triqs::utility;
using namespace triqs::gfs;
using namespace triqs;

triqs::mpi::communicator world;

// ----- TESTS ------------------

// Use uncorrelated samples from a normal distribution
// with known mean and standard deviation
// and compute the mean and standard deviation using
// the triqs::stat::acc_t

TEST(accumulator, g_tau_binning_mean_stddev) {

  using T     = gf<imtime>;
  using obs_t = mc_value<T>;
  using acc_t = accumulator<T>;

  auto zero  = T{{1, Fermion, 2}, {1, 1}};
  auto tmesh = zero.mesh();

  zero = 0;

  bool average = false, variance = true;
  accumulator_cargs acc_args{average, variance};
  acc_t acc{zero, acc_args};

  int seed = 1337;
  std::mt19937 gen(seed);
  double mean = 1.337, stddev = 0.1337;
  std::normal_distribution<double> distr(mean, stddev);

  auto g_markow(zero);

  int N = 1 << 10;
  std::cout << "number of samples N = " << N << "\n";
  
  for (auto iter : range(N)) {
    auto & g = acc.get_work_var(); // make sure to use the work_var func
    for (auto t : tmesh) g[t] = distr(gen);
    acc << g;
  }

  auto [opt_gf_mean, opt_gf_var, opt_auto_corr] = reduce(acc);

  auto meas_mean = (*opt_gf_mean)[0](0,0).real();
  auto meas_var = (*opt_gf_var)[0](0,0).real();
  auto meas_stddev = sqrt(meas_var);
  
  std::cout << "mean = " << mean << "\n";
  std::cout << "stddev = " << stddev << "\n";
  std::cout << "meas_mean = " << meas_mean << "\n";
  std::cout << "meas_var = " << meas_var << "\n";
  std::cout << "meas_stddev = " << meas_stddev << "\n";

  double eps = 1e-2;
  EXPECT_NEAR(mean, meas_mean, eps);  
  EXPECT_NEAR(stddev, meas_stddev, eps);  
}

// ----- TESTS ------------------

TEST(accumulator, g_tau_binning) {

  using T     = double;
  using obs_t = mc_value<T>;
  using acc_t = accumulator<T>;

  double zero = 0;

  int N = 1 << 25;
  std::cout << "number of samples N = " << N << "\n";

  bool average = false, variance = true;
  int auto_correlation_n_bins = 20;
  accumulator_cargs acc_args{average, variance, auto_correlation_n_bins};
  acc_t acc{zero, acc_args};

  double correlation_length = 50;
  
  int seed = 1337;
  std::mt19937 gen(seed);
  //std::normal_distribution<double> distr;

  boost::variate_generator<boost::mt19937, boost::normal_distribution<>> distr((boost::mt19937(seed)), (boost::normal_distribution<>()));

  double f = exp(-1. / correlation_length);  

  auto g_markow(zero);
  
  for (auto iter : range(N)) {

    //g_markow = f * g_markow + sqrt(1 - f*f) * distr(gen);    
    g_markow = f * g_markow + sqrt(1 - f*f) * distr();
    acc << g_markow;
    
  }

  auto auto_correlation_time_vector = reduce(acc.auto_correlation());
  
  for( auto [n, tau] : enumerate(auto_correlation_time_vector) ) {
    std::cout << n << " -- " << float(1<<n) << " -- " << tau << "\n";
  }
}
 
MAKE_MAIN;
