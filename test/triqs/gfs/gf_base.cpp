#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Base) {

  triqs::clef::placeholder<0> om_;
  double beta = 1;

  auto G  = gf<imfreq>{{beta, Fermion}, {2, 2}};
  auto Gc = G;
  auto G3 = G;

  auto Gt = gf<imtime>{{beta, Fermion, 100}, {2, 2}};

  auto Gv = G();

  EXPECT_ARRAY_ZERO(G(0));

  Gv.on_mesh(0) = 20;

  EXPECT_ARRAY_NEAR(Gv(0), G(0));
  EXPECT_ARRAY_NEAR(G(0), matrix<double>{{20, 0}, {0, 20}});

  Gv.on_mesh(0) = 0;

  auto Gv2       = slice_target(G(), range(0, 1), range(0, 1));
  Gv2.on_mesh(0) = 10;
  EXPECT_ARRAY_NEAR(G(0), matrix<double>{{10, 0}, {0, 0}});

  EXPECT_ARRAY_NEAR(eval(G(om_), om_ = 0), G(0));
  EXPECT_ARRAY_NEAR(eval(Gv(om_), om_ = 0), G(0));

  // Lazy assig
  G(om_) << (0.2 + om_ + 2.1);
  EXPECT_ARRAY_NEAR(G(0), matrix<dcomplex>{{2.3 + 3.1415926535897931_j, 0_j}, {0_j, 2.3 + 3.1415926535897931_j}});

  Gv(om_) << 1 / (om_ + 2.3);

  dcomplex z = 1 / (M_PI * 1_j + 2.3);
  EXPECT_ARRAY_NEAR(G(0), matrix<dcomplex>{{z, 0_j}, {0_j, z}});

  // copy
  Gc = G;
  EXPECT_GF_NEAR(G, Gc);

  // operations on gf
  G3 = G + Gc;

  // test for density
  EXPECT_ARRAY_NEAR(density(G3), matrix<double>{{1.8177540779781256, 0.0}, {0.0, 1.8177540779781256}});

  rw_h5(G, "G");

  {
    auto G0w     = gf<imfreq, scalar_valued>{{beta, Fermion, 100}};
    auto D0w     = gf<imfreq, scalar_valued>{{beta, Boson, 100}};
    auto Sigma_w = gf<imfreq, scalar_valued>{{beta, Fermion, 100}};
    G0w(om_) << 1 / (om_ - 3);

    for (auto const &nu : D0w.mesh()) Sigma_w(om_) << 2 * G0w(om_ - nu);
  }

  //
  {
    auto G0w     = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1, 1}};
    auto D0w     = gf<imfreq, matrix_valued>{{beta, Boson, 100}, {1, 1}};
    auto Sigma_w = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1, 1}};
    G0w(om_) << 1 / (om_ - 3);

    for (auto const &nu : D0w.mesh()) Sigma_w(om_) << 2 * G0w(om_ - nu);
  }
}

// Test the technique to avoid the infinity
TEST(Gf, EvaluatorMatrix) {

  double beta = 1;
  auto g      = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {2, 2}};

  triqs::clef::placeholder_prime<0> om_;
  g(om_) << 1 / (om_ + 2.3);

  auto f     = matsubara_freq{120, beta, Fermion};
  auto exact = matrix<dcomplex>{{1 / (f + 2.3), 0_j}, {0_j, 1 / (f + 2.3)}};

  EXPECT_ARRAY_NEAR(g(f), exact, 1e-14);
}

// Test the technique to avoid the infinity
TEST(Gf, EvaluatorScalar) {

  double beta = 1;
  auto g      = gf<imfreq, scalar_valued>{{beta, Fermion, 100}};

  triqs::clef::placeholder_prime<0> om_;
  g(om_) << 1 / (om_ + 2.3);

  auto f     = matsubara_freq{120, beta, Fermion};
  auto exact = 1 / (f + 2.3);

  EXPECT_COMPLEX_NEAR(g(f), exact, 1e-14);
}

// Test the technique to avoid the infinity
TEST(Gf, PhNoInfinity) {

  double beta = 1;
  auto g      = gf<imfreq, matrix_valued>{{beta, Fermion}, {2, 2}};

  triqs::clef::placeholder_prime<0> om_;
  g(om_) << 1 / (om_ + 2.3);
}

// Test the technique to avoid the infinity
TEST(Gf, PhNoInfinity_tau) {

  double beta = 1, a = 1;
  auto g = gf<imtime, matrix_valued>{{beta, Fermion, 10000}, {2, 2}};

  //triqs::clef::placeholder<0> tau_; // would not compile
  triqs::clef::placeholder_prime<0> tau_;
  g(tau_) << exp(-a * tau_) / (1 + exp(-beta * a));
}

TEST(Gf, ZeroM) {

  double beta = 1;
  auto g      = gf<imfreq>{{beta, Fermion}, {2, 2}};
  EXPECT_ARRAY_NEAR(make_zero_return(g), matrix<double>{{0, 0}, {0, 0}});
}
TEST(Gf, ZeroS) {

  double beta = 1;
  auto g      = gf<imfreq, scalar_valued>{{beta, Fermion}, {}};
  EXPECT_COMPLEX_NEAR(make_zero_return(g), 0);
}

TEST(Gf, SliceTargetScalar) {
  double beta = 1;
  auto g      = gf<imfreq>{{beta, Fermion}, {2, 2}};

  auto gs = slice_target_to_scalar(g, 0, 0);
}

TEST(Gf, TargetSpaceLoop) {
  double beta = 1;
  auto g1     = gf<imfreq>{{beta, Fermion}, {2, 2}};
  auto g2     = gf{g1};

  triqs::clef::placeholder<0> iw_;
  triqs::clef::placeholder<1> i_;
  triqs::clef::placeholder<2> j_;
  g1[iw_](i_, j_) << 1. / (iw_ + i_ + j_ * 0.1);

  for (auto iw : g2.mesh())
    for (auto [i, j] : g2.target_indices()) g2[iw](i, j) = 1. / (iw + i + j * 0.1);

  EXPECT_GF_NEAR(g1, g2);
}
MAKE_MAIN;
