#include <triqs/gfs.hpp>
#include <triqs/test_tools/gfs.hpp>
using namespace triqs::gfs;

TEST(Fourier, Block) {
 double beta   = 1;
 int n_iw      = 100;
 int n_tau     = 2 * n_iw + 1;
 auto iw_mesh  = gf_mesh<imfreq>{beta, Fermion, n_iw};
 auto tau_mesh = gf_mesh<imtime>{beta, Fermion, n_tau};
 auto shape    = make_shape(1, 1, 1);

 // -- Init Green functions

 placeholder<0> iw_;
 std::vector<gf<imfreq, tensor_valued<3>>> giw_vec;

 giw_vec.emplace_back(iw_mesh, shape);
 giw_vec[0](iw_) << 1.0 / (iw_ + 1.0);

 giw_vec.emplace_back(iw_mesh, shape);
 giw_vec[1](iw_) << 2.0 / (iw_ - 1.0);

 auto block_giw   = make_block_gf({"one", "two"}, giw_vec);
 auto block_giw_2 = block_giw;

 std::vector<gf<imtime, tensor_valued<3>>> gtau_vec;

 gtau_vec.emplace_back(tau_mesh, shape);
 gtau_vec.emplace_back(tau_mesh, shape);

 auto block_gtau = make_block_gf({"one", "two"}, gtau_vec);

 // -- Perform transforms

 block_gtau()  = inverse_fourier(block_giw);
 block_giw_2() = fourier(block_gtau);

 EXPECT_BLOCK_GF_NEAR(block_giw, block_giw_2);

 // -- Same for block2_gf

 std::vector<std::vector<gf<imfreq, tensor_valued<3>>>> giw_vecvec;
 giw_vecvec.push_back(giw_vec); 
 giw_vecvec.push_back(giw_vec); 

 auto block2_giw   = make_block2_gf({"one", "two"}, {"one", "two"}, giw_vecvec);
 auto block2_giw_2 = block2_giw;

 std::vector<std::vector<gf<imtime, tensor_valued<3>>>> gtau_vecvec;
 gtau_vecvec.push_back(gtau_vec); 
 gtau_vecvec.push_back(gtau_vec); 

 auto block2_gtau = make_block2_gf({"one", "two"}, {"one","two"}, gtau_vecvec);

 // -- Perform transforms

 block2_gtau()  = inverse_fourier(block2_giw);
 block2_giw_2() = fourier(block2_gtau);

 EXPECT_BLOCK2_GF_NEAR(block2_giw, block2_giw_2);
}

MAKE_MAIN;
