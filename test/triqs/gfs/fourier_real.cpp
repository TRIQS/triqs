#include <triqs/test_tools/gfs.hpp>

// Generic Fourier test function for different ranks
template <int TARGET_RANK> void test_fourier() {
  double precision = 1e-8;
  triqs::clef::placeholder<0> w_;
  double omega_max = 100;
  int L            = 1001;
  double t_max     = 10;
  int L_t          = 1001;
  double E         = -1;

  mini_vector<size_t, TARGET_RANK> shape{};

  if constexpr (TARGET_RANK == 2) // Matrix_valued
    shape = make_shape(2, 2);
  else if constexpr (TARGET_RANK == 3) // Tensor_valued<3>
    shape = make_shape(2, 2, 2);
  else if constexpr (TARGET_RANK == 4) // Tensor_valued<4>
    shape = make_shape(2, 2, 2, 2);

  using target_t = typename _target_from_type_rank<dcomplex, TARGET_RANK>::type;

  // === Test a Green function with a Single Pole ===


  auto w_mesh = gf_mesh<refreq>{-omega_max, omega_max, L};
  auto Gw1 = gf<refreq, target_t>{w_mesh, shape};
  Gw1(w_) << 1 / (w_ * w_ + E * E) + 1 / (w_ * w_ + 4 * E * E) - 4.5 / (w_ * w_ + 2 * E * E);

  auto Gt1 = make_gf_from_fourier(Gw1); 

  //verification that TF(TF^-1)=Id
  auto [tail, err] = fit_tail(Gw1);
  auto Gw1b = make_gf_from_fourier(Gt1, w_mesh, make_const_view(tail)); // FIXME const_view
  EXPECT_GF_NEAR(Gw1, Gw1b, precision);

  // Check against the exact result
  //auto Gt1_exact = Gt1;
  //Gt1_exact()    = 0.0;
  //double s       = (statistic == Fermion ? -1 : 1);
  //auto one_pole  = [&](double E, auto &&t) {
  //if(E>0)
  //return - exp(-E * t) / (1 - s * exp(-E * beta));
  //else
  //return s* exp(E * (beta - t)) / (1 - s * exp(E * beta));
  //};
  //for (auto const &t : Gt1.mesh()) { Gt1_exact[t] = one_pole(E, t) + one_pole(-2*E, t) - 4.5*one_pole(1.25*E,t); }
  //EXPECT_GF_NEAR(Gt1, Gt1_exact, precision);

  // Test the factory function
  //auto Gt2  = make_gf_from_fourier(Gw1, N_tau);
  //auto Gw2b = make_gf_from_fourier(Gt2, N_iw);
  //EXPECT_GF_NEAR(Gt2, Gt1, precision);
  //EXPECT_GF_NEAR(Gw2b, Gw1, precision);

  //// === Test Green function with a self-energy ===

  //auto Sigma = Gw1;
  //Sigma(w_) << 1 / (w_ - 2) + 3 / (w_ + 3);
  //Gw1(w_) << 1 / (w_ - E - Sigma[w_]);

  //Gt1()  = fourier(Gw1);
  //Gw1b() = fourier(Gt1);
  //EXPECT_GF_NEAR(Gw1, Gw1b, precision);

  //// Now lets do multiple fourier transforms
  //for(int i : range(10)){
  //Gt1()  = fourier(Gw1b);
  //Gw1b() = fourier(Gt1);
  //}
  //EXPECT_GF_NEAR(Gw1, Gw1b, precision);
}

TEST(FourierReal, Scalar) { test_fourier<0>(); }
TEST(FourierReal, Matrix) { test_fourier<2>(); }
TEST(FourierReal, Tensor3) { test_fourier<3>(); }
TEST(FourierReal, Tensor4) { test_fourier<4>(); }

MAKE_MAIN;
