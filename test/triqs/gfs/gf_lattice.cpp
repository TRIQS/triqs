#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>

using index_t = utility::mini_vector<int, 3>;
using K_t     = std::array<double, 3>;

TEST(Gfs, cyclic_lattice) {
  double beta = 1;
  triqs::clef::placeholder<0> r_;

  auto bl  = bravais_lattice{make_unit_matrix<double>(2)};
  auto m_r = gf_mesh<cyclic_lattice>{bl, 2};
  ASSERT_EQ(m_r.size(), 4);

  auto Gr = gf<cyclic_lattice>{m_r, {2, 2}};

  // Clef Assignment
  Gr(r_) << 1. + r_(0) + r_(1);
  EXPECT_COMPLEX_NEAR(Gr(index_t{0, 0, 0})(0, 0), 1);
  Gr(r_) << exp(-r_(0));

  std::cout << Gr.mesh() << "\n";
}

// FIXME : Test matrix case with same code ?
TEST(Gfs, brillouin_zone) {
  double beta = 1;
  triqs::clef::placeholder<0> k_;

  int n_k = 50;

  auto bz  = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
  auto m_k = gf_mesh<b_zone>{bz, n_k};
  ASSERT_EQ(m_k.size(), n_k * n_k);

  auto Gk = gf<b_zone, scalar_valued>{m_k}; //, {2, 2}};

  //Gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));
  for (auto &&k : Gk.mesh()) Gk[k] = -2 * (cos(k(0)) + cos(k(1)));

  ASSERT_EQ(Gk.mesh().locate_neighbours(arrays::vector<double>{0, 0, 0}), (utility::mini_vector<long, 3>({0, 0, 0})));
  EXPECT_COMPLEX_NEAR(Gk(K_t{0, 0, 0}), -4);
  EXPECT_COMPLEX_NEAR(Gk(K_t{M_PI, M_PI, M_PI}), 4);
  EXPECT_COMPLEX_NEAR(Gk(K_t{2 * M_PI, 2 * M_PI, 2 * M_PI}), -4);

  // reevaluate on the mesh itself.
  for (auto &&k : Gk.mesh()) {
    EXPECT_NEAR(k(0), k.index()[0] / double(n_k) * 2 * M_PI, 1.e-14);
    EXPECT_NEAR(k(1), k.index()[1] / double(n_k) * 2 * M_PI, 1.e-14);
    double res = -2 * (cos(k(0)) + cos(k(1)));
    EXPECT_COMPLEX_NEAR(Gk(K_t{k(0), k(1), 0}), res, 1.e-14);
  }

  // evaluate on a larger grid
  int n_k2 = 3 * n_k;
  for (int nkx = 0; nkx < n_k2; ++nkx)
    for (int nky = 0; nky < n_k2; ++nky) {
      double kx  = nkx / double(n_k2) * 2 * M_PI;
      double ky  = nky / double(n_k2) * 2 * M_PI;
      double res = -2 * (cos(kx) + cos(ky));
      EXPECT_COMPLEX_NEAR(Gk(K_t{kx, ky, 0}), res, 0.01);
    }
}

// ----------------------------------------------------

TEST(Gfs, brillouin_zoneMatrix) {
  double beta = 1;
  triqs::clef::placeholder<0> k_;

  int n_k = 50;

  auto bz  = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
  auto m_k = gf_mesh<b_zone>{bz, n_k};
  ASSERT_EQ(m_k.size(), n_k * n_k);

  auto Gk = gf<b_zone, matrix_valued>{m_k, {2, 2}};

  //Gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));
  for (auto &&k : Gk.mesh()) Gk[k] = -2 * (cos(k(0)) + cos(k(1)));

  ASSERT_EQ(Gk.mesh().locate_neighbours(arrays::vector<double>{0, 0, 0}), (utility::mini_vector<long, 3>({0, 0, 0})));
  //auto a = Gk(index_t{0, 0, 0});
  auto a = Gk(K_t{0, 0, 0});

  TRIQS_PRINT(a);
  EXPECT_COMPLEX_NEAR(a(0, 0), -4);

  // reevaluate on the mesh itself.
  for (auto &&k : Gk.mesh()) {
    EXPECT_NEAR(k(0), k.index()[0] / double(n_k) * 2 * M_PI, 1.e-14);
    EXPECT_NEAR(k(1), k.index()[1] / double(n_k) * 2 * M_PI, 1.e-14);
    double res = -2 * (cos(k(0)) + cos(k(1)));
    EXPECT_COMPLEX_NEAR(Gk(K_t{k(0), k(1), 0})(0, 0), res, 1.e-14);
  }

  // evaluate on a larger grid
  int n_k2 = 3 * n_k;
  for (int nkx = 0; nkx < n_k2; ++nkx)
    for (int nky = 0; nky < n_k2; ++nky) {
      double kx  = nkx / double(n_k2) * 2 * M_PI;
      double ky  = nky / double(n_k2) * 2 * M_PI;
      double res = -2 * (cos(kx) + cos(ky));
      EXPECT_COMPLEX_NEAR(Gk(K_t{kx, ky, 0})(0, 0), res, 0.01);
    }

  std::cout << Gk.mesh() << "\n";
}
MAKE_MAIN;
