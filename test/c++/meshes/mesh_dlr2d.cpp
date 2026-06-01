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

#include <triqs/mesh/concepts.hpp>
#include <triqs/mesh/dlr2d.hpp>
#include <triqs/mesh/dlr2d_imfreq.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <fmt/base.h>
#include <h5/h5.hpp>

#include <complex>
#include <string>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::dlr2d>);
static_assert(triqs::mesh::Mesh<triqs::mesh::dlr2d_imfreq>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::dlr2d>);
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::dlr2d_imfreq>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::dlr2d::mesh_point_t>);
static_assert(triqs::mesh::MeshPoint<triqs::mesh::dlr2d_imfreq::mesh_point_t>);

// Test 2D DLR coefficient mesh.
TEST(TRIQSMesh, DLR2D) {
  // default constructed mesh
  auto m1 = triqs::mesh::dlr2d{};
  EXPECT_EQ(m1.size(), 0);
  EXPECT_EQ(m1.rank(), 0);

  // another default constructed mesh (should be equal to the previous mesh)
  auto m2 = triqs::mesh::dlr2d{};
  EXPECT_EQ(m1, m2);

  // mesh with beta = 10, w_max = 1 and eps = 1e-10 (should be different from the previous meshes)
  auto m3 = triqs::mesh::dlr2d{10, 1, 1e-10, triqs::mesh::PH};
  EXPECT_NE(m1, m3);
  EXPECT_GT(m3.size(), 0);
  EXPECT_GT(m3.rank(), 0);

  // copy constructed mesh (should be equal to m3)
  auto m4 = m3;
  EXPECT_EQ(m3, m4);

  // print meshes
  std::cout << m1 << std::endl;
  std::cout << m3 << std::endl;

  // check mesh properties
  EXPECT_DOUBLE_EQ(m3.beta(), 10);
  EXPECT_DOUBLE_EQ(m3.w_max(), 1);
  EXPECT_DOUBLE_EQ(m3.eps(), 1e-10);
  EXPECT_EQ(m3.compressgrid(), false);
  EXPECT_EQ(m3.compressbasis(), true);
  EXPECT_EQ(m3.statistic(), triqs::mesh::Fermion);

  // check DLR frequencies are available
  EXPECT_EQ(static_cast<long>(m3.dlr_rf().size()), m3.rank());
  EXPECT_GT(m3.dlr2d_if().shape(0), 0);
  EXPECT_EQ(m3.dlr2d_if().shape(1), 2);
  // dlr2d_rf has 3 columns: [term_type, k_index, l_index]
  EXPECT_EQ(static_cast<long>(m3.dlr2d_rf().shape(0)), m3.size());
  EXPECT_EQ(m3.dlr2d_rf().shape(1), 3);

  // loop over all mesh points
  int i = 0;
  for (auto mp : m3) {
    EXPECT_EQ(mp.data_index(), i);
    EXPECT_EQ(mp.mesh_hash(), m3.mesh_hash());
    auto val = m3.to_value(i);
    EXPECT_EQ(mp.value().first, val.first);
    EXPECT_EQ(mp.value().second, val.second);
    // Test index_t <-> data_index_t conversion
    auto idx = m3.to_index(i);
    EXPECT_EQ(m3.to_data_index(idx), i);
    EXPECT_EQ(mp.index(), idx);
    ++i;
  }
  EXPECT_EQ(i, m3.size());

  // write and read to HDF5
  auto m3_r = rw_h5(m3, "triqs_mesh_dlr2d", "m3");
  EXPECT_EQ(m3, m3_r);
}

