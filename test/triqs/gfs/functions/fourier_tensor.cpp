#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;
TEST(Fourier, Tensor_valued){

 gf<imfreq, tensor_valued<3>> gw({1, Fermion, 100}, {1, 1, 1});
 placeholder<0> om_;

 gw(om_) << 1 / (om_ - 1);

 auto gt = make_gf_from_inverse_fourier(gw, 10000);

 auto gw2 = gw;
 gw2() = fourier(gt);

 auto [tail, err] = get_tail(gw);
 auto [tail2, err2] = get_tail(gw2);

 TRIQS_PRINT(tail);
 TRIQS_PRINT(tail2);

 EXPECT_ARRAY_NEAR(gw.data(), gw2.data(), 1e-6);
 EXPECT_ARRAY_NEAR(tail(range(4),ellipsis()), tail2(range(4),ellipsis()), 1e-5);
}

MAKE_MAIN;
