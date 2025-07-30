#include <triqs/test_tools/gfs.hpp>
#include <triqs/lattice/wannier_loader.hpp>
//#include <triqs/lattice/tb_hamiltonian.hpp>

using namespace triqs;

TEST(wannier_loader_test, svo_tb) { // NOLINT

  // load quantities from wannier90 using test file svo_tb.dat
  // TODO give this test data a more permanent home
  auto [R, H_R, r_mat] = read_wannier90_tb_data("svo");

  // create a tb_hopping object
  auto tb = tb_hamiltonian(R, H_R);

  // check that we get energies as expected at the gamma point
  nda::matrix<double> kpoint  = {{0., 0., 0.}};
  nda::matrix<double> Ek_band = tb.eigenvalues(kpoint); // shape (kpts, bands)
  for (auto i : {0, 1, 2}) { EXPECT_NEAR(11.6283, Ek_band(0, i), 1e-4); }

  // check energy values at a non-high sym point, where they are not degenerate
  kpoint = {{+0.330985915493, +0.338028169014, 0.}};
  // returns E(kpt, band)
  Ek_band = tb.eigenvalues(kpoint);
  EXPECT_NEAR(13.1134, Ek_band(0, 0), 1e-4);
  EXPECT_NEAR(13.2095, Ek_band(0, 1), 1e-4);
  EXPECT_NEAR(13.5010, Ek_band(0, 2), 1e-4);

  // TODO extend test to check velocities and berry connection contribution
  // TODO check calls on vectors of kpts
}

MAKE_MAIN;
