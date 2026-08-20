// Copyright (c) 2023 Simons Foundation
// Copyright (c) 2023 Hugo U.R. Strand
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

#include "triqs/gfs/functions/dlr.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <triqs/test_tools/gfs.hpp>
#include <h5/serialization.hpp>

using triqs::mesh::dlr;
using triqs::mesh::dlr_imfreq;
using triqs::mesh::dlr_imtime;

constexpr nda::clef::placeholder<0> iw_;
constexpr nda::clef::placeholder<1> iwp_;
constexpr nda::clef::placeholder<2> tau_;
constexpr nda::clef::placeholder<3> taup_;

using std::exp;

auto onefermion(auto tau, double eps, double beta) { return -exp(-eps * tau) / (1 + exp(-beta * eps)); }

// ------------------------------------------------------------
// Check mesh accessors
TEST(Gf, dlr_accessors) {

  double beta     = 5;
  double w_max    = 20.0;
  double eps      = 1e-10;
  bool symmetrize = true;

  { // dlr mesh
    auto g = gf<dlr, matrix_valued>{{beta, Fermion, w_max, eps, symmetrize}, {1, 1}};

    EXPECT_EQ(g.mesh().beta(), beta);
    EXPECT_EQ(g.mesh().eps(), eps);
    EXPECT_EQ(g.mesh().w_max(), w_max);
    EXPECT_EQ(g.mesh().statistic(), Fermion);
    EXPECT_EQ(g.mesh().symmetrize(), symmetrize);
  }

  { // dlr_imtime
    auto g = gf<dlr_imtime, matrix_valued>{{beta, Fermion, w_max, eps, symmetrize}, {1, 1}};

    EXPECT_EQ(g.mesh().beta(), beta);
    EXPECT_EQ(g.mesh().eps(), eps);
    EXPECT_EQ(g.mesh().w_max(), w_max);
    EXPECT_EQ(g.mesh().statistic(), Fermion);
    EXPECT_EQ(g.mesh().symmetrize(), symmetrize);
  }

  { // dlr_imfreq
    auto g = gf<dlr_imfreq, matrix_valued>{{beta, Fermion, w_max, eps, symmetrize}, {1, 1}};

    EXPECT_EQ(g.mesh().beta(), beta);
    EXPECT_EQ(g.mesh().eps(), eps);
    EXPECT_EQ(g.mesh().w_max(), w_max);
    EXPECT_EQ(g.mesh().statistic(), Fermion);
    EXPECT_EQ(g.mesh().symmetrize(), symmetrize);
  }
}

