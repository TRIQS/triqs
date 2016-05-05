#include <triqs/test_tools/gfs.hpp>
using namespace triqs::lattice;
/*
// In this test, we define a new evaluator for a multivar gf
// to implement the symmetry g(nu, nup, -omega) = conj(g(-nu,-nup,omega))
// The principle is simple : just write an evaluator with a view of the gf with the default evaluator.

struct my_evaluator {
 static constexpr int arity = 3;

 // keep a view of the gf with the default evaluator
 gf_const_view<cartesian_product<imfreq, imfreq, imfreq>, scalar_valued, nothing> gv;

 template <typename G> my_evaluator(G *g) : gv(*g) {}

 // all evaluation are rewritten with in terms of the default evaluator.
 template <typename G, typename Nu, typename Nup, typename Om>
 auto operator()(G const &g, Nu const &nu, Nup const &nup, Om const &omega) const {
  if (omega < 0)
   return conj(gv(-nu, -nup, -omega));
  else
   return gv(nu, nup, omega);
 }
};

using gf3_s = gf<cartesian_product<imfreq, imfreq, imfreq>, scalar_valued, nothing>;
using gf3_s_modif = gf<cartesian_product<imfreq, imfreq, imfreq>, scalar_valued, nothing, my_evaluator>;

TEST(Gf, EvaluatorWithSymmetry) {
 int nw = 5;
 double beta = 10;
 clef::placeholder<0> k_;
 clef::placeholder<1> q_;
 clef::placeholder<2> r_;
 clef::placeholder<3> iw_;
 clef::placeholder<4> inu_;
 clef::placeholder<5> inup_;

 // This would throw
 auto g_classic = gf3_s{{{beta, Fermion, 2 * nw}, {beta, Fermion, 2 * nw}, {beta, Boson, nw, matsubara_mesh_opt::positive_frequencies_only}}};

 auto g = gf3_s_modif{{{beta, Fermion, 2 * nw}, {beta, Fermion, 2 * nw}, {beta, Boson, nw, matsubara_mesh_opt::positive_frequencies_only}}};

 g(inu_, inup_, iw_) << inu_ + 10 * inup_ + 100 * iw_;

 auto nu = matsubara_freq{2, beta, Fermion};
 auto nup = matsubara_freq{2, beta, Fermion};
 auto omega = matsubara_freq{3, beta, Boson};

 EXPECT_CLOSE(g(nu, nup, omega), nu + 10 * nup + 100 * omega);
 EXPECT_CLOSE(g(-nu, -nup, omega), - nu - 10 * nup + 100 * omega);
 EXPECT_CLOSE(g(nu, nup, -omega), nu + 10 * nup - 100 * omega);

 EXPECT_THROW(g_classic(nu, nup, -omega), std::exception);
}
*/
MAKE_MAIN;

