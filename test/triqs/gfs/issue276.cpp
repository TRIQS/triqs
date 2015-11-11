#include "./common.hpp"

TEST(Gf, Issue276) {

 auto g = gf<imfreq, matrix_valued>{{1, Fermion, 6}, {1, 1}};
 auto g2 = gf<imfreq, matrix_valued>{{1, Fermion, 6}, {1, 1}};

 triqs::clef::placeholder<0> om_;
 g(om_) << 1 / (om_ + 3);
 g2(om_) << 1 / (om_ - 3);

 auto gc =g;
 auto gc2 =g2;

 {
  gf_const_view<imfreq> gv = g;
  gf_const_view<imfreq> gv2 = g2;

  EXPECT_GF_NEAR(g, gv);
  EXPECT_GF_NEAR(g2, gv2);

  swap(gv, gv2);

  EXPECT_GF_NEAR(g, gv2);
  EXPECT_GF_NEAR(g2, gv);
 
  EXPECT_GF_NEAR(g, gc);
  EXPECT_GF_NEAR(g2, gc2);
 }

 {
  g(om_) << 1 / (om_ + 3);
  g2(om_) << 1 / (om_ - 3);

  gf_view<imfreq> gv = g;
  gf_view<imfreq> gv2 = g2;

  EXPECT_GF_NEAR(g, gv);
  EXPECT_GF_NEAR(g2, gv2);

  swap(gv, gv2);

  EXPECT_GF_NEAR(g, gv2);
  EXPECT_GF_NEAR(g2, gv);
 }
 
}

MAKE_MAIN;