// ------------------------------------------------------------
// Take a gf in time, go to coeffs, to freq, and back and check evals.
TEST(Gf, dlr_mat) {

  double beta  = 5;
  double w_max = 20.0;
  double eps   = 1e-10;
  double e0    = 1.2;
  double tol   = 1.e-9;

  // imtime
  auto g1 = gf<dlr_imtime, matrix_valued>{{beta, Fermion, w_max, eps}, {1, 1}};
  for (auto tau : g1.mesh()) g1[tau] = onefermion(tau, e0, beta);

  // coefs
  auto g2 = make_gf_dlr(g1);

  // reverse transfo ok
  auto g1b = make_gf_dlr_imtime(g2);
  EXPECT_GF_NEAR(g1, g1b);
  EXPECT_EQ(g1.mesh().mesh_hash(), g1b.mesh().mesh_hash());

  // compare dlr_imfreq against exact result
  auto g3       = make_gf_dlr_imfreq(g2);
  auto g3_check = gf<dlr_imfreq, matrix_valued>{{beta, Fermion, w_max, eps}, {1, 1}};
  for (auto iw : g3_check.mesh()) g3_check[iw] = 1 / (iw - e0);

  EXPECT_GF_NEAR(g3, g3_check);
  EXPECT_EQ(g3.mesh().mesh_hash(), g3_check.mesh().mesh_hash());

  // eval g2 on tau is fine
  for (auto tau : g1.mesh()) { EXPECT_COMPLEX_NEAR(g2(tau)(0, 0), g1[tau](0, 0), tol); }
  // eval g2 on iw is fine
  for (auto iw : g3.mesh()) { EXPECT_COMPLEX_NEAR(g2(iw)(0, 0), g3[iw](0, 0), tol); }
}
// ------------------------------------------------------------
// simpler test, scalar_valued. Same as in Python.
TEST(Gf, dlr_mat2) {

  double beta  = 1.337;
  double w_max = 50.0;
  double eps   = 1e-11;
  double e0    = 1.42;
  double tol   = 1.e-9;

  auto gw = gf<dlr_imfreq, scalar_valued>{dlr_imfreq{beta, Fermion, w_max, eps}};
  for (auto iw : gw.mesh()) gw[iw] = 1 / (iw - e0);

  auto gc = make_gf_dlr(gw);
  auto gt = make_gf_dlr_imtime(gc);

  for (auto tau : gt.mesh()) { EXPECT_COMPLEX_NEAR(gc(tau), onefermion(tau, e0, beta), tol); }
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

  for (auto tau : m_tau) {
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

  for (auto [c1, c2] : itertools::zip(mc, mc2)) { EXPECT_EQ(c1, c2); }
  for (auto [c1, c2] : itertools::zip(mc, mc3)) { EXPECT_EQ(c1, c2); }
  for (auto [t1, t2] : itertools::zip(mt, mt2)) { EXPECT_CLOSE(t1, t2); }
  for (auto [t1, t2] : itertools::zip(mt, mt3)) { EXPECT_CLOSE(t1, t2); }
  for (auto [w1, w2] : itertools::zip(mw, mw2)) { EXPECT_CLOSE(dcomplex(w1), dcomplex(w2)); }
  for (auto [w1, w2] : itertools::zip(mw, mw3)) { EXPECT_CLOSE(dcomplex(w1), dcomplex(w2)); }
}

// ----------------------------------------------------------------

TEST(Gf, DLR_imtime_grid) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;

  auto mesh = dlr_imtime{beta, Fermion, w_max, eps};
  auto G1   = gf<dlr_imtime, scalar_valued>{mesh};
  G1[tau_] << onefermion(tau_, omega, beta);

  auto Gc = G1; // copy
  EXPECT_GF_NEAR(G1, Gc);

  auto Gv = G1(); // view
  EXPECT_GF_NEAR(Gv, Gc);

  // Take view and assign
  auto G2  = gf<dlr_imtime, scalar_valued>{mesh};
  auto G2v = gf_view(G2);
  G2v[tau_] << onefermion(tau_, omega, beta);

  EXPECT_GF_NEAR(G2, G2v);
  EXPECT_GF_NEAR(G1, G2v);
  EXPECT_GF_NEAR(G1, G2);

  auto GpG = G1 + G2; // Addition
  for (auto tau : mesh) {
    EXPECT_CLOSE(GpG[tau], G1[tau] * 2.);
    EXPECT_CLOSE(GpG[tau], 2. * G1[tau]);
  }

  auto G4 = 4. * G2; // Multiply with scalar
  for (auto tau : mesh) {
    EXPECT_CLOSE(G4[tau], G1[tau] * 4.);
    EXPECT_CLOSE(G4[tau], 4. * G1[tau]);
  }

  auto GG = G1 * G2; // Multiplication
  for (auto tau : mesh) { EXPECT_CLOSE(GG[tau], G1[tau] * G1[tau]); }

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

  for (auto iw : mesh) { G1[iw] = 1. / (iw - omega); }

  auto Gc = G1; // copy
  EXPECT_GF_NEAR(G1, Gc);

  auto Gv = G1(); // view
  EXPECT_GF_NEAR(Gv, Gc);

  // Take view and assign
  auto G2  = gf<dlr_imfreq, scalar_valued>{mesh};
  auto G2v = gf_view(G2);
  for (auto iw : mesh) { G2v[iw] = 1. / (iw - omega); }

  EXPECT_GF_NEAR(G2, G2v);
  EXPECT_GF_NEAR(G1, G2v);
  EXPECT_GF_NEAR(G1, G2);

  auto GpG = G1 + G2; // Addition
  for (auto iw : mesh) {
    EXPECT_CLOSE(GpG[iw], G1[iw] * 2.);
    EXPECT_CLOSE(GpG[iw], 2. * G1[iw]);
  }

  auto G4 = 4. * G2; // Multiply with scalar
  for (auto iw : mesh) {
    EXPECT_CLOSE(G4[iw], G1[iw] * 4.);
    EXPECT_CLOSE(G4[iw], 4. * G1[iw]);
  }

  auto GG = G1 * G2; // Multiplication
  for (auto iw : mesh) { EXPECT_CLOSE(GG[iw], G1[iw] * G1[iw]); }
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
  for (auto tau : mesh) gt2[tau] = onefermion(tau, omega, beta);
  EXPECT_GF_NEAR(gt, gt2);

  auto gw = gf<dlr_imfreq, scalar_valued>{{beta, Fermion, w_max, eps}};
  gw[iw_] << 1. / (iw_ - omega);

  auto gw2 = gf<dlr_imfreq, scalar_valued>{{beta, Fermion, w_max, eps}};
  for (auto iw : gw2.mesh()) gw2[iw] = 1 / (iw - omega);

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
  for (auto tau : tmesh) { EXPECT_CLOSE(g(tau), onefermion(tau, omega, beta)); }
}

