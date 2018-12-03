#include <triqs/gfs.hpp>
#include <triqs/test_tools/gfs.hpp>
using namespace triqs::gfs;

template <int TARGET_RANK> void test_fourier() {
  double precision = 1e-7;
  triqs::clef::placeholder<0> iw_;
  double beta               = 1;
  int N_iw                  = 1000;
  int N_tau                 = 6 * N_iw + 1;
  std::vector<double> E_vec = {1.0, 2.0, -4.0}; // Poles

  mini_vector<size_t, TARGET_RANK> shape{};

  if constexpr (TARGET_RANK == 2) // Matrix_valued
    shape = make_shape(2, 2);
  else if constexpr (TARGET_RANK == 3) // Tensor_valued<3>
    shape = make_shape(2, 2, 2);
  else if constexpr (TARGET_RANK == 4) // Tensor_valued<4>
    shape = make_shape(2, 2, 2, 2);

  using target_t = typename _target_from_type_rank<dcomplex, TARGET_RANK>::type;

  auto iw_mesh  = gf_mesh<imfreq>{beta, Fermion, N_iw};
  auto tau_mesh = gf_mesh<imtime>{beta, Fermion, N_tau};

  // -- Init Green functions

  std::vector<gf<imfreq, target_t>> giw_vec;
  std::vector<gf<imtime, target_t>> gtau_vec;

  for (double E : E_vec) {
    giw_vec.emplace_back(iw_mesh, shape);
    giw_vec[0](iw_) << 2.0 / (iw_ + E);
    gtau_vec.emplace_back(tau_mesh, shape);
  }

  auto block_giw   = make_block_gf({"one", "two", "three"}, giw_vec);
  auto block_giw_2 = block_giw;

  auto block_gtau = make_block_gf({"one", "two", "three"}, gtau_vec);

  // -- Perform transforms

  block_gtau()  = fourier(block_giw);
  block_giw_2() = fourier(block_gtau);
  EXPECT_BLOCK_GF_NEAR(block_giw, block_giw_2, precision);

  // -- Factory functions
  block_gtau()  = make_gf_from_fourier(block_giw);
  block_giw_2() = make_gf_from_fourier(block_gtau);
  EXPECT_BLOCK_GF_NEAR(block_giw, block_giw_2, precision);

  // ===  Same for block2_gf ===

  std::vector<std::vector<gf<imfreq, target_t>>> giw_vecvec;
  std::vector<std::vector<gf<imtime, target_t>>> gtau_vecvec;

  for (double E : E_vec) {
    giw_vecvec.push_back(giw_vec);
    gtau_vecvec.push_back(gtau_vec);
  }

  auto block2_giw   = make_block2_gf({"one", "two", "three"}, {"one", "two", "three"}, giw_vecvec);
  auto block2_giw_2 = block2_giw;

  auto block2_gtau = make_block2_gf({"one", "two", "three"}, {"one", "two", "three"}, gtau_vecvec);

  // -- Perform transforms

  block2_gtau()  = fourier(block2_giw);
  block2_giw_2() = fourier(block2_gtau);
  EXPECT_BLOCK2_GF_NEAR(block2_giw, block2_giw_2, precision);

  // -- Factory functions

  block2_gtau()  = make_gf_from_fourier(block2_giw);
  block2_giw_2() = make_gf_from_fourier(block2_gtau);
  EXPECT_BLOCK2_GF_NEAR(block2_giw, block2_giw_2, precision);
}

TEST(FourierBlock, Scalar) { test_fourier<0>(); }
TEST(FourierBlock, Matrix) { test_fourier<2>(); }
TEST(FourierBlock, Tensor3) { test_fourier<3>(); }
TEST(FourierBlock, Tensor4) { test_fourier<4>(); }

MAKE_MAIN;
