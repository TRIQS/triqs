#include <nda/gtest_tools.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs;
using namespace triqs::gfs;

TEST(gf_fp_tests, simple_construct) { // NOLINT

  // set up gf on fourier_poly mesh
  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  auto h0k       = gf{mesh::fourier_poly{displ_vec}, {2, 2}};
  double t       = 1.0;
  h0k()          = nda::diag({t, t});

  // check that we can search for the zero index
  auto Ridx = h0k.mesh().to_index({0, 1, 0});
  EXPECT_EQ(Ridx, 2);

  EXPECT_COMPLEX_NEAR(std::complex<double>(-0.097887, 0), h0k(std::tuple{0.25, 0.5, 0.05})(0, 0), 1e-5);
}

TEST(gf_fp_tests, test_evaluators) { // NOLINT

  // test comparing the two operator calls of the FP underlying the gf object

  // set up gf on fourier_poly mesh
  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  auto h0k       = gf{mesh::fourier_poly{displ_vec}, {2, 2}};
  double t       = 1.0;
  h0k()          = nda::diag({t, t});

  auto Hk_ab = h0k(std::tuple{0.33, 0.5, 0.33});

  // matrix of two kpoints -- we will check against the second one
  nda::matrix<double> kpoint = {{+0.15, +0.338028169014, -0.4}, {0.33, 0.5, 0.33}};
  auto Hk_ab_gemm            = h0k(kpoint);

  EXPECT_COMPLEX_NEAR(Hk_ab(0, 0), Hk_ab_gemm(1, 0, 0));
}

TEST(gf_fp_tests, gf_algebra) { // NOLINT

  // gf arithmetic: compose a Hamiltonian from separate hopping channels
  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}}};
  double t1 = 1.0, t2 = 0.5;

  // orbital 1 hopping
  auto h1 = gf{mesh::fourier_poly{displ_vec}, {2, 2}};
  h1()    = nda::diag({t1, 0.0});

  // orbital 2 hopping
  auto h2 = gf{mesh::fourier_poly{displ_vec}, {2, 2}};
  h2()    = nda::diag({0.0, t2});

  // compose full Hamiltonian using gf addition
  auto h0k = gf{h1 + h2};
  auto Hk  = h0k(std::tuple{0.25, 0.5, 0.0});
  EXPECT_COMPLEX_NEAR(Hk(0, 0), t1 * (std::cos(2 * M_PI * 0.25) + std::cos(2 * M_PI * 0.5)) * 2, 1e-12);
  EXPECT_COMPLEX_NEAR(Hk(1, 1), t2 * (std::cos(2 * M_PI * 0.25) + std::cos(2 * M_PI * 0.5)) * 2, 1e-12);

  // scale hopping using gf scalar multiplication
  auto h_scaled = gf{3.0 * h1};
  EXPECT_COMPLEX_NEAR(h_scaled(std::tuple{0.25, 0.5, 0.0})(0, 0), 3.0 * h1(std::tuple{0.25, 0.5, 0.0})(0, 0), 1e-12);
}

TEST(gf_fp_tests, eigenvalues) { // NOLINT

  // set up gf on fourier_poly mesh
  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  auto h0k       = gf{mesh::fourier_poly{displ_vec}, {2, 2}};
  double t       = 1.0;
  h0k()          = nda::diag({t, t});

  // batch evaluate H(k) and diagonalize per k-point
  nda::matrix<double> kpoints = {{0., 0., 0.}, {0.25, 0.5, 0.05}};
  auto Hk                     = h0k(kpoints); // [nk, norb, norb]

  auto ev0 = nda::linalg::eigvalsh(Hk(0, nda::ellipsis()));
  auto ev1 = nda::linalg::eigvalsh(Hk(1, nda::ellipsis()));

  // at gamma point both eigenvalues are 2*t*3 = 6 (sum of 6 cos(0))
  EXPECT_NEAR(ev0(0), 6.0, 1e-10);
  EXPECT_NEAR(ev0(1), 6.0, 1e-10);

  // at (0.25, 0.5, 0.05): 2*t*(cos(2pi*0.25) + cos(2pi*0.5) + cos(2pi*0.05))
  double expected = 2.0 * t * (std::cos(2 * M_PI * 0.25) + std::cos(2 * M_PI * 0.5) + std::cos(2 * M_PI * 0.05));
  EXPECT_NEAR(ev1(0), expected, 1e-10);
  EXPECT_NEAR(ev1(1), expected, 1e-10);
}

TEST(gf_fp_tests, eigenvectors) { // NOLINT

  // set up gf on fourier_poly mesh
  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  auto h0k       = gf{mesh::fourier_poly{displ_vec}, {2, 2}};
  double t       = 1.0;
  h0k()          = nda::diag({t, t});

  // check that the eigenvectors properly diagonalize H(k)
  nda::matrix<double> kpoints = {{0.33, 0.5, 0.33}};
  auto Hk                     = h0k(kpoints);

  nda::matrix<dcomplex> Hk_ab = Hk(0, nda::ellipsis());
  auto [ens, Uk]              = nda::linalg::eigh(Hk_ab);
  auto Hk_diag                = nda::dagger(Uk) * Hk_ab * Uk;

  EXPECT_COMPLEX_NEAR(Hk_diag(0, 0), ens(0), 1e-10);
  EXPECT_COMPLEX_NEAR(Hk_diag(1, 1), ens(1), 1e-10);
  EXPECT_COMPLEX_NEAR(Hk_diag(0, 1), 0.0, 1e-10);
}