// ----------------------------------------------------------------

// A particle-particle bubble: the dlr_imtime mesh hash is statistic-blind, so the bosonic mesh points may index the fermionic g.
TEST(Gf, dlr_pp_bubble) {

  double beta     = 2.0;
  double w_max    = 5.0;
  double eps      = 1e-12;
  bool symmetrize = true;

  double e = 1.5; // 2 * e < w_max, so the squared propagator is representable in the basis

  auto g   = gf<dlr_imtime, scalar_valued>{{beta, Fermion, w_max, eps, symmetrize}};
  auto chi = gf<dlr_imtime, scalar_valued>{{beta, Boson, w_max, eps, symmetrize}};

  EXPECT_EQ(g.mesh().mesh_hash(), chi.mesh().mesh_hash());

  for (auto tau : g.mesh()) g[tau] = onefermion(tau, e, beta);
  for (auto tau : chi.mesh()) chi[tau] = g[tau] * g[tau];

  // evaluate(dlr_imtime, ...) is deleted, so compare through the coefficient mesh
  auto chi_c = make_gf_dlr(chi);
  for (auto x : {0.1, 0.3, 0.5, 0.7, 0.9}) {
    double tau = x * beta;
    auto g_tau = onefermion(tau, e, beta);
    EXPECT_COMPLEX_NEAR(chi_c(tau), g_tau * g_tau, 1e-9);
  }
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
  for (auto iw : gw.mesh()) EXPECT_CLOSE(gw[iw], gw2[iw]);
}

// ----------------------------------------------------------------

TEST(Gf, DLR_imtime_fit) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;
  double omega = 1.337;
  int n_tau    = 1001;

  auto gtau = gf<imtime, scalar_valued>{{beta, Fermion, n_tau}};
  gtau[tau_] << onefermion(tau_, omega, beta);

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

  // BlockGf test
  auto Bgtau = block_gf{std::vector{gtau, gtau}};

  auto Bgcoef = fit_gf_dlr(Bgtau, w_max, eps);
  auto Bgtau2 = make_gf_imtime(Bgcoef, n_tau);
  EXPECT_BLOCK_GF_NEAR(Bgtau, Bgtau2);

  // Mesh overload: passing the dlr mesh directly is equivalent to (w_max, eps)
  auto dlr_mesh = mesh::dlr{beta, Fermion, w_max, eps};
  auto gcoef_m  = fit_gf_dlr(gtau, dlr_mesh);
  EXPECT_EQ(gcoef.mesh(), gcoef_m.mesh());
  EXPECT_GF_NEAR(gcoef, gcoef_m);

  auto Bgcoef_m = fit_gf_dlr(Bgtau, dlr_mesh);
  EXPECT_BLOCK_GF_NEAR(Bgcoef, Bgcoef_m);
}

// ----------------------------------------------------------------

