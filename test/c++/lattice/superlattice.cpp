
#include <triqs/tight_binding/superlattice.hpp>
#include <triqs/tight_binding/tb_hamiltonian.hpp>
#include <triqs/lattice/gloc.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/ostream.h>

// WHy this  ?
template <typename T, int Rank, typename Layout, char Algebra, typename Allocator>
struct fmt::formatter<nda::basic_array<T, Rank, Layout, Algebra, Allocator>> : fmt::ostream_formatter {};

using namespace triqs::lattice;
using namespace triqs::tb;

TEST(SuperLatticeTest, 2x2SquareLattice) {

  double t                                             = 1.0;
  std::vector<std::array<long, 2>> displ_vec           = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
  std::vector<nda::array<dcomplex, 2>> overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t})));

  auto tb = fourier_polynomial<2, 2>(displ_vec, overlap_mat_vec);

  auto SL = superlattice{{{2, 0}, {0, 2}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}};

  // Call the fold function
  auto result = fold(SL, tb);

  auto Rs    = result.get_R_list();
  auto Coefs = result.get_coefficients();

  auto expected_Rs = std::vector<std::array<long, 2>>{{0, 0}, {-1, 0}, {0, -1}, {1, 0}, {0, 1}};

  auto t_SL_0   = nda::matrix<dcomplex>{{0, t, t, 0}, {t, 0, 0, t}, {t, 0, 0, t}, {0, t, t, 0}};
  auto t_SL_1h  = nda::matrix<dcomplex>{{0, 0, 0, 0}, {t, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, t, 0}};
  auto t_SL_1v  = nda::matrix<dcomplex>{{0, 0, 0, 0}, {0, 0, 0, 0}, {t, 0, 0, 0}, {0, t, 0, 0}};
  auto t_SL_m1h = nda::matrix<dcomplex>{transpose(t_SL_1h)};
  auto t_SL_m1v = nda::matrix<dcomplex>{transpose(t_SL_1v)};

  auto expected_coeffs = std::vector<nda::matrix<dcomplex>>{t_SL_0, t_SL_m1h, t_SL_m1v, t_SL_1h, t_SL_1v};

  ASSERT_EQ(Rs, expected_Rs);
  ASSERT_EQ(Coefs.size(), expected_coeffs.size());
  for (size_t i = 0; i < Coefs.size(); ++i) {
    ASSERT_EQ(Coefs[i], expected_coeffs[i]) << "Mismatch at Coefs[" << i << "] = " << Coefs[i] << "\n  expected: " << expected_coeffs[i];
  }
}
// ------------------------------------------------
TEST(SuperLatticeTest, 2x2SquareLattice3d) {
  double t                                             = 1.0;
  std::vector<std::array<long, 3>> displ_vec           = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  std::vector<nda::array<dcomplex, 2>> overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t})));

  //auto tb = fourier_polynomial<2, 3>(displ_vec, overlap_mat_vec);
  auto tb = tb_hamiltonian(displ_vec, overlap_mat_vec);

  auto SL = superlattice{{{2, 0, 0}, {0, 2, 0}, {0, 0, 1}}, {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}};

  // Call the fold function
  auto result = fold(SL, tb);

  auto Rs    = result.get_R_list();
  auto Coefs = result.get_coefficients();

  auto expected_Rs = std::vector<std::array<long, 3>>{{0, 0, 1}, {0, 0, -1}, {0, 0, 0}, {-1, 0, 0}, {0, -1, 0}, {1, 0, 0}, {0, 1, 0}};

  auto t_SL_0   = nda::matrix<dcomplex>{{0, t, t, 0}, {t, 0, 0, t}, {t, 0, 0, t}, {0, t, t, 0}};
  auto t_SL_1h  = nda::matrix<dcomplex>{{0, 0, 0, 0}, {t, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, t, 0}};
  auto t_SL_1v  = nda::matrix<dcomplex>{{0, 0, 0, 0}, {0, 0, 0, 0}, {t, 0, 0, 0}, {0, t, 0, 0}};
  auto t_SL_m1h = nda::matrix<dcomplex>{transpose(t_SL_1h)};
  auto t_SL_m1v = nda::matrix<dcomplex>{transpose(t_SL_1v)};
  auto id       = nda::eye<dcomplex>(4);

  auto expected_coeffs = std::vector<nda::matrix<dcomplex>>{id, id, t_SL_0, t_SL_m1h, t_SL_m1v, t_SL_1h, t_SL_1v};

  ASSERT_EQ(Rs, expected_Rs);
  ASSERT_EQ(Coefs.size(), expected_coeffs.size());
  for (size_t i = 0; i < Coefs.size(); ++i) {
    ASSERT_EQ(Coefs[i], expected_coeffs[i]) << "Mismatch at Coefs[" << i << "] = " << Coefs[i] << "\n  expected: " << expected_coeffs[i];
  }
}
// ------------------------------------------------

