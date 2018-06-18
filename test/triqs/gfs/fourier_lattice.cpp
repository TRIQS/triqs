#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>

// Generic Fourier test function for different ranks
template <int TARGET_RANK> void test_fourier() {
  double precision = 10e-9;
  triqs::clef::placeholder<0> r_;
  double beta = 1;
  int N_k     = 2;

  mini_vector<size_t, TARGET_RANK> shape{};

  if constexpr (TARGET_RANK == 2) // Matrix_valued
    shape = make_shape(2, 2);
  else if constexpr (TARGET_RANK == 3) // Tensor_valued<3>
    shape = make_shape(2, 2, 2);
  else if constexpr (TARGET_RANK == 4) // Tensor_valued<4>
    shape = make_shape(2, 2, 2, 2);

  using target_t = typename _target_from_type_rank<dcomplex, TARGET_RANK>::type;

  auto bl = bravais_lattice{make_unit_matrix<double>(2)};
  auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

  auto Gr = gf<cyclic_lattice, target_t>{{bl, N_k}, shape};
  Gr(r_) << exp(-r_(0));

  auto Gk1 = gf<brillouin_zone, target_t>{{bz, N_k}, shape};
  Gk1()    = fourier(Gr);

  ///verification that TF(TF^-1)=Id
  auto Grb = gf<cyclic_lattice, target_t>{{bl, N_k}, shape};
  Grb()    = fourier(Gk1);
  EXPECT_GF_NEAR(Gr, Grb, precision);

  // Test the factory function
  auto Gk2  = make_gf_from_fourier(Gr);
  auto Gr2b = make_gf_from_fourier(Gk2);
  EXPECT_GF_NEAR(Gk1, Gk2, precision);
  EXPECT_GF_NEAR(Grb, Gr2b, precision);
}

TEST(FourierLattice, Scalar) { test_fourier<0>(); }
TEST(FourierLattice, Matrix) { test_fourier<2>(); }
TEST(FourierLattice, Tensor3) { test_fourier<3>(); }
TEST(FourierLattice, Tensor4) { test_fourier<4>(); }

MAKE_MAIN;
