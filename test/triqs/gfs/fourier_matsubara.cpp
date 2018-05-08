#include <triqs/test_tools/gfs.hpp>

// TODO
// Boson Case
//

// Generic Fourier test function for different ranks
template <int TARGET_RANK> void test_fourier(statistic_enum statistic) {
  double precision = 10e-9;
  triqs::clef::placeholder<0> om_;
  double beta = 1;
  int Nw      = 10000;
  double E    = 1;

  mini_vector<size_t, TARGET_RANK> shape{};

  if constexpr (TARGET_RANK == 2) // Matrix_valued
    shape = make_shape(2, 2);
  else if constexpr (TARGET_RANK == 3) // Tensor_valued<3>
    shape = make_shape(2, 2, 2);
  else if constexpr (TARGET_RANK == 4) // Tensor_valued<4>
    shape = make_shape(2, 2, 2, 2);

  using target_t = typename _target_from_type_rank<dcomplex, TARGET_RANK>::type;

  auto Gw1 = gf<imfreq, target_t>{{beta, statistic, Nw}, shape};
  Gw1(om_) << 1 / (om_ - E);

  auto Gt1 = gf<imtime, target_t>{{beta, statistic, 2 * Nw + 1}, shape};
  Gt1()    = inverse_fourier(Gw1);

  ///verification that TF(TF^-1)=Id
  auto Gw1b = gf<imfreq, target_t>{{beta, statistic, Nw}, shape};
  Gw1b()    = fourier(Gt1);
  EXPECT_GF_NEAR(Gw1, Gw1b, precision);

  // Check against the exact result
  auto Gt1_check = Gt1;
  Gt1_check()    = 0.0;
  double s       = (statistic == Fermion ? 1 : -1);
  for (auto const &t : Gt1.mesh()) { Gt1_check[t] = exp(-E * t) * ((t >= 0 ? -1 : 0) + 1 / (1 + s * exp(E * beta))); }
  EXPECT_ARRAY_NEAR(Gt1.data(), Gt1_check.data(), precision); //not comparing GF because of tails

  // Test the factory function
  auto Gt2  = make_gf_from_inverse_fourier(Gw1);
  auto Gw2b = make_gf_from_fourier(Gt2);
  EXPECT_GF_NEAR(Gt2, Gt1, precision);
  EXPECT_GF_NEAR(Gw2b, Gw1, precision);

  ///to verify that fourier computes (??)
  auto Gw2 = gf<imfreq, target_t>{{beta, statistic}, shape};
  Gw2()    = fourier(Gt1);
}

TEST(FourierMatsubara, FermionScalar) { test_fourier<0>(Fermion); }
TEST(FourierMatsubara, FermionMatrix) { test_fourier<2>(Fermion); }
TEST(FourierMatsubara, FermionTensor3) { test_fourier<3>(Fermion); }
TEST(FourierMatsubara, FermionTensor4) { test_fourier<4>(Fermion); }

TEST(FourierMatsubara, BosonScalar) { test_fourier<0>(Boson); }
TEST(FourierMatsubara, BosonMatrix) { test_fourier<2>(Boson); }
TEST(FourierMatsubara, BosonTensor3) { test_fourier<3>(Boson); }
TEST(FourierMatsubara, BosonTensor4) { test_fourier<4>(Boson); }

///check Fourier on positive-only freqs fails
TEST(Gfs, FourierMatsubaraAllFreq) {
  double precision = 10e-9;
  triqs::clef::placeholder<0> om_;
  double beta = 1;
  int Nw      = 10000;
  double E    = 1;
  auto Gw1    = gf<imfreq>{{beta, Fermion, Nw, matsubara_mesh_opt::positive_frequencies_only}, {2, 2}};
  Gw1(om_) << 1 / (om_ - E);
  auto Gt1 = gf<imtime>{{beta, Fermion, 2 * Nw + 1}, {2, 2}};
  ASSERT_THROW(Gt1() = inverse_fourier(Gw1), triqs::runtime_error);
}

MAKE_MAIN;