TEST(Gf, DLR_multivar_fit) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  double e1 = 1.0;
  double e2 = 2.0;

  int n_tau = 1001;

  auto it_mesh = imtime{beta, Fermion, n_tau};
  auto gtau    = gf{it_mesh * it_mesh};
  gtau[tau_, taup_] << onefermion(tau_, e1, beta) * onefermion(taup_, e2, beta);

  auto gcoef = fit_gf_dlr<0, 1>(gtau, w_max, eps);
  auto gtau2 = make_gf_imtime<0, 1>(gcoef, n_tau);
  EXPECT_GF_NEAR(gtau, gtau2);

  // BlockGf
  auto Bgtau = block_gf{std::vector{gtau, gtau}};

  auto Bgcoef = fit_gf_dlr<0, 1>(Bgtau, w_max, eps);
  auto Bgtau2 = make_gf_imtime<0, 1>(Bgcoef, n_tau);
  EXPECT_BLOCK_GF_NEAR(Bgtau, Bgtau2);

  for (double sigma : {0.1, 0.01, 0.001, 0.0001, 0.00001}) {
    auto gtau_noise = gtau;
    gtau_noise.data() += sigma * (nda::rand(n_tau, n_tau) - 0.5);
    auto gcoef_noise = fit_gf_dlr<0, 1>(gtau_noise, w_max, eps, false);
    auto gtau3       = make_gf_imtime<0, 1>(gcoef_noise, n_tau);
    EXPECT_GF_NEAR(gtau, gtau3, 30 * sigma);
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
  gt[tau_] << -nda::clef::exp(-omega * tau_) / (1 + nda::clef::exp(-beta * omega));

  auto gc  = make_gf_dlr(gt);
  auto gt2 = gt;
  gt2[tau_] << gc(beta - tau_);

  // Interpolation in imaginary time using dlr grid (efficient by design)
  for (auto tau : gt2.mesh()) {
    double val = -std::exp(-omega * (beta - tau)) / (1 + std::exp(-beta * omega));
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

  auto gw  = gf<dlr_imfreq, scalar_valued>{{beta, Fermion, w_max, eps}};
  auto gw2 = gf<dlr_imfreq, scalar_valued>{{beta2, Fermion, w_max, eps}};
  for (auto iw : gw.mesh()) EXPECT_DEBUG_DEATH(gw2[iw], "Precondition m.mesh_hash");
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

  // Density from dlr is efficient (by design)
  // only requires interpolation at \tau=\beta ( n = -G(\beta) )
  auto gc = make_gf_dlr(gt);
  auto nc = triqs::gfs::density(gc);
  EXPECT_COMPLEX_NEAR(nc, 1 / (1 + std::exp(beta * omega)), 1.e-9);

  auto nt = triqs::gfs::density(gt);
  EXPECT_COMPLEX_NEAR(nt, 1 / (1 + std::exp(beta * omega)), 1.e-9);

  auto gw = make_gf_dlr_imfreq(gc);
  auto nw = triqs::gfs::density(gw);
  EXPECT_COMPLEX_NEAR(nw, 1 / (1 + std::exp(beta * omega)), 1.e-9);
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
  auto gw = make_gf_dlr_imfreq(gc);

  // BlockGf test
  auto Bgtau  = block_gf{std::vector{gt, gt}};
  auto Bgcoef = make_gf_dlr(Bgtau);
  auto Bgiwn  = make_gf_dlr_imfreq(Bgcoef);

  rw_h5(gt, "g_dlr_imtime");
  rw_h5(gc, "g_dlr");
  rw_h5(gw, "g_dlr_imfreq");
  rw_h5(Bgtau, "bg_dlr_imtime");
  rw_h5(Bgcoef, "bg_dlr");
  rw_h5(Bgiwn, "bg_dlr_imfreq");
}

// ----------------------------------------------------------------
// Partial test with a more complex function. ?
TEST(Gf, DLR_two_poles) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  double e1 = 0.0;
  double e2 = 2.0;

  auto gtau = gf<dlr_imtime, scalar_valued>{{beta, Fermion, w_max, eps}};
  gtau[tau_] << 0.5 * onefermion(tau_, e1, beta) + 0.5 * onefermion(tau_, e2, beta);

  auto giw = make_gf_dlr_imfreq(gtau);

  auto G2_iw = giw;
  G2_iw[iw_] << 0.5 / (iw_ - e1) + 0.5 / (iw_ - e2);

  EXPECT_GF_NEAR(giw, G2_iw);
  EXPECT_GF_NEAR(giw, make_gf_from_fourier(gtau));
}

// ----------------------------------------------------------------
TEST(Gf, DLR_multivar) {

  double beta  = 2.0;
  double w_max = 5.0;
  double eps   = 1e-10;

  double e1 = 1.0;
  double e2 = 2.0;

  auto dlr_it_mesh = dlr_imtime{beta, Fermion, w_max, eps};
  auto gtau        = gf{dlr_it_mesh * dlr_it_mesh};
  gtau[tau_, taup_] << onefermion(tau_, e1, beta) * onefermion(taup_, e2, beta);

  auto giw = make_gf_dlr_imfreq<0, 1>(gtau);

  auto G2_iw = giw;
  G2_iw[iw_, iwp_] << 1.0 / (iw_ - e1) / (iwp_ - e2);

  EXPECT_GF_NEAR(giw, G2_iw);
  EXPECT_GF_NEAR(giw, (make_gf_from_fourier<0, 1>(gtau)));
}

