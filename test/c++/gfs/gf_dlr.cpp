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
#include <h5/serialization.hpp>

TEST(Gf, dlr_imtime) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;
  auto mesh = gf_mesh<triqs::mesh::dlr_imtime>{beta, Fermion, lambda, eps};

  std::cout << mesh << "\n";
  std::cout << "Rank " << mesh.size() << "\n";
  
  EXPECT_EQ(mesh.lambda(), lambda);
  EXPECT_EQ(mesh.eps(), eps);
  
  for (const auto &tau : mesh ) {
    std::cout << tau << "\n";
    EXPECT_TRUE(tau <= beta);
    EXPECT_TRUE(tau >= 0.0);
  }
}

TEST(Gf, dlr_coeffs) {

  // dlr_coeffs -> dlr_refreq
  
  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;
  auto mesh = gf_mesh<triqs::mesh::dlr_coeffs>{beta, Fermion, lambda, eps};

  std::cout << mesh << "\n";
  std::cout << "Rank " << mesh.size() << "\n";
  
  EXPECT_CLOSE(mesh.lambda(), lambda);
  EXPECT_CLOSE(mesh.eps(), eps);
  
  for (const auto &dlr_rf : mesh ) {
    std::cout << dlr_rf << "\n";
  }
}

TEST(Gf, dlr_coeffs_imtime) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;
  auto cmesh = gf_mesh<triqs::mesh::dlr_coeffs>{beta, Fermion, lambda, eps};
  auto tmesh = gf_mesh<triqs::mesh::dlr_imtime>{beta, Fermion, lambda, eps};

  std::cout << cmesh << "\n";
  std::cout << tmesh << "\n";

  auto tmesh_ref = triqs::mesh::dlr_imtime(cmesh);
  auto cmesh_ref = triqs::mesh::dlr_coeffs(tmesh); // remove gf_mesh everywhere
  
  for (const auto &[t1, t2] : itertools::zip(tmesh, tmesh_ref) ) {
    EXPECT_CLOSE(t1, t2);
  }
  for (const auto &[c1, c2] : itertools::zip(cmesh, cmesh_ref) ) {
    EXPECT_CLOSE(c1, c2);
  } 
}

TEST(Gf, dlr_imtime_grid) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps    = 1e-10;
  double omega  = 1.337;

  auto mesh = gf_mesh<triqs::mesh::dlr_imtime>{beta, Fermion, lambda, eps};
  auto G1 = gf<triqs::mesh::dlr_imtime, scalar_valued>{mesh};
  
  for (const auto &tau : mesh ) {
    G1[tau] = std::exp(-omega * tau) / (1 + std::exp(-beta * omega));
  }

  auto Gc = G1; // copy
  EXPECT_GF_NEAR(G1, Gc);

  auto Gv = G1(); // view
  EXPECT_GF_NEAR(Gv, Gc);
  
  // Take view and assign
  auto G2 = gf<triqs::mesh::dlr_imtime, scalar_valued>{mesh};
  auto G2v = gf_view(G2);
  for (const auto &tau : mesh ) {
    G2v[tau] = std::exp(-omega * tau) / (1 + std::exp(-beta * omega));
  }
  
  EXPECT_GF_NEAR(G2, G2v);
  EXPECT_GF_NEAR(G1, G2v);
  EXPECT_GF_NEAR(G1, G2);

  auto GpG = G1 + G2; // Addition
  for (const auto &tau : mesh ) {
    EXPECT_CLOSE(GpG[tau], G1[tau] * 2.);
    EXPECT_CLOSE(GpG[tau], 2. * G1[tau]);
  }

  auto G4 = 4. * G2; // Multiply with scalar
  for (const auto &tau : mesh ) {
    EXPECT_CLOSE(G4[tau], G1[tau] * 4.);
    EXPECT_CLOSE(G4[tau], 4. * G1[tau]);
  }

  auto GG = G1 * G2; // Multiplication
  for (const auto &tau : mesh ) {
    EXPECT_CLOSE(GG[tau], G1[tau] * G1[tau]);
  }

  std::cout << GG.mesh().domain() << "\n";
  std::cout << GG.mesh().domain().statistic << "\n";
  //EXPECT_TRUE(GG.mesh().domain().statistic == Boson); // Currently fails. FIXME!  
}

