#include "./common.hpp"
#include <triqs/gfs/functions/functions.hpp>

TEST(Gf, Base) {

 triqs::clef::placeholder<0> om_;
 double beta = 1;

 auto G = gf<imfreq>{{beta, Fermion}, {2, 2}};
 auto Gc = G;
 auto G3 = G;

 auto Gt = gf<imtime>{{beta, Fermion, 100}, {2, 2}};

 auto Gv = G();

 EXPECT_ARRAY_ZERO(G(0));

 Gv.on_mesh(0) = 20;

 EXPECT_ARRAY_NEAR(Gv(0), G(0));
 EXPECT_ARRAY_NEAR(G(0), matrix<double>{{20, 0}, {0, 20}});

 Gv.on_mesh(0) = 0;

 auto Gv2 = slice_target(G(), range(0, 1), range(0, 1));
 Gv2.on_mesh(0) = 10;
 EXPECT_ARRAY_NEAR(G(0), matrix<double>{{10, 0}, {0, 0}});

 EXPECT_ARRAY_NEAR(eval(G(om_), om_ = 0), G(0));
 EXPECT_ARRAY_NEAR(eval(Gv(om_), om_ = 0), G(0));

 // Lazy assig
 G(om_) << (0.2 + om_ + 2.1);
 EXPECT_ARRAY_NEAR(G(0), matrix<dcomplex>{{2.3 + 3.1415926535897931_j, 0_j}, {0_j, 2.3 + 3.1415926535897931_j}});

 EXPECT_ARRAY_NEAR(G.singularity()(-1), matrix<double>{{1, 0}, {0, 1}});
 EXPECT_ARRAY_NEAR(G.singularity()(0), matrix<double>{{2.3, 0.0}, {0.0, 2.3}});
 EXPECT_ARRAY_ZERO(G.singularity()(1));
 EXPECT_ARRAY_ZERO(G.singularity()(2));

 auto t1 = G.singularity();

 Gv(om_) << 1 / (om_ + 2.3);
 
 dcomplex z = 1/( M_PI* 1_j + 2.3);
 EXPECT_ARRAY_NEAR(G(0), matrix<dcomplex>{{z, 0_j}, {0_j, z}});
 EXPECT_ARRAY_ZERO(G.singularity()(-1));
 EXPECT_ARRAY_ZERO(G.singularity()(0));
 EXPECT_ARRAY_NEAR(G.singularity()(1), matrix<double>{{1, 0}, {0, 1}});
 EXPECT_ARRAY_NEAR(G.singularity()(2), matrix<double>{{-2.3, 0.0}, {0.0, -2.3}});
 EXPECT_ARRAY_NEAR(G.singularity()(3), matrix<double>{{5.29, 0.0}, {0.0, 5.29}});
 EXPECT_ARRAY_NEAR(G.singularity()(7), matrix<double>{{148.03588899999994, 0.0}, {0.0, 148.03588899999994}});

 // tail
 auto t = G.singularity();
 EXPECT_ARRAY_NEAR(t1.data(), inverse(G.singularity()).data());
 EXPECT_EQ(t.order_min(), -1);
 EXPECT_ARRAY_NEAR(Gv2.singularity()(2), matrix<double>{{-2.3}});

 // copy
 Gc = G;
 EXPECT_GF_NEAR(G, Gc);

 // operations on gf
 G3 = G + Gc;

 // test for density
 EXPECT_ARRAY_NEAR(density(G3), matrix<double>{{1.8177540779781256, 0.0}, {0.0, 1.8177540779781256}});

 auto tt = G.singularity() + G.singularity();
 EXPECT_ARRAY_NEAR(tt(2), 2 * matrix<double>{{-2.3, 0.0}, {0.0, -2.3}});

 rw_h5(G);

 {
  auto G0w = gf<imfreq, scalar_valued>{{beta, Fermion, 100}};
  auto D0w = gf<imfreq, scalar_valued>{{beta, Boson, 100}};
  auto Sigma_w = gf<imfreq, scalar_valued>{{beta, Fermion, 100}};
  G0w(om_) << 1 / (om_ - 3);

  for (auto const& nu : D0w.mesh()) Sigma_w(om_) << 2 * G0w(om_ - nu);
 }

 //
 {
  auto G0w = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1, 1}};
  auto D0w = gf<imfreq, matrix_valued>{{beta, Boson, 100}, {1, 1}};
  auto Sigma_w = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1, 1}};
  G0w(om_) << 1 / (om_ - 3);

  for (auto const& nu : D0w.mesh()) Sigma_w(om_) << 2 * G0w(om_ - nu);
 }
}
MAKE_MAIN;
