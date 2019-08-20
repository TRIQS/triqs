#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::clef;
using namespace triqs::lattice;

TEST(GfMeshProd, Prod) {

  mesh::imfreq m{1, Fermion, 10};
  mesh::imtime p{1, Fermion, 10};

  mesh::prod<imfreq, imfreq> m2{m, m};
  mesh::prod<imfreq, imfreq, imfreq> m3{m, m, m};
  mesh::prod<imfreq, imfreq, imfreq, imfreq> m4{m, m, m, m};
  mesh::prod<imfreq, imfreq, imfreq, imfreq, imfreq> m5{m, m, m, m, m};

  mesh::prod<imfreq, imtime> mp{m, p};
  mesh::prod<imtime, imfreq> pm{p, m};
  mesh::prod<imfreq, imtime, imfreq> mpm{m, p, m};
  mesh::prod<imfreq, imfreq, imtime, imfreq> mmpm{m, m, p, m};
  mesh::prod<imfreq, imfreq, imtime, imfreq, imtime> mmpmp{m, m, p, m, p};

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
