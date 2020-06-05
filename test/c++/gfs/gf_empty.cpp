#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;

TEST(Gf, Exceptions) {

  double beta = 10.0;

  auto g = gf<imtime, scalar_valued>{};

  ASSERT_DEATH(g[closest_mesh_pt(5.3)], "");
}
MAKE_MAIN;
