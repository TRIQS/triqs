#include "./common.hpp"

TEST(Gf, ImfreqPositiveFreqOrNot) {

 double beta = 1;
 int n_im_freq = 400;
 auto G = gf<imfreq, scalar_valued>{{beta, Fermion, n_im_freq, false}};
 auto G_pos_only = gf<imfreq, scalar_valued>{{beta, Fermion, n_im_freq, true}};

 EXPECT_EQ(G.mesh().positive_only(), 0);
 EXPECT_EQ(G_pos_only.mesh().positive_only(), 1);

 triqs::clef::placeholder<0> om_;
 G(om_) << 1 / (om_ + 2.3);
 G_pos_only(om_) << 1 / (om_ + 2.3);

 auto n = triqs::gfs::density(G);
 auto n_pos_only = triqs::gfs::density(G_pos_only);

 EXPECT_NEAR(n, n_pos_only, 1.e-9);

 rw_h5(G);
 rw_h5(G_pos_only);
}
MAKE_MAIN;

