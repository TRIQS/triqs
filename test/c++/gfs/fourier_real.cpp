// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

double lorentzian(double w, double E) { return 2 * E / (w * w + E * E); };
std::complex<double> lorentzian_inverse(double t, double E) { return std::exp(-E * std::abs(t)); };
double theta(double x) { return x > 0 ? 1.0 : (x < 0 ? 0.0 : 0.5); };

// Generic Fourier test function for different ranks
template <int TARGET_RANK> void test_fourier() {
  double precision = 1e-4;
  double w_max = 40;
  int Nw       = 1001;
  double E     = 1;

  std::array<long, TARGET_RANK> shape{};

  if constexpr (TARGET_RANK == 2) // Matrix_valued
    shape = make_shape(2, 2);
  else if constexpr (TARGET_RANK == 3) // Tensor_valued<3>
    shape = make_shape(2, 2, 2);
  else if constexpr (TARGET_RANK == 4) // Tensor_valued<4>
    shape = make_shape(2, 2, 2, 2);

  using target_t = typename _target_from_type_rank<dcomplex, TARGET_RANK>::type;

  auto w_mesh = mesh::refreq{-w_max, w_max, Nw};
  auto t_mesh = make_adjoint_mesh(w_mesh);
  auto Gw1    = gf<refreq, target_t>{w_mesh, shape};

  // === Fourier of a simple Lorentzian ===

  for (auto w : w_mesh) Gw1[w] = lorentzian(w, E);

  auto [tail, err] = fit_tail(Gw1);
  auto Gt1         = make_gf_from_fourier(Gw1, t_mesh, tail);

  // verification that TF(TF^-1)=Id
  auto Gw1b = make_gf_from_fourier(Gt1, w_mesh, tail);
  EXPECT_GF_NEAR(Gw1, Gw1b, precision);

  // Comparision against exact result
  auto Gt1_exact = Gt1;
  for (auto t : t_mesh) Gt1_exact[t] = lorentzian_inverse(t, E);
  EXPECT_GF_NEAR(Gt1, Gt1_exact, precision);

  // === Fourier of a Product of theta function and exponential ===

  for (auto t : t_mesh) Gt1[t] = 0.5i * (lorentzian_inverse(-t, E) * theta(-t) - lorentzian_inverse(t, E) * theta(t));

  auto known_moments = make_zero_tail(Gt1);

  if constexpr (TARGET_RANK == 2) { // Matrix valued case needs special treatment
    known_moments(1, 0, 0) = 1.;
    known_moments(1, 1, 1) = 1.;
  } else
    known_moments(1, ellipsis()) = 1.;

  Gw1() = fourier(Gt1, known_moments);

  // verification that TF(TF^-1)=Id
  auto Gt1b = make_gf_from_fourier(Gw1, t_mesh, known_moments);
  EXPECT_GF_NEAR(Gt1, Gt1b, precision);

  // Comparision against exact result
  auto Gw1_exact = Gw1;
  for (auto w : w_mesh) Gw1_exact[w] = 0.5 / (w + E * 1i) + 0.5 / (w - E * 1i);
  EXPECT_GF_NEAR(Gw1, Gw1_exact, precision);
}

TEST(FourierReal, Scalar) { test_fourier<0>(); }
TEST(FourierReal, Matrix) { test_fourier<2>(); }
TEST(FourierReal, Tensor3) { test_fourier<3>(); }
TEST(FourierReal, Tensor4) { test_fourier<4>(); }

MAKE_MAIN;
