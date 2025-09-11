// Copyright (c) 2022-2023 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Nils Wentzell

#include <triqs/mesh/matsubara_freq.hpp>
#include <triqs/mesh/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <complex>
#include <iostream>
#include <numbers>

// Test fixture for matsubara_freq tests.
struct TRIQSMesh : ::testing::Test {
  protected:
  triqs::mesh::matsubara_freq om_5{5, std::numbers::pi, triqs::mesh::Boson};
  triqs::mesh::matsubara_freq om_3{3, std::numbers::pi, triqs::mesh::Boson};
  triqs::mesh::matsubara_freq om_m5{-5, std::numbers::pi, triqs::mesh::Boson};
  triqs::mesh::matsubara_freq om_m3{-3, std::numbers::pi, triqs::mesh::Boson};
  triqs::mesh::matsubara_freq nu_5{5, std::numbers::pi, triqs::mesh::Fermion};
  triqs::mesh::matsubara_freq nu_3{3, std::numbers::pi, triqs::mesh::Fermion};
  triqs::mesh::matsubara_freq nu_m5{-5, std::numbers::pi, triqs::mesh::Fermion};
  triqs::mesh::matsubara_freq nu_m3{-3, std::numbers::pi, triqs::mesh::Fermion};
};

// Check two Matsubara frequencies for equality.
void check(const triqs::mesh::matsubara_freq &om1, const triqs::mesh::matsubara_freq &om2) {
  EXPECT_EQ(om1.n, om2.n);
  EXPECT_EQ(om1.beta, om2.beta);
  EXPECT_EQ(om1.statistic, om2.statistic);
}

TEST_F(TRIQSMesh, MatsubaraFreqDefaultConstructor) {
  // default constructed Matsubara frequency
  auto om = triqs::mesh::matsubara_freq{};
  EXPECT_EQ(om.n, 0);
  EXPECT_EQ(om.beta, 0.0);
  EXPECT_EQ(om.statistic, triqs::mesh::Fermion);
}

TEST_F(TRIQSMesh, MatsubaraFreqWithGivenParameters) {
  using namespace std::complex_literals;
  using std::numbers::pi;

  // construct a bosonic Matsubara frequency with index 1 and beta pi
  auto om = triqs::mesh::matsubara_freq{1, pi, triqs::mesh::Boson};
  EXPECT_EQ(om.n, 1);
  EXPECT_EQ(om.beta, pi);
  EXPECT_EQ(om.statistic, triqs::mesh::Boson);
  EXPECT_COMPLEX_NEAR(std::complex<double>{om}, 2i, 1e-12);

  // construct a bosonic Matsubara frequency with index -1 and beta pi
  auto nu = triqs::mesh::matsubara_freq{-1, pi, triqs::mesh::Fermion};
  EXPECT_EQ(nu.n, -1);
  EXPECT_EQ(nu.beta, pi);
  EXPECT_EQ(nu.statistic, triqs::mesh::Fermion);
  EXPECT_COMPLEX_NEAR(std::complex<double>{nu}, -1i, 1e-12);
}

TEST_F(TRIQSMesh, MatsubaraFreqConversionToComplex) {
  // bosonic Matsubara frequencies
  EXPECT_COMPLEX_NEAR(std::complex<double>{om_5}, 10i, 1e-12);
  EXPECT_COMPLEX_NEAR(std::complex<double>{om_3}, 6i, 1e-12);
  EXPECT_COMPLEX_NEAR(std::complex<double>{om_m5}, -10i, 1e-12);
  EXPECT_COMPLEX_NEAR(std::complex<double>{om_m3}, -6i, 1e-12);

  // fermionic Matsubara frequencies
  EXPECT_COMPLEX_NEAR(std::complex<double>{nu_5}, 11i, 1e-12);
  EXPECT_COMPLEX_NEAR(std::complex<double>{nu_3}, 7i, 1e-12);
  EXPECT_COMPLEX_NEAR(std::complex<double>{nu_m5}, -9i, 1e-12);
  EXPECT_COMPLEX_NEAR(std::complex<double>{nu_m3}, -5i, 1e-12);
}

TEST_F(TRIQSMesh, MatsubaraFreqUnaryMinus) {
  using std::numbers::pi;
  using namespace triqs::mesh;

  // -boson
  check(-om_5, matsubara_freq{-5, pi, Boson});
  check(-om_3, matsubara_freq{-3, pi, Boson});
  check(-om_m5, matsubara_freq{5, pi, Boson});
  check(-om_m3, matsubara_freq{3, pi, Boson});

  // -fermion
  check(-nu_5, matsubara_freq{-6, pi, Fermion});
  check(-nu_3, matsubara_freq{-4, pi, Fermion});
  check(-nu_m5, matsubara_freq{4, pi, Fermion});
  check(-nu_m3, matsubara_freq{2, pi, Fermion});
}

