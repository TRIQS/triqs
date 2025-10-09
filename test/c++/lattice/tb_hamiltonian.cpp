#include <nda/gtest_tools.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <triqs/lattice/bz_integrators.hpp>
#include <triqs/utility/integration/adaptive.hpp>

using namespace triqs;
using namespace triqs::lattice;

TEST(tb_tests, simple_construct) { // NOLINT

  // set up tb_hopping object
  std::vector<std::array<long, 3>> displ_vec = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t                                   = 1.0;
  std::vector<nda::array<dcomplex, 2>> overlap_mat_vec =
     std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t, t})));
  auto tb = tb_hamiltonian(displ_vec, overlap_mat_vec);

  // check that we can search for the zero index
  auto Ridx = tb.get_R_idx({0, 1, 0});
  EXPECT_EQ(Ridx, 2);

  EXPECT_COMPLEX_NEAR(std::complex<double>(-0.097887, 0), tb(0.25, 0.5, 0.05)(0, 0), 1e-5);
}

TEST(tb_tests, placeholder_call) { // NOLINT

  // set up tb_hopping object
  double t                                   = 1.0;
  std::vector<std::array<long, 3>> displ_vec = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  auto overlap_mat_vec                       = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t, t})));
  auto tb                                    = tb_hamiltonian(displ_vec, overlap_mat_vec);

  nda::clef::placeholder<1> ky_;
  {
    auto tb_exp = tb(0.33, 0.5, ky_);
    auto tb1    = eval(tb_exp, ky_ = 0.1234);
    EXPECT_COMPLEX_NEAR(tb(0.33, 0.5, 0.1234)(0, 0), tb1(0, 0));
  }
}

TEST(tb_tests, test_evaluators) { // NOLINT

  // test comparing the two operator calls of the FP underyling the tb object

  // set up tb_hopping object
  std::vector<std::array<long, 3>> displ_vec = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t                                   = 1.0;
  auto overlap_mat_vec                       = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t, t})));
  auto tb_H                                  = tb_hamiltonian(displ_vec, overlap_mat_vec);

  auto Hk_ab = tb_H(0.33, 0.5, 0.33);

  // matrix of two kpoints -- we will check against the second one
  nda::matrix<double> kpoint = {{+0.15, +0.338028169014, -0.4}, {0.33, 0.5, 0.33}};
  auto Hk_ab_gemm            = tb_H(kpoint);

  EXPECT_COMPLEX_NEAR(Hk_ab(0, 0), Hk_ab_gemm(1, 0, 0));
}

TEST(tb_tests, tb_ptr_test) { // NOLINT

  // set up tb_hopping object
  int norb                                   = 2;
  std::vector<std::array<long, 3>> displ_vec = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t                                   = 1.0;
  auto overlap_mat_vec                       = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t, t})));
  auto tb                                    = tb_hamiltonian(displ_vec, overlap_mat_vec);
  auto I                                     = nda::eye<dcomplex>(norb);

  double omega = 0.2;
  double Gamma = 0.5;

  // 2d integrations
  namespace ph    = triqs::lattice::placeholders;
  auto f_wk       = inverse(ph::w * I - tb(ph::kx, ph::ky, ph::kz) + Gamma * I * 1i);
  auto result_ptr = integrate_ptr(f_wk, std::vector{omega}, {10, 10, 1}, {});

  // value below in adaptive case is more precise answer to integral -- this is an inaccurate answer for ptr with 10x pts
  dcomplex answer = {-0.21999931548255128, -0.30777709520545604};

  EXPECT_COMPLEX_NEAR(result_ptr(0, 0, 0), answer, 1.e-2);
}

TEST(tb_tests, tb_adaptive_test) { // NOLINT

  // set up tb_hopping object
  std::vector<std::array<long, 3>> displ_vec = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t                                   = 1.0;
  auto overlap_mat_vec                       = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t, t})));
  auto tb                                    = tb_hamiltonian(displ_vec, overlap_mat_vec);
  int norb                                   = 2;
  auto I                                     = nda::eye<dcomplex>(norb);

  std::pair<double, double> D = {0, 1};
  nda::clef::placeholder<0> kx_;
  nda::clef::placeholder<1> ky_;

  double om    = 0.2;
  double Gamma = 0.5;

  // adaptive
  auto int_1d_adapt = triqs::utility::integrate_1d_adapt<nda::matrix<dcomplex>>{1e-5};
  auto f_wk         = inverse(om * I - tb(kx_, ky_, 0.) + Gamma * I * 1i);
  auto result_adapt = integrate(int_1d_adapt, integrate(int_1d_adapt, f_wk, ky_ = D), kx_ = D);

  dcomplex answer = {-0.22338077801907302, -0.34352111773477229};
  EXPECT_COMPLEX_NEAR(result_adapt(0, 0), answer, 1.e-5);
}

TEST(tb_tests, h5_read_write) {

  std::vector<std::array<long, 3>> displ_vec = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t                                   = 1.0;
  auto overlap_mat_vec                       = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag(nda::vector<dcomplex>{t, t})));
  auto tb                                    = tb_hamiltonian(displ_vec, overlap_mat_vec);

  // write
  {
    auto file = h5::file{"test_tb.h5", 'w'};
    auto grp  = h5::group{file};
    h5_write(grp, "hamiltonian", tb);
  }

  // read
  {
    auto file  = h5::file{"test_tb.h5", 'r'};
    auto grp   = h5::group{file};
    auto tb_in = h5::h5_read<triqs::tb_hamiltonian>(grp, "hamiltonian");
    EXPECT_EQ(tb, tb_in);
  }
}

MPI_TEST_MAIN
