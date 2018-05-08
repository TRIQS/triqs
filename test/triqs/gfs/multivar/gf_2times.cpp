#include <triqs/test_tools/gfs.hpp>

TEST(Gf, TwoTimes) {

 auto m = gf_mesh<retime>{0, 10, 99};
 auto G = gf<cartesian_product<retime, retime>>{{m, m}, {2, 2}};
 auto gg = gf<retime>{{m}, {2, 2}};

 triqs::clef::placeholder<0> t_;
 triqs::clef::placeholder<1> tp_;

 double beta = 1;
 int n_im_freq = 100;

 auto G_w_wn2 = gf<cartesian_product<imfreq, imfreq>>(
     {gf_mesh<imfreq>(beta, Fermion, n_im_freq), gf_mesh<imfreq>(beta, Fermion, n_im_freq)}, {2, 2});

 std::cout << G_w_wn2.mesh();
}
MAKE_MAIN;

