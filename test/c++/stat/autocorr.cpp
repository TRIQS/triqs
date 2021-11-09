
#include <cmath>
#include <random>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/stat/accumulator.hpp>
#include <triqs/gfs.hpp>

using namespace triqs;
using namespace triqs::stat;
using namespace triqs::utility;
using namespace nda;

int seed = 1567;
mpi::communicator world;

void expect_near(double x, double y) {
  EXPECT_NEAR(x, y, (1e-14 * std::max(std::abs(x), std::abs(y))));
}

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
    c = 0;
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
    if (n == 1)
      bins[b] = 0; //corner case
    else {
      bins[b] = sum_xi2 - sum_xi * sum_xi;
      bins[b] /= n - 1;
      bins[b] = std::sqrt(bins[b]);
    }
  }

  auto [errors, counts] = AA.log_bin_errors();

  for (auto [n, b] : itertools::enumerate(errors)) { expect_near(bins[n], b); }
}

// ------------------------

// test used in the Python test.

nda::array<double, 1> f(int N, int seed) {
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
  nda::array<double, 1> R(n_log_bins);
  auto [errors, counts] = AA.log_bin_errors();

  for (auto n : range(n_log_bins)) {
    R(n) = tau_estimate_from_errors(errors[n], errors[0]);
    // std::cout << std::setprecision(16) << R(n) << ",\n";
  }
  return R;
}

// Pure non regression for f.
// The correct test is to use the notebook to plot it and check it ...
TEST(Statistics, Autocorrelation1) {

#if (_LIBCPP_VERSION) // Random Number Generator depends on stdlib
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
#else
  array<double, 1> ref{0,
                       0.49500070196566004,
                       1.4751377999469928,
                       3.3963611526851967,
                       7.0886813152148651,
                       13.908706177323447,
                       25.581834487933019,
                       42.999631572543862,
                       63.457704194323313,
                       80.675562273420638,
                       95.324082977480884,
                       98.482210345129545,
                       97.113193039035124,
                       88.263315942316453,
                       93.303452075454317,
                       109.09508192180849,
                       134.71116077156964,
                       180.07504249788664,
                       357.66967972277371,
                       236.44414886907433};
#endif

  EXPECT_ARRAY_NEAR(ref, f((1 << 20), 100), 1.e-9);
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

  A a0{{1., 2.}, {2., 1.}};

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

    A atemp{{1. * i, 2. * i}, {2. * i, 1. * i}};
    bb << atemp;
  }

  auto [errors_b1, counts_b1] = b1.log_bin_errors();
  auto [errors_b2, counts_b2] = b2.log_bin_errors();
  auto [errors_b, counts_b]   = b.log_bin_errors();

  for (auto [x1, x2, a] : itertools::zip(errors_b1, errors_b2, errors_b)) {
    expect_near(x1, a(0, 0));
    expect_near(x2, a(0, 1));
    expect_near(x1, a(1, 1));
    expect_near(x2, a(1, 0));
  }

  auto [errors_bb, counts_bb] = b.log_bin_errors();

  for (auto [x, y] : itertools::zip(errors_bb, errors_b)) { EXPECT_ARRAY_NEAR(x, y, 1.e-15); }

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

  for (auto [x1, x2, a] : itertools::zip(b1.linear_bins(), b2.linear_bins(), b.linear_bins())) {
    expect_near(x1, a(0, 0));
    expect_near(x2, a(0, 1));
    expect_near(x1, a(1, 1));
    expect_near(x2, a(1, 0));
  }
}

MAKE_MAIN;
