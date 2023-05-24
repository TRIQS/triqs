#include <triqs/stat/mean_error.hpp>
#include <triqs/arrays.hpp>
#include <itertools/itertools.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>
#include <mpi/mpi.hpp>

using namespace triqs::stat;
using nda::conj;

// *****************************************************************************

TEST(Stat, mean_only) {
  std::vector<double> data{0., 1., 2., 3.};
  auto ave = mean(data);
  static_assert(std::is_same_v<decltype(ave), double>);
  EXPECT_EQ(ave, 1.5);
}

TEST(Stat, mean_and_err) {
  std::vector<double> data{0., 1., 2., 3.};
  auto [ave, err] = mean_and_err(data);

  static_assert(std::is_same_v<decltype(ave), double>);
  static_assert(std::is_same_v<decltype(err), double>);
  EXPECT_EQ(ave, 1.5);
  EXPECT_DOUBLE_EQ(err, std::sqrt(2 * (0.5 * 0.5 + 1.5 * 1.5) / 12));
}

TEST(Stat, mean_only_mpi) {
  mpi::communicator world;
  int rank = world.rank();
  std::vector<double> data{4. * rank + 1, 4. * rank + 2, 4. * rank + 3, 4. * rank + 4};

  auto ave_serial = mean(data);
  static_assert(std::is_same_v<decltype(ave_serial), double>, "EE");
  EXPECT_EQ(ave_serial, 2.5 + 4 * rank);

  int size     = world.size();
  auto ave_mpi = mean_mpi(world, data);
  static_assert(std::is_same_v<decltype(ave_mpi), double>);
  EXPECT_EQ(ave_mpi, (1.0 + 4.0 * size) / 2.0);
}

TEST(Stat, mean_and_err_mpi) {
  mpi::communicator world;
  int rank = world.rank();
  std::vector<double> data{4. * rank + 1, 4. * rank + 2, 4. * rank + 3, 4. * rank + 4};

  auto [ave_serial, err_serial] = mean_and_err(data);

  static_assert(std::is_same_v<decltype(ave_serial), double>, "EE");

  static_assert(std::is_same_v<decltype(ave_serial), double>);
  static_assert(std::is_same_v<decltype(err_serial), double>);
  EXPECT_EQ(ave_serial, 2.5 + 4 * rank);             // =
  EXPECT_DOUBLE_EQ(err_serial, std::sqrt(5.0 / 12)); // sqrt((n + 1) / 12)

  int size                = world.size();
  auto [ave_mpi, err_mpi] = mean_and_err_mpi(world, data);

  static_assert(std::is_same_v<decltype(ave_mpi), double>);
  static_assert(std::is_same_v<decltype(err_mpi), double>);
  EXPECT_EQ(ave_mpi, (1.0 + 4.0 * size) / 2.0);
  EXPECT_DOUBLE_EQ(err_mpi, std::sqrt((4.0 * size + 1.0) / 12));
}

// Tests with arrays

MAKE_MAIN;
