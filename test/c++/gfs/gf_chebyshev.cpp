// Copyright (c) 2025 Simons Foundation
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

#include <triqs/test_tools/gfs.hpp>

// Test function: smooth function for interpolation
double test_function(double tau, double beta, double omega) {
  // Fermi function shifted to interval [0, beta]
  return 1.0 / (1.0 + std::exp(omega * (tau - beta / 2.0)));
}

TEST(GfChebyshev, Mesh) {
  double const beta = 10.0;
  auto const N      = 20;
  auto mesh_c       = mesh::chebyshev(beta, Fermion, N);

  std::cout << mesh_c << "\n";
  std::cout << "Size " << mesh_c.size() << "\n";

  for (const auto &mp : mesh_c) { std::cout << "index: " << mp.index() << ", tau: " << mp.value() << "\n"; }

  EXPECT_EQ(mesh_c.size(), N);
  EXPECT_EQ(mesh_c.beta(), beta);
  EXPECT_EQ(mesh_c.statistic(), Fermion);
}

TEST(GfChebyshev, ScalarValued) {
  double const beta  = 10.0;
  double const omega = 1.0;
  auto const N       = 30;
  auto mesh_c        = mesh::chebyshev(beta, Fermion, N);

  auto g = gf<chebyshev, scalar_valued>{mesh_c};

  // Fill with test function values at Chebyshev points
  for (auto const &mp : mesh_c) { g[mp.data_index()] = test_function(mp.value(), beta, omega); }

  // Test interpolation at various tau values
  auto tau_test  = mesh::imtime(beta, Fermion, 50);
  double max_err = 0.0;
  for (auto t : tau_test) {
    double exact  = test_function(t.value(), beta, omega);
    auto interp   = g(t);
    double err    = std::abs(exact - std::real(interp));
    max_err       = std::max(max_err, err);
  }

  std::cout << "Max interpolation error: " << max_err << "\n";
  EXPECT_LT(max_err, 1e-7); // Chebyshev should have exponential convergence
}

TEST(GfChebyshev, MatrixValued) {
  double const beta = 5.0;
  auto const N      = 15;

  auto mesh_c = mesh::chebyshev(beta, Fermion, N);
  auto g      = gf<chebyshev, matrix_valued>{mesh_c, {2, 2}};
  g()         = 0.0;

  // Set a simple linear function: g(tau) = tau * I
  for (auto const &mp : mesh_c) { g[mp.data_index()] = nda::eye<double>(2) * mp.value(); }

  // Check interpolation at midpoint
  double tau_mid = beta / 2.0;
  auto g_mid     = g(tau_mid);
  EXPECT_NEAR(std::real(g_mid(0, 0)), tau_mid, 1e-10);
  EXPECT_NEAR(std::real(g_mid(1, 1)), tau_mid, 1e-10);
  EXPECT_NEAR(std::real(g_mid(0, 1)), 0.0, 1e-14);
  EXPECT_NEAR(std::real(g_mid(1, 0)), 0.0, 1e-14);
}

TEST(GfChebyshev, InterpolationAtMeshPoints) {
  double const beta = 10.0;
  auto const N      = 20;
  auto mesh_c       = mesh::chebyshev(beta, Fermion, N);

  auto g = gf<chebyshev, scalar_valued>{mesh_c};

  // Fill with arbitrary values
  for (auto const &mp : mesh_c) { g[mp.data_index()] = mp.value() * mp.value(); }

  // Interpolation at mesh points should be exact (to machine precision)
  for (auto const &mp : mesh_c) {
    double exact = mp.value() * mp.value();
    auto interp  = g(mp.value());
    EXPECT_NEAR(exact, std::real(interp), 1e-12);
  }
}

TEST(GfChebyshev, InterpolationConvergence) {
  double const beta  = 10.0;
  double const omega = 2.0;

  std::vector<double> errors;

  for (int N : {10, 15, 20, 25, 30}) {
    auto mesh_c = mesh::chebyshev(beta, Fermion, N);
    auto g      = gf<chebyshev, scalar_valued>{mesh_c};

    for (auto const &mp : mesh_c) { g[mp.data_index()] = test_function(mp.value(), beta, omega); }

    // Test at a few points
    double max_err = 0.0;
    for (int i = 1; i < 20; ++i) {
      double tau   = beta * i / 20.0;
      double exact = test_function(tau, beta, omega);
      auto interp  = g(tau);
      max_err      = std::max(max_err, std::abs(exact - std::real(interp)));
    }
    errors.push_back(max_err);
    std::cout << "N = " << N << ", max error = " << max_err << "\n";
  }

  // Error should decrease (exponential convergence)
  for (size_t i = 1; i < errors.size(); ++i) { EXPECT_LT(errors[i], errors[i - 1]); }
}

TEST(GfChebyshev, HDF5) {
  double const beta = 10.0;
  auto const N      = 20;

  auto mesh_c = mesh::chebyshev(beta, Fermion, N);
  auto g1     = gf<chebyshev, scalar_valued>{mesh_c};
  for (auto const &mp : mesh_c) { g1[mp.data_index()] = mp.value() * mp.value(); }

  // Write and read
  auto g2 = rw_h5(g1, "test_gf_chebyshev", "g");

  EXPECT_EQ(g1.mesh(), g2.mesh());
  EXPECT_ARRAY_NEAR(g1.data(), g2.data());
}

MAKE_MAIN;
