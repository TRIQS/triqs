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
#include <triqs/mesh/discrete.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::discrete>);

// MeshWithValues concept.
static_assert(!triqs::mesh::MeshWithValues<triqs::mesh::discrete>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::discrete::mesh_point_t>);

// Check mesh point for equality.
void check_mp(auto mp1, auto i, auto m) {
  EXPECT_EQ(mp1.index(), i);
  EXPECT_EQ(mp1.data_index(), i);
  EXPECT_EQ(mp1.mesh_hash(), m.mesh_hash());
}

TEST(TRIQSMesh, DiscreteBasics) {
  // default constructed mesh
  auto m1 = triqs::mesh::discrete{};
  EXPECT_EQ(m1.size(), 0);

  // another default constructed mesh (should be equal to the previous mesh)
  auto m2 = triqs::mesh::discrete{};
  EXPECT_EQ(m1, m2);
  EXPECT_EQ(m1.mesh_hash(), m2.mesh_hash());

  // mesh of size 0 (should be equal to default constructed mesh)
  auto m3 = triqs::mesh::discrete{0};
  EXPECT_EQ(m1, m3);
  EXPECT_EQ(m1.mesh_hash(), m3.mesh_hash());

  // mesh of size 10 (should be different from the previous meshes)
  auto m4 = triqs::mesh::discrete{10};
  EXPECT_EQ(m4.size(), 10);
  EXPECT_NE(m1, m4);
  EXPECT_NE(m1.mesh_hash(), m4.mesh_hash());

  // print meshes
  std::cout << m1 << std::endl;
  std::cout << m4 << std::endl;

  // validity of indices
  EXPECT_FALSE(m1.is_index_valid(0));
  EXPECT_FALSE(m4.is_index_valid(-1));
  for (int i = 0; i < 10; ++i) EXPECT_TRUE(m4.is_index_valid(i));
  EXPECT_FALSE(m4.is_index_valid(10));
}

TEST(TRIQSMesh, DiscreteLoopOverMeshPoints) {
  // create mesh of size 0
  auto m1 = triqs::mesh::discrete{0};
  EXPECT_EQ(m1.size(), 0);

  // loop over all mesh points
  int i1 = 0;
  for ([[maybe_unused]] auto mp : m1) ++i1;
  EXPECT_EQ(i1, m1.size());

  // create mesh of size 10
  auto m2 = triqs::mesh::discrete{10};
  EXPECT_EQ(m2.size(), 10);

  // loop over all mesh points
  int i2 = 0;
  for (auto mp : m2) {
    check_mp(mp, i2, m2);
    check_mp(m2(i2), i2, m2);
    check_mp(m2[i2], i2, m2);
    EXPECT_EQ(m2.to_data_index(i2), i2);
    EXPECT_EQ(m2.to_index(i2), i2);
    ++i2;
  }
  EXPECT_EQ(i2, m2.size());
}

TEST(TRIQSMesh, DiscreteHDF5) {
  // create mesh
  auto m1 = triqs::mesh::discrete(10);

  // write and read to HDF5
  auto m1_r = rw_h5(m1, "triqs_mesh_discrete", "m1");
  EXPECT_EQ(m1, m1_r);
}

MAKE_MAIN;
