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
// Authors: GitHub Copilot

#include <triqs/lattice/bravais_lattice.hpp>
#include <triqs/mesh/concepts.hpp>
#include <triqs/mesh/prod.hpp>
#include <triqs/mesh/discrete.hpp>
#include <triqs/mesh/imfreq.hpp>
#include <triqs/mesh/cyclat.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <array>
#include <iostream>
#include <tuple>

using mesh_pdd_t = triqs::mesh::prod<triqs::mesh::discrete, triqs::mesh::discrete>;

// Mesh concept.
static_assert(triqs::mesh::Mesh<mesh_pdd_t>);

// MeshWithValues concept.
static_assert(!triqs::mesh::MeshWithValues<mesh_pdd_t>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<mesh_pdd_t::mesh_point_t>);

// Check product mesh points.
template <typename MP, typename IndexT, typename DataIndexT>
void check_mp_prod(MP const &mp, IndexT const &idx, DataIndexT const &didx, uint64_t mhash) {
  EXPECT_EQ(mp.index(), idx);
  EXPECT_EQ(mp.data_index(), didx);
  EXPECT_EQ(mp.mesh_hash(), mhash);
}

// Check comparison operators for meshes.
template <typename M> void check_eq_neq(M const &a, M const &eq, M const &neq) {
  EXPECT_TRUE(a == eq);
  EXPECT_FALSE(a != eq);
  EXPECT_FALSE(a == neq);
  EXPECT_TRUE(a != neq);
}

TEST(TRIQSMesh, ProdZeroSize) {
  using namespace triqs::mesh;

  // product with one zero-sized component yields zero-sized product
  auto const m = prod{discrete{0}, imfreq{10.0, Fermion, 1}};
  EXPECT_EQ(m.size(), 0);
  int cnt = 0;
  for ([[maybe_unused]] auto mp : m) ++cnt;
  EXPECT_EQ(cnt, 0);

  // invalid indices in any component make the product index invalid
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(0l, 0l)));

  // print
  std::cout << m << std::endl;

  // equality/inequality checks
  check_eq_neq(m, discrete{0} * imfreq{10.0, Fermion, 1}, discrete{1} * imfreq{10.0, Fermion, 1});
}

TEST(TRIQSMesh, ProdDiscrete1D) {
  using namespace triqs::mesh;

  // single discrete component (1d product)
  auto const m_d0 = discrete{5};
  auto const m  = prod{m_d0};

  EXPECT_EQ(m.size(), m_d0.size());
  EXPECT_EQ(m.size_of_components(), (std::array<long, 1>{m_d0.size()}));

  // components accessor
  auto const &comps = m.components();
  EXPECT_EQ(std::get<0>(comps), m_d0);

  // iterate and verify mapping and accessors
  long dlin = 0;
  for ([[maybe_unused]] auto mp : m) {
    auto const idx  = std::make_tuple(dlin);
    auto const didx = std::make_tuple(dlin);
    check_mp_prod(mp, idx, didx, m.mesh_hash());
    EXPECT_TRUE(m.is_index_valid(idx));
    EXPECT_EQ(m.to_data_index(idx), didx);
    EXPECT_EQ(m.to_index(didx), idx);
    check_mp_prod(m[didx], idx, didx, m.mesh_hash());
    check_mp_prod(m(idx), idx, didx, m.mesh_hash());
    ++dlin;
  }
  EXPECT_EQ(dlin, m.size());
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(5l)));

  // print
  std::cout << m << std::endl;

  // hdf5 round-trip
  auto const m_r = rw_h5(m, "triqs_mesh_prod", "m_d1");
  EXPECT_EQ(m, m_r);

  // equality/inequality checks
  check_eq_neq(m, prod{m_d0}, prod{discrete{4}});
}

// Removed ProdInequality test; equality/inequality is checked within each specific test via check_eq_neq

