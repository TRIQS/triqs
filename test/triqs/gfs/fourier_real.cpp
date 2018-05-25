#include <triqs/test_tools/gfs.hpp>

double lorentzian(double w, double E) { return 2 * E / (w * w + E * E); };
std::complex<double> lorentzian_inverse(double t, double E) { return std::exp(-E * std::abs(t)); };
double theta(double x) { return x > 0 ? 1.0 : (x < 0 ? 0.0 : 0.5); };

// Generic Fourier test function for different ranks
template <int TARGET_RANK> void test_fourier() {
  double precision = 1e-4;
  triqs::clef::placeholder<0> w_;
  triqs::clef::placeholder<1> t_;
  double w_max = 40;
  int Nw       = 1001;
  double E     = 1;

  mini_vector<size_t, TARGET_RANK> shape{};

  if constexpr (TARGET_RANK == 2) // Matrix_valued
    shape = make_shape(2, 2);
  else if constexpr (TARGET_RANK == 3) // Tensor_valued<3>
    shape = make_shape(2, 2, 2);
  else if constexpr (TARGET_RANK == 4) // Tensor_valued<4>
    shape = make_shape(2, 2, 2, 2);

  using target_t = typename _target_from_type_rank<dcomplex, TARGET_RANK>::type;

  auto w_mesh = gf_mesh<refreq>{-w_max, w_max, Nw};
  auto Gw1    = gf<refreq, target_t>{w_mesh, shape};

  // === Fourier of a simple Lorentzian ===

  for (auto const &w : w_mesh) Gw1[w] = lorentzian(w, E);

  auto Gt1           = make_gf_from_fourier(Gw1);
  auto const &t_mesh = Gt1.mesh();

  // verification that TF(TF^-1)=Id
  auto [tail, err] = fit_tail(Gw1);
  auto Gw1b        = make_gf_from_fourier(Gt1, w_mesh, make_const_view(tail(range(0, 3), ellipsis()))); // FIXME const_view
  EXPECT_GF_NEAR(Gw1, Gw1b, precision);

  // Comparision against exact result
  auto Gt1_exact = Gt1;
  for (auto const &t : t_mesh) Gt1_exact[t] = lorentzian_inverse(t, E);
  EXPECT_GF_NEAR(Gt1, Gt1_exact, precision);

  // === Fourier of a Product of theta function and exponential ===

  for (auto const &t : t_mesh) Gt1[t] = 0.5_j * (lorentzian_inverse(-t, E) * theta(-t) - lorentzian_inverse(t, E) * theta(t));

  auto known_moments = make_zero_tail(Gt1);

  if constexpr (TARGET_RANK == 2) { // Matrix valued case needs special treatment
    known_moments(1, 0, 0) = 1.;
    known_moments(1, 1, 1) = 1.;
  } else
    known_moments(1, ellipsis()) = 1.;

  Gw1() = fourier(Gt1, make_const_view(known_moments));

  // verification that TF(TF^-1)=Id
  auto Gt1b = make_gf_from_fourier(Gw1, t_mesh, make_const_view(known_moments)); // FIXME const view
  EXPECT_GF_NEAR(Gt1, Gt1b, precision);

  // Comparision against exact result
  auto Gw1_exact = Gw1;
  for (auto const &w : w_mesh) Gw1_exact[w] = 0.5 / (w + E * 1_j) + 0.5 / (w - E * 1_j);
  EXPECT_GF_NEAR(Gw1, Gw1_exact, precision);
} 

TEST(FourierReal, Scalar) { test_fourier<0>(); }
TEST(FourierReal, Matrix) { test_fourier<2>(); }
TEST(FourierReal, Tensor3) { test_fourier<3>(); }
TEST(FourierReal, Tensor4) { test_fourier<4>(); }

MAKE_MAIN;
