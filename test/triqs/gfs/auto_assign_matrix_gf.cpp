#include <triqs/test_tools/gfs.hpp>
using triqs::clef::placeholder;
using triqs::clef::placeholder_prime;

TEST(Gf, AutoAssignMatrixGf1) {

 double beta = 2.3;
 auto g = gf<imfreq, matrix_valued>({beta, Fermion, 10}, {2, 2});
 auto sig = matrix<double>{{1, -1}, {-1, 1}};
 
 placeholder_prime<0> om_;
 placeholder<1> u_;
 placeholder<2> v_;

 g[om_](u_, v_) << 1 / (om_ - 3) * sig(u_, v_);

 // CHECK 
 for (int om = -10; om < 10; ++om) EXPECT_CLOSE_ARRAY(g[om](), sig / ((2 * om + 1) * M_PI * 1_j / beta - 3));
}

// ---------------------------------

TEST(Gf, AutoAssignMatrixGf2) {

 double beta = 2.3;
 auto a = array<gf<imfreq, scalar_valued>, 2>(2, 2);
 auto sig = matrix<double>{{1, -1}, {-1, 1}};
 
 placeholder_prime<0> om_;
 placeholder<1> u_;
 placeholder<2> v_;

 auto g0 = gf<imfreq, scalar_valued>({beta, Fermion, 10}, {});
 a(u_, v_) << g0;
 a(u_, v_)[om_] << 1 / (om_ - 3) * sig(u_, v_);

 // CHECK
 for (int u = 0; u < 2; ++u)
  for (int v = 0; v < 2; ++v)
   for (int om = -10; om < 10; ++om) EXPECT_CLOSE(a(u,v)[om], sig(u,v) / ((2 * om + 1) * M_PI * 1_j / beta - 3));
}

MAKE_MAIN;