TEST(TRIQSMesh, ProdDiscrete2D) {
  using namespace triqs::mesh;

  // two discrete components (2d product)
  auto const m_d0 = discrete{3};
  auto const m_d1 = discrete{2};
  auto const m  = prod{m_d0, m_d1};

  EXPECT_EQ(m.size(), m_d0.size() * m_d1.size());
  EXPECT_EQ(m.size_of_components(), (std::array<long, 2>{m_d0.size(), m_d1.size()}));

  // components accessor
  auto const &comps = m.components();
  EXPECT_EQ(std::get<0>(comps), m_d0);
  EXPECT_EQ(std::get<1>(comps), m_d1);

  // iterate in c-order (last varies fastest)
  long dlin = 0;
  auto it   = m.begin();
  for (long i = 0; i < m_d0.size(); ++i) {
    for (long j = 0; j < m_d1.size(); ++j) {
      auto const idx  = std::make_tuple(i, j);
      auto const didx = std::make_tuple(i, j);
      auto const mp   = *it++;
      check_mp_prod(mp, idx, didx, m.mesh_hash());
      EXPECT_TRUE(m.is_index_valid(idx));
      EXPECT_EQ(m.to_data_index(idx), didx);
      EXPECT_EQ(m.to_index(didx), idx);
      check_mp_prod(m[didx], idx, didx, m.mesh_hash());
      check_mp_prod(m(idx), idx, didx, m.mesh_hash());
      ++dlin;
    }
  }
  EXPECT_EQ(dlin, m.size());
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(3l, 0l)));
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(0l, 2l)));

  // print
  std::cout << m << std::endl;

  // hdf5 round-trip
  auto const m_r = rw_h5(m, "triqs_mesh_prod", "m_d2");
  EXPECT_EQ(m, m_r);

  // equality/inequality checks
  check_eq_neq(m, m_d0 * m_d1, m_d0 * discrete{3});
}

TEST(TRIQSMesh, ProdDiscrete3D) {
  using namespace triqs::mesh;

  // three discrete components
  auto const m_d0 = discrete{2};
  auto const m_d1 = discrete{3};
  auto const m_d2 = discrete{4};
  auto const m  = prod{m_d0, m_d1, m_d2};

  EXPECT_EQ(m.size(), m_d0.size() * m_d1.size() * m_d2.size());
  EXPECT_EQ(m.size_of_components(), (std::array<long, 3>{m_d0.size(), m_d1.size(), m_d2.size()}));

  // components accessor
  auto const &comps = m.components();
  EXPECT_EQ(std::get<0>(comps), m_d0);
  EXPECT_EQ(std::get<1>(comps), m_d1);
  EXPECT_EQ(std::get<2>(comps), m_d2);

  // iterate in c-order (last varies fastest)
  long dlin = 0;
  auto it   = m.begin();
  for (long i = 0; i < m_d0.size(); ++i) {
    for (long j = 0; j < m_d1.size(); ++j) {
      for (long k = 0; k < m_d2.size(); ++k) {
        auto const idx  = std::make_tuple(i, j, k);
        auto const didx = std::make_tuple(i, j, k);
        auto const mp   = *it++;
        check_mp_prod(mp, idx, didx, m.mesh_hash());
        EXPECT_TRUE(m.is_index_valid(idx));
        EXPECT_EQ(m.to_data_index(idx), didx);
        EXPECT_EQ(m.to_index(didx), idx);
        check_mp_prod(m[didx], idx, didx, m.mesh_hash());
        check_mp_prod(m(idx), idx, didx, m.mesh_hash());
        ++dlin;
      }
    }
  }
  EXPECT_EQ(dlin, m.size());
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(2l, 0l, 0l)));
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(0l, 3l, 0l)));
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(0l, 0l, 4l)));

  // print
  std::cout << m << std::endl;

  // hdf5 round-trip
  auto const m_r = rw_h5(m, "triqs_mesh_prod", "m_d3");
  EXPECT_EQ(m, m_r);

  // equality/inequality checks
  check_eq_neq(m, (m_d0 * m_d1) * m_d2, (m_d0 * m_d1) * discrete{5});
}

