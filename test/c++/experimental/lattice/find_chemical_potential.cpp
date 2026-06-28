#include <nda/nda.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/experimental/lattice/bz_integrators.hpp>
#include <triqs/mesh/imfreq.hpp>
#include <triqs/mesh/dlr_imfreq.hpp>
#include <triqs/experimental/lattice/gloc.hpp>
#include <triqs/experimental/lattice/wannier_loader.hpp>

using namespace triqs;
using namespace experimental::lattice;

TEST(find_mu_tests, find_mu_multiorbital_svo) { // NOLINT

  auto tb = make_tb_hk_from_w90_tb_file("./svo");

  double beta  = 10;
  long n_iw    = 51;
  auto iw_mesh = mesh::imfreq{beta, gfs::Fermion, n_iw};

  // make a zero self energy with spin up and down channels
  auto Sigma       = gfs::gf<mesh::imfreq>{iw_mesh, {tb.n_orbitals(), tb.n_orbitals()}};
  auto Sigma_block = gfs::block_gf<mesh::imfreq>({"up", "dn"}, Sigma);

  // calculate mu, as well as density from the returned mu value to check it works
  // in both directions
  bz_int_options options{.k_grid = {12, 12, 12}, .k_grid_max = {12, 12, 12}, .run_adaptive = false};
  auto mu          = find_chemical_potential(1.0, tb, Sigma_block, options);
  auto gloc_result = gloc(tb, mu, Sigma_block, options);

  double n = 0;
  for (auto [ib, block_name] : itertools::enumerate(gloc_result)) { n += real(nda::trace(density(gloc_result[ib]))); }

  // check against reference values that this works
  // DFT value: Efermi = 12.6263 -- can be replicated with 12x12x12 grid and small beta
  EXPECT_NEAR(mu, 12.580778955845707, 1e-4);
  EXPECT_NEAR(n, 1.0, 1e-4);
}

TEST(find_mu_tests, find_mu_multiorbital_svo_dlr) { // NOLINT

  auto tb = make_tb_hk_from_w90_tb_file("./svo");

  double beta = 10;
  auto w_mesh = mesh::dlr_imfreq{beta, gfs::Fermion, 20.0, 1e-8};

  // make a zero self energy with spin up and down channels on a DLR Matsubara mesh
  auto Sigma       = gfs::gf<mesh::dlr_imfreq>{w_mesh, {tb.n_orbitals(), tb.n_orbitals()}};
  auto Sigma_block = gfs::block_gf<mesh::dlr_imfreq>({"up", "dn"}, Sigma);

  // use the same fixed grid as the imfreq test for a fair comparison
  bz_int_options options{.k_grid = {12, 12, 12}, .k_grid_max = {12, 12, 12}, .run_adaptive = false};
  auto mu          = find_chemical_potential(1.0, tb, Sigma_block, options);
  auto gloc_result = gloc(tb, mu, Sigma_block, options);

  double n = 0;
  for (auto [ib, block_name] : itertools::enumerate(gloc_result)) { n += real(nda::trace(density(gloc_result[ib]))); }

  // the compact DLR Matsubara mesh reproduces the dense imfreq reference (mu ~ 12.5808)
  EXPECT_NEAR(mu, 12.580778955845707, 1e-4);
  EXPECT_NEAR(n, 1.0, 1e-4);
}

MAKE_MAIN;

//