// Test 2D DLR imfreq mesh.
TEST(TRIQSMesh, DLR2DImfreq) {
  // default constructed mesh
  auto m1 = triqs::mesh::dlr2d_imfreq{};
  EXPECT_EQ(m1.size(), 0);
  EXPECT_EQ(m1.rank(), 0);

  // another default constructed mesh (should be equal to the previous mesh)
  auto m2 = triqs::mesh::dlr2d_imfreq{};
  EXPECT_EQ(m1, m2);

  // mesh with beta = 10, w_max = 1 and eps = 1e-10 (should be different from the previous meshes)
  auto m3 = triqs::mesh::dlr2d_imfreq{10, 1, 1e-10, triqs::mesh::PH};
  EXPECT_NE(m1, m3);
  EXPECT_GT(m3.size(), 0);
  EXPECT_GT(m3.rank(), 0);

  // copy constructed mesh (should be equal to m3)
  auto m4 = m3;
  EXPECT_EQ(m3, m4);

  // print meshes
  std::cout << m1 << std::endl;
  std::cout << m3 << std::endl;

  // check mesh properties
  EXPECT_DOUBLE_EQ(m3.beta(), 10);
  EXPECT_DOUBLE_EQ(m3.w_max(), 1);
  EXPECT_DOUBLE_EQ(m3.eps(), 1e-10);
  EXPECT_EQ(m3.compressgrid(), false);
  EXPECT_EQ(m3.compressbasis(), true);
  EXPECT_EQ(m3.statistic(), triqs::mesh::Fermion);

  // check DLR frequencies are available
  EXPECT_EQ(static_cast<long>(m3.dlr_rf().size()), m3.rank());
  EXPECT_EQ(static_cast<long>(m3.dlr2d_if().shape(0)), m3.size());
  EXPECT_EQ(m3.dlr2d_if().shape(1), 2);

  // loop over all mesh points
  int i = 0;
  for (auto mp : m3) {
    EXPECT_EQ(mp.data_index(), i);
    EXPECT_EQ(mp.mesh_hash(), m3.mesh_hash());
    auto idx = m3.to_index(i);
    EXPECT_EQ(mp.index()[0], idx[0]);
    EXPECT_EQ(mp.index()[1], idx[1]);
    auto val = m3.to_value(idx);
    EXPECT_EQ(mp.value().first.n, val.first.n);
    EXPECT_EQ(mp.value().second.n, val.second.n);
    ++i;
  }
  EXPECT_EQ(i, m3.size());

  // write and read to HDF5
  auto m3_r = rw_h5(m3, "triqs_mesh_dlr2d_imfreq", "m3");
  EXPECT_EQ(m3, m3_r);
}

// Test structured bindings for dlr2d_imfreq mesh points.
TEST(TRIQSMesh, DLR2DImfreqStructuredBindings) {
  auto m = triqs::mesh::dlr2d_imfreq{10, 1, 1e-10, triqs::mesh::PH};

  static_assert(std::tuple_size_v<triqs::mesh::dlr2d_imfreq::mesh_point_t> == 2);

  int i = 0;
  for (auto [w1, w2] : m) {
    auto mp  = m[i];
    auto val = mp.value();
    EXPECT_EQ(w1.n, val.first.n);
    EXPECT_EQ(w2.n, val.second.n);
    EXPECT_DOUBLE_EQ(w1.beta, m.beta());
    EXPECT_DOUBLE_EQ(w2.beta, m.beta());
    ++i;
  }
  EXPECT_EQ(i, m.size());
}

// Test constructing dlr2d/dlr2d_imfreq meshes from each other.
TEST(TRIQSMesh, DLR2DConversions) {
  auto m_dlr2d1        = triqs::mesh::dlr2d{10, 1, 1e-10, triqs::mesh::PH};
  auto m_dlr2d_imfreq1 = triqs::mesh::dlr2d_imfreq{10, 1, 1e-10, triqs::mesh::PH};

  // dlr2d -> dlr2d_imfreq
  auto m_dlr2d_imfreq2 = triqs::mesh::dlr2d_imfreq{m_dlr2d1};
  EXPECT_EQ(m_dlr2d_imfreq1, m_dlr2d_imfreq2);

  // dlr2d_imfreq -> dlr2d
  auto m_dlr2d2 = triqs::mesh::dlr2d{m_dlr2d_imfreq1};
  EXPECT_EQ(m_dlr2d1, m_dlr2d2);
}

