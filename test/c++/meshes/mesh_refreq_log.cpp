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
#include <triqs/mesh/refreq_log.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>

#include <cmath>
#include <iostream>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::refreq_log>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::refreq_log>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::refreq_log::mesh_point_t>);

TEST(TRIQSMesh, RefreqLogSizeZero) {
  auto m = triqs::mesh::refreq_log{};
  EXPECT_EQ(m.size(), 0);
  EXPECT_FALSE(m.is_index_valid(0));

  // empty loop
  int idx = 0;
  for ([[maybe_unused]] auto mp : m) ++idx;
  EXPECT_EQ(idx, 0);

  // comparison
  EXPECT_EQ(m, triqs::mesh::refreq_log{});

  // print
  std::cout << m << std::endl;
}

TEST(TRIQSMesh, RefreqLogConstruction) {
  auto m = triqs::mesh::refreq_log{0.1, 10.0, 2.0};

  EXPECT_DOUBLE_EQ(m.eps(), 0.1);
  EXPECT_DOUBLE_EQ(m.w_max(), 10.0);
  EXPECT_DOUBLE_EQ(m.ratio(), 2.0);
  EXPECT_GT(m.size(), 0);
  EXPECT_EQ(m.size() % 2, 0); // always even (symmetric)

  // positive points: 10, 5, 2.5, 1.25, 0.625, 0.3125, 0.15625 (last >= 0.1)
  // negative mirrors: 7 positive + 7 negative = 14 total
  EXPECT_EQ(m.size(), 14);

  std::cout << m << std::endl;
}

TEST(TRIQSMesh, RefreqLogSymmetry) {
  auto m = triqs::mesh::refreq_log{0.01, 10.0, 2.0};
  long n = m.size();
  EXPECT_EQ(n % 2, 0);

  // check symmetry: pts[i] == -pts[n-1-i]
  for (long i = 0; i < n / 2; ++i) EXPECT_DOUBLE_EQ(m.to_value(i), -m.to_value(n - 1 - i));
}

TEST(TRIQSMesh, RefreqLogSorted) {
  auto m = triqs::mesh::refreq_log{0.001, 5.0, 1.5};
  for (long i = 1; i < m.size(); ++i) EXPECT_LT(m.to_value(i - 1), m.to_value(i));
}

TEST(TRIQSMesh, RefreqLogGeometricSequence) {
  double eps   = 0.1;
  double w_max = 10.0;
  double ratio = 2.0;
  auto m       = triqs::mesh::refreq_log{eps, w_max, ratio};

  // the positive half should form a geometric sequence
  long n_pos = m.size() / 2;
  for (long i = 0; i < n_pos - 1; ++i) {
    double w_cur  = m.to_value(n_pos + i);
    double w_next = m.to_value(n_pos + i + 1);
    EXPECT_NEAR(w_next / w_cur, ratio, 1e-12);
  }
}

TEST(TRIQSMesh, RefreqLogIteration) {
  auto m = triqs::mesh::refreq_log{0.1, 10.0, 2.0};

  int idx = 0;
  for (auto mp : m) {
    EXPECT_EQ(mp.index(), idx);
    EXPECT_EQ(mp.data_index(), idx);
    EXPECT_DOUBLE_EQ(mp.value(), m.to_value(idx));
    EXPECT_EQ(mp.mesh_hash(), m.mesh_hash());
    EXPECT_TRUE(m.is_index_valid(idx));
    ++idx;
  }
  EXPECT_EQ(idx, m.size());
  EXPECT_FALSE(m.is_index_valid(idx));
}

TEST(TRIQSMesh, RefreqLogIndexing) {
  auto m = triqs::mesh::refreq_log{0.1, 10.0, 2.0};

  // operator[] and operator()
  EXPECT_DOUBLE_EQ(m[0].value(), m.to_value(0));
  EXPECT_DOUBLE_EQ(m(0).value(), m.to_value(0));
  EXPECT_DOUBLE_EQ(m[m.size() - 1].value(), m.to_value(m.size() - 1));

  // first and last values
  EXPECT_DOUBLE_EQ(m.to_value(0), -10.0);
  EXPECT_DOUBLE_EQ(m.to_value(m.size() - 1), 10.0);
}

