
#include <cmath>
#include <random>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics/accumulator.hpp>
#include <triqs/gfs.hpp>

using namespace triqs;
using namespace triqs::stat;
using namespace triqs::utility;

int seed = 1567;
triqs::mpi::communicator world;

// ----- TESTS ------------------

TEST(Stat, AutoCorrDoubleManualCheck) {

  using namespace triqs::gfs;
  int N = 1000;

  int n_log_bins = 10;
  accumulator<double> AA(0.0, n_log_bins); // no linear bins

  {
    long c = 0;
    std::mt19937 gen(seed);
    std::normal_distribution<double> distr;
    for (long i = 1; i <= N; ++i) {
      auto x = distr(gen);
      AA << x;
    }
  }

  // manual check
  long c = 0;
  std::vector<double> bins(n_log_bins, 0);
  for (int b = 0; b < 10; ++b) {
    int B         = (1 << b);
    double acc    = 0;
    int n         = 0;
    double sum_xi = 0, sum_xi2 = 0;
    std::mt19937 gen(seed);
    std::normal_distribution<double> distr;
    c =0;
    for (long i = 1; i <= N; ++i) {
      auto x = distr(gen);
      acc += x;
      if (i % B == 0) {
        acc /= B;
        sum_xi += acc;
        sum_xi2 += acc * acc;
        acc = 0;
        n++;
      }
    }
    sum_xi /= n;
    sum_xi2 /= n;
    bins[b] = sum_xi2 - sum_xi * sum_xi;
    bins[b] /= n - 1;
    if (n == 1) bins[b] = 0; //corner case
  }

  auto variances = AA.auto_corr_variances(world);

  for (auto [n, b] : enumerate(variances)) {
    EXPECT_NEAR(bins[n], b, 1.e-15);
  }
}

// ------------------------

// test used in the Python test.

triqs::arrays::array<double, 1> f(int N, int seed) {
  int n_log_bins = 20;
  accumulator<double> AA(0, n_log_bins);

  // the number series
  double correlation_length = 100;
  double f                  = exp(-1. / correlation_length);
  std::mt19937 gen(seed);
  std::normal_distribution<double> distr;

  // produce the number and fill AA
  double B = distr(gen);
  AA << B;
  for (size_t i = 1; i < N; i++) {
    B = f * B + sqrt(1 - f * f) * distr(gen);
    AA << B;
  }

  // estimates of tau in an array
  triqs::arrays::array<double, 1> R(n_log_bins);
  auto variances = AA.auto_corr_variances(world);

  for (auto n : range(n_log_bins)) {
    R(n) = tau_estimates(variances, n);
   // std::cout << std::setprecision(16) << R(n) << ",\n";
  }
  return R;
}

// Pure non regression for f.
// The correct test is to use the notebook to plot it and check it ...
TEST(Statistics, Autocorrelation1) {

  array<double, 1> ref{0.0,
                       0.4950005239857558,
                       1.475123545342739,
                       3.396281611622532,
                       7.088413844139182,
                       13.90722680534667,
                       25.57423684228299,
                       42.98716259789485,
                       63.42456878696709,
                       80.62025970970781,
                       95.28358485415579,
                       98.49664895527435,
                       97.13672378798729,
                       88.42310405734598,
                       93.43126509720925,
                       109.0993086536505,
                       134.0631425516443,
                       179.0800626286223,
                       355.2173638000497,
                       231.6692554974257};

  EXPECT_ARRAY_NEAR(ref, f((1 << 20), 100), 1.e-10);
}

// ----- TESTS ------------------

TEST(Stat, LinBinCompress) {

  int bin_size = 100;
  int N        = 1000;

  accumulator<double> AA(0.0, 0, 0, bin_size);
  std::vector<double> bins(N / bin_size + 1, 0);

  {
    for (long i = 0; i < N; ++i) {
      auto x = 1; // i; //generator();
      AA << x;
    }
  }

  {
    for (int i = 0; i < N; ++i)
      bins[i / 100] += 1; // i; //generator();
                          //bins[b] /= (N / n_bins);

    //AA.print();
    //for (auto [n, b] : enumerate(AA.bins())) { EXPECT_NEAR(bins[n], b, 1.e-15); }
  }
}

//------------------------------------

TEST(Stat, LogBinArray) {

  using A = array<double, 2>;

  A a0{{1, 2}, {2, 1}};

  auto zero = a0;
  zero      = 0;

  accumulator<double> b1{0.0, 10};
  accumulator<double> b2{0.0, 10};
  accumulator<A> b{zero, 10};
  accumulator<A> bb{zero, 10};

  for (int i = 0; i <= 100; ++i) {
    b1 << i;
    b2 << 2 * i;
    b << i * a0;

    bb(0, 0) += i * 1;
    bb(0, 1) += i * 2;
    bb(1, 0) += i * 2;
    bb(1, 1) += i * 1;
    bb.advance();
  }

  for (auto [x1, x2, a] : triqs::utility::zip(b1.auto_corr_variances(world), b2.auto_corr_variances(world), b.auto_corr_variances(world))) {
    EXPECT_NEAR(x1, a(0, 0), 1.e-15);
    EXPECT_NEAR(x2, a(0, 1), 1.e-15);
    EXPECT_NEAR(x1, a(1, 1), 1.e-15);
    EXPECT_NEAR(x2, a(1, 0), 1.e-15);
  }
  
  for (auto [x, y ] : triqs::utility::zip(bb.auto_corr_variances(world), b.auto_corr_variances(world))) {
    EXPECT_ARRAY_NEAR(x, y, 1.e-15);
  }

  auto f = h5::file("auto_corr_array.h5", 'w');
  h5_write(f, "scalar", b1);
  h5_write(f, "array", b);
}

//------------------------------------

TEST(Stat, LinBinArray) {

  using A = array<double, 2>;

  A a0{{1, 2}, {2, 1}};
  auto zero = a0;
  zero      = 0;

  accumulator<double> b1{0.0, 0, 10};
  accumulator<double> b2{0.0, 0, 10};
  accumulator<A> b{zero, 0, 10};

  for (int i = 0; i <= 100; ++i) {
    b1 << i;
    b2 << 2 * i;
    b << i * a0;
  }

  for (auto [x1, x2, a] : triqs::utility::zip(b1.linear_bins(), b2.linear_bins(), b.linear_bins())) {
    EXPECT_NEAR(x1, a(0, 0), 1.e-15);
    EXPECT_NEAR(x2, a(0, 1), 1.e-15);
    EXPECT_NEAR(x1, a(1, 1), 1.e-15);
    EXPECT_NEAR(x2, a(1, 0), 1.e-15);
  }
}

MAKE_MAIN;