TEST(TRIQSMesh, ProdDiscrete4D) {
  using namespace triqs::mesh;

  // four discrete components (use nontrivial sizes)
  auto const m_d0 = discrete{2};
  auto const m_d1 = discrete{1};
  auto const m_d2 = discrete{3};
  auto const m_d3 = discrete{4};
  auto const m  = prod{m_d0, m_d1, m_d2, m_d3};

  EXPECT_EQ(m.size(), m_d0.size() * m_d1.size() * m_d2.size() * m_d3.size());
  EXPECT_EQ(m.size_of_components(), (std::array<long, 4>{m_d0.size(), m_d1.size(), m_d2.size(), m_d3.size()}));

  // components accessor
  auto const &comps = m.components();
  EXPECT_EQ(std::get<0>(comps), m_d0);
  EXPECT_EQ(std::get<1>(comps), m_d1);
  EXPECT_EQ(std::get<2>(comps), m_d2);
  EXPECT_EQ(std::get<3>(comps), m_d3);

  // iterate in c-order (last varies fastest)
  long dlin = 0;
  auto it   = m.begin();
  for (long i = 0; i < m_d0.size(); ++i) {
    for (long j = 0; j < m_d1.size(); ++j) {
      for (long k = 0; k < m_d2.size(); ++k) {
        for (long l = 0; l < m_d3.size(); ++l) {
          auto const idx  = std::make_tuple(i, j, k, l);
          auto const didx = std::make_tuple(i, j, k, l);
          auto const mp   = *it++;
          check_mp_prod(mp, idx, didx, m.mesh_hash());
          EXPECT_TRUE(m.is_index_valid(idx));
          EXPECT_EQ(m.to_data_index(idx), didx);
          EXPECT_EQ(m.to_index(didx), idx);
          check_mp_prod(m[didx], idx, didx, m.mesh_hash());
          check_mp_prod(m(idx), idx, didx, m.mesh_hash());
          ++dlin;
        }
      }
    }
  }
  EXPECT_EQ(dlin, m.size());
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(2l, 0l, 0l, 0l)));
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(0l, 1l, 0l, 0l)));
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(0l, 0l, 3l, 0l)));
  EXPECT_FALSE(m.is_index_valid(std::make_tuple(0l, 0l, 0l, 4l)));

  // print
  std::cout << m << std::endl;

  // hdf5 round-trip
  auto const m_r = rw_h5(m, "triqs_mesh_prod", "m_d4");
  EXPECT_EQ(m, m_r);

  // equality/inequality checks
  check_eq_neq(m, ((m_d0 * m_d1) * m_d2) * m_d3, ((m_d0 * m_d1) * m_d2) * discrete{5});
}

TEST(TRIQSMesh, ProdCyclatImfreq) {
  using namespace triqs::mesh;
  using namespace triqs::lattice;

  // small cyclat and imfreq meshes
  auto const bl = bravais_lattice{nda::eye<double>(3)};
  auto const m_cl = cyclat{bl, std::array<long, 3>{2, 1, 1}}; // size 2
  auto const m_iw = imfreq{10.0, Fermion, 1};                 // size 2
  auto const m  = prod{m_cl, m_iw};                           // size 4

  EXPECT_EQ(m.size(), 4);
  EXPECT_EQ(m.size_of_components(), (std::array<long, 2>{m_cl.size(), m_iw.size()}));

  // iterate and compare to nested component iteration order
  auto it = m.begin();
  for (long dcl = 0; dcl < m_cl.size(); ++dcl) {
    auto const idx_cl = m_cl.to_index(dcl);
    for (long diw = 0; diw < m_iw.size(); ++diw) {
      auto const idx_iw = m_iw.to_index(diw);

      auto const expected_idx  = std::make_tuple(idx_cl, idx_iw);
      auto const expected_didx = std::make_tuple(dcl, diw);

      auto const mp = *it++;
      check_mp_prod(mp, expected_idx, expected_didx, m.mesh_hash());

      // mapping
      EXPECT_TRUE(m.is_index_valid(expected_idx));
      EXPECT_EQ(m.to_data_index(expected_idx), expected_didx);
      EXPECT_EQ(m.to_index(expected_didx), expected_idx);

      // accessors
      check_mp_prod(m[expected_didx], expected_idx, expected_didx, m.mesh_hash());
      check_mp_prod(m(expected_idx), expected_idx, expected_didx, m.mesh_hash());
    }
  }
  EXPECT_EQ(it, m.end());

  // print
  std::cout << m << std::endl;

  // hdf5 round-trip
  auto const m_r = rw_h5(m, "triqs_mesh_prod", "m_cl_iw");
  EXPECT_EQ(m, m_r);

  // equality/inequality checks
  check_eq_neq(m, m_cl * m_iw, m_cl * imfreq{10.0, Fermion, 2});
}

MAKE_MAIN;
