#define TRIQS_DEBUG
#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Issue275) {

  //auto g_half = gf<imfreq, matrix_valued>{{1, Fermion, 3, matsubara_mesh_opt::positive_frequencies_only}, {1, 1}};
  //auto g_full = gf<imfreq, matrix_valued>{{1, Fermion, 6}, {1, 1}};

  //triqs::clef::placeholder<0> om_;
  //g_half(om_) << 1 / (om_ + 3);

  //auto l = om_ >> (g_half(om_)(0, 0));

  //for (auto const& w : g_full.mesh())
  //EXPECT_COMPLEX_NEAR(l(w), g_half(w)(0, 0));
}

// ----------------------------------------------------------

TEST(Gf, Issue276) {

  auto g  = gf<imfreq, matrix_valued>{{1, Fermion, 6}, {1, 1}};
  auto g2 = gf<imfreq, matrix_valued>{{1, Fermion, 6}, {1, 1}};

  triqs::clef::placeholder<0> om_;
  g(om_) << 1 / (om_ + 3);
  g2(om_) << 1 / (om_ - 3);

  auto gc  = g;
  auto gc2 = g2;

  {
    gf_const_view<imfreq> gv  = g;
    gf_const_view<imfreq> gv2 = g2;

    EXPECT_GF_NEAR(g, gv);
    EXPECT_GF_NEAR(g2, gv2);

    swap(gv, gv2);

    EXPECT_GF_NEAR(g, gv2);
    EXPECT_GF_NEAR(g2, gv);

    EXPECT_GF_NEAR(g, gc);
    EXPECT_GF_NEAR(g2, gc2);
  }

  {
    g(om_) << 1 / (om_ + 3);
    g2(om_) << 1 / (om_ - 3);

    gf_view<imfreq> gv  = g;
    gf_view<imfreq> gv2 = g2;

    EXPECT_GF_NEAR(g, gv);
    EXPECT_GF_NEAR(g2, gv2);

    swap(gv, gv2);

    EXPECT_GF_NEAR(g, gv2);
    EXPECT_GF_NEAR(g2, gv);
  }
}

// ----------------------------------------------------------

// Shall we reimplement this or too strict ?
/*
TEST(Gf, Issue285) {
 triqs::clef::placeholder<0> om_;
 auto G  = gf<imfreq>{{1, Fermion, 3, matsubara_mesh_opt::positive_frequencies_only}, {1, 1}};
 auto G2 = gf<imfreq>{{1, Fermion, 3}, {1, 1}};
 G(om_) << 1 / (om_ - 1);

 std::cerr << "w.n\t w\t\t 1/(w-1) \t\t G(w)\t\t G[w]" << std::endl;
 for (auto const& w : G2.mesh()) { // note the G2, not G
  std::cerr << w.n << "\t" << dcomplex(w) << "\t" << 1 / (dcomplex(w) - 1) << "\t" << G(w)(0, 0) << "\n";
  EXPECT_THROW(G[w](0, 0), triqs::runtime_error);
 }
}
*/
// ----------------------------------------------------------

TEST(Gf, Issue319) {
  double beta = 20;
  int n_iw    = 100;
  int dim     = 2;
  int rank    = 0; //world.rank();

  gf<prod<imfreq, imfreq, imfreq>> g4_w{{{beta, Fermion, n_iw}, {beta, Fermion, n_iw}, {beta, Boson, 25}}, {dim, dim}};

  auto G4_w  = make_block_gf(1, g4_w);
  auto mesh  = G4_w[0].mesh();
  auto N     = mesh.size();
  auto slice = itertools::chunk_range(0, N, 1, 0); // world.size(), world.rank());
  auto mpg   = triqs::mesh::mesh_pt_generator<mesh::prod<imfreq, imfreq, imfreq>>(&G4_w[0].mesh());

  mpg += slice.first;
  mpg.to_point();
}

MAKE_MAIN;