// ----------------------------------------------------------------

TEST(Gf, DLR_FT_iw_to_tau) {
  auto beta    = 2.0;
  auto mu      = 0.5;
  auto wmax    = 10 * beta;
  auto eps_DLR = 1e-10;

  auto const iw_mesh = dlr_imfreq{beta, Fermion, wmax, eps_DLR};
  auto G0_iw         = gf<dlr_imfreq, scalar_valued>{iw_mesh};
  for (auto iw : G0_iw.mesh()) { G0_iw[iw] = 1.0 / (iw + mu); }

  auto G0 = make_gf_from_fourier(G0_iw);
  for (auto tau : G0.mesh()) { EXPECT_COMPLEX_NEAR(G0[tau], onefermion(tau, -mu, beta), 10 * eps_DLR); }
}

TEST(Gf, MakeGfDlrImfreqFromImfreq) {
  using triqs::mesh::imfreq;

  double beta  = 10.0;
  long n_iw    = 400;
  double w_max = 10.0;
  double eps   = 1e-10;
  double tol   = 1e-8;

  // scalar_valued
  {
    auto g   = gf<imfreq, scalar_valued>{imfreq{beta, Fermion, n_iw}};
    double e = 1.42;
    for (auto iw : g.mesh()) g[iw] = 1.0 / (iw - e);

    auto g_dlr_iw = make_gf_dlr_imfreq(g, w_max, eps);
    EXPECT_EQ(g_dlr_iw.mesh().w_max(), w_max);

    for (auto const &mp : g_dlr_iw.mesh()) EXPECT_COMPLEX_NEAR(g_dlr_iw[mp], g[g.mesh()(mp.index())], tol);

    auto g_rec = make_gf_imfreq(make_gf_dlr(g_dlr_iw), n_iw);
    EXPECT_GF_NEAR(g_rec, g, tol);
  }

  {
    auto g = gf<imfreq, matrix_valued>{imfreq{beta, Fermion, n_iw}, {2, 2}};
    nda::array<double, 1> poles{1.42, -0.7};
    for (auto iw : g.mesh())
      for (int i = 0; i < 2; ++i) g[iw](i, i) = 1.0 / (iw - poles(i));

    auto g_dlr_iw = make_gf_dlr_imfreq(g, w_max, eps);
    auto g_rec    = make_gf_imfreq(make_gf_dlr(g_dlr_iw), n_iw);
    EXPECT_GF_NEAR(g_rec, g, tol);
  }

  {
    auto g   = gf<imfreq, scalar_valued>{imfreq{beta, Boson, n_iw}};
    double e = 0.7;
    for (auto iw : g.mesh()) g[iw] = 1.0 / (iw - e);

    auto g_dlr_iw = make_gf_dlr_imfreq(g, w_max, eps);
    EXPECT_EQ(g_dlr_iw.mesh().statistic(), Boson);
    auto g_rec = make_gf_imfreq(make_gf_dlr(g_dlr_iw), n_iw);
    EXPECT_GF_NEAR(g_rec, g, tol);
  }
}

TEST(Gf, MakeGfDlrImfreqFromImfreqBlock) {
  using triqs::mesh::imfreq;

  double beta  = 10.0;
  long n_iw    = 400;
  double w_max = 10.0;
  double eps   = 1e-10;
  double tol   = 1e-8;

  auto make_pole = [&](double e) {
    auto g = gf<imfreq, scalar_valued>{imfreq{beta, Fermion, n_iw}};
    for (auto iw : g.mesh()) g[iw] = 1.0 / (iw - e);
    return g;
  };
  auto g_up = make_pole(1.42);
  auto g_dn = make_pole(-0.7);
  auto Bg   = make_block_gf({"up", "dn"}, std::vector{g_up, g_dn});

  auto Bg_dlr_iw = make_gf_dlr_imfreq(Bg, w_max, eps);
  EXPECT_EQ(Bg_dlr_iw[0].mesh().w_max(), w_max);

  auto Bg_rec = make_gf_imfreq(make_gf_dlr(Bg_dlr_iw), n_iw);
  EXPECT_GF_NEAR(Bg_rec[0], Bg[0], tol);
  EXPECT_GF_NEAR(Bg_rec[1], Bg[1], tol);
}

