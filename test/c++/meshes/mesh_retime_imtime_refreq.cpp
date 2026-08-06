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

#include <triqs/mesh/concepts.hpp>
#include <triqs/mesh/imtime.hpp>
#include <triqs/mesh/refreq.hpp>
#include <triqs/mesh/retime.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <numbers>
#include <type_traits>
#include <vector>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::retime>);
static_assert(triqs::mesh::Mesh<triqs::mesh::refreq>);
static_assert(triqs::mesh::Mesh<triqs::mesh::imtime>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::retime>);
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::refreq>);
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::imtime>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::retime::mesh_point_t>);
static_assert(triqs::mesh::MeshPoint<triqs::mesh::refreq::mesh_point_t>);
static_assert(triqs::mesh::MeshPoint<triqs::mesh::imtime::mesh_point_t>);

// Check mesh point.
void check_mp(auto mp, auto idx, auto val, auto hash) {
  EXPECT_EQ(mp.index(), idx);
  EXPECT_EQ(mp.data_index(), idx);
  EXPECT_NEAR(mp.value(), val, 1e-14);
  EXPECT_EQ(mp.mesh_hash(), hash);
}

// Mesh value generator.
struct value_gen {
  double a     = 0.0;
  double delta = 0.0;
  int i        = 0;
  auto operator()() { return a + i++ * delta; }
};

// Test a mesh of size 0.
void check_size_zero(auto const &m) {
  EXPECT_EQ(m.size(), 0);
  EXPECT_FALSE(m.is_index_valid(0));

  // empty loop
  int idx = 0;
  for ([[maybe_unused]] auto mp : m) ++idx;
  EXPECT_EQ(idx, 0);

  // comparison operators
  EXPECT_TRUE(std::decay_t<decltype(m)>() == m);
  EXPECT_FALSE(std::decay_t<decltype(m)>() != m);

  // print
  std::cout << m << std::endl;
}

// Test a mesh of size 1.
void check_size_one(auto const &m, double val) {
  EXPECT_EQ(m.size(), 1);
  EXPECT_EQ(m.delta(), 0.0);
  EXPECT_EQ(m.delta_inv(), std::numeric_limits<double>::infinity());

  // loop over 1 mesh point
  int idx = 0;
  for ([[maybe_unused]] auto mp : m) {
    check_mp(mp, idx, val, m.mesh_hash());
    EXPECT_TRUE(m.is_index_valid(idx));
    ++idx;
  }
  EXPECT_EQ(idx, 1);
  EXPECT_FALSE(m.is_index_valid(idx));

  // index <-> data index <-> value
  auto cmp = triqs::mesh::closest_mesh_point_t{val};
  EXPECT_EQ(m.to_data_index(0), 0);
  EXPECT_EQ(m.to_data_index(cmp), 0);
  EXPECT_EQ(m.to_index(0), 0);
  EXPECT_EQ(m.to_index(cmp), 0);
  EXPECT_DOUBLE_EQ(m.to_value(0), val);

  // subscript operator
  check_mp(m[0], 0, val, m.mesh_hash());
  check_mp(m[cmp], 0, val, m.mesh_hash());

  // function call operator
  check_mp(m(0), 0, val, m.mesh_hash());

  // print
  std::cout << m << std::endl;
}

