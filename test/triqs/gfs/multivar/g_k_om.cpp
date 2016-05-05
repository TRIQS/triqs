#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;
using triqs::clef::placeholder;
using triqs::utility::mindex;

TEST(Gf, GkOm) {

 double beta = 1;
 auto bz = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};

 int n_bz = 20; 

 auto g_eps = gf<brillouin_zone>{{bz, n_bz}, {1, 1}};

 // Default, with a tail.
 auto G = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, 100}}, {1, 1}};
 // Same : the gf_default_singularity is the same
 static_assert(std::is_same<std14::decay_t<decltype(G.singularity())>, gf<brillouin_zone, tail_valued<matrix_valued>>>::value, "!!");

 // auto G = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued, gf<brillouin_zone, tail>>{{{bz, 100}, {beta, Fermion,
 // 100}}, {1, 1}};

 // try to assign to expression
 placeholder<0> k_;
 placeholder<1> w_;

 auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
 G(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));

 if (0) { // to check speed of computation, which is  long due to tail (inversion ?)
  auto t = __tail<matrix_valued>::omega(1, 1);
  auto t2 = t;
  for (int u = 0; u < 10000; ++u) {
   t2 = (t + u - inverse(t - u));
   // t2 = inverse(t + u - inverse(t - u));
  }
 }

 for (auto k : std::get<0>(G.mesh().components())) {
  EXPECT_CLOSE(G.singularity()[k](1)(0, 0), 1);
  EXPECT_CLOSE(G.singularity()[k](2)(0, 0), eval(eps_k, k_ = k));
 }

 auto gk = curry<0>(G);

 //std::cerr << G.singularity()[{0, 0, 0}] << std::endl;
 //std::cerr << gk[{0, 0, 0}].singularity() << std::endl;

 for (auto k : gk.mesh()) {
  auto p = gk[k];
  EXPECT_CLOSE(gk[k].singularity()(1)(0, 0), 1);
  EXPECT_CLOSE(gk[k].singularity()(2)(0, 0), eval(eps_k, k_ = k));
 }

 for (auto k : gk.mesh()) {
  auto p = partial_eval<0>(G, mindex(0, 0, 0));
  EXPECT_CLOSE(gk[k].singularity()(1)(0, 0), 1);
  EXPECT_CLOSE(gk[k].singularity()(2)(0, 0), eval(eps_k, k_ = k));
 }

 rw_h5(G, "ess_g_k_om.h5", "g");
}

TEST(Gf, GkOmSlice) {

 double beta = 1;
 auto bz = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};
 int n_bz = 20; 

 auto G = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, 100}}, {2, 2}};

 // try to assign to expression
 placeholder<0> k_;
 placeholder<1> w_;

 auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
 G(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));

 auto gs1 = slice_target(G, range(0, 1), range(0, 1));

 // do not allow this
 auto gs2 = slice_target_to_scalar(G, 0, 0);
}



MAKE_MAIN;
