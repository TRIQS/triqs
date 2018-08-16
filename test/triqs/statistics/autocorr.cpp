#include <cmath>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics/accumulator.hpp>
#include <boost/version.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
using namespace triqs::stat;
using namespace triqs;
using namespace boost;
using namespace triqs::utility;

// Some Correlated gaussian
template <typename TimeSeries> void boost_independent_gaussian_vector(TimeSeries &t, int seed) {
  boost::variate_generator<boost::mt19937, boost::normal_distribution<>> generator((boost::mt19937(seed)), (boost::normal_distribution<>()));
  for (size_t i = 0; i < t.size(); ++i) t[i] = generator();
}

template <typename TimeSeries> void correlated_gaussian_vector(TimeSeries &t, int seed, size_t correlation_length) {
  boost_independent_gaussian_vector(t, seed);
  TimeSeries B(t.size());
  B[0]     = t[0];
  double f = exp(-1. / correlation_length);
  for (size_t i = 1; i < t.size(); i++) B[i] = f * B[i - 1] + sqrt(1 - f * f) * t[i];
  t = B;
}

template <typename TimeSeries> void correlated_gaussian_vector(TimeSeries &t, int seed, size_t correlation_length, double avg) {
  boost_independent_gaussian_vector(t, seed);
  TimeSeries B(t.size());
  B[0]     = t[0];
  double f = exp(-1. / correlation_length);
  for (size_t i = 1; i < t.size(); i++) B[i] = f * B[i - 1] + sqrt(1 - f * f) * t[i];
  t = B;
  for (size_t i = 1; i < t.size(); i++) t[i] = t[i] + avg;
}

int seed = 1567;
triqs::mpi::communicator world;

// ----- TESTS ------------------

