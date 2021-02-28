#include <itertools/itertools.hpp>
#include <numeric>
#include <triqs/arrays.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>

#include <cmath>
#include <random>

// #include <format>

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

TEST(Stat, LogBinErrorsMPI_EqualSize) {

  accumulator<double> acc_i{0.0, -1};
  accumulator<double> acc_all{0.0, -1};

  int N = 1024;

  std::mt19937 gen(123);
  std::normal_distribution<double> distr{1.0, 2.0};

  for (long i = 0; i < N; ++i) {
    auto x = distr(gen);
    acc_all << double(x);
    if (c.size() * i / N == c.rank()) { acc_i << x; }
  }

  auto [errs, counts] = acc_i.log_bin_errors_mpi(c);

  if (c.rank() == 0) {
    auto errs_all = acc_all.log_bin_errors();
    for (int i = 0; i < errs.size(); i++) { EXPECT_NEAR(errs[i], errs_all[i], std::numeric_limits<double>::epsilon() * 10); }
  }
}

TEST(Stat, LogBinErrorsMPI_UnEqualSize) {

  accumulator<double> acc_i{0.0, -1};
  accumulator<double> acc_all{0.0, -1};

  int N1 = 32;
  int N2 = 1024;

  std::mt19937 gen(123);
  std::normal_distribution<double> distr{1.0, 2.0};

  for (long i = 0; i < N2; ++i) {
    auto x = distr(gen);
    acc_all << double(x);
    if (c.rank() == 0) { acc_i << x; }
  }

  for (long i = 0; i < N1; ++i) {
    auto x = distr(gen);
    acc_all << double(x);
    if (c.rank() == 1) { acc_i << x; }
  }

  auto [errs, counts] = acc_i.log_bin_errors_mpi(c);

  if (c.rank() == 0) {
    auto errs_all = acc_all.log_bin_errors();
    for (int i = 0; i < errs.size(); i++) { EXPECT_NEAR(errs[i], errs_all[i], errs[i] * std::numeric_limits<double>::epsilon() * 20); }
  }
}

MAKE_MAIN;
