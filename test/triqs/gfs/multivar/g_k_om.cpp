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

namespace triqs::clef {
  TRIQS_CLEF_MAKE_FNT_LAZY(sum_gf);
}

placeholder<0> k_;
placeholder<1> w_;
placeholder_prime<0> wp_;
using gf_bz_imfreq_mat = gf<cartesian_product<b_zone, imfreq>, matrix_valued>;
using gf_bz_imtime_mat = gf<cartesian_product<b_zone, imtime>, matrix_valued>;

auto _ = all_t{};

// ------------------------------------------------------------

// FIXME : Does not test much ...
TEST(Gf, GkOm) {

  double beta = 1;
  int n_bz    = 20;

  auto bz    = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};
  auto g_eps = gf<b_zone>{{bz, n_bz}, {1, 1}};

  auto G     = gf_bz_imfreq_mat{{{bz, n_bz}, {beta, Fermion, 100}}, {1, 1}};
  auto Sigma = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1, 1}};
  Sigma()    = 0;

  auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));

  G(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));

  // broken with wp -> w
  //G[k_, w_] <<  G(k_, w_) ;//1 / (w_ - eps_k - 1 / (w_ + 2));

  G[k_, w_] << G[k_, w_] + 1 / (w_ - eps_k - 1 / (w_ + 2));

  G[k_, wp_] << 2 * G[k_, wp_];

  G[k_, w_] << 2 * Sigma(w_);

  G[k_, w_] << 2 * Sigma[w_];

  G[k_, w_] << 1 / (w_ - eps_k - 1 / (w_ + 2));

  rw_h5(G, "ess_g_k_om.h5", "g");
}

// -------------------------------------------------------------
using K_t = std::array<double, 3>;

TEST(Gkom, Eval) {
  double beta = 5;

  int n_k = 40;
  int n_w = 5;

  auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
  auto g  = gf<cartesian_product<b_zone, imfreq>, scalar_valued>{{{bz, n_k}, {beta, Fermion, n_w}}};

  //Gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));
  for (auto [k, w] : g.mesh()) g[k, w] = 1 / (w + 3 + 2 * (cos(k(0)) + cos(k(1))));

  // reevaluate on the mesh itself.
  for (auto [k, w] : g.mesh()) {
    EXPECT_NEAR(k(0), k.index()[0] / double(n_k) * 2 * M_PI, 1.e-14);
    EXPECT_NEAR(k(1), k.index()[1] / double(n_k) * 2 * M_PI, 1.e-14);
    dcomplex res = 1 / (w + 3 + 2 * (cos(k(0)) + cos(k(1))));
    EXPECT_COMPLEX_NEAR(g(K_t{k(0), k(1), 0}, w), res, 1.e-14);
  }

  //// evaluate on a larger grid
  int n_k2 = 3 * n_k;
  for (int nkx = 0; nkx < n_k2; ++nkx)
    for (int nky = 0; nky < n_k2; ++nky)
      for (auto w : std::get<1>(g.mesh())) {
        double kx    = nkx / double(n_k2) * 2 * M_PI;
        double ky    = nky / double(n_k2) * 2 * M_PI;
        dcomplex res = 1 / (w + 3 + 2 * (cos(kx) + cos(ky)));
        EXPECT_COMPLEX_NEAR(g(K_t{kx, ky, 0}, w), res, 0.05 * std::abs(res));
      }
}

// -------------------------------------------------------------

// test g(k, all_t)
TEST(Gkom, EvalSlice) {
  double beta = 5;

  int n_k = 40;
  int n_w = 5;

  auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
  auto g  = gf<cartesian_product<b_zone, imfreq>, scalar_valued>{{{bz, n_k}, {beta, Fermion, n_w}}};

  //Gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));
  for (auto [k, w] : g.mesh()) g[k, w] = 1 / (w + 3 + 2 * (cos(k(0)) + cos(k(1))));

  auto pi_pi = K_t{M_PI, M_PI, 0};

  auto gw = g(pi_pi, all_t{});

  EXPECT_TRUE(std::get<1>(g.mesh()) == gw.mesh());
  for (auto const &w : gw.mesh()) EXPECT_COMPLEX_NEAR(gw[w], g(pi_pi, w), 1.e-12);
}

MAKE_MAIN;