// Test a mesh of size L.
void check_size_l(auto const &m, auto const &vals) {
  using triqs::mesh::closest_mesh_point_t;

  // check the calls that take a closest_mesh_point_t
  auto check_cmp = [](auto m, auto cmp, auto idx, auto val) {
    EXPECT_EQ(m.to_index(cmp), idx);
    EXPECT_EQ(m.to_data_index(cmp), idx);
    check_mp(m[cmp], idx, val, m.mesh_hash());
  };

  EXPECT_EQ(m.size(), vals.size());
  EXPECT_DOUBLE_EQ(m.delta(), vals[1] - vals[0]);
  EXPECT_DOUBLE_EQ(m.delta_inv(), 1 / m.delta());

  // loop over mesh points
  int idx = 0;
  for ([[maybe_unused]] auto mp : m) {
    check_mp(mp, idx, vals[idx], m.mesh_hash());
    EXPECT_TRUE(m.is_index_valid(idx));

    // index <-> data index <-> value, subscript operator, function call operator
    EXPECT_EQ(m.to_data_index(idx), idx);
    EXPECT_EQ(m.to_index(idx), idx);
    EXPECT_NEAR(m.to_value(idx), vals[idx], 1e-14);
    check_mp(m[idx], idx, vals[idx], m.mesh_hash());
    check_mp(m(idx), idx, vals[idx], m.mesh_hash());
    if (idx > 0) {
      check_cmp(m, closest_mesh_point_t{vals[idx] - 0.25 * m.delta()}, idx, vals[idx]);
      check_cmp(m, closest_mesh_point_t{vals[idx] - 0.75 * m.delta()}, idx - 1, vals[idx - 1]);
    }
    if (idx < m.size() - 1) {
      check_cmp(m, closest_mesh_point_t{vals[idx] + 0.25 * m.delta()}, idx, vals[idx]);
      check_cmp(m, closest_mesh_point_t{vals[idx] + 0.75 * m.delta()}, idx + 1, vals[idx + 1]);
    }
    ++idx;
  }
  EXPECT_EQ(idx, m.size());
  EXPECT_FALSE(m.is_index_valid(idx));

  // copy constructor and comparison
  using mesh_t = std::decay_t<decltype(m)>;
  auto m2      = m;
  EXPECT_TRUE(m == m2);
  EXPECT_FALSE(m != m2);
  EXPECT_FALSE(m == mesh_t{});
  EXPECT_TRUE(m != mesh_t{});

  // print
  std::cout << m << std::endl;
}

TEST(TRIQSMesh, RetimeSizeZero) {
  // default constructed retime mesh with size 0
  check_size_zero(triqs::mesh::retime{});
}

TEST(TRIQSMesh, RefreqSizeZero) {
  // default constructed refreq mesh with size 0
  check_size_zero(triqs::mesh::refreq{});
}

TEST(TRIQSMesh, ImtimeSizeZero) {
  // default constructed imtime mesh with size 0
  check_size_zero(triqs::mesh::imtime{});
}

TEST(TRIQSMesh, RetimeSizeOne) {
  using std::numbers::pi;

  // retime mesh with size 1
  check_size_one(triqs::mesh::retime{0, 0, 1}, 0);
  check_size_one(triqs::mesh::retime{pi, pi, 1}, pi);
  check_size_one(triqs::mesh::retime{-pi, -pi, 1}, -pi);
}

TEST(TRIQSMesh, RefreqSizeOne) {
  using std::numbers::pi;

  // refreq mesh with size 1
  check_size_one(triqs::mesh::refreq{0, 0, 1}, 0);
  check_size_one(triqs::mesh::refreq{pi, pi, 1}, pi);
  check_size_one(triqs::mesh::refreq{-pi, -pi, 1}, -pi);
}

TEST(TRIQSMesh, ImtimeSizeOne) {
  // retime mesh with size 1
  check_size_one(triqs::mesh::imtime{0, triqs::mesh::Boson, 1}, 0);
}

TEST(TRIQSMesh, RetimeSizeL) {
  // retime mesh with size 2
  auto vals2 = std::vector<double>(2);
  std::ranges::generate(vals2, value_gen{.a = -1.0, .delta = 2.0});
  check_size_l(triqs::mesh::retime{-1.0, 1.0, 2}, vals2);

  // retime mesh with size 11
  auto vals11 = std::vector<double>(11);
  std::ranges::generate(vals11, value_gen{.a = -1.0, .delta = 0.2});
  check_size_l(triqs::mesh::retime{-1, 1, 11}, vals11);
}

