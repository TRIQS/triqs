#include "nda/layout/range.hpp"
#include <algorithm>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/lattice/bz_integrators.hpp>
#include <triqs/mesh/imfreq.hpp>
#include <triqs/lattice/gloc.hpp>
#include <triqs/lattice/tb_hamiltonian.hpp>

using namespace triqs;

TEST(gloc_tests, gloc_single_orbital) { // NOLINT

  // set up a tb_hamiltonian
  double t             = 1.0;
  auto displ_vec       = std::vector<std::array<long, 3>>{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t})));
  auto tb              = tb_hamiltonian(displ_vec, overlap_mat_vec);
  int norb             = 1;

  double beta  = 10;
  long n_iw    = 5;
  auto iw_mesh = mesh::imfreq{beta, gfs::Fermion, n_iw, mesh::imfreq::option::positive_frequencies_only};

  // make a zero self energy
  // TODO what is the right way to use this block2gf make function?
  auto Sigma = gfs::gf<mesh::imfreq>{iw_mesh, {norb, norb}};

  // call the integration function
  lattice::bz_int_options options{.tolerance = 1.e-3, .k_grid_dims = {10, 10, 10}, .n_k_max = 40};

  auto Gloc = gloc(tb, 0., Sigma, options);

  std::vector<dcomplex> ref    = {{0, -0.419074}, {0, -0.364261}, {0, -0.316034}, {0, -0.275981}, {0, -0.243231}};
  std::vector<dcomplex> result = iw_mesh | std::views::transform([&Gloc](auto w) { return Gloc[w](0, 0); }) | std::ranges::to<std::vector>();
  for (auto idx_w : nda::range(n_iw)) { EXPECT_COMPLEX_NEAR(ref[idx_w], result[idx_w], 1e-6); }
}

TEST(gloc_tests, gloc_multiple_orbitals) { // NOLINT

  // set up tb_hopping object
  std::vector<std::array<long, 3>> displ_vec = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t                                   = 1.0;
  auto overlap_mat_vec                       = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t, t})));
  auto tb                                    = tb_hamiltonian(displ_vec, overlap_mat_vec);
  int norb                                   = 2;

  double beta  = 10;
  long n_iw    = 5;
  auto iw_mesh = mesh::imfreq{beta, gfs::Fermion, n_iw, mesh::imfreq::option::positive_frequencies_only};

  // make a zero self energy
  auto Sigma = gfs::gf<mesh::imfreq>{iw_mesh, {norb, norb}};

  // call the integration function
  lattice::bz_int_options options{.tolerance = 1.e-3, .k_grid_dims = {10, 10, 10}, .n_k_max = 40};

  auto Gloc = gloc(tb, 0., Sigma, options);
  //for (auto w : iw_mesh) { std::cout << Gloc[w] << std::endl; }

  // check against reference values that this works
  std::vector<dcomplex> ref    = {{0, -0.419074}, {0, -0.364261}, {0, -0.316034}, {0, -0.275981}, {0, -0.243231}};
  std::vector<dcomplex> result = iw_mesh | std::views::transform([&Gloc](auto w) { return Gloc[w](0, 0); }) | std::ranges::to<std::vector>();
  for (auto idx_w : nda::range(n_iw)) { EXPECT_COMPLEX_NEAR(ref[idx_w], result[idx_w], 1e-6); }
}

MAKE_MAIN;
