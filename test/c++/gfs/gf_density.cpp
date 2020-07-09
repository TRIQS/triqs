#include <triqs/test_tools/gfs.hpp>

TEST(Gf, DensityFermion) {

  double beta   = 1;
  int n_im_freq = 400;
  auto G        = gf<imfreq, scalar_valued>{{beta, Fermion, n_im_freq}};

  EXPECT_EQ(G.mesh().positive_only(), 0);

  triqs::clef::placeholder<0> om_;
  G(om_) << 1 / (om_ + 2.3);

  auto n = triqs::gfs::density(G);

  EXPECT_COMPLEX_NEAR(n, 1 / (1 + std::exp(-beta * 2.3)), 1.e-9);

  rw_h5(G, "DensFerm");
}

TEST(Gf, DensityBoson) {

  double beta   = 1;
  int n_im_freq = 400;
  auto G        = gf<imfreq, scalar_valued>{{beta, Boson, n_im_freq}};

  EXPECT_EQ(G.mesh().positive_only(), 0);

  triqs::clef::placeholder<0> om_;
  G(om_) << 1 / (om_ - 2.3); // nb! need positive energy for a free boson

  auto n = triqs::gfs::density(G);

  EXPECT_COMPLEX_NEAR(n, 1 / (-1 + std::exp(beta * 2.3)), 1.e-9);

  rw_h5(G, "DensBos");
}

TEST(Gf, Density_with_not_all_moments) {

  double beta   = 1;
  int n_im_freq = 400;
  auto G        = gf<imfreq, scalar_valued>{{beta, Fermion, n_im_freq}};

  EXPECT_EQ(G.mesh().positive_only(), 0);

  triqs::clef::placeholder<0> om_;

  G(om_) << 1.0;
  EXPECT_THROW(triqs::gfs::density(G), triqs::runtime_error);

  G(om_) << om_;
  EXPECT_THROW(triqs::gfs::density(G), triqs::runtime_error);
}

MAKE_MAIN;
