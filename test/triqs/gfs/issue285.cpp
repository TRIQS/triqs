#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Base) {
 triqs::clef::placeholder<0> om_;
 auto G = gf<imfreq>{{1, Fermion, 3, matsubara_mesh_opt::positive_frequencies_only}, {1, 1}};
 auto G2 = gf<imfreq>{{1, Fermion, 3}, {1, 1}};
 G(om_) << 1 / (om_ - 1);

 std::cerr << "w.n\t w\t\t 1/(w-1) \t\t G(w)\t\t G[w]" << std::endl;
 for (auto const& w : G2.mesh()) { // note the G2, not G
  std::cerr << w.n << "\t" << dcomplex(w) << "\t" << 1 / (dcomplex(w) - 1) << "\t" << G(w)(0, 0) << "\n";
  EXPECT_THROW(G[w](0, 0), triqs::runtime_error);
 }
}
MAKE_MAIN;
