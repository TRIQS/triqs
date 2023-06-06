// Copyright (c) 2023 Simons Foundation
// Copyright (c) 2023 Hugo Strand
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
// Authors: Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#include <gtest/gtest.h>
#include <stdexcept>
#include <triqs/test_tools/gfs.hpp>
#include <h5/serialization.hpp>

using triqs::mesh::dlr;
using triqs::mesh::dlr_imfreq;
using triqs::mesh::dlr_imtime;

constexpr triqs::clef::placeholder<0> iw_;
constexpr triqs::clef::placeholder<1> tau_;

constexpr auto _ = all_t{}; // NOLINT
using std::exp;

double onefermion(double tau, double eps, double beta) { return -exp(-eps * tau) / (1 + exp(-beta * eps)); }

// ------------------------------------------------------------
// Take a gf in time, go to coeffs, to freq, and back and check evals.
TEST(Gf, G_dlr_mat) {

  double beta  = 5;
  double w_max = 20.0;
  double eps   = 1e-10;
  double e0    = 1.2;
  double tol   = 1.e-9;

  // imtime
  auto g1 = gf<dlr_imtime, matrix_valued>{{beta, Fermion, w_max, eps}, {1, 1}};
  for (auto const &tau : g1.mesh()) g1[tau] = onefermion(tau, e0, beta);

  // coefs
  auto g2 = make_gf_dlr(g1);

  // reverse transfo ok
  auto g1b = make_gf_dlr_imtime(g2);
  EXPECT_GF_NEAR(g1, g1b);
  EXPECT_EQ(g1.mesh().mesh_hash(), g1b.mesh().mesh_hash());

  // imfreq
  auto g3 = make_gf_dlr_imfreq(g2);

  // reverse transfo ok
  auto g2b = make_gf_dlr(g3);
  EXPECT_GF_NEAR(g2, g2b);
  EXPECT_EQ(g2.mesh().mesh_hash(), g2b.mesh().mesh_hash());

  // check value
  auto g3_check = gf<dlr_imfreq, matrix_valued>{{beta, Fermion, w_max, eps}, {1, 1}};
  for (auto const &w : g3_check.mesh()) g3_check[w] = 1 / (w - e0);

  EXPECT_GF_NEAR(g3, g3_check);
  EXPECT_EQ(g3.mesh().mesh_hash(), g3_check.mesh().mesh_hash());

  // eval g2 on tau is fine
  for (auto const &tau : g1.mesh()) { EXPECT_COMPLEX_NEAR(g2(tau)(0, 0), g1[tau](0, 0), tol); }
  // eval g2 on w is fine
  for (auto const &w : g3.mesh()) { EXPECT_COMPLEX_NEAR(g2(w)(0, 0), g3[w](0, 0), tol); }
}
// ------------------------------------------------------------
// simpler test, scalar_valued. Same as in Python.
TEST(Gf, G_dlr_mat2) {

  double beta  = 1.337;
  double w_max = 50.0;
  double eps   = 1e-11;
  double e0    = 1.42;
  double tol   = 1.e-9;

  auto gw = gf<dlr_imfreq, scalar_valued>{dlr_imfreq{beta, Fermion, w_max, eps}};
  for (auto const &w : gw.mesh()) gw[w] = 1 / (w - e0);

  auto gc = make_gf_dlr(gw);
  auto gt = make_gf_dlr_imtime(gc);

  for (auto const &tau : gt.mesh()) { EXPECT_COMPLEX_NEAR(gc(tau), onefermion(tau, e0, beta), tol); }
}

// ------------------------------------------------------------
// Very basic properties of dlr_imtime

TEST(Gf, DLR_basic) {
  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  auto m_tau   = dlr_imtime{beta, Fermion, w_max, eps};
  auto m_w_f   = dlr_imfreq{beta, Fermion, w_max, eps};
  auto m_w_b   = dlr_imfreq{beta, Boson, w_max, eps};

  EXPECT_EQ(m_tau.w_max(), w_max);
  EXPECT_EQ(m_w_f.w_max(), w_max);
  EXPECT_EQ(m_w_b.w_max(), w_max);
  EXPECT_EQ(m_tau.eps(), eps);
  EXPECT_EQ(m_w_f.eps(), eps);
  EXPECT_EQ(m_w_b.eps(), eps);

  for (const auto &tau : m_tau) {
    EXPECT_TRUE(tau <= beta);
    EXPECT_TRUE(tau >= 0.0);
  }
}

