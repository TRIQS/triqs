#include <nda/gtest_tools.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/tb/tb_hamiltonian.hpp>
#include <triqs/lattice/bz_integrators.hpp>
#include <triqs/utility/integration/adaptive.hpp>

using namespace triqs;
using namespace triqs::lattice;
using namespace triqs::tb;

TEST(tb_tests, simple_construct) { // NOLINT

  // set up tb_hopping object
  auto displ_vec       = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag({t, t})));
  auto tb              = tb_hamiltonian(displ_vec, overlap_mat_vec);

  // check that we can search for the zero index
  auto Ridx = tb.get_R_idx({0, 1, 0});
  EXPECT_EQ(Ridx, 2);

  EXPECT_COMPLEX_NEAR(std::complex<double>(-0.097887, 0), tb(0.25, 0.5, 0.05)(0, 0), 1e-5);
}

TEST(tb_tests, placeholder_call) { // NOLINT

  // set up tb_hopping object
  auto displ_vec       = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag({t, t})));
  auto tb              = tb_hamiltonian(displ_vec, overlap_mat_vec);

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
  auto displ_vec       = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag({t, t})));
  auto tb_H            = tb_hamiltonian(displ_vec, overlap_mat_vec);

  auto Hk_ab = tb_H(0.33, 0.5, 0.33);

  // matrix of two kpoints -- we will check against the second one
  nda::matrix<double> kpoint = {{+0.15, +0.338028169014, -0.4}, {0.33, 0.5, 0.33}};
  auto Hk_ab_gemm = tb_H(kpoint);

  EXPECT_COMPLEX_NEAR(Hk_ab(0, 0), Hk_ab_gemm(1, 0, 0));
}

TEST(tb_tests, tb_ptr_test) { // NOLINT

  // set up tb_hopping object
  int norb             = 2;
  auto displ_vec       = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag({t, t})));
  auto tb              = tb_hamiltonian(displ_vec, overlap_mat_vec);
  auto I               = nda::eye<dcomplex>(norb);

  double omega = 0.2;
  double Gamma = 0.5;

  // 2d integrations
  namespace ph    = triqs::lattice::placeholders;
  auto f_wk       = nda::linalg::inv(ph::w * I - tb(ph::kx, ph::ky, ph::kz) + Gamma * I * 1i);
  auto result_ptr = integrate_ptr(f_wk, std::vector{omega}, {10, 10, 1}, {});

  // value below in adaptive case is more precise answer to integral -- this is an inaccurate answer for ptr with 10x pts
  dcomplex answer = {-0.21999931548255128, -0.30777709520545604};

  EXPECT_COMPLEX_NEAR(result_ptr(0, 0, 0), answer, 1.e-2);
}

TEST(tb_tests, tb_adaptive_test) { // NOLINT

  // set up tb_hopping object
  auto displ_vec       = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag({t, t})));
  auto tb              = tb_hamiltonian(displ_vec, overlap_mat_vec);
  int norb             = 2;
  auto I               = nda::eye<dcomplex>(norb);

  std::pair<double, double> D = {0, 1};
  nda::clef::placeholder<0> kx_;
  nda::clef::placeholder<1> ky_;

  double om    = 0.2;
  double Gamma = 0.5;

  // adaptive
  auto int_1d_adapt = triqs::utility::integrate_1d_adapt<nda::matrix<dcomplex>>{1e-5};
  auto f_wk         = nda::linalg::inv(om * I - tb(kx_, ky_, 0.) + Gamma * I * 1i);
  auto result_adapt = integrate(int_1d_adapt, integrate(int_1d_adapt, f_wk, ky_ = D), kx_ = D);

  dcomplex answer = {-0.22338077801907302, -0.34352111773477229};
  EXPECT_COMPLEX_NEAR(result_adapt(0, 0), answer, 1.e-5);
}

TEST(tb_tests, h5_read_write) {

  mpi::communicator world;
  if (world.rank() != 0) GTEST_SKIP();

  auto displ_vec       = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag({t, t})));
  auto tb              = tb_hamiltonian(displ_vec, overlap_mat_vec);

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
    auto tb_in = h5::h5_read<tb_hamiltonian>(grp, "hamiltonian");
    EXPECT_EQ(tb, tb_in);
  }
}

TEST(tb_tests, mutate_through_accessors) { // NOLINT

  auto displ_vec       = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag({t, t})));
  auto tb              = tb_hamiltonian(displ_vec, overlap_mat_vec);

  // by-R indexing must yield a mutable view
  tb[{1, 0, 0}](0, 0) = 42.;
  EXPECT_EQ(tb.get_coeff_arr()(tb.get_R_idx({1, 0, 0}), 0, 0), dcomplex{42.});

  // by-long-index indexing must yield a mutable view
  auto ridx      = tb.get_R_idx({0, 1, 0});
  tb[ridx](1, 1) = 7.;
  EXPECT_EQ(tb.get_coeff_arr()(ridx, 1, 1), dcomplex{7.});

  // hoppings() range must yield mutable views
  for (auto h_R : tb.hoppings()) h_R *= 2.;
  EXPECT_EQ(tb.get_coeff_arr()(tb.get_R_idx({1, 0, 0}), 0, 0), dcomplex{84.});
  EXPECT_EQ(tb.get_coeff_arr()(ridx, 1, 1), dcomplex{14.});
}

TEST(tb_tests, mpi_broadcast) {

  mpi::communicator world;

  auto displ_vec       = std::vector<std::array<long, 3>>{{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::array<dcomplex, 2>(nda::diag({t, t})));
  auto tb_ref          = tb_hamiltonian(displ_vec, overlap_mat_vec);

  // Only rank 0 has the actual data, others have default-constructed object
  auto tb = world.rank() == 0 ? tb_ref : tb_hamiltonian{};

  mpi::broadcast(tb, world, 0);

  EXPECT_EQ(tb, tb_ref);
}

MPI_TEST_MAIN
