#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;
using triqs::clef::placeholder;
using triqs::utility::mindex;

template <typename Function, typename Mesh> auto sum_gf(Function const &f, Mesh const &m) {
 auto res = make_matrix(0 * f(*(m.begin())));
 for (auto const &x : m) //
  res = res + f(x);
 return res;
}

namespace triqs {
 namespace clef {
  TRIQS_CLEF_MAKE_FNT_LAZY(sum_gf);
 }
}

placeholder<0> k_;
placeholder<1> w_;
using gf_bz_imfreq_mat = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued>;
using gf_bz_imtime_mat = gf<cartesian_product<brillouin_zone, imtime>, matrix_valued>;

auto _ = var_t{};

// ------------------------------------------------------------

TEST(Gf, GkOm) {

 double beta = 1;
 int n_bz    = 20;

 auto bz    = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};
 auto g_eps = gf<brillouin_zone>{{bz, n_bz}, {1, 1}};

 auto G = gf_bz_imfreq_mat{{{bz, n_bz}, {beta, Fermion, 100}}, {1, 1}};

 static_assert(std::is_same<std14::decay_t<decltype(G.singularity())>, gf<brillouin_zone, tail_valued<matrix_valued>>>::value,
               "!!");

 auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
 G(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));

 for (auto k : std::get<0>(G.mesh())) {
  static_assert(
      std::is_same<std14::decay_t<decltype(G.singularity()[k])>, std14::decay_t<decltype(G[k][_].singularity())>>::value, "!!");

  EXPECT_CLOSE(G.singularity()[k](1)(0, 0), 1);
  EXPECT_CLOSE(G.singularity()[k](2)(0, 0), eval(eps_k, k_ = k));

  EXPECT_CLOSE(G[k][_].singularity()(1)(0, 0), 1);
  EXPECT_CLOSE(G[k][_].singularity()(2)(0, 0), eval(eps_k, k_ = k));
 }

 rw_h5(G, "ess_g_k_om.h5", "g");
}

// ------------------------------------------------------------

TEST(Gf, GkOmSlice) {

 double beta = 1;
 auto bz     = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};
 int n_bz    = 20;

 auto G = gf_bz_imfreq_mat{{{bz, n_bz}, {beta, Fermion, 100}}, {2, 2}};

 auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
 G(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));

 auto gs1 = slice_target(G, range(0, 1), range(0, 1));
 auto gs2 = slice_target_to_scalar(G, 0, 0);

 // NO TEST !
}

// ------------------------------------------------------------

TEST(Gf, SumGf) {
 double beta = 1;
 int n_k     = 8;
 auto bz     = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
 auto g_eps  = gf<brillouin_zone>{{bz, n_k}, {1, 1}};

 auto G_k_iom   = gf_bz_imfreq_mat{{{bz, n_k}, {beta, Fermion, 100}}, {1, 1}};
 auto G_k_iom_2 = G_k_iom;

 EXPECT_EQ(std::get<0>(G_k_iom.mesh()).size(), n_k * n_k);

 auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
 G_k_iom(k_, w_) << 1 / (w_ - eps_k);

 // for (auto const & [k,w] : G_k_iom.mesh())
 for (auto const &k : std::get<0>(G_k_iom.mesh()))
  for (auto const &w : std::get<1>(G_k_iom.mesh()))
   G_k_iom_2[k][w] = 1 / (dcomplex(w) + 2 * (cos(k[0]) + cos(k[1])));

 EXPECT_ARRAY_NEAR(G_k_iom.data(), G_k_iom_2.data());

 auto G_loc = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1, 1}};

 // FIXME : vector is awful ! std::array<??> or alias
 auto r  = G_k_iom(utility::mini_vector<int, 3>{0, 0, 0}, matsubara_freq{0, beta, Fermion});
 auto r5 = sum_gf(k_ >> G_k_iom(k_, 0), g_eps.mesh());

 placeholder_prime<0> wp_;
 G_loc(wp_) << sum_gf(k_ >> G_k_iom(k_, wp_), g_eps.mesh());

 auto G_k_tau = gf_bz_imtime_mat{{{bz, n_k}, {beta, Fermion, 201}}, {1, 1}};

 // old curry test, before removing curry
 //curry<0>(G_k_tau)[k_] << inverse_fourier(curry<0>(G_k_iom)[k_]);
 //auto G_k_tau1 = G_k_tau;

 for (auto const &k : std::get<0>(G_k_iom.mesh()))
  G_k_tau[k][_] = inverse_fourier(G_k_iom[k][_]);

 //EXPECT_ARRAY_NEAR(G_k_tau.data(), G_k_tau1.data());
}

MAKE_MAIN;