// ----------------------------------------------------------------
// test cross construction of dlr meshes
TEST(Gf, DLR_cross_construction) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  auto mc = dlr{beta, Fermion, w_max, eps};
  auto mt = dlr_imtime{beta, Fermion, w_max, eps};
  auto mw = dlr_imfreq{beta, Fermion, w_max, eps};

  auto mc2 = dlr(mt);
  auto mc3 = dlr(mw);
  auto mt2 = dlr_imtime(mc);
  auto mt3 = dlr_imtime(mw);
  auto mw2 = dlr_imfreq(mc);
  auto mw3 = dlr_imfreq(mt);

  EXPECT_EQ(mc.mesh_hash(), mc2.mesh_hash());
  EXPECT_EQ(mc.mesh_hash(), mc3.mesh_hash());
  EXPECT_NE(mc.mesh_hash(), mt.mesh_hash());
  EXPECT_NE(mc.mesh_hash(), mw.mesh_hash());

  EXPECT_EQ(mt.mesh_hash(), mt2.mesh_hash());
  EXPECT_EQ(mt.mesh_hash(), mt3.mesh_hash());
  EXPECT_NE(mt.mesh_hash(), mc.mesh_hash());
  EXPECT_NE(mt.mesh_hash(), mw.mesh_hash());

  EXPECT_EQ(mw.mesh_hash(), mw2.mesh_hash());
  EXPECT_EQ(mw.mesh_hash(), mw3.mesh_hash());
  EXPECT_NE(mw.mesh_hash(), mt.mesh_hash());
  EXPECT_NE(mw.mesh_hash(), mc.mesh_hash());

  for (const auto &[c1, c2] : itertools::zip(mc, mc2)) { EXPECT_EQ(c1, c2); }
  for (const auto &[c1, c2] : itertools::zip(mc, mc3)) { EXPECT_EQ(c1, c2); }
  for (const auto &[t1, t2] : itertools::zip(mt, mt2)) { EXPECT_CLOSE(t1, t2); }
  for (const auto &[t1, t2] : itertools::zip(mt, mt3)) { EXPECT_CLOSE(t1, t2); }
  for (const auto &[w1, w2] : itertools::zip(mw, mw2)) { EXPECT_CLOSE(dcomplex(w1), dcomplex(w2)); }
  for (const auto &[w1, w2] : itertools::zip(mw, mw3)) { EXPECT_CLOSE(dcomplex(w1), dcomplex(w2)); }
}

// ----------------------------------------------------------------

TEST(Gf, DLR_imtime_grid) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto mesh = dlr_imtime{beta, Fermion, w_max, eps};
  auto G1   = gf<dlr_imtime, scalar_valued>{mesh};

  for (const auto &tau : mesh) { G1[tau] = std::exp(-omega * tau) / (1 + std::exp(-beta * omega)); }

  auto Gc = G1; // copy
  EXPECT_GF_NEAR(G1, Gc);

  auto Gv = G1(); // view
  EXPECT_GF_NEAR(Gv, Gc);

  // Take view and assign
  auto G2  = gf<dlr_imtime, scalar_valued>{mesh};
  auto G2v = gf_view(G2);
  for (const auto &tau : mesh) { G2v[tau] = std::exp(-omega * tau) / (1 + std::exp(-beta * omega)); }

  EXPECT_GF_NEAR(G2, G2v);
  EXPECT_GF_NEAR(G1, G2v);
  EXPECT_GF_NEAR(G1, G2);

  auto GpG = G1 + G2; // Addition
  for (const auto &tau : mesh) {
    EXPECT_CLOSE(GpG[tau], G1[tau] * 2.);
    EXPECT_CLOSE(GpG[tau], 2. * G1[tau]);
  }

  auto G4 = 4. * G2; // Multiply with scalar
  for (const auto &tau : mesh) {
    EXPECT_CLOSE(G4[tau], G1[tau] * 4.);
    EXPECT_CLOSE(G4[tau], 4. * G1[tau]);
  }

  auto GG = G1 * G2; // Multiplication
  for (const auto &tau : mesh) { EXPECT_CLOSE(GG[tau], G1[tau] * G1[tau]); }

  EXPECT_TRUE(GG.mesh().statistic() == Boson);
}

