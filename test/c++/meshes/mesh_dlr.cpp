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
#include <triqs/mesh/dlr.hpp>
#include <triqs/mesh/dlr_imfreq.hpp>
#include <triqs/mesh/dlr_imtime.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>

#include <complex>
#include <string>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::dlr>);
static_assert(triqs::mesh::Mesh<triqs::mesh::dlr_imfreq>);
static_assert(triqs::mesh::Mesh<triqs::mesh::dlr_imtime>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::dlr>);
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::dlr_imfreq>);
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::dlr_imtime>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::dlr::mesh_point_t>);
static_assert(triqs::mesh::MeshPoint<triqs::mesh::dlr_imfreq::mesh_point_t>);
static_assert(triqs::mesh::MeshPoint<triqs::mesh::dlr_imtime::mesh_point_t>);

// Test DLR type meshes.
template <typename T> void test_mesh(std::string const &hdf5_str) {
  // default constructed mesh
  auto m1 = T{};
  EXPECT_EQ(m1.size(), 0);

  // another default constructed mesh (should be equal to the previous mesh)
  auto m2 = T{};
  EXPECT_EQ(m1, m2);

  // mesh with beta = 10, statistic = Fermion, w_max = 5 and eps = 1e-6 (should be different from the previous meshes)
  auto m3 = T{5, triqs::mesh::Fermion, 1, 1e-6};
  EXPECT_NE(m1, m3);

  // copy constructed mesh (should be equal to m3)
  auto m4 = m3;
  EXPECT_EQ(m3, m4);

  // print meshes
  std::cout << m1 << std::endl;
  std::cout << m3 << std::endl;

  // validity of indices
  EXPECT_FALSE(m1.is_index_valid(0));
  EXPECT_FALSE(m3.is_index_valid(-1));
  EXPECT_TRUE(m3.is_index_valid(0));
  EXPECT_TRUE(m3.is_index_valid(m3.size() - 1));

  // loop over all mesh points
  auto check_mp = [](auto mp1, auto mp2) {
    EXPECT_EQ(mp1.index(), mp2.index());
    EXPECT_EQ(mp1.data_index(), mp2.data_index());
    EXPECT_EQ(std::complex<double>(mp1.value()), std::complex<double>(mp2.value()));
    EXPECT_EQ(mp1.mesh_hash(), mp2.mesh_hash());
  };
  int i = 0;
  for (auto mp : m1) {
    EXPECT_EQ(mp.data_index(), i);
    EXPECT_EQ(mp.mesh_hash(), m1.mesh_hash());
    EXPECT_EQ(std::complex<double>(mp.value()), std::complex<double>(m1.to_value(i)));
    check_mp(mp, m1(i));
    check_mp(mp, m1[i]);
    EXPECT_EQ(m1.to_data_index(i), i);
    EXPECT_EQ(m1.to_index(i), i);
    ++i;
  }
  EXPECT_EQ(i, m1.size());

  // write and read to HDF5
  auto m3_r = rw_h5(m3, hdf5_str, "m3");
  EXPECT_EQ(m3, m3_r);
}

// Test DLR mesh.
TEST(TRIQSMesh, DLR) { test_mesh<triqs::mesh::dlr>("triqs_mesh_dlr"); }

// Test DLR imfreq mesh.
TEST(TRIQSMesh, DLRImfreq) { test_mesh<triqs::mesh::dlr_imfreq>("triqs_mesh_dlr_imfreq"); }

// Test DLR imtime mesh.
TEST(TRIQSMesh, DLRImtime) { test_mesh<triqs::mesh::dlr_imtime>("triqs_mesh_dlr_imtime"); }

// Test concstructing DLR/imtime/imfreq meshes from other DLR/imtime/imfreq meshes.
TEST(TRIQSMesh, DLRConversions) {
  auto m_dlr1        = triqs::mesh::dlr{10, triqs::mesh::Fermion, 1, 1e-6};
  auto m_dlr_imtime1 = triqs::mesh::dlr_imtime{10, triqs::mesh::Fermion, 1, 1e-6};
  auto m_dlr_imfreq1 = triqs::mesh::dlr_imfreq{10, triqs::mesh::Fermion, 1, 1e-6};

  // DLR -> imtime, imfreq
  auto m_dlr_imtime2 = triqs::mesh::dlr_imtime{m_dlr1};
  auto m_dlr_imfreq2 = triqs::mesh::dlr_imfreq{m_dlr1};
  EXPECT_EQ(m_dlr_imtime1, m_dlr_imtime2);
  EXPECT_EQ(m_dlr_imfreq1, m_dlr_imfreq2);

  // imtime -> DLR, imfreq
  auto m_dlr3        = triqs::mesh::dlr{m_dlr_imtime1};
  auto m_dlr_imfreq3 = triqs::mesh::dlr_imfreq{m_dlr_imtime1};
  EXPECT_EQ(m_dlr1, m_dlr3);
  EXPECT_EQ(m_dlr_imfreq1, m_dlr_imfreq3);

  // imfreq -> DLR, imtime
  auto m_dlr4        = triqs::mesh::dlr{m_dlr_imfreq1};
  auto m_dlr_imtime4 = triqs::mesh::dlr_imtime{m_dlr_imfreq1};
  EXPECT_EQ(m_dlr1, m_dlr4);
  EXPECT_EQ(m_dlr_imtime1, m_dlr_imtime4);
}

// The tau nodes are statistic-independent: fermionic and bosonic dlr_imtime meshes share the hash
// and the mesh points, but compare unequal.
TEST(TRIQSMesh, DLRImtimeStatisticBlindHash) {
  auto m_f = triqs::mesh::dlr_imtime{10, triqs::mesh::Fermion, 1, 1e-6};
  auto m_b = triqs::mesh::dlr_imtime{10, triqs::mesh::Boson, 1, 1e-6};

  EXPECT_EQ(m_f.mesh_hash(), m_b.mesh_hash());
  EXPECT_NE(m_f, m_b);

  EXPECT_EQ(m_f.size(), m_b.size());
  for (auto [mp_f, mp_b] : itertools::zip(m_f, m_b)) {
    EXPECT_EQ(mp_f.index(), mp_b.index());
    EXPECT_EQ(mp_f.value(), mp_b.value());
  }

  // The imaginary frequency grids genuinely differ between statistics
  EXPECT_NE(triqs::mesh::dlr_imfreq(10, triqs::mesh::Fermion, 1, 1e-6), triqs::mesh::dlr_imfreq(10, triqs::mesh::Boson, 1, 1e-6));
}

MAKE_MAIN;