TEST_F(TRIQSMesh, MatsubaraFreqAddition) {
  using namespace std::complex_literals;
  using std::numbers::pi;
  using namespace triqs::mesh;

  // boson + boson
  check(om_5 + om_3, matsubara_freq{8, pi, Boson});
  check(om_3 + om_5, matsubara_freq{8, pi, Boson});
  check(om_5 + om_m5, matsubara_freq{0, pi, Boson});
  check(om_m5 + om_5, matsubara_freq{0, pi, Boson});
  check(om_5 + om_m3, matsubara_freq{2, pi, Boson});
  check(om_m5 + om_3, matsubara_freq{-2, pi, Boson});

  // fermion + fermion
  check(nu_5 + nu_3, matsubara_freq{9, pi, Boson});
  check(nu_3 + nu_5, matsubara_freq{9, pi, Boson});
  check(nu_5 + nu_m5, matsubara_freq{1, pi, Boson});
  check(nu_m5 + nu_5, matsubara_freq{1, pi, Boson});
  check(nu_5 + nu_m3, matsubara_freq{3, pi, Boson});
  check(nu_m5 + nu_3, matsubara_freq{-1, pi, Boson});

  // boson + fermion
  check(om_5 + nu_3, matsubara_freq{8, pi, Fermion});
  check(om_m3 + nu_m5, matsubara_freq{-8, pi, Fermion});
  check(om_m5 + nu_3, matsubara_freq{-2, pi, Fermion});
  check(om_3 + nu_m3, matsubara_freq{0, pi, Fermion});

  // fermion + boson
  check(nu_5 + om_3, matsubara_freq{8, pi, Fermion});
  check(nu_m5 + om_m3, matsubara_freq{-8, pi, Fermion});
  check(nu_3 + om_m5, matsubara_freq{-2, pi, Fermion});
  check(nu_m3 + om_3, matsubara_freq{0, pi, Fermion});

  // boson + scalar
  EXPECT_COMPLEX_NEAR(om_5 + 3i, 13i, 1e-12);
  EXPECT_COMPLEX_NEAR(om_m5 + 3i, -7i, 1e-12);
  EXPECT_COMPLEX_NEAR(om_m5 + 11i, 1i, 1e-12);

  // fermion + scalar
  EXPECT_COMPLEX_NEAR(nu_5 + 3i, 14i, 1e-12);
  EXPECT_COMPLEX_NEAR(nu_m5 + 3i, -6i, 1e-12);
  EXPECT_COMPLEX_NEAR(nu_m5 + 11i, 2i, 1e-12);

  // scalar + boson
  EXPECT_COMPLEX_NEAR(3i + om_5, 13i, 1e-12);
  EXPECT_COMPLEX_NEAR(3i + om_m5, -7i, 1e-12);
  EXPECT_COMPLEX_NEAR(11i + om_m5, 1i, 1e-12);

  // scalar + fermion
  EXPECT_COMPLEX_NEAR(3i + nu_5, 14i, 1e-12);
  EXPECT_COMPLEX_NEAR(3i + nu_m5, -6i, 1e-12);
  EXPECT_COMPLEX_NEAR(11i + nu_m5, 2i, 1e-12);
}

TEST_F(TRIQSMesh, MatsubaraFreqSubtraction) {
  using std::numbers::pi;
  using namespace triqs::mesh;

  // boson - boson
  check(om_5 - om_3, matsubara_freq{2, pi, Boson});
  check(om_3 - om_5, matsubara_freq{-2, pi, Boson});
  check(om_5 - om_m5, matsubara_freq{10, pi, Boson});
  check(om_m5 - om_5, matsubara_freq{-10, pi, Boson});
  check(om_5 - om_m3, matsubara_freq{8, pi, Boson});
  check(om_m5 - om_3, matsubara_freq{-8, pi, Boson});

  // fermion - fermion
  check(nu_5 - nu_3, matsubara_freq{2, pi, Boson});
  check(nu_3 - nu_5, matsubara_freq{-2, pi, Boson});
  check(nu_5 - nu_m5, matsubara_freq{10, pi, Boson});
  check(nu_m5 - nu_5, matsubara_freq{-10, pi, Boson});
  check(nu_5 - nu_m3, matsubara_freq{8, pi, Boson});
  check(nu_m5 - nu_3, matsubara_freq{-8, pi, Boson});

  // boson - fermion
  check(om_5 - nu_3, matsubara_freq{1, pi, Fermion});
  check(om_m3 - nu_m5, matsubara_freq{1, pi, Fermion});
  check(om_m5 - nu_3, matsubara_freq{-9, pi, Fermion});
  check(om_3 - nu_m3, matsubara_freq{5, pi, Fermion});

  // fermion - boson
  check(nu_5 - om_3, matsubara_freq{2, pi, Fermion});
  check(nu_m5 - om_m3, matsubara_freq{-2, pi, Fermion});
  check(nu_3 - om_m5, matsubara_freq{8, pi, Fermion});
  check(nu_m3 - om_3, matsubara_freq{-6, pi, Fermion});

  // boson - scalar
  EXPECT_COMPLEX_NEAR(om_5 - 3i, 7i, 1e-12);
  EXPECT_COMPLEX_NEAR(om_m5 - 3i, -13i, 1e-12);
  EXPECT_COMPLEX_NEAR(om_5 - 11i, -1i, 1e-12);

  // fermion - scalar
  EXPECT_COMPLEX_NEAR(nu_5 - 3i, 8i, 1e-12);
  EXPECT_COMPLEX_NEAR(nu_m5 - 3i, -12i, 1e-12);
  EXPECT_COMPLEX_NEAR(nu_5 - 12i, -1i, 1e-12);

  // scalar - boson
  EXPECT_COMPLEX_NEAR(3i - om_5, -7i, 1e-12);
  EXPECT_COMPLEX_NEAR(3i - om_m5, 13i, 1e-12);
  EXPECT_COMPLEX_NEAR(11i - om_5, 1i, 1e-12);

  // scalar - fermion
  EXPECT_COMPLEX_NEAR(3i - nu_5, -8i, 1e-12);
  EXPECT_COMPLEX_NEAR(3i - nu_m5, 12i, 1e-12);
  EXPECT_COMPLEX_NEAR(10i - nu_5, -1i, 1e-12);
}

