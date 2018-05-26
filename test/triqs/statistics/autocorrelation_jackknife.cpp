#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics.hpp>
#include <boost/version.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
using uint = unsigned int;
using namespace triqs::statistics;
using namespace boost;

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

// ----- TESTS ------------------

TEST(Statistics, Simple) {
  // very simple test to test that everything works

  observable<double> X;
  X << 1.0;
  X << -1.0;
  X << .5;
  X << .0;
  std::cout << average_and_error(X) << std::endl;
  std::cout << average_and_error(X * X) << std::endl;
}

// ------------------------

TEST(Statistics, Autocorrelation1) {

  int N = 100000, L = 100;
  int seed = 1567;
  std::vector<double> A(N);
  correlated_gaussian_vector(A, seed, L);
  double intrinsic_variance = 1;

  // normal distribution has changed in boost after 1.55, so results will be differents

#if BOOST_VERSION > 106300
  EXPECT_EQ(autocorrelation_time(A), 121);
  EXPECT_CLOSE(autocorrelation_time_from_binning(A, intrinsic_variance), 116.0001586780634);
  EXPECT_CLOSE(autocorrelation_time_from_binning(A), 91.836181159626847);
#elif BOOST_VERSION > 105500
  EXPECT_EQ(autocorrelation_time(A), 119);
  EXPECT_CLOSE(autocorrelation_time_from_binning(A, intrinsic_variance), 114.71362779226516);
  EXPECT_CLOSE(autocorrelation_time_from_binning(A), 92.781874985705826);
#else
  EXPECT_EQ(autocorrelation_time(A), 89);
  EXPECT_CLOSE(autocorrelation_time_from_binning(A, intrinsic_variance), 80.280569756204429);
  EXPECT_CLOSE(autocorrelation_time_from_binning(A), 95.267077422645201);
#endif
}

// ------------------------

TEST(Statistics, Autocorrelation2) {

  int N = 100000, L = 40;
  int seed   = 1567;
  double avg = 2;
  std::vector<double> A(N);
  correlated_gaussian_vector(A, seed, L, avg);

  observable<double> V;
  for (auto &x : A) V << x;
#if BOOST_VERSION > 106300
  EXPECT_EQ(autocorrelation_time(V), 47);
  EXPECT_CLOSE(autocorrelation_time_from_binning(V), 36.600655522946184);
  EXPECT_CLOSE(autocorrelation_time(V * V), 44);
#elif BOOST_VERSION > 105500
  EXPECT_EQ(autocorrelation_time(V), 47);
  EXPECT_CLOSE(autocorrelation_time_from_binning(V), 38.56211345863931);
  EXPECT_CLOSE(autocorrelation_time(V * V), 44);
#else
  EXPECT_EQ(autocorrelation_time(V), 40);
  EXPECT_CLOSE(autocorrelation_time_from_binning(V), 37.50308656560204);
  EXPECT_CLOSE(autocorrelation_time(V * V), 37);
#endif
}

// ------------------------

///construct correlated series a of expectation value 10 and compute average/error bar
TEST(Statistics, Simple2) {
  int corr_length = 400; //correlation length
  std::vector<double> a(4000000);

  variate_generator<mt19937, normal_distribution<>> generator(mt19937(100405), normal_distribution<>());

  a[0]     = generator();
  double f = exp(-1. / corr_length);
  for (int i = 1; i < a.size(); i++) a[i] = f * a[i - 1] + sqrt(1 - f * f) * generator();

  for (auto &x : a) x += 10;

  //std::cout << "tau from autocorr. function  = " << autocorrelation_time(a) << std::endl;
  std::cout << "tau from binning  = " << autocorrelation_time_from_binning(a) << std::endl;

  //fill observable with the series
  observable<double> A;
  for (auto const &x : a) A << x;

  std::cout << "average of a = " << average_and_error(A) << std::endl;
  std::cout << "average of a*a = " << average_and_error(A * A) << std::endl;
}

MAKE_MAIN;
