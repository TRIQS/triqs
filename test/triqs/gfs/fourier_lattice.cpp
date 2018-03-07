#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>

TEST(Gfs, FourierLattice) {
  double precision = 10e-9;
  triqs::clef::placeholder<0> r_;
  double beta = 1;
  int N       = 2;

  auto bl = bravais_lattice{make_unit_matrix<double>(2)};
  auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

  auto Gr = gf<cyclic_lattice, matrix_valued>{{N, N}, {2, 2}};
  Gr(r_) << exp(-r_(0));

  auto Gk1 = gf<brillouin_zone, matrix_valued>{{bz, N}, {2, 2}};
  Gk1()    = fourier(Gr);

  ///verification that TF(TF^-1)=Id
  auto Grb = gf<cyclic_lattice, matrix_valued>{{N, N}, {2, 2}};
  Grb()    = inverse_fourier(Gk1);
  EXPECT_GF_NEAR(Gr, Grb, precision);

  // Test the factory function
  auto Gk2  = make_gf_from_fourier(Gr);
  auto Gr2b = make_gf_from_inverse_fourier(Gk2);
  EXPECT_GF_NEAR(Gk1, Gk2, precision);
  EXPECT_GF_NEAR(Grb, Gr2b, precision);

  // Test EXCEPTION for non-diagonal periodization matrix
  auto per_mat = matrix<int>{{{2, 1, 0}, {-1, 2, 0}, {0, 0, 1}}};
  Gr           = gf<cyclic_lattice, matrix_valued>{{bl, per_mat}, {2, 2}};
  Gr(r_) << exp(-r_(0));
  Gk1 = gf<brillouin_zone, matrix_valued>{{bz, per_mat}, {2, 2}};
  EXPECT_THROW(Gk1() = fourier(Gr), triqs::runtime_error);
}
MAKE_MAIN;
