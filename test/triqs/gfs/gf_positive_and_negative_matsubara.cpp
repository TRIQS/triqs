#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Density) {

 double beta = 1;
 int n_im_freq = 400;
 auto G = gf<imfreq, scalar_valued>{{beta, Fermion, n_im_freq}};

 EXPECT_EQ(G.mesh().positive_only(), 0);

 triqs::clef::placeholder<0> om_;
 G(om_) << 1 / (om_ + 2.3);

 auto n = triqs::gfs::density(G);

 EXPECT_NEAR_COMPLEX(n, 1/(1+std::exp(-beta * 2.3)), 1.e-9);

 rw_h5(G);
}
MAKE_MAIN;