TEST(Stat, AutoCorrMechanics) {

  using namespace triqs::gfs;
  stacked_gf<gf<imfreq>> S;
  auto SS = S;

  int n_bins = 10;
  accumulators::auto_correlation<double> AA(n_bins);

  std::vector<double> bins(n_bins);
  int N = 1000;

  {
    boost::variate_generator<boost::mt19937, boost::normal_distribution<>> generator((boost::mt19937(seed)), (boost::normal_distribution<>()));
    for (long i = 1; i <= N; ++i) {
      auto x = generator();
      AA << x;
    }
  }

  for (int b = 0; b < 10; ++b) {
    int B         = (1 << b);
    double acc    = 0;
    int n         = 0;
    double sum_xi = 0, sum_xi2 = 0;
    boost::variate_generator<boost::mt19937, boost::normal_distribution<>> generator((boost::mt19937(seed)), (boost::normal_distribution<>()));
    for (long i = 1; i <= N; ++i) {
      auto x = generator();
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
  }

  auto variances = mpi_reduce(AA, world);

  for (auto [n, b] : enumerate(variances)) { EXPECT_NEAR(bins[n], b, 1.e-15); }
}

// ------------------------

// test used in the Python test.

triqs::arrays::array<double, 1> f(int N, int seed) {
  int n_bins = 20;
  accumulators::auto_correlation<double> AA(0, n_bins);

  // the number series
  double correlation_length = 100;
  boost::variate_generator<boost::mt19937, boost::normal_distribution<>> generator((boost::mt19937(seed)), (boost::normal_distribution<>()));
  double f = exp(-1. / correlation_length);

  // produce the number and fill AA
  double B = generator();
  AA << B;
  for (size_t i = 1; i < N; i++) {
    B = f * B + sqrt(1 - f * f) * generator();
    AA << B;
  }

  // estimates of tau in an array
  triqs::arrays::array<double, 1> R(n_bins);
  auto variances = mpi_reduce(AA, world);
  for (auto n : range(n_bins)) R(n) = tau_estimates(variances, n);
  return R;
}

// Pure non regression for f.
// The correct test is to use the notebook to plot it and check it ...
TEST(Statistics, Autocorrelation1) {

  array<double, 1> ref{0.0,
                       0.4950876953783262,
                       1.4756091996287506,
                       3.3987263640872767,
                       7.0981428852990414,
                       13.946373448354995,
                       25.745329467881898,
                       43.300286069894909,
                       63.316766187196883,
                       82.667762720744449,
                       96.266732250253796,
                       95.905703723387447,
                       95.701569095843197,
                       91.459465310338089,
                       97.070597387268876,
                       86.248202610154976,
                       92.957897165303081,
                       108.83889189238811,
                       85.814470300949424,
                       -0.24081441247234747};

  EXPECT_ARRAY_NEAR(ref, f((1 << 20), 100), 1.e-10);
}

// ----- TESTS ------------------

TEST(Stat, Binned) {

  int bin_size = 100;
  int N        = 1000;

  accumulators::binned<double> AA(0.0, bin_size, 5);
  std::vector<double> bins(N / bin_size + 1, 0);

  {
    boost::variate_generator<boost::mt19937, boost::normal_distribution<>> generator((boost::mt19937(seed)), (boost::normal_distribution<>()));
    for (long i = 0; i < N; ++i) {
      auto x = 1; // i; //generator();
      AA << x;
    }
  }

  {
    boost::variate_generator<boost::mt19937, boost::normal_distribution<>> generator((boost::mt19937(seed)), (boost::normal_distribution<>()));

    for (int i = 0; i < N; ++i)
      bins[i / 100] += 1; // i; //generator();
                          //bins[b] /= (N / n_bins);

    AA.print();
    //for (auto [n, b] : enumerate(AA.bins())) { EXPECT_NEAR(bins[n], b, 1.e-15); }
  }
}

//------------------------------------

TEST(AutoCorr, array) {

  using A = array<double, 2>;

  A a0{{1, 2}, {2, 1}};

  auto zero = a0;
  zero      = 0;

  accumulators::auto_correlation<double> b1{0.0, 10};
  accumulators::auto_correlation<double> b2{0.0, 10};
  accumulators::auto_correlation<A> b{zero, 10};

  for (int i = 0; i <= 100; ++i) {
    b1 << i;
    b2 << 2 * i;
    b << i * a0;
  }

  for (auto [x1, x2, a] : triqs::utility::zip(reduce(b1), reduce(b2), reduce(b))) {
    if (!std::isfinite(x1)) continue;
    EXPECT_NEAR(x1, a(0, 0), 1.e-15);
    EXPECT_NEAR(x2, a(0, 1), 1.e-15);
    EXPECT_NEAR(x1, a(1, 1), 1.e-15);
    EXPECT_NEAR(x2, a(1, 0), 1.e-15);
  }

  for (auto [x1, x2, a] : triqs::utility::zip(mpi_reduce(b1, world), mpi_reduce(b2, world), mpi_reduce(b, world))) {
    if (!std::isfinite(x1)) continue;
    EXPECT_NEAR(x1, a(0, 0), 1.e-15);
    EXPECT_NEAR(x2, a(0, 1), 1.e-15);
    EXPECT_NEAR(x1, a(1, 1), 1.e-15);
    EXPECT_NEAR(x2, a(1, 0), 1.e-15);
  }

  for (auto [x1, x2, a] : triqs::utility::zip(mpi_all_reduce(b1, world), mpi_all_reduce(b2, world), mpi_all_reduce(b, world))) {
    if (!std::isfinite(x1)) continue;
    EXPECT_NEAR(x1, a(0, 0), 1.e-15);
    EXPECT_NEAR(x2, a(0, 1), 1.e-15);
    EXPECT_NEAR(x1, a(1, 1), 1.e-15);
    EXPECT_NEAR(x2, a(1, 0), 1.e-15);
  }

  auto f = h5::file("auto_corr_array.h5", 'w');
  h5_write(f, "scalar", b1);
  h5_write(f, "array", b);
}

TEST(Binned, array) {

  using A = array<double, 2>;

  A a0{{1, 2}, {2, 1}};
  auto zero = a0;
  zero      = 0;

  accumulators::binned<double> b1{0.0, 10, 0};
  accumulators::binned<double> b2{0.0, 10, 0};
  accumulators::binned<A> b{zero, 10, 0};

  for (int i = 0; i <= 100; ++i) {
    b1 << i;
    b2 << 2 * i;
    b << i * a0;
  }

  for (auto [x1, x2, a] : triqs::utility::zip(reduce(b1), reduce(b2), reduce(b))) {
    if (!std::isfinite(x1)) continue;
    EXPECT_NEAR(x1, a(0, 0), 1.e-15);
    EXPECT_NEAR(x2, a(0, 1), 1.e-15);
    EXPECT_NEAR(x1, a(1, 1), 1.e-15);
    EXPECT_NEAR(x2, a(1, 0), 1.e-15);
  }
}

MAKE_MAIN;
