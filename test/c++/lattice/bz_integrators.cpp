#include <nda/clef/make_lazy.hpp>
#include <nda/matrix_functions.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/lattice/bz_integrators.hpp>
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <nda/nda.hpp>

double eps(double kx, double ky, double kz) { return std::cos(kx) + std::cos(ky) + std::cos(kz); }
CLEF_MAKE_FNT_LAZY(eps);

namespace nda {
  CLEF_MAKE_FNT_LAZY(inverse);
}

namespace triqs::lattice {

  TEST(bz_integrators, adaptive) { // NOLINT

    // Create a Matsubara-frequency mesh
    int n_iw     = 10;
    auto iw_mesh = mesh::imfreq{10, triqs::gfs::Fermion, n_iw};

    // expression to integrate
    namespace ph = triqs::lattice::placeholders;

    // expression to integrate
    auto ex = 1. / (ph::w + eps(ph::kx, ph::ky, ph::kz) + 1.1 * 1i);

    // the relevant parameter here is tolerance -- others should not matter to us.
    adaptive_options options{.tolerance = 1.e-2};
    dcomplex iw = iw_mesh[7];

    // call the integration for this block -- this operates in place on g
    // returns a lambda that takes frequency -- feed it (iw)
    auto result = integrate_adaptive(ex, options)(iw);

    dcomplex answer = {0.385522, 0.0731833};
    EXPECT_COMPLEX_NEAR(answer, result, 1.e-3);
  }

  TEST(bz_integrators, ptr) { // NOLINT

    // Create a Matsubara-frequency mesh
    int n_iw     = 10;
    auto iw_mesh = mesh::imfreq{10, triqs::gfs::Fermion, n_iw};

    // expression to integrate
    namespace ph = triqs::lattice::placeholders;
    auto ex      = 1. / (ph::w + eps(ph::kx, ph::ky, ph::kz) + 0.1 * 1i);

    // call the integration for this block -- this operates in place on g
    auto result = integrate_ptr(ex, std::vector{iw_mesh[7].value()}, {10, 10, 10}, {});

    // explicit comparison to serial result
    dcomplex sum = 0;
    int nk       = 10;
    dcomplex iw  = iw_mesh[7];
    for (int i = 0; i < nk; ++i) {
      for (int j = 0; j < nk; ++j) {
        for (int k = 0; k < nk; ++k) {
          double kx = 1. / double(nk) * i;
          double ky = 1. / double(nk) * j;
          double kz = 1. / double(nk) * k;
          sum += 1. / (iw + eps(kx, ky, kz) + 0.1 * 1i);
        }
      }
    }
    sum /= double(nk * nk * nk);
    EXPECT_COMPLEX_NEAR(sum, result(0, 0, 0), 1.e-13);
  }

  TEST(bz_integrators, bz_integrate) { // NOLINT

    // check the full ptr-adaptive integration procedure runs to the end properly

    // set up tb_hopping object
    auto units                                 = nda::matrix<double>{{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}};
    int norb                                   = 2;
    auto atom_orb_pos                          = std::vector(norb, nda::vector<double>{0., 0., 0.});
    std::vector<std::array<long, 3>> displ_vec = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
    double t                                   = 1.0;
    auto overlap_mat_vec                       = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t, t})));
    auto tb                                    = tb_hamiltonian(displ_vec, overlap_mat_vec);
    auto I                                     = nda::eye<dcomplex>(norb);

    // create expression
    namespace ph = triqs::lattice::placeholders;
    // here is a case with only 2 positive iw points on the mesh
    // one of them is converged by PTR, and one by adaptive in the end
    double Gamma = 0.025;
    double beta  = 10;
    long n_iw    = 2;
    auto expr_kw = nda::inverse(ph::w * I - tb(ph::kx, ph::ky, ph::kz) + Gamma * I * 1i);

    auto iw_mesh = mesh::imfreq{beta, triqs::gfs::Fermion, n_iw, triqs::mesh::imfreq::option::positive_frequencies_only};
    gf<mesh::imfreq, matrix_valued> result(iw_mesh, {norb, norb});
    bz_int_options options{.tolerance = 1.e-2, .k_grid_dims = {10, 10, 10}, .n_k_max = 20};
    result = integrate_bz(expr_kw, iw_mesh, options);

    EXPECT_COMPLEX_NEAR(dcomplex{0.00011757400118563716, -0.4167788744251697}, result[0](0, 0), 1.e-5);
    EXPECT_COMPLEX_NEAR(dcomplex{-2.72737e-17, -0.362975}, result[1](0, 0), 1.e-5);
  }
} // namespace triqs::lattice

MPI_TEST_MAIN
