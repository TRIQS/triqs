#include <limits>
#include <triqs/stat/mean_error.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/arrays.hpp>
#include <itertools/itertools.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>
#include <mpi/mpi.hpp>
#include <cmath>
#include <random>

using namespace triqs::stat::details;
using nda::conj;

mpi::communicator c;

TEST(Stat, ReduceSingleMQ) {
  log_binning<double> logbin_i{0.0, 2};
  log_binning<double> logbin_all{0.0, 2};

  int N = 2048;

  std::mt19937 gen(123);
  std::normal_distribution<double> distr{1.0, 2.0};

  for (long i = 0; i < N; ++i) {
    auto x = distr(gen);
    logbin_all << double(x);
    if (c.size() * i / N == c.rank()) { logbin_i << x; }
  }

  auto [M, Q, count_k] = mpi_reduce_MQ(logbin_i.Mk.at(0), logbin_i.Qk.at(0), logbin_i.count, c);

  if (c.rank() == 0) {
    EXPECT_NEAR(M, logbin_all.Mk.at(0), logbin_all.Mk.at(0) * std::numeric_limits<double>::epsilon() * 20);
    EXPECT_NEAR(Q, logbin_all.Qk.at(0), logbin_all.Qk.at(0) * std::numeric_limits<double>::epsilon() * 20);
    EXPECT_EQ(count_k, logbin_all.count);
  }

  auto [Mk1, Qk1, count_k1] = mpi_reduce_MQ(logbin_i.Mk.at(1), logbin_i.Qk.at(1), logbin_i.count >> 1, c);

  if (c.rank() == 0) {
    EXPECT_NEAR(Mk1, logbin_all.Mk.at(1), logbin_all.Mk.at(1) * std::numeric_limits<double>::epsilon() * 20);
    EXPECT_NEAR(Qk1, logbin_all.Qk.at(1), logbin_all.Qk.at(1) * std::numeric_limits<double>::epsilon() * 20);
    EXPECT_EQ(count_k1, logbin_all.count / 2);
  }
}

MAKE_MAIN;
