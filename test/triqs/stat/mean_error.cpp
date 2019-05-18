#include <itertools/itertools.hpp>
#include <triqs/arrays.hpp>
#include <triqs/stat/mean_error.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <typeinfo>

using namespace triqs::stat;
using triqs::arrays::conj_r;

// *****************************************************************************

TEST(Stat, mean_and_err) {
  std::vector<double> data{0., 1., 2., 3.};
  auto [mean, err] = mean_and_err(data);
  EXPECT_EQ(typeid(mean), typeid(static_cast<double>(0.0)));
  EXPECT_EQ(typeid(err), typeid(static_cast<double>(0.0)));

  EXPECT_EQ(mean, 1.5);
  EXPECT_DOUBLE_EQ(err, std::sqrt(2 * (0.5 * 0.5 + 1.5 * 1.5) / 12));
}


MAKE_MAIN;