// ----------------------------------------------------------------
// Check basic behaviour of GF. Not really useful ...
TEST(Gf, DLR_imfreq_grid) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto mesh = dlr_imfreq{beta, Fermion, w_max, eps};
  auto G1   = gf<dlr_imfreq, scalar_valued>{mesh};

  for (const auto &iw : mesh) { G1[iw] = 1. / (iw - omega); }

  auto Gc = G1; // copy
  EXPECT_GF_NEAR(G1, Gc);

  auto Gv = G1(); // view
  EXPECT_GF_NEAR(Gv, Gc);

  // Take view and assign
  auto G2  = gf<dlr_imfreq, scalar_valued>{mesh};
  auto G2v = gf_view(G2);
  for (const auto &iw : mesh) { G2v[iw] = 1. / (iw - omega); }

  EXPECT_GF_NEAR(G2, G2v);
  EXPECT_GF_NEAR(G1, G2v);
  EXPECT_GF_NEAR(G1, G2);

  auto GpG = G1 + G2; // Addition
  for (const auto &iw : mesh) {
    EXPECT_CLOSE(GpG[iw], G1[iw] * 2.);
    EXPECT_CLOSE(GpG[iw], 2. * G1[iw]);
  }

  auto G4 = 4. * G2; // Multiply with scalar
  for (const auto &iw : mesh) {
    EXPECT_CLOSE(G4[iw], G1[iw] * 4.);
    EXPECT_CLOSE(G4[iw], 4. * G1[iw]);
  }

  auto GG = G1 * G2; // Multiplication
  for (const auto &iw : mesh) { EXPECT_CLOSE(GG[iw], G1[iw] * G1[iw]); }
}

// ----------------------------------------------------------------
// Test usage of clef

TEST(Gf, DLR_clef) {
  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto gt = gf<dlr_imtime, scalar_valued>{{beta, Fermion, w_max, eps}};
  gt[tau_] << -nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  // another construction
  auto mesh = dlr_imtime{beta, Fermion, w_max, eps};
  auto gt2  = gf{mesh};

  static_assert(std::is_same_v<decltype(gt2), decltype(gt)>);
  for (const auto &tau : mesh) gt2[tau] = onefermion(tau, omega, beta);
  EXPECT_GF_NEAR(gt, gt2);

  auto gw = gf<dlr_imfreq, scalar_valued>{{beta, Fermion, w_max, eps}};
  gw[iw_] << 1. / (iw_ - omega);

  auto gw2 = gf<dlr_imfreq, scalar_valued>{{beta, Fermion, w_max, eps}};

  // Should not compile. Wrong mesh point
  //for (const auto &iw : mesh) gw2[iw] = 1 / (iw - omega);
  for (const auto &iw : gw2.mesh()) gw2[iw] = 1 / (iw - omega);

  EXPECT_GF_NEAR(gw, gw2);
}

// ----------------------------------------------------------------

TEST(Gf, DLR_imtime_interpolation) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  long dlr_idx = 6; // Pick one DLR frequency

  auto g            = gf<dlr, scalar_valued>{{beta, Fermion, w_max, eps}};
  g()               = 0.0;
  g.data()[dlr_idx] = 1.0;

  auto cmesh   = g.mesh();
  auto tmesh   = dlr_imtime(cmesh);
  double omega = 1. / beta * cmesh.dlr_freq()[dlr_idx];

  // WARNING : + here because cppdlr is one global minus sign off from usual convention
  for (auto const &tau : tmesh) { EXPECT_CLOSE(g(tau), (-onefermion(tau, omega, beta))); }
}

// ----------------------------------------------------------------

TEST(Gf, DLR_imfreq_interpolation) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto gw = gf<dlr_imfreq, scalar_valued>{{beta, Fermion, w_max, eps}};
  gw[iw_] << 1. / (iw_ - omega);

  auto gc = make_gf_dlr(gw);

  auto gw2 = gw;
  gw2[iw_] << gc(iw_); // Interpolate DLR in imaginary frequency

  EXPECT_GF_NEAR(gw, gw2);
  for (auto const &iw : gw.mesh()) EXPECT_CLOSE(gw[iw], gw2[iw]);
}

// ----------------------------------------------------------------

TEST(Gf, DLR_imtime_fit) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;
  int n_tau    = 1001;

  auto gtau = gf<imtime, scalar_valued>{{beta, Fermion, n_tau}};
  for (auto tau : gtau.mesh()) gtau[tau] = onefermion(tau, omega, eps);

  auto gcoef = fit_gf_dlr(gtau, w_max, eps);
  auto gtau2 = make_gf_imtime(gcoef, n_tau);
  EXPECT_GF_NEAR(gtau, gtau2);

  for (double sigma : {0.1, 0.01, 0.001, 0.0001, 0.00001}) {
    auto gtau_noise = gtau;
    gtau_noise.data() += sigma * (nda::rand(gtau.mesh().size()) - 0.5);
    auto gcoef_noise = fit_gf_dlr(gtau_noise, w_max, eps);
    auto gtau3       = make_gf_imtime(gcoef_noise, n_tau);
    EXPECT_GF_NEAR(gtau, gtau3, sigma);
  }
}

// ----------------------------------------------------------------

