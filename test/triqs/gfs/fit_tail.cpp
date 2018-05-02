#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs/singularity/fit_tail.hpp>

using namespace triqs::arrays;

TEST(Gf, FitTailBasic) { // NOLINT

  triqs::clef::placeholder<0> iom_;
  double beta = 10;
  int N       = 100;

  auto iw_mesh = gf_mesh<imfreq>{beta, Fermion, N};

  // Set the fraction of mesh points to use for the tail fit
  double tail_fraction = 0.3;
  iw_mesh.set_tail_parameters(tail_fraction);

  auto gw   = gf<imfreq>{iw_mesh, {1, 1}};
  auto gw_s = gf<imfreq, scalar_valued>{iw_mesh, {}};

  // Initialize the Green functions
  array<dcomplex, 1> c{0, 1, 3, 5};
  gw(iom_) << c(0) + c(1) / iom_ + c(2) / iom_ / iom_ + c(3) / iom_ / iom_ / iom_;
  gw_s(iom_) << c(0) + c(1) / iom_ + c(2) / iom_ / iom_ + c(3) / iom_ / iom_ / iom_;

  // ==== Fix only the 0th moment to 0
  {
    auto known_moments   = array<dcomplex, 3>{{{0.0}}};
    auto known_moments_s = array<dcomplex, 1>{0.0};

    // Get the tail using least square fitting
    auto [tail, err]     = get_tail(gw, known_moments);
    auto [tail_s, err_s] = get_tail(gw_s, known_moments_s);

    EXPECT_ARRAY_NEAR(c, tail(range(0, 4), 0, 0), 1e-9);
    EXPECT_ARRAY_NEAR(c, tail_s(range(0, 4)), 1e-9);
  }

  // ==== Now fix both the 0th and 1st moment
  {
    auto known_moments   = array<dcomplex, 3>{{{0.0}}, {{1.0}}};
    auto known_moments_s = array<dcomplex, 1>{0.0, 1.0};

    // Get the tail using least square fitting
    auto [tail, err]     = get_tail(gw, known_moments);
    auto [tail_s, err_s] = get_tail(gw_s, known_moments_s);

    EXPECT_ARRAY_NEAR(c, tail(range(4), 0, 0), 1e-9);
    EXPECT_ARRAY_NEAR(c, tail_s(range(4)), 1e-9);
  }
}

// ------------------------------------------------------------------------------

TEST(Gf, FitTailReal_F_and_B) { // NOLINT

  triqs::clef::placeholder<0> iom_;
  double beta = 10;
  int N       = 100;

  auto gw_f = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
  auto gw_b = gf<imfreq>{{beta, Boson, N - 1}, {1, 1}};

  // Set the fraction of mesh points to use for the tail fit
  double tail_fraction = 0.3;
  gw_f.mesh().set_tail_parameters(tail_fraction);
  gw_b.mesh().set_tail_parameters(tail_fraction);

  // Initialize the Green functions
  gw_f(iom_) << 1 / (iom_ - 1);
  gw_b(iom_) << 1 / (iom_ - 1);

  // Fix both the 0th and 1st moment
  auto known_moments = array<dcomplex, 3>{{{0.0}}, {{1.0}}};

  auto [tail_f, err_r] = get_tail(gw_f, known_moments);
  auto [tail_b, err_b] = get_tail(gw_b, known_moments);

  auto tail_exact = array<dcomplex, 1>{0.0, 1.0, 1.0, 1.0, 1.0};

  EXPECT_ARRAY_NEAR(tail_exact, tail_f(range(5), 0, 0), 1e-9);
  EXPECT_ARRAY_NEAR(tail_exact, tail_b(range(5), 0, 0), 1e-9);
}

// ------------------------------------------------------------------------------

TEST(Gf, FitTailComplex) { // NOLINT

  triqs::clef::placeholder<0> iom_;
  double beta = 10;
  int N       = 200;

  auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};

  // Initialize the Green functions
  auto a = dcomplex(1.0, 0.4);
  gw(iom_) << 1 / (iom_ - a);

  // Fix both the 0th and 1st moment
  auto known_moments = array<dcomplex, 3>{{{0.0}}, {{1.0}}};

  auto [tail, err] = get_tail(gw, known_moments);

  auto tail_exact = array<dcomplex, 1>{dcomplex(0, 0), dcomplex(1,0), a, std::pow(a, 2), std::pow(a, 3)};

  EXPECT_ARRAY_NEAR(tail_exact, tail(range(5), 0, 0), 1e-8);
}

// ------------------------------------------------------------------------------

namespace triqs::gfs {
  template <typename M1, typename M2, typename... Ms>
  gf_mesh(M1, M2, Ms...)->gf_mesh<cartesian_product<typename M1::var_t, typename M2::var_t, typename Ms::var_t...>>;
}

TEST(Gf, FitTailMultivar) { // NOLINT

  triqs::clef::placeholder<0> iom_;
  triqs::clef::placeholder<1> k_;

  double beta = 10;
  int N       = 100;
  int N_k     = 4;

  auto BL        = bravais_lattice(matrix<double>{{1, 0}, {0, 1}});
  auto k_mesh    = gf_mesh<brillouin_zone>(BL, N_k);
  auto iw_mesh   = gf_mesh<imfreq>{beta, Fermion, N};
  auto prod_mesh = gf_mesh{k_mesh, iw_mesh};

  auto gw = gf{prod_mesh, {1, 1}};

  // Initialize the Multivariable Green functions
  gw(k_, iom_) << 1 / (iom_ - cos(k_[0]) * cos(k_[1]));

  // Fix both the 0th and 1st moment
  auto known_moments = array<dcomplex, 4>(2, N_k * N_k, 1, 1);
  known_moments(0, range(), 0, 0) = 0.0;
  known_moments(1, range(), 0, 0) = 1.0;

  // Fit for all k-points. Resulting shape is (N_orders, N_k * N_k, 1, 1)
  auto [tail, err] = get_tail<1>(gw, known_moments);

  // Calculate the exact tail
  auto tail_exact = array<dcomplex, 2>(5, N_k * N_k);
  for(auto & k : k_mesh){
    double eps_k = cos(k[0]) * cos(k[1]);
    tail_exact(range(), k.linear_index()) = array<dcomplex, 1>{0.0, 1.0, eps_k, std::pow(eps_k, 2), std::pow(eps_k, 3)};
  }

  EXPECT_ARRAY_NEAR(tail_exact, tail(range(5), range(), 0, 0), 1e-8);
}

MAKE_MAIN;
