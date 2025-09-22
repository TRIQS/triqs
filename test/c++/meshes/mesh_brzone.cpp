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
#include <triqs/lattice/brillouin_zone.hpp>
#include <triqs/mesh/concepts.hpp>
#include <triqs/mesh/brzone.hpp>
#include <triqs/mesh/k_expr.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <nda/stdutil/array.hpp>

#include <array>
#include <iostream>
#include <numbers>
#include <vector>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::brzone>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::brzone>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::brzone::mesh_point_t>);

// BzMeshpoint concept.
static_assert(triqs::mesh::BzMeshPoint<triqs::mesh::brzone::mesh_point_t>);

// Check mesh point.
void check_mp(auto mp, auto const &idx, auto data_idx, auto const &val, auto hash) {
  EXPECT_EQ(mp.index(), idx);
  EXPECT_EQ(mp.data_index(), data_idx);
  EXPECT_ARRAY_NEAR(mp.value(), val);
  EXPECT_EQ(mp.mesh_hash(), hash);
}

// Test a brzone mesh.
void check_mesh(triqs::mesh::brzone const &m) {
  // scaled BZ basis vectors
  auto B = nda::make_regular(nda::transpose(m.bz().units()));
  B(nda::range::all, 0) /= m.dims()[0];
  B(nda::range::all, 1) /= m.dims()[1];
  B(nda::range::all, 2) /= m.dims()[2];
  EXPECT_ARRAY_NEAR(m.units(), nda::transpose(B));

  // loop over mesh points
  int d_idx   = 0;
  auto idx_rg = itertools::product_range(m.dims()[0], m.dims()[1], m.dims()[2]);
  for (auto idx_it = idx_rg.begin(); [[maybe_unused]] auto mp : m) {
    auto const idx_tup = *idx_it++;
    auto const idx     = std::array<long, 3>{std::get<0>(idx_tup), std::get<1>(idx_tup), std::get<2>(idx_tup)};
    auto const val     = B(nda::range::all, nda::range(m.bz().ndim())) * nda::basic_array_view{idx};
    check_mp(mp, idx, d_idx, val, m.mesh_hash());
    EXPECT_TRUE(m.is_index_valid(idx));

    // index <-> data index <-> value, subscript operator, function call operator
    auto cmp = triqs::mesh::closest_mesh_point_t{val};
    EXPECT_EQ(m.to_data_index(idx), d_idx);
    EXPECT_EQ(m.to_data_index(cmp), d_idx);
    EXPECT_EQ(m.to_index(d_idx), idx);
    EXPECT_EQ(m.to_index(cmp), idx);
    check_mp(m[d_idx], idx, d_idx, val, m.mesh_hash());
    check_mp(m(idx), idx, d_idx, val, m.mesh_hash());
    check_mp(m[cmp], idx, d_idx, val, m.mesh_hash());
    EXPECT_ARRAY_NEAR(m.to_value(idx), val);
    EXPECT_EQ(m.index_modulo(idx + 3l * m.dims()), idx);
    ++d_idx;
  }
  EXPECT_EQ(d_idx, m.size());
  EXPECT_FALSE(m.is_index_valid(std::array<long, 3>{m.dims()[0], m.dims()[1], m.dims()[2]}));

  // print
  std::cout << m << std::endl;
}

TEST(TRIQSMesh, Brzone1D) {
  using namespace triqs::mesh;
  using namespace triqs::lattice;

  auto bz = brillouin_zone{bravais_lattice{nda::matrix<double>{{1.0}}}};

  // 1D brzone with 1x1x1 k-points
  auto m = brzone{bz, {1, 1, 1}};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{1, 1, 1}));

  // 1D brzone with 5x1x1 k-points
  m = brzone{bz, 5};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{5, 1, 1}));
}

TEST(TRIQSMesh, Brzone2D) {
  using namespace triqs::mesh;
  using namespace triqs::lattice;

  auto bz = brillouin_zone{bravais_lattice{nda::matrix<double>{{1.2, 0.0}, {0.5, 0.7}}}};

  // 2D brzone with 1x1x1 k-points
  auto m = brzone{bz, {1, 1, 1}};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{1, 1, 1}));

  // 2D brzone with 2x2x1 k-points
  m = brzone{bz, 2};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{2, 2, 1}));
}