TEST(gf_fp_tests, scalar_evaluate_k) { // NOLINT

  // scalar-valued gf on fourier_poly mesh
  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  auto g         = gf<mesh::fourier_poly, scalar_valued>{mesh::fourier_poly{displ_vec}};
  for (auto const &[i, mp] : itertools::enumerate(g.mesh())) g[mp] = dcomplex(double(i + 1), 0);

  auto sum_at = [&](std::array<double, 3> k) {
    dcomplex s = 0;
    for (long i = 0; i < static_cast<long>(displ_vec.size()); ++i) {
      auto const &R = displ_vec[i];
      double kR     = k[0] * R[0] + k[1] * R[1] + k[2] * R[2];
      s += dcomplex(double(i + 1), 0) * std::exp(2i * M_PI * kR);
    }
    return s;
  };

  // single-k via tuple
  auto k1 = std::tuple{0.25, 0.5, 0.05};
  EXPECT_COMPLEX_NEAR(g(k1), sum_at({0.25, 0.5, 0.05}), 1e-12);

  // batch via nda matrix [nk, 3]
  nda::matrix<double> ks = {{0.1, 0.2, 0.3}, {0.25, 0.5, 0.05}};
  auto batch             = g(ks); // shape [nk]
  EXPECT_COMPLEX_NEAR(batch(0), sum_at({0.1, 0.2, 0.3}), 1e-12);
  EXPECT_COMPLEX_NEAR(batch(1), sum_at({0.25, 0.5, 0.05}), 1e-12);
}

TEST(gf_fp_tests, tensor3_evaluate_k) { // NOLINT

  // rank-3 tensor-valued gf on fourier_poly mesh with non-trivial R-dependence
  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}}};
  auto g         = gf<mesh::fourier_poly, tensor_valued<3>>{mesh::fourier_poly{displ_vec}, {2, 2, 2}};

  // set g[mp] = (double(R[0]) + 1) * ones(2,2,2), with a single off-pattern element to break symmetry
  for (auto const &mp : g.mesh()) {
    auto const R = mp.value();
    g[mp]        = (double(R[0]) + 1.) * nda::ones<dcomplex>(2, 2, 2);
    g[mp](0, 1, 1) += dcomplex(0, R[1]); // imag part breaks the real symmetry
  }

  // explicit reference sum
  auto ref_at = [&](std::array<double, 3> k) {
    auto s = nda::array<dcomplex, 3>::zeros(std::array{2l, 2l, 2l});
    for (long i = 0; i < static_cast<long>(displ_vec.size()); ++i) {
      auto const &R = displ_vec[i];
      double kR     = k[0] * R[0] + k[1] * R[1] + k[2] * R[2];
      s += std::exp(2i * M_PI * kR) * g.data()(i, nda::ellipsis{});
    }
    return s;
  };

  // single-k
  auto k1     = std::tuple{0.15, 0.25, 0.05};
  auto got_s  = g(k1);
  auto want_s = ref_at({0.15, 0.25, 0.05});
  for (long a = 0; a < 2; ++a)
    for (long b = 0; b < 2; ++b)
      for (long c = 0; c < 2; ++c) EXPECT_COMPLEX_NEAR(got_s(a, b, c), want_s(a, b, c), 1e-12);

  // batch
  nda::matrix<double> ks = {{0.1, 0.2, 0.3}, {0.15, 0.25, 0.05}};
  auto got_b             = g(ks); // shape [nk, 2, 2, 2]
  auto want_b            = ref_at({0.15, 0.25, 0.05});
  for (long a = 0; a < 2; ++a)
    for (long b = 0; b < 2; ++b)
      for (long c = 0; c < 2; ++c) EXPECT_COMPLEX_NEAR(got_b(1, a, b, c), want_b(a, b, c), 1e-12);
}

TEST(gf_fp_tests, h5_read_write) {

  mpi::communicator world;
  if (world.rank() != 0) GTEST_SKIP();

  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  auto h0k       = gf{mesh::fourier_poly{displ_vec}, {2, 2}};
  double t       = 1.0;
  h0k()          = nda::diag({t, t});

  // write
  {
    auto file = h5::file{"test_gf_fp.h5", 'w'};
    auto grp  = h5::group{file};
    h5_write(grp, "hamiltonian", h0k);
  }

  // read
  {
    auto file   = h5::file{"test_gf_fp.h5", 'r'};
    auto grp    = h5::group{file};
    auto h0k_in = h5::h5_read<gf<mesh::fourier_poly, matrix_valued>>(grp, "hamiltonian");
    EXPECT_GF_NEAR(h0k, h0k_in, 1e-15);
  }
}

TEST(gf_fp_tests, mpi_broadcast) {

  mpi::communicator world;

  auto displ_vec = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}}};
  auto h0k_ref   = gf{mesh::fourier_poly{displ_vec}, {2, 2}};
  double t       = 1.0;
  h0k_ref()      = nda::diag({t, t});

  // Only rank 0 has the actual data, others have default-constructed object
  auto h0k = world.rank() == 0 ? h0k_ref : gf<mesh::fourier_poly, matrix_valued>{};

  mpi::broadcast(h0k, world, 0);

  EXPECT_GF_NEAR(h0k, h0k_ref, 1e-15);
}

MPI_TEST_MAIN
