#include <triqs/test_tools/gfs.hpp>
#include <triqs/experimental/lattice/wannier_loader.hpp>

using namespace triqs::experimental::lattice;

TEST(wannier_loader_test, svo_tb_and_hr) { // NOLINT

  // ----------------------------------------------------------------------------
  // Load in and test reading of hr and tb files from Wannier90

  // load quantities from wannier90 using test file svo_tb.dat
  auto [R, H_R, _, latt_vec] = read_wannier90_tb_data("svo");
  // load in the same information using svo_hr.dat, from the same calculation
  // to confirm they give the same final answers
  auto tb_from_hr = make_tb_hk_from_w90_hr_file("svo");

  // confirm the lattice vectors are read in correctly
  for (auto i : {0, 1, 2})
    for (auto j : {0, 1, 2})
      if (i == j)
        EXPECT_NEAR(latt_vec(i, j), 3.8419797311652863, 1e-9);
      else
        EXPECT_NEAR(latt_vec(i, j), 0.0, 1e-9);

  // create a tb_hopping object
  auto tb_from_tb = tb_hk(R, H_R);

  // ----------------------------------------------------------------------------
  // check that we get energies as expected at the gamma point, and for a
  // non-high sym point, where they are not degenerate.
  nda::matrix<double> gamma  = {{0., 0., 0.}};
  nda::matrix<double> kpoint = {{+0.330985915493, +0.338028169014, 0.}};

  // These should be the same for both datasets
  for (auto tb : {&tb_from_tb, &tb_from_hr}) {
    // check gamma
    auto Ek_band = tb->eigenvalues(gamma);
    for (auto i : {0, 1, 2}) { EXPECT_NEAR(11.6283, Ek_band(0, i), 1e-4); }

    // check the arbitrary point
    Ek_band = tb->eigenvalues(kpoint);
    EXPECT_NEAR(13.1134, Ek_band(0, 0), 1e-4);
    EXPECT_NEAR(13.2095, Ek_band(0, 1), 1e-4);
    EXPECT_NEAR(13.5010, Ek_band(0, 2), 1e-4);

    // check that the eigenvectors of the Hamiltonian properly diagonalize it at the arbitrary point
    auto [ens, eigvecs]         = tb->eigenvectors(kpoint);
    nda::matrix<dcomplex> Uk    = eigvecs(0, nda::range::all, nda::range::all); // pull out the second kpt for testing
    nda::matrix<dcomplex> Hk_ab = tb->operator()(kpoint)(0, nda::range::all, nda::range::all);
    auto Hk_diag                = nda::dagger(Uk) * Hk_ab * Uk;

    EXPECT_COMPLEX_NEAR(Hk_diag(0, 0), Ek_band(0, 0), 1e-4);
    EXPECT_COMPLEX_NEAR(Hk_diag(1, 1), Ek_band(0, 1), 1e-4);
    EXPECT_COMPLEX_NEAR(Hk_diag(2, 2), Ek_band(0, 2), 1e-4);
  }
}

MAKE_MAIN;