TEST_F(TRIQSMesh, MatsubaraFreqMultiplication) {
  // boson * boson
  EXPECT_COMPLEX_NEAR(om_5 * om_3, 10i * 6i, 1e-12);
  EXPECT_COMPLEX_NEAR(om_3 * om_5, 6i * 10i, 1e-12);

  // fermion * fermion
  EXPECT_COMPLEX_NEAR(nu_5 * nu_3, 11i * 7i, 1e-12);
  EXPECT_COMPLEX_NEAR(nu_3 * nu_5, 11i * 7i, 1e-12);

  // boson * fermion
  EXPECT_COMPLEX_NEAR(om_5 * nu_3, 10i * 7i, 1e-12);

  // fermion * boson
  EXPECT_COMPLEX_NEAR(nu_3 * om_5, 7i * 10i, 1e-12);

  // boson * scalar
  EXPECT_COMPLEX_NEAR(om_5 * 3i, 10i * 3i, 1e-12);

  // scalar * boson
  EXPECT_COMPLEX_NEAR(3i * om_5, 3i * 10i, 1e-12);

  // fermion * scalar
  EXPECT_COMPLEX_NEAR(nu_5 * 3i, 11i * 3i, 1e-12);

  // scalar * fermion
  EXPECT_COMPLEX_NEAR(3i * nu_5, 3i * 11i, 1e-12);
}

TEST_F(TRIQSMesh, MatsubaraFreqDivision) {
  // boson / boson
  EXPECT_COMPLEX_NEAR(om_5 / om_3, 10i / 6i, 1e-12);
  EXPECT_COMPLEX_NEAR(om_3 / om_5, 6i / 10i, 1e-12);

  // fermion / fermion
  EXPECT_COMPLEX_NEAR(nu_5 / nu_3, 11i / 7i, 1e-12);
  EXPECT_COMPLEX_NEAR(nu_3 / nu_5, 7i / 11i, 1e-12);

  // boson / fermion
  EXPECT_COMPLEX_NEAR(om_5 / nu_3, 10i / 7i, 1e-12);

  // fermion / boson
  EXPECT_COMPLEX_NEAR(nu_3 / om_5, 7i / 10i, 1e-12);

  // boson / scalar
  EXPECT_COMPLEX_NEAR(om_5 / 3i, 10i / 3i, 1e-12);

  // scalar / boson
  EXPECT_COMPLEX_NEAR(3i / om_5, 3i / 10i, 1e-12);

  // boson / fermion
  EXPECT_COMPLEX_NEAR(om_5 / nu_3, 10i / 7i, 1e-12);

  // fermion / boson
  EXPECT_COMPLEX_NEAR(nu_3 / om_5, 7i / 10i, 1e-12);
}

TEST_F(TRIQSMesh, MatsubaraFreqKronecker) {
  // unary kronecker
  EXPECT_FALSE(triqs::mesh::kronecker(om_5));
  EXPECT_FALSE(triqs::mesh::kronecker(om_m5));
  EXPECT_FALSE(triqs::mesh::kronecker(nu_5));
  EXPECT_FALSE(triqs::mesh::kronecker(nu_m5));
  EXPECT_TRUE(triqs::mesh::kronecker(triqs::mesh::matsubara_freq{0, 10, triqs::mesh::Boson}));
  EXPECT_TRUE(triqs::mesh::kronecker(triqs::mesh::matsubara_freq{0, 10, triqs::mesh::Fermion}));

  // binary kronecker
  EXPECT_FALSE(triqs::mesh::kronecker(om_5, om_3));
  EXPECT_FALSE(triqs::mesh::kronecker(om_5, om_m5));
  EXPECT_FALSE(triqs::mesh::kronecker(om_5, nu_3));
  EXPECT_FALSE(triqs::mesh::kronecker(om_5, nu_m5));
  EXPECT_TRUE(triqs::mesh::kronecker(om_5, om_5));
  EXPECT_TRUE(triqs::mesh::kronecker(om_5, nu_5));
}

MAKE_MAIN;
