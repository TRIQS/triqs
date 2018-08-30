#include <triqs/test_tools/arrays.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs;

TEST(gf, t2) {

  using g_t = gf<imtime, matrix_valued>;

  auto zero  = g_t{{1, Fermion, 10}, {1, 1}};
  zero()     = 0;

  auto gg = zero;

  gg = (zero + zero) / 2.0;
}
MAKE_MAIN;
