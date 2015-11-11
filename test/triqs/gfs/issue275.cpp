#include "./common.hpp"

TEST(Gf, Issue275) {

 auto g_half = gf<imfreq,matrix_valued> {{1, Fermion, 3, matsubara_mesh_opt::positive_frequencies_only},{1,1}};
 auto g_full = gf<imfreq,matrix_valued> {{1, Fermion, 6},{1,1}};

 triqs::clef::placeholder<0> om_;
 g_half(om_) << 1/(om_+3);

 auto l = om_ >> (g_half(om_)(0,0));

 for (auto const& w : g_full.mesh()) EXPECT_NEAR_COMPLEX(l(w), g_half(w)(0, 0));
}

MAKE_MAIN;
