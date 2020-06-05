#include <triqs/test_tools/gfs.hpp>
#include <iostream>
using namespace triqs::gfs;
using namespace triqs::clef;

placeholder<0> iw;
double beta = 1.0;

TEST(make_gf, from_view) {

  // Create and intialize G1
  gf<imfreq> G1{{beta, Fermion, 5}, {2, 2}};
  G1[iw] << 1.0 / iw;

  // Create G2 from view of G1
  auto G2 = make_gf(G1());

  EXPECT_GF_NEAR(G1, G2);
}

TEST(make_gf, new_layout) {

  // Create and intialize G1
  gf<imfreq> G1{{beta, Fermion, 5}, {2, 2}};
  G1[iw] << 1.0 / iw;

  // Create G2 from G1 with new memory layout
  auto G2 = make_gf(G1, make_memory_layout(1, 0, 2));

  for (int i = 0; i < G1.data().indexmap().domain().number_of_elements(); ++i) {
    std::cerr << G1.data().data_start()[i] << " " << G2.data().data_start()[i] << "\n";
  }

  EXPECT_EQ(G1.memory_layout(), make_memory_layout(0, 1, 2));
  EXPECT_EQ(G2.memory_layout(), make_memory_layout(1, 0, 2));
  EXPECT_GF_NEAR(G1, G2);
}

TEST(make_gf, from_mesh_and_target) {

  // Create and intialize G1
  gf<imfreq> G1{{beta, Fermion, 5}, {2, 2}};
  G1[iw] << 1.0 / iw;

  // Create G2 with same mesh and target as G1
  auto G2 = make_gf(G1.mesh(), G1.target());

  // Fill G2 with same values as G1
  G2() = G1;

  EXPECT_GF_NEAR(G1, G2);
}

TEST(make_gf, from_mesh_target_and_layout) {

  // Create and intialize G1
  gf<imfreq> G1{{beta, Fermion, 5}, {2, 2}, make_memory_layout(1, 0, 2)};
  G1[iw] << 1.0 / iw;

  // Create G2 with same mesh and target as G1
  auto G2 = make_gf(G1.mesh(), G1.target(), G1.memory_layout());

  // Fill G2 with same values as G1
  G2() = G1;

  EXPECT_EQ(G1.memory_layout(), make_memory_layout(1, 0, 2));
  EXPECT_EQ(G2.memory_layout(), make_memory_layout(1, 0, 2));
  EXPECT_GF_NEAR(G1, G2);
}

MAKE_MAIN;
