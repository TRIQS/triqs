#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;

TEST(Gf, G_om_nu_nup) {

 double beta = 1;

 auto G = gf<cartesian_product<imfreq, imfreq, imfreq>, tensor_valued<4>>{
     {{beta, Boson, 3}, {beta, Fermion, 10}, {beta, Fermion, 10}}, {2, 2, 2, 2}};

 // try to assign to expression
 placeholder<2> om_;
 placeholder<0> nu_;
 placeholder<1> nup_;

 G(om_, nu_, nup_) << 1 / (nu_ - 2.1 * nup_ + 1.6 * om_ + 1);

 dcomplex res1 = G[{2, 3, 5}](0, 0, 0, 0);
 dcomplex res = 1 / (matsubara_freq { 3, beta, Fermion } - 2.1 * matsubara_freq { 5, beta, Fermion } +
                     1.6 * matsubara_freq { 2, beta, Boson }  + 1);
 EXPECT_CLOSE(res1, res);
 rw_h5(G, "ess_g_om_nu_nup.h5", "g");

 // Curry
 // auto g_cur = curry<0>(G);

 auto g0 = partial_eval<0>(G, 0);

 // No, it is a view...
 //rw_h5(g0);
}
MAKE_MAIN;
