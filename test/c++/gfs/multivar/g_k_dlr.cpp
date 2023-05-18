#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;
using triqs::clef::placeholder;

auto _ = all_t{};
using std::exp;

double eps_k(auto const &k) { return -2 * (cos(k[0]) + cos(k[1])); }
double onefermion(double tau, double eps, double beta) { return -exp(-eps * tau) / (1 + exp(-beta * eps)); }

// ------------------------------------------------------------

TEST(Gf, G_dlr_mat) {

  double beta   = 5;
  int n_bz      = 10;
  double Lambda = 100.0;
  double eps    = 1e-10;
  double e0     = 1.2;

  auto g1 = gf<dlr_imtime, matrix_valued>{{beta, Fermion, Lambda, eps}, {1, 1}};

  for (auto const &tau : g1.mesh()) g1[tau] = onefermion(tau, e0, beta);

  auto g2 = make_gf_dlr_coeffs(g1);

  for (auto const &tau : g1.mesh()) { EXPECT_COMPLEX_NEAR(g2(tau)(0, 0), g1[tau](0, 0)); }

  auto g3 = make_gf_dlr_imfreq(g2);

  auto g3_check = gf<dlr_imfreq, matrix_valued>{{beta, Fermion, Lambda, eps}, {1, 1}};

  for (auto const &w : g3_check.mesh()) g3_check[w] = 1 / (w - e0); // FIXME should be w - e0

  PRINT(g3.data());
  PRINT(g3_check.data());

  EXPECT_GF_NEAR(g3, g3_check);
}

// ------------------------------------------------------------

TEST(Gf, Gk_dlr_mat) {

  double beta   = 5;
  int n_bz      = 10;
  double Lambda = 100.0;
  double eps    = 1e-10;

  auto bz = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};

  auto g1 = gf<prod<brzone, dlr_imtime>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, Lambda, eps}}, {1, 1}};

  for (auto const &[k, tau] : g1.mesh()) g1[k, tau] = onefermion(tau, eps_k(k), beta);

  // do nothing
  auto g1b = partial_transform<1>(make_const_view(g1), [](auto &&gg) { return gg; });
  EXPECT_GF_NEAR(g1, g1b);

  auto g2 = partial_transform<1>(make_const_view(g1), [](auto &&gg) { return make_gf_dlr_coeffs(gg); });
  auto g3 = partial_transform<1>(make_const_view(g2), [](auto &&gg) { return make_gf_dlr_imfreq(gg); });

  auto g3_check = gf<prod<brzone, dlr_imfreq>, matrix_valued>{{{bz, n_bz}, {beta, Fermion, Lambda, eps}}, {1, 1}};

  for (auto const &[k, w] : g3_check.mesh()) g3_check[k, w] = 1 / (w - eps_k(k));

  EXPECT_GF_NEAR(g3, g3_check);
}

// ------------------------------------------------------------
// same with other orders

TEST(Gf, G_dlr_k_mat) {

  double beta   = 5;
  int n_bz      = 10;
  double Lambda = 100.0;
  double eps    = 1e-10;

  auto bz = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};

  auto g1 = gf<prod<dlr_imtime, brzone>, matrix_valued>{{{beta, Fermion, Lambda, eps}, {bz, n_bz}}, {1, 1}};

  for (auto const &[tau, k] : g1.mesh()) g1[tau, k] = onefermion(tau, eps_k(k), beta);

  // do nothing
  auto g1b = partial_transform<0>(make_const_view(g1), [](auto &&gg) { return gg; });
  EXPECT_GF_NEAR(g1, g1b);

  auto g2 = partial_transform<0>(make_const_view(g1), [](auto &&gg) { return make_gf_dlr_coeffs(gg); });
  auto g3 = partial_transform<0>(make_const_view(g2), [](auto &&gg) { return make_gf_dlr_imfreq(gg); });

  auto g3_check = gf<prod<dlr_imfreq, brzone>, matrix_valued>{{{beta, Fermion, Lambda, eps}, {bz, n_bz}}, {1, 1}};

  for (auto const &[w, k] : g3_check.mesh()) g3_check[w, k] = 1 / (w - eps_k(k));

  EXPECT_GF_NEAR(g3, g3_check);
}

MAKE_MAIN;