TEST(Gf, dlr_imtime_grid_clef) {
  double beta   = 2.0;
  double lambda = 10.0;
  double eps    = 1e-10;
  double omega  = 1.337;

  auto G = gf<triqs::mesh::dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};
  triqs::clef::placeholder<0> tau_;
  G(tau_) << nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  auto mesh = gf_mesh<triqs::mesh::dlr_imtime>{beta, Fermion, lambda, eps};
  auto G1 = gf<triqs::mesh::dlr_imtime, scalar_valued>{mesh};
  
  for (const auto &tau : mesh ) {
    G1[tau] = std::exp(-omega * tau) / (1 + std::exp(-beta * omega));
  }

  EXPECT_GF_NEAR(G, G1);
}

TEST(Gf, dlr_interpolation) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;

  long dlr_idx = 6; // Pick one DLR frequency

  auto G_dlr = gf<triqs::mesh::dlr_coeffs, scalar_valued>{{beta, Fermion, lambda, eps}};
  G_dlr() = 0.0;
  G_dlr.data()[dlr_idx] = 1.0;

  auto cmesh = G_dlr.mesh();
  auto tmesh = triqs::mesh::dlr_imtime(cmesh);
  
  auto G_tau = gf<triqs::mesh::dlr_imtime, scalar_valued>{tmesh};
  triqs::clef::placeholder<0> tau_;

  double omega = 1./beta * cmesh.index_to_point(dlr_idx);
  G_tau(tau_) << nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  // Interpolation in imaginary time using dlr grid (efficient by design)

  for (auto const &tau : G_tau.mesh()) {
    EXPECT_CLOSE(G_tau[tau], G_dlr(tau));
  }

}

TEST(Gf, dlr_coeffs_imtime_conversion) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;
  double omega = 1.337;

  triqs::clef::placeholder<0> tau_;

  auto G_tau = gf<triqs::mesh::dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};
  G_tau(tau_) << nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  // Transform from dlr_imtime to dlr_coeffs

  auto G_dlr = dlr_coeffs_from_dlr_imtime(G_tau);
  for (auto const &tau : G_tau.mesh()) EXPECT_CLOSE(G_tau[tau], G_dlr(tau));
  
  // Transform from dlr_coeffs to dlr_imtime

  auto G_tau_ref = dlr_imtime_from_dlr_coeffs(G_dlr);
  EXPECT_GF_NEAR(G_tau, G_tau_ref);
  
  // Problematic API
  //auto G_dlr = make_gf_from(G_tau);
  // -------- PROBLEMATIC ---------------
  //auto G_matsub = make_gf_from_fourier(G_dlr);
  //auto G_tau = make_gf_from_fourier(G_dlr);
  // -------- PROBLEMATIC ---------------
  
  // Ok API.

  //G_dlr.set_from(G_tau);
  //G_tau.set_from(G_dlr);

  //G_dlr.set_from(G_matsub);
  //G_matsub.set_from(G_dlr);
}

TEST(Gf, dlr_density) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;

  auto G_tau = gf<triqs::mesh::dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};

  double omega = 1.337;
  triqs::clef::placeholder<0> tau_;
  G_tau(tau_) << -nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  // Density from dlr_imtime not supported (by design)
  //EXPECT_THROW(triqs::gfs::density(G_tau), triqs::runtime_error);
  
  // Density from dlr is efficient (by design)
  // only requires interpolation at \tau=\beta ( n = -G(\beta) )
  auto G_dlr = dlr_coeffs_from_dlr_imtime(G_tau); 
  auto n = triqs::gfs::density(G_dlr);
  EXPECT_COMPLEX_NEAR(n, 1 / (1 + std::exp(beta * omega)), 1.e-9);
}

TEST(Gf, dlr_tau_rev) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;

  auto G_tau = gf<triqs::mesh::dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};

  double omega = 1.337;
  triqs::clef::placeholder<0> tau_;
  G_tau(tau_) << nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  auto G_tau_rev = G_tau; // copy
  
  auto G_dlr = dlr_coeffs_from_dlr_imtime(G_tau);
  G_tau_rev(tau_) << G_dlr(beta - tau_);

  // Interpolation in imaginary time using dlr grid (efficient by design)
  for (auto const &tau : G_tau_rev.mesh()) {
    double val = std::exp(-omega * (beta - tau)) / (1 + std::exp(-beta * omega));
    EXPECT_COMPLEX_NEAR(G_tau_rev[tau].real(), val, eps);
    EXPECT_COMPLEX_NEAR(G_tau_rev[tau], G_dlr(beta - tau), eps);
  }
}