TEST(Gf, FindWMaxScalar) {
  using triqs::mesh::imfreq;

  double beta = 10.0;
  long n_iw   = 400;
  double eps  = 1e-10;
  double e    = 1.42;

  auto g = gf<imfreq, scalar_valued>{imfreq{beta, Fermion, n_iw}};
  for (auto iw : g.mesh()) g[iw] = 1.0 / (iw - e);

  double w_max = find_w_max(g, eps);
  EXPECT_GT(w_max, std::abs(e));
  EXPECT_LT(w_max, 200.0);

  auto g_dlr_iw = make_gf_dlr_imfreq(g, w_max, eps);
  auto g_rec    = make_gf_imfreq(make_gf_dlr(g_dlr_iw), n_iw);
  EXPECT_GF_NEAR(g_rec, g, 10 * eps);
}

TEST(Gf, FindWMaxBlockUsesWorstPole) {
  using triqs::mesh::imfreq;

  double beta = 10.0;
  long n_iw   = 400;
  double eps  = 1e-10;

  auto make_pole = [&](double e) {
    auto g = gf<imfreq, scalar_valued>{imfreq{beta, Fermion, n_iw}};
    for (auto iw : g.mesh()) g[iw] = 1.0 / (iw - e);
    return g;
  };
  auto g_small = make_pole(0.2);
  auto g_large = make_pole(5.0);
  auto Bg      = make_block_gf({"s", "l"}, std::vector{g_small, g_large});

  double w_max = find_w_max(Bg, eps);
  EXPECT_GT(w_max, 5.0);

  auto Bg_dlr_iw = make_gf_dlr_imfreq(Bg, w_max, eps);
  auto Bg_rec    = make_gf_imfreq(make_gf_dlr(Bg_dlr_iw), n_iw);
  EXPECT_GF_NEAR(Bg_rec[0], Bg[0], 10 * eps);
  EXPECT_GF_NEAR(Bg_rec[1], Bg[1], 10 * eps);
}

// Failure modes: imfreq mesh too small to cover the DLR nodes; search bound below init.
TEST(Gf, MakeGfDlrImfreqErrors) {
  using triqs::mesh::imfreq;

  double beta = 10.0;

  auto g_small_grid = gf<imfreq, scalar_valued>{imfreq{beta, Fermion, 5}};
  for (auto iw : g_small_grid.mesh()) g_small_grid[iw] = 1.0 / (iw - 1.0);
  EXPECT_THROW(make_gf_dlr_imfreq(g_small_grid, 100.0, 1e-10), triqs::runtime_error);

  auto g_ok = gf<imfreq, scalar_valued>{imfreq{beta, Fermion, 400}};
  for (auto iw : g_ok.mesh()) g_ok[iw] = 1.0 / (iw - 1.42);
  EXPECT_THROW(find_w_max(g_ok, 1e-10, true, 1.0, 0.5), triqs::runtime_error);
}

// A self-energy-like input with a non-zero high-frequency tail (constant offset) cannot be
// represented by the DLR (which only spans sums of decaying poles). find_w_max should report
// failure for any w_max, since the round-trip residual stays near |c| at large |iw|.
TEST(Gf, FindWMaxConstantOffsetThrows) {
  using triqs::mesh::imfreq;

  double beta = 10.0;
  long n_iw   = 400;
  double eps  = 1e-10;
  double e    = 1.42;
  double c    = 0.7; // Hartree-Fock-like static shift

  auto g = gf<imfreq, scalar_valued>{imfreq{beta, Fermion, n_iw}};
  for (auto iw : g.mesh()) g[iw] = c + 1.0 / (iw - e);
  EXPECT_THROW(find_w_max(g, eps), triqs::runtime_error);

  // block_gf: a single offending block must still trigger the failure.
  auto g_ok = gf<imfreq, scalar_valued>{imfreq{beta, Fermion, n_iw}};
  for (auto iw : g_ok.mesh()) g_ok[iw] = 1.0 / (iw - e);
  auto Bg = make_block_gf({"ok", "bad"}, std::vector{g_ok, g});
  EXPECT_THROW(find_w_max(Bg, eps), triqs::runtime_error);
}

MAKE_MAIN;
