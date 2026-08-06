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
#include <triqs/mesh/chebyshev.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::chebyshev>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::chebyshev>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::chebyshev::mesh_point_t>);

// Check mesh point for equality.
void check_mp(auto mp1, auto i, auto m) {
  EXPECT_EQ(mp1.index(), i);
  EXPECT_EQ(mp1.data_index(), i);
  EXPECT_EQ(mp1.mesh_hash(), m.mesh_hash());
  EXPECT_DOUBLE_EQ(mp1.value(), m.to_value(i));
}

TEST(TRIQSMesh, ChebyshevBasics) {
  // default constructed mesh
  auto m1 = triqs::mesh::chebyshev{};
  EXPECT_EQ(m1.size(), 0);

  // mesh of size 10
  auto m3 = triqs::mesh::chebyshev{5, triqs::mesh::Fermion, 10};
  EXPECT_EQ(m3.size(), 10);
  EXPECT_EQ(m3.beta(), 5);
  EXPECT_EQ(m3.statistic(), triqs::mesh::Fermion);

  // print mesh
  std::cout << m3 << std::endl;

  // validity of indices
  EXPECT_FALSE(m1.is_index_valid(0));
  EXPECT_FALSE(m3.is_index_valid(-1));
  for (int i = 0; i < 10; ++i) EXPECT_TRUE(m3.is_index_valid(i));
  EXPECT_FALSE(m3.is_index_valid(10));
}

TEST(TRIQSMesh, ChebyshevPointsInInterval) {
  double const beta = 10.0;
  int const N       = 20;
  auto m            = triqs::mesh::chebyshev{beta, triqs::mesh::Fermion, N};

  // All points should be in [0, beta]
  for (auto const &mp : m) {
    EXPECT_GE(mp.value(), 0.0);
    EXPECT_LE(mp.value(), beta);
  }

  // Points should be in ascending order
  for (int i = 1; i < N; ++i) { EXPECT_GT(m[i].value(), m[i - 1].value()); }

  // First point should be close to 0, last point close to beta
  EXPECT_LT(m[0].value(), beta * 0.1);
  EXPECT_GT(m[N - 1].value(), beta * 0.9);
}

TEST(TRIQSMesh, ChebyshevLoopOverMeshPoints) {
  // create mesh of size 10
  auto m = triqs::mesh::chebyshev{10, triqs::mesh::Fermion, 10};
  EXPECT_EQ(m.size(), 10);

  // loop over all mesh points
  int i = 0;
  for (auto mp : m) {
    check_mp(mp, i, m);
    check_mp(m(i), i, m);
    check_mp(m[i], i, m);
    EXPECT_EQ(m.to_data_index(i), i);
    EXPECT_EQ(m.to_index(i), i);
    ++i;
  }
  EXPECT_EQ(i, m.size());
}

TEST(TRIQSMesh, ChebyshevMeshPointArithmetic) {
  auto m = triqs::mesh::chebyshev{10, triqs::mesh::Fermion, 5};

  auto mp = m[2]; // Get a mesh point in the middle

  // Test arithmetic with scalars
  EXPECT_DOUBLE_EQ(mp + 1.0, mp.value() + 1.0);
  EXPECT_DOUBLE_EQ(1.0 + mp, 1.0 + mp.value());
  EXPECT_DOUBLE_EQ(mp - 1.0, mp.value() - 1.0);
  EXPECT_DOUBLE_EQ(1.0 - mp, 1.0 - mp.value());
  EXPECT_DOUBLE_EQ(mp * 2.0, mp.value() * 2.0);
  EXPECT_DOUBLE_EQ(2.0 * mp, 2.0 * mp.value());
  EXPECT_DOUBLE_EQ(mp / 2.0, mp.value() / 2.0);
  EXPECT_DOUBLE_EQ(1.0 / mp, 1.0 / mp.value());
}

TEST(TRIQSMesh, ChebyshevHDF5) {
  // create mesh
  auto m1 = triqs::mesh::chebyshev{10, triqs::mesh::Boson, 15};

  // write and read to HDF5
  auto m1_r = rw_h5(m1, "triqs_mesh_chebyshev", "m1");
  EXPECT_EQ(m1, m1_r);

  // Check that points are preserved
  for (int i = 0; i < m1.size(); ++i) { EXPECT_DOUBLE_EQ(m1[i].value(), m1_r[i].value()); }
}

TEST(TRIQSMesh, ChebyshevEquality) {
  auto m1 = triqs::mesh::chebyshev{10, triqs::mesh::Fermion, 20};
  auto m2 = triqs::mesh::chebyshev{10, triqs::mesh::Fermion, 20};
  auto m3 = triqs::mesh::chebyshev{10, triqs::mesh::Fermion, 15};
  auto m4 = triqs::mesh::chebyshev{5, triqs::mesh::Fermion, 20};
  auto m5 = triqs::mesh::chebyshev{10, triqs::mesh::Boson, 20};

  EXPECT_EQ(m1, m2);
  EXPECT_NE(m1, m3); // Different N
  EXPECT_NE(m1, m4); // Different beta
  EXPECT_NE(m1, m5); // Different statistics
}

MAKE_MAIN;
