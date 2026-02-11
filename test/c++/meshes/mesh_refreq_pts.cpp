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
#include <triqs/mesh/refreq_pts.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>

#include <cmath>
#include <iostream>
#include <vector>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::refreq_pts>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::refreq_pts>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::refreq_pts::mesh_point_t>);

TEST(TRIQSMesh, RefreqPtsSizeZero) {
  auto m = triqs::mesh::refreq_pts{};
  EXPECT_EQ(m.size(), 0);
  EXPECT_FALSE(m.is_index_valid(0));

  // empty loop
  int idx = 0;
  for ([[maybe_unused]] auto mp : m) ++idx;
  EXPECT_EQ(idx, 0);

  // comparison
  EXPECT_EQ(m, triqs::mesh::refreq_pts{});

  // print
  std::cout << m << std::endl;
}

TEST(TRIQSMesh, RefreqPtsFromVector) {
  auto pts = std::vector<double>{-5.0, -1.0, 0.0, 1.0, 5.0};
  auto m   = triqs::mesh::refreq_pts{pts};
  EXPECT_EQ(m.size(), 5);
  EXPECT_EQ(m.points(), pts);

  // check mesh point values
  for (long i = 0; i < m.size(); ++i) {
    auto mp = m[i];
    EXPECT_EQ(mp.index(), i);
    EXPECT_EQ(mp.data_index(), i);
    EXPECT_DOUBLE_EQ(mp.value(), pts[i]);
    EXPECT_EQ(mp.mesh_hash(), m.mesh_hash());
  }
}

TEST(TRIQSMesh, RefreqPtsFromInitializerList) {
  auto m = triqs::mesh::refreq_pts{-3.0, -1.0, 0.5, 2.0};
  EXPECT_EQ(m.size(), 4);
  EXPECT_DOUBLE_EQ(m.to_value(0), -3.0);
  EXPECT_DOUBLE_EQ(m.to_value(3), 2.0);
}

TEST(TRIQSMesh, RefreqPtsIteration) {
  auto pts = std::vector<double>{-2.0, -0.5, 0.0, 0.5, 2.0};
  auto m   = triqs::mesh::refreq_pts{pts};

  int idx = 0;
  for (auto mp : m) {
    EXPECT_EQ(mp.index(), idx);
    EXPECT_EQ(mp.data_index(), idx);
    EXPECT_DOUBLE_EQ(mp.value(), pts[idx]);
    EXPECT_TRUE(m.is_index_valid(idx));
    ++idx;
  }
  EXPECT_EQ(idx, m.size());
  EXPECT_FALSE(m.is_index_valid(idx));
}

TEST(TRIQSMesh, RefreqPtsIndexing) {
  auto m = triqs::mesh::refreq_pts{std::vector<double>{-5.0, -1.0, 0.0, 1.0, 5.0}};

  // operator[] and operator()
  EXPECT_DOUBLE_EQ(m[0].value(), -5.0);
  EXPECT_DOUBLE_EQ(m[2].value(), 0.0);
  EXPECT_DOUBLE_EQ(m[4].value(), 5.0);
  EXPECT_DOUBLE_EQ(m(0).value(), -5.0);
  EXPECT_DOUBLE_EQ(m(4).value(), 5.0);

  // to_data_index / to_index
  EXPECT_EQ(m.to_data_index(2), 2);
  EXPECT_EQ(m.to_index(2), 2);
  EXPECT_DOUBLE_EQ(m.to_value(2), 0.0);
}

