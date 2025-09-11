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

#include <triqs/lattice/bravais_lattice.hpp>
#include <triqs/mesh/concepts.hpp>
#include <triqs/mesh/cyclat.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <nda/stdutil/array.hpp>

#include <array>
#include <iostream>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::cyclat>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::cyclat>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::cyclat::mesh_point_t>);

// Check mesh point.
void check_mp(auto mp, auto const &idx, auto data_idx, auto hash) {
  EXPECT_EQ(mp.index(), idx);
  EXPECT_EQ(mp.data_index(), data_idx);
  EXPECT_EQ(mp.mesh_hash(), hash);
}

// Test a cyclat mesh.
void check_mesh(triqs::mesh::cyclat const &m) {
  // loop over mesh points
  int d_idx   = 0;
  auto idx_rg = itertools::product_range(m.dims()[0], m.dims()[1], m.dims()[2]);
  for (auto idx_it = idx_rg.begin(); [[maybe_unused]] auto mp : m) {
    auto const idx_tup = *idx_it++;
    auto const idx     = std::array<long, 3>{std::get<0>(idx_tup), std::get<1>(idx_tup), std::get<2>(idx_tup)};
    check_mp(mp, idx, d_idx, m.mesh_hash());
    EXPECT_TRUE(m.is_index_valid(idx));

    // index <-> data index <-> value, subscript operator, function call operator
    auto const val = m.to_value(idx);
    auto const cmp = triqs::mesh::closest_mesh_point_t{val};
    EXPECT_EQ(m.to_data_index(idx), d_idx);
    EXPECT_EQ(m.to_data_index(cmp), d_idx);
    EXPECT_EQ(m.to_index(d_idx), idx);
    EXPECT_EQ(m.to_index(cmp), idx);
    check_mp(m[d_idx], idx, d_idx, m.mesh_hash());
    check_mp(m(idx), idx, d_idx, m.mesh_hash());
    check_mp(m[cmp], idx, d_idx, m.mesh_hash());
    EXPECT_EQ(m.index_modulo(idx + 3l * m.dims()), idx);
    ++d_idx;
  }
  EXPECT_EQ(d_idx, m.size());
  EXPECT_FALSE(m.is_index_valid(std::array<long, 3>{m.dims()[0], m.dims()[1], m.dims()[2]}));

  // print
  std::cout << m << std::endl;
}

TEST(TRIQSMesh, Cyclat1D) {
  using namespace triqs::mesh;
  using namespace triqs::lattice;

  // 1D cyclat with 1x1x1 supercell
  auto m = cyclat{bravais_lattice{nda::matrix<double>{{2}}}, {1, 1, 1}};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{1, 1, 1}));

  // 1D cyclat with 5x1x1 supercell
  m = cyclat{bravais_lattice{nda::matrix<double>{{1.3}}}, 5};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{5, 1, 1}));
}

TEST(TRIQSMesh, Cyclat2D) {
  using namespace triqs::mesh;
  using namespace triqs::lattice;

  // 2D cyclat with 1x1x1 supercell
  auto m = cyclat{bravais_lattice{nda::matrix<double>{{1.2, 0.0}, {0.5, 0.7}}}, {1, 1, 1}};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{1, 1, 1}));

  // 1D cyclat with 2x2x1 supercell
  m = cyclat{bravais_lattice{nda::matrix<double>{{0.5, 0.5}, {-0.5, 0.5}}}, 2};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{2, 2, 1}));
}

TEST(TRIQSMesh, Cyclat3D) {
  using namespace triqs::mesh;
  using namespace triqs::lattice;

  // 3D cyclat with 1x1x1 supercell
  auto m = cyclat{};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{1, 1, 1}));

  // 3D cyclat with 5x5x5 supercell
  m = cyclat{bravais_lattice{nda::matrix<double>{{0.5, 0.5, 0}, {-0.5, 0.5, 0}, {0, 0, 1}}}, 5};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{5, 5, 5}));

  // 3D cyclat with 2x3x4 supercell
  m = cyclat{bravais_lattice{nda::matrix<double>{{0.5, 0.5, 0}, {-0.5, 0.5, 0}, {0, 0, 1}}}, {2, 3, 4}};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{2, 3, 4}));
}

TEST(TRIQSMesh, CyclatHDF5) {
  // create cyclat mesh with 2x2x3 supercell
  auto m = triqs::mesh::cyclat{triqs::lattice::bravais_lattice{nda::eye<double>(3) * 0.5}, {2, 2, 3}};

  // write and read to HDF5
  auto m_r = rw_h5(m, "triqs_mesh_cyclat", "m");
  EXPECT_EQ(m, m_r);
}

MAKE_MAIN;
