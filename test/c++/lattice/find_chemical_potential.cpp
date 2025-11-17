#include <nda/nda.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/lattice/bz_integrators.hpp>
#include <triqs/mesh/imfreq.hpp>
#include <triqs/lattice/gloc.hpp>
#include <triqs/tb/wannier_loader.hpp>

using namespace triqs;
using namespace tb;

TEST(find_mu_tests, find_mu_multiorbital_svo) { // NOLINT

  auto [R, H_R, r_mat] = read_wannier90_tb_data("../tight_binding/svo"); //"../tight_binding/svo");
  auto tb              = tb_hamiltonian(R, H_R);

  double beta  = 10;
  long n_iw    = 51;
  auto iw_mesh = mesh::imfreq{beta, gfs::Fermion, n_iw};

  // make a zero self energy with spin up and down channels
  auto Sigma       = gfs::gf<mesh::imfreq>{iw_mesh, {tb.n_orbitals(), tb.n_orbitals()}};
  auto Sigma_block = gfs::block_gf<mesh::imfreq>({"up", "dn"}, Sigma);

  // calculate mu, as well as density from the returned mu value to check it works
  // in both directions
  lattice::bz_int_options options{.k_grid = {12, 12, 12}, .k_grid_max = {12, 12, 12}, .run_adaptive = false};
  auto mu   = lattice::find_chemical_potential(1.0, tb, Sigma_block, options);
  auto gloc = lattice::gloc(tb, mu, Sigma_block, options);

  double n = 0;
  for (auto [ib, block_name] : itertools::enumerate(gloc)) { n += real(nda::trace(density(gloc[ib]))); }

  // check against reference values that this works
  // DFT value: Efermi = 12.6263 -- can be replicated with 12x12x12 grid and small beta
  EXPECT_NEAR(mu, 12.580778955845707, 1e-4);
  EXPECT_NEAR(n, 1.0, 1e-4);
}

MAKE_MAIN;

//