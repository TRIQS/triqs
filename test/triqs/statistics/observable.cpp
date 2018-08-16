#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics/accumulator.hpp>
#include <triqs/gfs.hpp>
using namespace triqs::stat;
using namespace triqs::gfs;
using namespace triqs;

triqs::mpi::communicator world;

// ----- TESTS ------------------

TEST(Observable, ConstructDouble) {
  using obs_t = mc_value<double>;
  using acc_t = accumulator<double>;

  int n_log_bins = 10, bin_size = 100, n_lin_bins = -1;
  acc_t a{0, n_log_bins, n_lin_bins, bin_size};

  for (int i = 0; i <= 10; ++i) a << i;

  // FIXME: Does note exist anymore
  // auto av = a.empirical_average(world);
  // EXPECT_NEAR(av, 5, 1.e-15);

  {
    auto f = h5::file("acc_d.h5", 'w');
    h5_write(f, "acc", a);
  }

  auto a_b   = acc_t{};
  {
    auto f = h5::file("acc_d.h5", 'r');
    h5_read(f, "acc", a_b);
  }

  {
    auto f = h5::file("acc_db.h5", 'w');
    h5_write(f, "acc", a_b);
  }
}
//----------------------------------------

TEST(Observable, hdf5Array) {

  using A = array<double, 2>;
  A a0{{1, 2}, {2, 1}};
  auto zero = a0;
  zero()    = 0;

  using acc_t = accumulator<A>;

  int n_log_bins = 10, bin_size = 100, n_lin_bins = -1;
  acc_t a{zero, n_log_bins, n_lin_bins, bin_size};

  for (int i = 0; i <= 10; ++i) a << a0;
  {
    auto f = h5::file("obs_a.h5", 'w');
    h5_write(f, "acc", a);
  }

  auto a_b   = acc_t{};
  {
    auto f = h5::file("obs_a.h5", 'r');
    h5_read(f, "acc", a_b);
  }
  {
    auto f = h5::file("obs_ab.h5", 'w');
    h5_write(f, "acc", a_b);
  }
}

//----------------------------------------
/*
TEST(Observable, Gf) {

  using obs_t = mc_value<gf<imfreq>>;
  using acc_t = accumulator<gf<imfreq, tensor_valued<2>>>;

  auto zero = gf<imfreq>{{1, Fermion, 10}, {2, 2}};
  zero      = 0;
  auto g    = zero;

  int n_log_bins = 10, bin_size = 100, n_lin_bins = -1;
  acc_t a{zero, n_log_bins, n_lin_bins, bin_size};

  for (int i = 0; i <= 10; ++i)
    for (int j = 0; j <= 10; ++j) a << g;

  auto av = a.empirical_average(world);

  {
    auto f = h5::file("obs_gf.h5", 'w');
    h5_write(f, "acc", a);
  }

  auto a_b   = acc_t{};
  {
    auto f = h5::file("obs_gf.h5", 'r');
    h5_read(f, "acc", a_b);
  }
  {
    auto f = h5::file("obs_gfb.h5", 'w');
    h5_write(f, "acc", a_b);
  }
}
*/
MAKE_MAIN;
