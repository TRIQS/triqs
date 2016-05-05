#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;

TEST(Gf, AutoAssignMatrixGf2) {

 double beta = 2.3;
 auto g2 = gf<cartesian_product<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 10}, {beta, Fermion, 10}}, {2, 2}};

 placeholder<0> i_;
 placeholder<1> j_;
 placeholder<3> om_;
 placeholder<4> nu_;

 g2(om_, nu_)(i_, j_) << i_ + j_ + 2 * om_ - 0.3 * nu_;

 // CHECK
 for (int i = 0; i < 2; ++i)
  for (int j = 0; j < 2; ++j)
   for (int om = 0; om < 10; ++om)
    for (int nu = 0; nu < 10; ++nu) {
     auto xom = ((2 * om + 1) * M_PI * 1_j / beta);
     auto xnu = ((2 * nu + 1) * M_PI * 1_j / beta);
     EXPECT_CLOSE(g2.data()(10+om, 10 +nu, i, j), i + j + 2 * xom - 0.3 * xnu);
    }
}

MAKE_MAIN;

