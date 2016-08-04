#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;
TEST(Fourier, Tensor_valued){

 gf<imfreq, tensor_valued<3>> gw({1, Fermion, 300}, {1, 1, 1});
 placeholder<0> om_;

 gw(om_) << 1 / (om_ - 1);

 auto gt = make_gf_from_inverse_fourier(gw, 1000);

 auto gw2 = gw;
 gw2() = fourier(gt);

 EXPECT_CLOSE_ARRAY(gw.data(), gw2.data());
}

MAKE_MAIN;
