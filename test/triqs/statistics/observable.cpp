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

  accumulator_cargs p{false, true, 10, 100};
  acc_t a{p};

  for (int i = 0; i <= 10; ++i) a << i;

  auto obs = mpi_reduce(a, world);
  EXPECT_NEAR(obs.value.value(), 5, 1.e-15);

  {
    auto f = h5::file("obs_d.h5", 'w');
    h5_write(f, "acc", a);
    h5_write(f, "obs", obs);
  }

  auto a_b   = acc_t{};
  auto obs_b = obs_t{};
  {
    auto f = h5::file("obs_d.h5", 'r');
    h5_read(f, "acc", a_b);
    h5_read(f, "obs", obs_b);
  }

  {
    auto f = h5::file("obs_db.h5", 'w');
    h5_write(f, "acc", a_b);
    h5_write(f, "obs", obs_b);
  }
}
//----------------------------------------

TEST(Observable, hdf5Array) {

  using A = array<double, 2>;
  A a0{{1, 2}, {2, 1}};
  auto zero = a0;
  zero()    = 0;

  using obs_t = mc_value<A>;
  using acc_t = accumulator<A>;

  accumulator_cargs p{false, true, 10, 100};
  acc_t a{zero, p};

  for (int i = 0; i <= 10; ++i) a << i;

  auto obs = mpi_reduce(a, world);

  auto f = h5::file("obs_array.h5", 'w');
  h5_write(f, "obs", a);
  h5_write(f, "obs", obs);

  {
    auto f = h5::file("obs_a.h5", 'w');
    h5_write(f, "acc", a);
    h5_write(f, "obs", obs);
  }

  auto a_b   = acc_t{};
  auto obs_b = obs_t{};
  {
    auto f = h5::file("obs_a.h5", 'r');
    h5_read(f, "acc", a_b);
    h5_read(f, "obs", obs_b);
  }
  {
    auto f = h5::file("obs_ab.h5", 'w');
    h5_write(f, "acc", a_b);
    h5_write(f, "obs", obs_b);
  }
}

//----------------------------------------

TEST(Observable, Gf) {

  using obs_t = mc_value<gf<imfreq>>;
  using acc_t = accumulator<gf<imfreq>>;

  auto zero = gf<imfreq>{{1, Fermion, 10}, {2, 2}};
  zero      = 0;
  auto g    = zero;

  accumulator_cargs p{false, true, 10, 100};
  acc_t a{zero, p};

  for (int i = 0; i <= 10; ++i)
    for (int j = 0; j <= 10; ++j) a << g;

  auto obs = mpi_reduce(a, world);

  {
    auto f = h5::file("obs_gf.h5", 'w');
    h5_write(f, "acc", a);
    h5_write(f, "obs", obs);
  }

  auto a_b   = acc_t{};
  auto obs_b = obs_t{};
  {
    auto f = h5::file("obs_gf.h5", 'r');
    h5_read(f, "acc", a_b);
    h5_read(f, "obs", obs_b);
  }
  {
    auto f = h5::file("obs_gfb.h5", 'w');
    h5_write(f, "acc", a_b);
    h5_write(f, "obs", obs_b);
  }
}

MAKE_MAIN;
