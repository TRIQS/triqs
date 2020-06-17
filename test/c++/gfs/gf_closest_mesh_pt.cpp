#include <triqs/test_tools/gfs.hpp>

TEST(Gf, ClosestMeshPoint) {

  double beta = 1;
  double tmax = 1;
  int N       = 12;

  auto g = gf<retime, scalar_valued>{{-tmax, tmax, N}};

  triqs::clef::placeholder<0> t_;
  g(t_) << 1.0 * t_;

  //
  for (auto const &t : g.mesh()) std::cerr << double(t) << std::endl;

  EXPECT_CLOSE(g[closest_mesh_pt(0.1)], 1.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.1)], -1.0 / (N - 1));

  EXPECT_CLOSE(g[closest_mesh_pt(0.00001)], 1.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.00001)], -1.0 / (N - 1));

  EXPECT_CLOSE(g[closest_mesh_pt(0.2)], 3.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.2)], -3.0 / (N - 1));

  EXPECT_CLOSE(g[closest_mesh_pt(0.3)], 3.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.3)], -3.0 / (N - 1));

  EXPECT_CLOSE(g[closest_mesh_pt(0.4)], 5.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.4)], -5.0 / (N - 1));
}
MAKE_MAIN;
