#include <triqs/gfs.hpp>
#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;

TEST(Gf, block2_gf_segfault) {

  double beta = 1.23;
  int n_fermionic = 2;
  
  gf_mesh<imfreq> mesh_f{beta, Fermion, n_fermionic};
  
  using G_iw_t = block2_gf<imfreq, matrix_valued>;
  using g_iw_t = gf<imfreq, matrix_valued>;
  
  auto g = g_iw_t{mesh_f, {1, 1}};

  std::vector<g_iw_t> vec;

  vec.push_back(g);
  vec.push_back(g);

  std::vector<std::vector<g_iw_t>> vec_vec;

  vec_vec.push_back(vec);
  vec_vec.push_back(vec);
  
  G_iw_t G_iw = make_block2_gf({"AA", "AB"},{"BA", "BB"}, vec_vec);

  G_iw() = 2.;

  // ===========================================================
  // BUG START
  // ===========================================================
  
  G_iw = G_iw / 2.; // This segfaults on triqs/unstable commit da793fb

  // ===========================================================
  // BUG END
  // ===========================================================

  for( auto const & g : G_iw ) {
    for( auto const & w : g.mesh() ) {
      std::cout << g[w] << "\n";
      auto val = g[w](0,0);
      EXPECT_EQ(val, 1.0);
    }
  }
}

MAKE_MAIN;
