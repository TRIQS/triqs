#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;

TEST(Gf, Exceptions) {

  double beta = 10.0;

  auto g = gf<imtime, scalar_valued>{};

  EXPECT_THROW(g[closest_mesh_pt(5.3)], triqs::arrays::key_error);
}
MAKE_MAIN;