TEST(TRIQSMesh, Brzone3D) {
  using namespace triqs::mesh;
  using namespace triqs::lattice;

  auto bz = brillouin_zone{bravais_lattice{nda::matrix<double>{{0.5, 0.5, 0}, {-0.5, 0.5, 0}, {0, 0, 1}}}};

  // 3D brzone with 1x1x1 k-points
  auto m = brzone{bz, {1, 1, 1}};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{1, 1, 1}));

  // 2D brzone with 3x3x3 k-points
  m = brzone{bz, 3};
  check_mesh(m);
  EXPECT_EQ(m.dims(), (std::array<long, 3>{3, 3, 3}));
}

TEST(TRIQSMesh, BrzoneKExpr) {
  using namespace triqs::mesh;
  using namespace triqs::lattice;
  using kvec_t = triqs::mesh::brzone::value_t;
  using std::numbers::pi;

  // create brzone mesh with 2x2x1 k-points
  auto bz = brillouin_zone{bravais_lattice{nda::eye<double>(2) * 2 * pi}};
  auto m  = brzone{bz, 2};

  // get mesh points and k-vectors
  auto mps   = std::vector{m[0], m[1], m[2], m[3]};
  auto kidxs = std::vector{mps[0].index(), mps[1].index(), mps[2].index(), mps[3].index()};
  auto kvecs = std::vector{mps[0].value(), mps[1].value(), mps[2].value(), mps[3].value()};

  // check k-expressions
  EXPECT_EQ((mps[1] + mps[2]).index(), kidxs[3]);
  EXPECT_EQ((-mps[1]).index(), -kidxs[1]);
  EXPECT_EQ((mps[3] - mps[1]).index(), kidxs[2]);
  EXPECT_EQ((2 * mps[1]).index(), 2l * kidxs[1]);

  // check to_data_index with k-expressions
  EXPECT_EQ(m.to_data_index(mps[1] + mps[2]), 3);
  EXPECT_EQ(m.to_data_index(-mps[1]), 1);

  // check closest_index
  EXPECT_EQ(m.closest_index(kvecs[0]), kidxs[0]);
  EXPECT_EQ(m.closest_index(kvecs[1]), kidxs[1]);
  EXPECT_EQ(m.closest_index(mps[2].value()), kidxs[2]);
  EXPECT_EQ(m.closest_index(mps[3].value()), kidxs[3]);
  kvec_t const b1 = m.units()(0, nda::range(3)) * (0.5 - 1e-8);
  kvec_t const b2 = m.units()(1, nda::range(3)) * (0.5 - 1e-8);
  for (auto const &[kvec, kidx] : itertools::zip(kvecs, kidxs)) {
    for (auto x : {1.0, -1.0, 3.0, -3.0}) {
      EXPECT_EQ(m.closest_index(x * kvec + b1), kidx);
      EXPECT_EQ(m.closest_index(x * kvec + b2), kidx);
      EXPECT_EQ(m.closest_index(x * kvec - b1), kidx);
      EXPECT_EQ(m.closest_index(x * kvec - b2), kidx);
      EXPECT_EQ(m.closest_index(x * kvec + b1 + b2), kidx);
      EXPECT_EQ(m.closest_index(x * kvec + b1 - b2), kidx);
      EXPECT_EQ(m.closest_index(x * kvec - b1 + b2), kidx);
      EXPECT_EQ(m.closest_index(x * kvec - b1 - b2), kidx);
    }
  }
}

TEST(TRIQSMesh, BrzoneHDF5) {
  using std::numbers::pi;

  // create brzone mesh with 2x2x3 k-points
  auto bz = triqs::lattice::brillouin_zone{triqs::lattice::bravais_lattice{nda::eye<double>(3) * pi}};
  auto m  = triqs::mesh::brzone{bz, {2, 2, 3}};

  // write and read to HDF5
  auto m_r = rw_h5(m, "triqs_mesh_brzone", "m");
  EXPECT_EQ(m, m_r);
}

MAKE_MAIN;
