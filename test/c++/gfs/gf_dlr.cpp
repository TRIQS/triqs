// Copyright (c) 2023 Hugo U. R. Strand
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
// Authors: Hugo U. R. Strand

#include <triqs/test_tools/gfs.hpp>

TEST(Gf, dlr_imtime) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;
  auto mesh = gf_mesh<triqs::mesh::dlr_imtime>{beta, Fermion, lambda, eps};

  std::cout << "Rank " << mesh.size() << "\n";
  
  EXPECT_CLOSE(mesh.lambda(), lambda);
  EXPECT_CLOSE(mesh.eps(), eps);
  
}

/*
TEST(Gf, dlr_imtime_grid) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;
  double omega = 1.337;

  auto mesh = gf_mesh<dlr_imtime>{beta, Fermion, lambda, eps};
  auto G1 = gf<dlr_imtime, scalar_valued>{mesh};
  
  for (const auto &tau : mesh ) {
    G1(tau) = std::exp(-omega * tau) / (1 + std::exp(-beta * omega));
  }

  auto Gc = G1; // copy
  EXPECT_GF_NEAR(G1, Gc);

  auto Gv = G1(); // view
  EXPECT_GF_NEAR(Gv, Gc);
  
  // Take view and assign
  auto G2 = gf<dlr_imtime, scalar_valued>{mesh};
  auto G2v = gf_view(G2);
  for (const auto &tau : mesh ) {
    G2v(tau) = std::exp(-omega * tau) / (1 + std::exp(-beta * omega));
  }
  
  EXPECT_GF_NEAR(G2, G2v);
  EXPECT_GF_NEAR(G1, G2v);
  EXPECT_GF_NEAR(G1, G2);

  auto GpG = G1 + G2; // Addition
  for (const auto &tau : mesh ) {
    EXPECT_CLOSE(GpG(tau), G1(tau) * 2.);
    EXPECT_CLOSE(GpG(tau), 2. * G1(tau));
  }

  auto G4 = 4. * G2; // Multiply with scalar
  for (const auto &tau : mesh ) {
    EXPECT_CLOSE(G4(tau), G1(tau) * 4.);
    EXPECT_CLOSE(G4(tau), 4. * G1(tau));
  }

  auto GG = G1 * G2; // Multiplication
  for (const auto &tau : mesh ) {
    EXPECT_CLOSE(GG(tau), G1(tau) * G1(tau));
  }
  EXPECT_TRUE(GG.mesh().domain().statistic == Boson);
}

TEST(Gf, dlr_interpolation) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;

  auto G_tau = gf<dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};

  double omega = 1.337;
  triqs::clef::placeholder<0> tau_;
  G_tau(tau_) << std::exp(-omega * tau_) / (1 + std::exp(-beta * omega));

  int ntau = 10
  auto tmesh = mesh::imtime{beta, Fermion, ntau};

  // Interpolation on dlr_imtime grid should not be supported (by design)
  for (auto const &tau : tmesh) {
    EXPECT_THROW(G_tau(tau), triqs::runtime_error);
  }

  // Interpolation in imaginary time using dlr grid (efficient by design)
  auto G_dlr = make_gf_from_fourier(G_tau);
  for (auto const &tau : tmesh) {
    EXPECT_CLOSE(G_dlr(tau), std::exp(-omega * tau) / (1 + std::exp(-beta * omega));
  }

  rw_h5(G_tau, "g_dlr_imtime");
  rw_h5(G_dlr, "g_dlr");
}

TEST(Gf, dlr_density) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;

  auto G_tau = gf<dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};

  double omega = 1.337;
  triqs::clef::placeholder<0> tau_;
  G_tau(tau_) << std::exp(-omega * tau_) / (1 + std::exp(-beta * omega));

  // Density from dlr_imtime not supported (by design)
  EXPECT_THROW(triqs::gfs::density(G_tau), triqs::runtime_error);
  
  // Density from dlr is efficient (by design)
  // only requires interpolation at \tau=\beta ( n = -G(\beta) )
  auto G_dlr = make_gf_from_fourier(G_tau); 
  auto n = triqs::gfs::density(G_dlr);
  EXPECT_COMPLEX_NEAR(n, 1 / (1 + std::exp(-beta * omega)), 1.e-9);

  rw_h5(G, "DLRDensFerm");
}

TEST(Gf, dlr_tau_rev) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;

  auto G_tau = gf<dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};

  double omega = 1.337;
  triqs::clef::placeholder<0> tau_;
  G_tau(tau_) << std::exp(-omega * tau_) / (1 + std::exp(-beta * omega));

  G_tau_rev = G_tau;
  auto G_dlr = make_gf_from_fourier(G_tau);
  G_tau_rev(tau_) << G_dlr(beta - tau_);

  // Interpolation in imaginary time using dlr grid (efficient by design)
  for (auto const &tau : G_tau_rev.mesh()) {
    EXPECT_CLOSE(G_tau_rev(tau), std::exp(-omega * (beta - tau)) / (1 + std::exp(-beta * omega)));
    EXPECT_CLOSE(G_tau_rev(tau), G_dlr(beta - tau));
  }
}
*/

MAKE_MAIN;
