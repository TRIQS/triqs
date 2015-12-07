#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;

TEST(Gf, G_nu_nup) {

 double beta = 1;

 auto G = gf<cartesian_product<imfreq, imfreq>, tensor_valued<4>>{{{beta, Fermion, 100}, {beta, Fermion, 100}}, {2, 2, 2, 2},
                                                                  make_memory_layout(0, 2, 3, 1, 4, 5)};

 auto sh = triqs::arrays::memory_layout<6>{triqs::utility::mini_vector<int, 6>{0, 2, 3, 1, 4, 5}};
 EXPECT_EQ(G.data().indexmap().get_memory_layout(), sh);
 
 placeholder<0> nu_;
 placeholder<1> nup_;

 G(nu_, nup_) << 1 / (nu_ + nup_ + 1);

 dcomplex res1 = G[{2, 3}](0,0,0,0);
 dcomplex res = 1 / (matsubara_freq { 2, beta, Fermion } + matsubara_freq { 3, beta, Fermion } + 1);
 EXPECT_CLOSE(res1, res);
 rw_h5(G, "ess_g_nu_nup.h5", "g");
}
MAKE_MAIN;
