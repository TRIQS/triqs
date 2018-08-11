#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::clef;
using namespace triqs::lattice;

TEST(GfMeshProd, Prod) {

  gf_mesh<imfreq> m{1, Fermion, 10};
  gf_mesh<imtime> p{1, Fermion, 10};

  gf_mesh<cartesian_product<imfreq, imfreq>> m2{m, m};
  gf_mesh<cartesian_product<imfreq, imfreq, imfreq>> m3{m, m, m};
  gf_mesh<cartesian_product<imfreq, imfreq, imfreq, imfreq>> m4{m, m, m, m};
  gf_mesh<cartesian_product<imfreq, imfreq, imfreq, imfreq, imfreq>> m5{m, m, m, m, m};

  gf_mesh<cartesian_product<imfreq, imtime>> mp{m, p};
  gf_mesh<cartesian_product<imtime, imfreq>> pm{p, m};
  gf_mesh<cartesian_product<imfreq, imtime, imfreq>> mpm{m, p, m};
  gf_mesh<cartesian_product<imfreq, imfreq, imtime, imfreq>> mmpm{m, m, p, m};
  gf_mesh<cartesian_product<imfreq, imfreq, imtime, imfreq, imtime>> mmpmp{m, m, p, m, p};

  EXPECT_EQ(m2, m * m);
  EXPECT_EQ(m4, m2 * m2);
  EXPECT_EQ(m3, m * m2);
  EXPECT_EQ(m3, m2 * m);
  EXPECT_EQ(m5, m2 * m * m * m);
  EXPECT_EQ(m5, m * m2 * m2);

  EXPECT_EQ(mp, m * p);
  EXPECT_EQ(mmpm, m2 * pm);
  EXPECT_EQ(mpm, mp * m);
  EXPECT_EQ(mpm, m * pm);
  EXPECT_EQ(mmpmp, m * mp * mp);
}
MAKE_MAIN;