TEST(Gf, dlr_h5) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;
  double omega = 1.337;

  triqs::clef::placeholder<0> tau_;

  auto G_tau = gf<triqs::mesh::dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};
  G_tau(tau_) << nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));
  auto G_dlr = dlr_coeffs_from_dlr_imtime(G_tau);

  auto filename = "data_dlr_h5_rw.h5";
  auto name = "G_tau";
  auto G_tau_ref = G_tau;

  {
    h5::file file(filename, 'w');
    h5_write(file, name, G_tau);
  }  
  {
    h5::file file(filename, 'r');
    h5_read(file, name, G_tau_ref);
  }

  EXPECT_GF_NEAR(G_tau, G_tau_ref);

  // The serialization used in rw_h5(...) causes Segfaults

  // Serialize explicitly
  std::cout << "serialize 1\n";
  auto s  = h5::serialize(G_tau);
  std::cout << "deserialize 1\n";
  //auto x2 = h5::deserialize<gf<triqs::mesh::dlr_imtime, scalar_valued>>(s); // SEGFAULTs
  std::cout << "serialize 2\n";
  auto s2 = h5::serialize(G_tau);  

  //EXPECT_EQ_ARRAY(s, s2); // << "Test h5 save, load, save, compare has failed !";
  
  //auto G_tau_ref2 = rw_h5(G_tau, "g_dlr_imtime"); // SEGFAULTS Fixme!
  //EXPECT_GF_NEAR(G_tau, G_tau_ref2);

  //auto G_dlr_ref2 = rw_h5(G_dlr, "g_dlr_coeffs");
  //EXPECT_GF_NEAR(G_dlr, G_dlr_ref2);
}

/*
TEST(Gf, dlr_dyson) {

  double beta   = 2.0;
  double lambda = 10.0;
  double eps = 1e-10;
  double omega = 1.337;
  double U = 10.0;

  triqs::clef::placeholder<0> tau_;

  auto G0_tau = gf<triqs::mesh::dlr_imtime, scalar_valued>{{beta, Fermion, lambda, eps}};
  G0_tau(tau_) << -nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  auto G0_dlr = dlr_coeffs_from_dlr_imtime(G0_tau);

  auto G0_tau_rev = G0_tau;
  G0_tau_rev(tau_) << G0_dlr(beta - tau_);

  auto Sigma_tau = G0_tau;
  Sigma_tau(tau_) << U*U * G0_dlr(tau_) * G0_dlr(tau_) * G0_dlr(beta - tau_);
  auto Sigma_dlr = dlr_coeffs_from_dlr_imtime(Sigma_tau);

  // Solve Dyson equation in DLR coefficients

  //auto G_dlr = G0_dlr; // Remove me.
  auto G_dlr = solve_dyson_equation(G0_dlr, Sigma_dlr); // Implement!

  // Solve Dyson equation in Matsubara frequency using equidistant meshes
  
  auto tmesh = mesh::imtime{beta, Fermion, 10000};
  gf<imtime, scalar_valued> G0_tau_lin{tmesh};
  G0_tau_lin(tau_) << G0_dlr(tau_);
  auto G0_w = make_gf_from_fourier(G0_tau_lin);

  gf<imtime, scalar_valued> Sigma_tau_lin{tmesh};
  Sigma_tau_lin(tau_) << Sigma_dlr(tau_);
  auto Sigma_w = make_gf_from_fourier(Sigma_tau_lin);
  
  gf<imfreq, scalar_valued> G_w{G0_w.mesh()};
  triqs::clef::placeholder<0> w_;
  G_w(w_) << G0_w(w_) / ( 1 - G0_w(w_) * Sigma_w(w_) );
  auto G_tau_lin = make_gf_from_fourier(G_w, tmesh);

  gf<imtime, scalar_valued> G_tau_lin_ref{tmesh};
  G_tau_lin_ref(tau_) << G_dlr(tau_);

  EXPECT_GF_NEAR(G_tau_lin, G_tau_lin_ref);
}

*/

MAKE_MAIN;
