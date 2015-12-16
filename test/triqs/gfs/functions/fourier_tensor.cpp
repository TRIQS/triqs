#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs/functions/fourier_tensor.hpp>

using namespace triqs::gfs;
TEST(Fourier, Tensor_valued){

 tail t(1,1);
 t(1)=1;

 array<tail, 3> tail_3(1,1,1); 
 tail_3() = tail(1,1);
 tail_3() = t;

 gf<imfreq, tensor_valued<3>, nothing> gw({1, Fermion, 300},{1,1,1});
 placeholder<0> om_;
 gw(om_)<<1/(om_-1);
 auto gt = inverse_fourier(gw, tail_3, 1000);

 t(1)=0;
 auto gw2 = fourier(gt,tail_3, 300);

 EXPECT_CLOSE_ARRAY(gw.data(), gw2.data());
}

MAKE_MAIN;
