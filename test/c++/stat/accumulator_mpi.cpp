#include <itertools/itertools.hpp>
#include <triqs/arrays.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>

#include <cmath>
#include <random>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::stat;
using namespace triqs::stat::details;

using nda::conj;

using vec_d = std::vector<double>;
using vec_c = std::vector<std::complex<double>>;
using vec_i = std::vector<int>;

mpi::communicator c;

TEST(Stat, Accumulator_LogBinErrors_MPI) {
  int n_log_bins_max = -1, lin_bin_capacity = 0, n_lin_bins_max = 0;
  accumulator<double> my_acc{0.0, n_log_bins_max, n_lin_bins_max, lin_bin_capacity};

  int N = (1024) / c.size();

  std::mt19937 gen(123);
  std::normal_distribution<double> distr{1.0, 2.0};

  // TODO: Error in Discard... Does not correctly align
  for (long i = 0; i < c.rank() * N; ++i) { distr(gen); }
  // gen.discard(c.rank() * N );

  for (long i = 0; i < N; ++i) {
    auto x = distr(gen);
    my_acc << x;
  }

  auto log_errors = my_acc.log_bin_errors_mpi(c);

  if (c.rank() == 0) {

    std::vector<double> results_np1 = {
       0.06756823571, 0.06738503966, 0.0657708692, 0.06329394592, 0.06851719085, 0.0616704721, 0.05894556666,
    };

    for (int n = 0; n < results_np1.size(); n++) { EXPECT_NEAR(log_errors[n], results_np1[n], 1e-8); }
  }
}

// *****************************************************************************

MAKE_MAIN;
