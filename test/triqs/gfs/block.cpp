#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Block) {

  double beta = 1.;
  auto G1     = gf<imfreq>({beta, Fermion}, {2, 2});
  auto G2     = G1;
  auto G3     = G1;

  triqs::clef::placeholder<0> w_;
  G1(w_) << 1. / (w_ + 2.);
  G2(w_) << 2. / (w_ - 2.);

  // Constructors
  auto G_vec = std::vector{G1, G2, G3};

  auto B1 = block_gf{G_vec};
  auto B2 = block_gf{B1()};
  auto B3 = block_gf<imfreq>({"0", "1", "2"}, G_vec);
  auto B4 = make_block_gf({"0", "1", "2"}, G_vec);
  auto B5 = make_block_gf(2, G1);

  auto V1       = make_block_gf_view(G1, G2, G3);
  auto view_vec = std::vector<gf_view<imfreq>>{G1, G2, G3};
  auto V2       = make_block_gf_view(view_vec);

  EXPECT_BLOCK_GF_NEAR(B1, B2);
  EXPECT_BLOCK_GF_NEAR(B1, B3);
  EXPECT_BLOCK_GF_NEAR(B1, B4);
  EXPECT_BLOCK_GF_NEAR(B1, V1);
  EXPECT_BLOCK_GF_NEAR(B1, V2);

  // H5 read write
  EXPECT_BLOCK_GF_NEAR(B1, rw_h5(B1, "block", "B1"));

  // Data write/read access
  B1[0][0] = 1.0;
  EXPECT_ARRAY_NEAR(B1[0](0), matrix<double>{{1.0, 0.0}, {0.0, 1.0}});

  // Operations
  B1 = B1 / 2.0;
  B1 = B1 * 4.0;
  EXPECT_CLOSE(B1[0][0](0, 0), 2.0);
  B1 = B1 + B1 * B1;
  EXPECT_CLOSE(B1[0][0](0, 0), 6.0);
  B1 = B1 + B1() * B1();
  EXPECT_CLOSE(B1[0][0](0, 0), 42.0);

  // View Access
  V1[0] = 20.;
  EXPECT_ARRAY_NEAR(G1(0), matrix<double>{{20, 0}, {0, 20}});
  EXPECT_ARRAY_NEAR(V2[0](0), matrix<double>{{20, 0}, {0, 20}});

  // Loops
  for (auto &g : B1) { g[0] = 20; }
  EXPECT_CLOSE(B1[0][0](0, 0), 20);
  for (auto &g : B1()) { g[0] = 40; }
  EXPECT_CLOSE(B1[0][0](0, 0), 40);

  // Clef expressions
  clef::placeholder<0> b_;
  clef::placeholder<1> iw_;
  B1[b_][iw_] << b_ / (iw_ + 2);
  auto B11 = B1;
  B1[b_](iw_) << B11[b_](iw_) * B1[b_](iw_) * B11[b_](iw_);

  // Reinterpretation (compile checks)
  auto G1_scalar = gf<imfreq, scalar_valued>{{beta, Fermion}, {}};
  auto B1_scalar = make_block_gf(3, G1_scalar);
  auto B1_interp = reinterpret_scalar_valued_gf_as_matrix_valued(B1_scalar);

  // inversion
  auto inv_G1 = inverse(G1);
  auto B      = make_block_gf(3, G1);
  auto inv_B  = inverse(B);
  for (auto &g : inv_B) EXPECT_GF_NEAR(g, inv_G1);
}

TEST(Block, AssignmentOperator) {
  auto g = gf<imfreq, scalar_valued>{{1, Fermion}};
  auto G = make_block_gf({"up", "down"}, {g, g});
  block_gf<imfreq, scalar_valued> G2;
  G2 = G;
  ASSERT_EQ(G2.data().size(), G.data().size());
  ASSERT_EQ(G2.block_names().size(), G.block_names().size());
}

TEST(Block, Order) {
  double beta = 1;
  auto G1     = gf<imfreq>({beta, Fermion}, {1, 1});
  auto G2     = gf<imfreq>({beta, Fermion}, {1, 1});
  triqs::clef::placeholder<0> w_;
  G1(w_) << 1 / (w_ + 2);
  G2(w_) << 1 / (w_ - 2);

  auto B_ab = make_block_gf({"a", "b"}, {G1, G2});
  auto B_ba = make_block_gf({"b", "a"}, {G2, G1});

  auto B_res = B_ab;
  for (auto const &ind : range(B_res.size())) { B_res[ind] = B_ba(ind); }
  std::cout << B_ab[0](0) << std::endl;
  std::cout << B_ba[1](0) << std::endl;
  std::cout << B_res[0](0) << std::endl;
}

MAKE_MAIN;
