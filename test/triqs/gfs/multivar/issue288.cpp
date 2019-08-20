#include <triqs/test_tools/gfs.hpp>

TEST(GfCartesian, H5_RW_EvaluatorS) {
  double beta = 1;
  auto g      = gf<prod<imfreq, imfreq>, scalar_valued>{{{beta, Fermion, 5}, {beta, Boson, 5}}};
  g()         = 2;

  h5::file file("g_nu_nuph5", H5F_ACC_TRUNC);
  h5_write(file, "g", g);
  gf<prod<imfreq, imfreq>, scalar_valued> g2{};
  h5_read(file, "g", g2);

  EXPECT_ARRAY_NEAR(g.data(), g2.data());
  //EXPECT_GF_NEAR(g, g2);

  auto w0  = matsubara_freq(0, beta, Fermion);
  auto W10 = matsubara_freq(10, beta, Boson);
  auto W0  = matsubara_freq(0, beta, Boson);

  EXPECT_COMPLEX_NEAR(g(w0, W0), g2(w0, W0), 1.e-13);
  EXPECT_COMPLEX_NEAR(g(w0, W10), g2(w0, W10), 1.e-13);
}

TEST(GfCartesian, H5_RW_EvaluatorM) {
  double beta = 1;
  auto g      = gf<prod<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 5}, {beta, Boson, 5}}, {1, 1}};
  g()         = 2;

  h5::file file("g_nu_nuph5", H5F_ACC_TRUNC);
  h5_write(file, "g", g);
  gf<prod<imfreq, imfreq>, matrix_valued> g2{};
  h5_read(file, "g", g2);

  EXPECT_ARRAY_NEAR(g.data(), g2.data());
  //EXPECT_GF_NEAR(g, g2);

  auto w0  = matsubara_freq(0, beta, Fermion);
  auto W10 = matsubara_freq(10, beta, Boson);
  auto W0  = matsubara_freq(0, beta, Boson);

  EXPECT_ARRAY_NEAR(g(w0, W0), g2(w0, W0));
  EXPECT_ARRAY_NEAR(g(w0, W10), g2(w0, W10));
}

MAKE_MAIN;