TEST(TRIQSMesh, RefreqPtsClosestMeshPoint) {
  auto m = triqs::mesh::refreq_pts{std::vector<double>{-5.0, -1.0, 0.0, 1.0, 5.0}};
  using triqs::mesh::closest_mesh_point_t;

  // exact match
  EXPECT_EQ(m.to_index(closest_mesh_point_t{0.0}), 2);
  EXPECT_EQ(m.to_index(closest_mesh_point_t{-5.0}), 0);
  EXPECT_EQ(m.to_index(closest_mesh_point_t{5.0}), 4);

  // between points - closer to left
  EXPECT_EQ(m.to_index(closest_mesh_point_t{-0.4}), 2); // closer to 0.0
  EXPECT_EQ(m.to_index(closest_mesh_point_t{0.4}), 2);  // closer to 0.0

  // between points - closer to right
  EXPECT_EQ(m.to_index(closest_mesh_point_t{0.6}), 3);  // closer to 1.0
  EXPECT_EQ(m.to_index(closest_mesh_point_t{-0.6}), 1); // closer to -1.0

  // operator[] with closest_mesh_point_t
  EXPECT_DOUBLE_EQ(m[closest_mesh_point_t{0.4}].value(), 0.0);
  EXPECT_DOUBLE_EQ(m[closest_mesh_point_t{0.6}].value(), 1.0);
}

TEST(TRIQSMesh, RefreqPtsMeshPointArithmetic) {
  auto m  = triqs::mesh::refreq_pts{std::vector<double>{-1.0, 0.0, 1.0}};
  auto mp = m[2]; // value = 1.0

  EXPECT_DOUBLE_EQ(mp + 2.0, 3.0);
  EXPECT_DOUBLE_EQ(mp - 0.5, 0.5);
  EXPECT_DOUBLE_EQ(mp * 3.0, 3.0);
  EXPECT_DOUBLE_EQ(mp / 2.0, 0.5);
  EXPECT_DOUBLE_EQ(2.0 + mp, 3.0);
  EXPECT_DOUBLE_EQ(5.0 - mp, 4.0);
  EXPECT_DOUBLE_EQ(3.0 * mp, 3.0);
  EXPECT_DOUBLE_EQ(4.0 / mp, 4.0);

  // implicit conversion
  double v = mp;
  EXPECT_DOUBLE_EQ(v, 1.0);
}

TEST(TRIQSMesh, RefreqPtsEvaluate) {
  auto m = triqs::mesh::refreq_pts{std::vector<double>{0.0, 1.0, 2.0, 3.0}};

  // linear function f(i) = 2*i => f(w) = 2*w
  auto f_lin = [](long i) { return 2.0 * i; };

  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, f_lin, 0.0), 0.0);
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, f_lin, 0.5), 1.0);
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, f_lin, 1.5), 3.0);
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, f_lin, 3.0), 6.0);

  // non-uniform mesh
  auto m2    = triqs::mesh::refreq_pts{std::vector<double>{-10.0, -1.0, 0.0, 1.0, 10.0}};
  auto f_val = [](long i) {
    double vals[] = {-10.0, -1.0, 0.0, 1.0, 10.0};
    return vals[i];
  };

  // exact mesh points
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m2, f_val, -10.0), -10.0);
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m2, f_val, 0.0), 0.0);
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m2, f_val, 10.0), 10.0);
}

TEST(TRIQSMesh, RefreqPtsHDF5) {
  auto m = triqs::mesh::refreq_pts{std::vector<double>{-5.0, -1.0, 0.0, 1.0, 5.0}};

  auto m_r = rw_h5(m, "triqs_mesh_refreq_pts", "m");
  EXPECT_EQ(m, m_r);
  EXPECT_EQ(m.size(), m_r.size());
  for (long i = 0; i < m.size(); ++i) EXPECT_DOUBLE_EQ(m.to_value(i), m_r.to_value(i));
}

TEST(TRIQSMesh, RefreqPtsEquality) {
  auto m1 = triqs::mesh::refreq_pts{std::vector<double>{-1.0, 0.0, 1.0}};
  auto m2 = m1;
  auto m3 = triqs::mesh::refreq_pts{std::vector<double>{-1.0, 0.0, 2.0}};

  EXPECT_EQ(m1, m2);
  EXPECT_NE(m1, m3);
  EXPECT_NE(m1, triqs::mesh::refreq_pts{});
}

TEST(TRIQSMesh, RefreqPtsUnsortedThrows) { EXPECT_THROW(triqs::mesh::refreq_pts(std::vector<double>{3.0, 1.0, 2.0}), triqs::runtime_error); }

MAKE_MAIN;