TEST(SuperLatticeTest, 2x1StaggeredCluster) {

  // square lattice.
  double t                                             = 1.0;
  std::vector<std::array<long, 2>> displ_vec           = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
  std::vector<nda::array<dcomplex, 2>> overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t})));
  auto tb                                              = fourier_polynomial<2, 2>(displ_vec, overlap_mat_vec);

  auto SL = superlattice{{{1, -1}, {1, 1}}, {{0, 0}, {1, 0}}};

  // Call the fold function
  auto result = fold(SL, tb);

  auto Rs    = result.get_R_list();
  auto Coefs = result.get_coefficients();
  //for (size_t i = 0; i < Rs.extent(0); ++i) { std::cout << "R_SL[" << i << "] = " << Rs[i] << ", Coef_SL[" << i << "] = " << Coefs[i] << std::endl; }

  auto expected_Rs = std::vector<std::array<long, 2>>{{0, 0}, {-1, -1}, {-1, 0}, {0, -1}, {1, 1}, {0, 1}, {1, 0}};

  auto t_SL_00   = nda::matrix<dcomplex>{{0, t}, {t, 0}};
  auto t_SL_10   = nda::matrix<dcomplex>{{0, 0}, {1, 0}};
  auto t_SL_01   = nda::matrix<dcomplex>{{0, 0}, {1, 0}};
  auto t_SL_11   = nda::matrix<dcomplex>{{0, 0}, {1, 0}};
  auto t_SL_m10  = nda::matrix<dcomplex>{transpose(t_SL_10)};
  auto t_SL_0m1  = nda::matrix<dcomplex>{transpose(t_SL_01)};
  auto t_SL_m1m1 = nda::matrix<dcomplex>{transpose(t_SL_11)};

  auto expected_coeffs = std::vector<nda::matrix<dcomplex>>{t_SL_00, t_SL_m1m1, t_SL_m10, t_SL_0m1, t_SL_11, t_SL_01, t_SL_10};

  ASSERT_EQ(Rs, expected_Rs);
  ASSERT_EQ(Coefs.size(), expected_coeffs.size());
  for (size_t i = 0; i < Coefs.size(); ++i) {
    ASSERT_EQ(Coefs[i], expected_coeffs[i]) << "Mismatch at R =" << Rs[i] << "  Coefs[" << i << "] = " << Coefs[i]
                                            << "\n  expected: " << expected_coeffs[i];
  }
}

double random_constant() {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dist(0, 1);
  return dist(gen);
}

///-----------------------------------------
auto eps   = 1.e-12;
auto w_max = 10.0;

auto make_random_self(int M) {
  auto g           = gfs::gf<mesh::dlr_imfreq>{{5.0, triqs::mesh::statistic_enum::Fermion, w_max, eps}, {M, M}};
  auto const &mesh = g.mesh();
  auto A           = random_constant();
  auto B           = random_constant();
  for (auto &&[n, iw] : itertools::enumerate(mesh)) { g.data()(n, nda::range::all, nda::range::all) = A + B / (iw + 10); }
  return g;
}

// // ------------------------------------------------
TEST(SuperLatticeTest, 2x2SquareLattice3d_with_gloc) {
  double t                                             = 1.0;
  std::vector<std::array<long, 3>> displ_vec           = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  std::vector<nda::array<dcomplex, 2>> overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t})));

  auto tb = tb_hamiltonian(displ_vec, overlap_mat_vec);

  auto SL    = superlattice{{{2, 0, 0}, {0, 2, 0}, {0, 0, 1}}, {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}};
  auto tb_AF = fold(SL, tb);

  auto Sigma1 = make_random_self(tb.n_orbitals());
  auto M      = tb_AF.n_orbitals();
  auto Sigma2 = gfs::gf<mesh::dlr_imfreq>{Sigma1.mesh(), {M, M}};
  for (auto i : nda::range(M)) Sigma2.data()(nda::range::all, i, i) = Sigma1.data()(nda::range::all, 0, 0);

  auto opt1 = lattice::bz_int_options{.k_grid = {10, 10, 5}, .k_grid_max = {10, 10, 5}, .run_adaptive = false};
  auto opt2 = lattice::bz_int_options{.k_grid = {5, 5, 5}, .k_grid_max = {5, 5, 5}, .run_adaptive = false};
  auto g1   = gloc(tb, 0.0, Sigma1, opt1);
  auto g2   = gloc(tb_AF, 0.0, Sigma2, opt2);

  for (auto i : nda::range(M)) EXPECT_ARRAY_NEAR(g1.data()(nda::range::all, 0, 0), (g2.data()(nda::range::all, i, i)), 1e-12);
}

MAKE_MAIN;