TEST(TRIQSMesh, RefreqSizeL) {
  // refreq mesh with size 2
  auto vals2 = std::vector<double>(2);
  std::ranges::generate(vals2, value_gen{.a = -1.0, .delta = 2.0});
  check_size_l(triqs::mesh::refreq{-1.0, 1.0, 2}, vals2);

  // refreq mesh with size 11
  auto vals11 = std::vector<double>(11);
  std::ranges::generate(vals11, value_gen{.a = -1.0, .delta = 0.2});
  check_size_l(triqs::mesh::refreq{-1, 1, 11}, vals11);
}

TEST(TRIQSMesh, ImtimeSizeL) {
  // imtime mesh with size 2
  auto vals2 = std::vector<double>(2);
  std::ranges::generate(vals2, value_gen{.a = 0.0, .delta = 2.0});
  check_size_l(triqs::mesh::imtime{2.0, triqs::mesh::Fermion, 2}, vals2);

  // imtime mesh with size 11
  auto vals11 = std::vector<double>(11);
  std::ranges::generate(vals11, value_gen{.a = 0.0, .delta = 0.1});
  check_size_l(triqs::mesh::imtime{1.0, triqs::mesh::Boson, 11}, vals11);
}

TEST(TRIQSMesh, RetimeEvaluate) {
  // linear function to interpolate
  auto fx = [](double x) { return 1.0 + -0.5 * x; };

  // retime mesh
  auto m = triqs::mesh::retime{-1.0, 1.0, 11};

  // function defined on the mesh points
  auto fi = [&](long i) { return fx(m.to_value(i)); };

  // check interpolation
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, -1.0), fx(-1.0));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, -0.2637), fx(-0.2637));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, 0.61727), fx(0.61727));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, 1.0), fx(1.0));
}

TEST(TRIQSMesh, RefreqEvaluate) {
  // linear function to interpolate
  auto fx = [](double x) { return 1.0 + -0.5 * x; };

  // refreq mesh
  auto m = triqs::mesh::refreq{-1.0, 1.0, 11};

  // function defined on the mesh points
  auto fi = [&](long i) { return fx(m.to_value(i)); };

  // check interpolation
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, -1.0), fx(-1.0));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, -0.2637), fx(-0.2637));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, 0.61727), fx(0.61727));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, 1.0), fx(1.0));
}

TEST(TRIQSMesh, ImtimeEvaluate) {
  // linear function to interpolate
  auto fx = [](double x) { return 1.0 + -0.5 * x; };

  // retime mesh
  auto m = triqs::mesh::imtime{1.0, triqs::mesh::Fermion, 11};

  // function defined on the mesh points
  auto fi = [&](long i) { return fx(m.to_value(i)); };

  // check interpolation
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, 0.0), fx(0.0));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, 0.2637), fx(0.2637));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, 0.61727), fx(0.61727));
  EXPECT_DOUBLE_EQ(triqs::mesh::evaluate(m, fi, 1.0), fx(1.0));
}

TEST(TRIQSMesh, RetimeHDF5) {
  // create mesh
  auto m = triqs::mesh::retime{-2.1234, 0.98765, 27};

  // write and read to HDF5
  auto m_r = rw_h5(m, "triqs_mesh_retime", "m");
  EXPECT_EQ(m, m_r);
}

TEST(TRIQSMesh, RefreqHDF5) {
  // create mesh
  auto m = triqs::mesh::refreq{-2.1234, 0.98765, 27};

  // write and read to HDF5
  auto m_r = rw_h5(m, "triqs_mesh_refreq", "m");
  EXPECT_EQ(m, m_r);
}

TEST(TRIQSMesh, ImtimeHDF5) {
  // create mesh
  auto m = triqs::mesh::imtime{2.12345, triqs::mesh::Boson, 27};

  // write and read to HDF5
  auto m_r = rw_h5(m, "triqs_mesh_imtime", "m");
  EXPECT_EQ(m, m_r);
}

MAKE_MAIN;