TEST(TRIQSMesh, RefreqLogClosestMeshPoint) {
  auto m = triqs::mesh::refreq_log{0.1, 10.0, 2.0};
  using triqs::mesh::closest_mesh_point_t;

  // exact match at w_max
  auto idx_wmax = m.to_index(closest_mesh_point_t{10.0});
  EXPECT_DOUBLE_EQ(m.to_value(idx_wmax), 10.0);

  // exact match at -w_max
  auto idx_neg_wmax = m.to_index(closest_mesh_point_t{-10.0});
  EXPECT_DOUBLE_EQ(m.to_value(idx_neg_wmax), -10.0);

  // value close to 5.0 should map to index of 5.0
  auto idx_5 = m.to_index(closest_mesh_point_t{4.9});
  EXPECT_DOUBLE_EQ(m.to_value(idx_5), 5.0);

  // operator[] with closest_mesh_point_t
  EXPECT_DOUBLE_EQ(m[closest_mesh_point_t{10.0}].value(), 10.0);
}

TEST(TRIQSMesh, RefreqLogMeshPointArithmetic) {
  auto m  = triqs::mesh::refreq_log{0.1, 10.0, 2.0};
  auto mp = m[m.size() - 1]; // value = 10.0

  EXPECT_DOUBLE_EQ(mp + 2.0, 12.0);
  EXPECT_DOUBLE_EQ(mp - 3.0, 7.0);
  EXPECT_DOUBLE_EQ(mp * 2.0, 20.0);
  EXPECT_DOUBLE_EQ(mp / 5.0, 2.0);
  EXPECT_DOUBLE_EQ(2.0 + mp, 12.0);
  EXPECT_DOUBLE_EQ(3.0 * mp, 30.0);

  // implicit conversion
  double v = mp;
  EXPECT_DOUBLE_EQ(v, 10.0);
}

TEST(TRIQSMesh, RefreqLogEvaluate) {
  auto m = triqs::mesh::refreq_log{0.1, 10.0, 2.0};

  // constant function
  auto f_const = [](long) { return 1.0; };
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, f_const, 0.5), 1.0);
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, f_const, -3.0), 1.0);

  // function equal to mesh values: evaluation at mesh points should be exact
  auto f_id = [&](long i) { return m.to_value(i); };
  for (long i = 0; i < m.size(); ++i) { EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, f_id, m.to_value(i)), m.to_value(i)); }
}

TEST(TRIQSMesh, RefreqLogHDF5) {
  auto m = triqs::mesh::refreq_log{1e-4, 10.0, 1.2};

  auto m_r = rw_h5(m, "triqs_mesh_refreq_log", "m");
  EXPECT_EQ(m, m_r);
  EXPECT_DOUBLE_EQ(m_r.eps(), 1e-4);
  EXPECT_DOUBLE_EQ(m_r.w_max(), 10.0);
  EXPECT_DOUBLE_EQ(m_r.ratio(), 1.2);
  EXPECT_EQ(m.size(), m_r.size());

  // verify points are identical after round-trip
  for (long i = 0; i < m.size(); ++i) EXPECT_DOUBLE_EQ(m.to_value(i), m_r.to_value(i));
}

TEST(TRIQSMesh, RefreqLogEquality) {
  auto m1 = triqs::mesh::refreq_log{0.1, 10.0, 2.0};
  auto m2 = m1;
  auto m3 = triqs::mesh::refreq_log{0.1, 10.0, 3.0};

  EXPECT_EQ(m1, m2);
  EXPECT_NE(m1, m3);
  EXPECT_NE(m1, triqs::mesh::refreq_log{});
}

TEST(TRIQSMesh, RefreqLogEpsEqualsWmax) {
  // edge case: eps == w_max -> only two points: -eps and +eps
  auto m = triqs::mesh::refreq_log{1.0, 1.0, 2.0};
  EXPECT_EQ(m.size(), 2);
  EXPECT_DOUBLE_EQ(m.to_value(0), -1.0);
  EXPECT_DOUBLE_EQ(m.to_value(1), 1.0);
}

MAKE_MAIN;