// Test different compression settings.
TEST(TRIQSMesh, DLR2DCompressionSettings) {
  // Default: compressgrid = false, compressbasis = true
  auto m1 = triqs::mesh::dlr2d{10, 1, 1e-10, triqs::mesh::PH};
  EXPECT_EQ(m1.compressgrid(), false);
  EXPECT_EQ(m1.compressbasis(), true);

  // No compression
  auto m2 = triqs::mesh::dlr2d{10, 1, 1e-10, triqs::mesh::PH, false, false};
  EXPECT_EQ(m2.compressgrid(), false);
  EXPECT_EQ(m2.compressbasis(), false);

  // Different compression settings should give different hashes
  EXPECT_NE(m1.mesh_hash(), m2.mesh_hash());
}

// Test channel parameter.
TEST(TRIQSMesh, DLR2DChannel) {

  // Particle-hole channel
  auto m1 = triqs::mesh::dlr2d{10, 1, 1e-10, triqs::mesh::PH};
  EXPECT_EQ(m1.channel(), triqs::mesh::PH);

  // Particle-particle channel
  auto m2 = triqs::mesh::dlr2d{10, 1, 1e-10, triqs::mesh::PP};
  EXPECT_EQ(m2.channel(), triqs::mesh::PP);
  EXPECT_NE(m1, m2); // Different channel -> different mesh hash

  // Test channel on dlr2d_imfreq
  auto m3 = triqs::mesh::dlr2d_imfreq{10, 1, 1e-10, triqs::mesh::PP};
  EXPECT_EQ(m3.channel(), triqs::mesh::PP);

  // Channel is preserved in conversions
  auto m4 = triqs::mesh::dlr2d{m3};
  EXPECT_EQ(m4.channel(), triqs::mesh::PP);

  auto m5 = triqs::mesh::dlr2d_imfreq{m3};
  EXPECT_EQ(m5.channel(), triqs::mesh::PP);
}

// Test evaluation at Matsubara frequency pairs.
TEST(TRIQSMesh, DLR2DEvaluate) {
  using namespace triqs::mesh;

  auto m = dlr2d{10, 1, 1e-10, PH};

  // Create coefficients
  nda::array<dcomplex, 1> coefs(m.size());
  coefs() = 0.0;

  // Set some non-trivial coefficients
  for (long i = 0; i < std::min(m.size(), 5l); ++i) { coefs(i) = dcomplex(1.0, 0.5) * double(i + 1); }

  // Create lambda for accessing coefficients
  auto f = [&coefs](long i) { return coefs(i); };

  // Create matsubara frequencies for evaluation
  auto iw0 = matsubara_freq(0, m.beta(), Fermion);
  auto iw1 = matsubara_freq(1, m.beta(), Fermion);
  auto iw5 = matsubara_freq(5, m.beta(), Fermion);
  auto iwm5 = matsubara_freq(-5, m.beta(), Fermion);

  // Evaluate at a few frequency pairs using std::pair<matsubara_freq, matsubara_freq>
  auto val1 = evaluate(m, f, std::make_pair(iw0, iw0));
  auto val2 = evaluate(m, f, std::make_pair(iw1, iw0));
  auto val3 = evaluate(m, f, std::make_pair(iw0, iw1));
  auto val4 = evaluate(m, f, std::make_pair(iw5, iwm5));

  // Results should be complex numbers (exact values depend on cppdlr2d implementation)
  EXPECT_TRUE(std::isfinite(val1.real()));
  EXPECT_TRUE(std::isfinite(val1.imag()));
  EXPECT_TRUE(std::isfinite(val2.real()));
  EXPECT_TRUE(std::isfinite(val3.real()));
  EXPECT_TRUE(std::isfinite(val4.real()));

  // Evaluating with zero coefficients should give zero
  coefs() = 0.0;
  auto val_zero = evaluate(m, f, std::make_pair(iw0, iw0));
  EXPECT_NEAR(std::abs(val_zero), 0.0, 1e-14);
}

MAKE_MAIN;