// Test some interpolation. Not really necessary
TEST(Gf, DLR_ph_sym_interpolation) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  auto gt = gf<dlr_imtime, scalar_valued>{{beta, Fermion, w_max, eps}};

  double omega = 1.337;
  gt[tau_] << +nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  auto gc  = make_gf_dlr(gt);
  auto gt2 = gt;
  gt2[tau_] << gc(beta - tau_);

  // Interpolation in imaginary time using dlr grid (efficient by design)
  for (auto const &tau : gt2.mesh()) {
    double val = std::exp(-omega * (beta - tau)) / (1 + std::exp(-beta * omega));
    EXPECT_COMPLEX_NEAR(gt2[tau].real(), val, eps);
    EXPECT_COMPLEX_NEAR(gt2[tau], gc(beta - tau), eps);
  }
}
// ----------------------------------------------------------------

TEST(Gf, DLR_mesh_point_mismatch) {

  double beta  = 2.0;
  double beta2 = 2 * beta;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto gw  = gf<dlr_imfreq, scalar_valued>{{beta, Fermion, w_max, eps}};
  auto gw2 = gf<dlr_imfreq, scalar_valued>{{beta2, Fermion, w_max, eps}};
  for (auto const &iw : gw.mesh()) EXPECT_DEBUG_DEATH(gw2[iw], "Precondition m.mesh_hash");

  // THIS PART SHOULD NOT COMPILE AND GIVE A GOOD ERROR MESSAGE
  // auto gwold = gf<imfreq, scalar_valued>{{beta2, Fermion}};
  // for (auto const &iw : gw.mesh()) gwold[iw] = 0; // must not compile
}

// ----------------------------------------------------------------
//
TEST(Gf, DLR_density) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  auto gt = gf<dlr_imtime, scalar_valued>{{beta, Fermion, w_max, eps}};

  double omega = 1.337;
  gt[tau_] << -nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  // SHOULD NOT COMPILE. DELETED FUNCTION
  // auto d = triqs::gfs::density(gt);

  // Density from dlr is efficient (by design)
  // only requires interpolation at \tau=\beta ( n = -G(\beta) )
  auto gc = make_gf_dlr(gt);
  auto n  = triqs::gfs::density(gc);
  EXPECT_COMPLEX_NEAR(n, 1 / (1 + std::exp(beta * omega)), 1.e-9);
}
// ----------------------------------------------------------------
// Same with matrix
TEST(Gf, DLR_density_matrix) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  auto gt = gf<dlr_imtime, matrix_valued>{{beta, Fermion, w_max, eps}, {2, 2}};

  double omega = 1.337;
  gt[tau_] << -nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  // SHOULD NOT COMPILE. DELETED FUNCTION
  // auto d = triqs::gfs::density(gt);

  // Density from dlr is efficient (by design)
  // only requires interpolation at \tau=\beta ( n = -G(\beta) )
  auto gc = make_gf_dlr(gt);
  auto n  = triqs::gfs::density(gc);
  auto d  = 1 / (1 + std::exp(beta * omega));
  EXPECT_ARRAY_NEAR(n, (nda::matrix<double>{{d, 0}, {0, d}}), 1.e-9);
}

// ----------------------------------------------------------------
TEST(Gf, DLR_h5) {
  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto gt = gf<dlr_imtime, scalar_valued>{{beta, Fermion, w_max, eps}};
  gt[tau_] << nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));
  auto gc = make_gf_dlr(gt);
  auto gw = make_gf_dlr_imtime(gc);

  rw_h5(gt, "g_dlr_imtime");
  rw_h5(gc, "g_dlr");
  rw_h5(gw, "g_dlr_imfreq");
}

// ----------------------------------------------------------------
// Partial test with a more complex function. ?
TEST(Gf, DLR_two_poles) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  double e1 = 0.0;
  double e2 = 2.0;

  auto G_tau = gf<dlr_imtime, scalar_valued>{{beta, Fermion, w_max, eps}};
  G_tau[tau_] <<                                                          //
     -0.5 * nda::clef::exp(-e1 * tau_) / (1 + nda::clef::exp(-beta * e1)) //
        - 0.5 * nda::clef::exp(-e2 * tau_) / (1 + nda::clef::exp(-beta * e2));

  auto G_dlr = make_gf_dlr(G_tau);
  auto G_iw  = make_gf_dlr_imfreq(G_dlr);

  auto gw2 = G_iw;
  gw2[iw_] << 0.5 / (iw_ - e1) + 0.5 / (iw_ - e2);

  EXPECT_GF_NEAR(G_iw, gw2);
}

// ----------------------------------------------------------------

MAKE_MAIN;
