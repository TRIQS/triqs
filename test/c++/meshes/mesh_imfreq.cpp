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
#include <triqs/mesh/imfreq.hpp>
#include <triqs/mesh/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <fmt/ranges.h>
#include <fmt/std.h>

#include <algorithm>
#include <complex>
#include <iostream>
#include <numbers>
#include <vector>

// Mesh concept.
static_assert(triqs::mesh::Mesh<triqs::mesh::imfreq>);

// MeshWithValues concept.
static_assert(triqs::mesh::MeshWithValues<triqs::mesh::imfreq>);

// MeshPoint concept.
static_assert(triqs::mesh::MeshPoint<triqs::mesh::imfreq::mesh_point_t>);

// Check mesh point.
void check_mp(auto mp, auto idx, auto data_idx, auto val, auto hash) {
  EXPECT_EQ(mp.index(), idx);
  EXPECT_EQ(mp.data_index(), data_idx);
  EXPECT_COMPLEX_NEAR(mp.value(), val);
  EXPECT_EQ(mp.mesh_hash(), hash);
}

// Mesh value generator.
struct value_gen {
  double w_min{0.0};
  double delta{0.0};
  int i{0};
  value_gen(double beta, triqs::mesh::statistic_enum stat, int n_iw, triqs::mesh::imfreq::option opt) : delta(2.0 * M_PI / beta) {
    if (opt == triqs::mesh::imfreq::option::positive_frequencies_only) {
      w_min = (stat == triqs::mesh::Boson ? 0.0 : delta / 2);
    } else {
      w_min = (stat == triqs::mesh::Boson ? (-n_iw + 1) * delta : -n_iw * delta + delta / 2);
    }
  }
  auto operator()() { return std::complex<double>{0, w_min + i++ * delta}; }
};

// Mesh index generator.
struct index_gen {
  int n_min{0};
  int i{0};
  index_gen(triqs::mesh::statistic_enum stat, int n_iw, triqs::mesh::imfreq::option opt) {
    if (opt == triqs::mesh::imfreq::option::positive_frequencies_only) {
      n_min = 0;
    } else {
      n_min = (stat == triqs::mesh::Boson ? -n_iw + 1 : -n_iw);
    }
  }
  auto operator()() { return i++ + n_min; }
};

// Test a mesh of size 0.
void check_size_zero(triqs::mesh::imfreq const &m) {
  using namespace triqs::mesh;

  EXPECT_EQ(m.size(), 0);
  EXPECT_FALSE(m.is_index_valid(0));

  // empty loop
  int idx = 0;
  for ([[maybe_unused]] auto mp : m) ++idx;
  EXPECT_EQ(idx, 0);

  // comparison operators
  auto opt = (m.positive_only() ? imfreq::option::positive_frequencies_only : imfreq::option::all_frequencies);
  EXPECT_TRUE(imfreq(m.beta(), m.statistic(), m.n_iw(), opt) == m);
  EXPECT_FALSE(imfreq(m.beta(), m.statistic(), m.n_iw(), opt) != m);

  // print
  std::cout << m << std::endl;
}

// Test a mesh of size L.
void check_size_l(triqs::mesh::imfreq const &m, auto const &vals, auto const &idxs) {
  using triqs::mesh::closest_mesh_point_t;

  EXPECT_EQ(m.size(), vals.size());
  EXPECT_COMPLEX_NEAR(m.w_max(), vals.back());
  EXPECT_EQ(m.first_index(), idxs.front());
  EXPECT_EQ(m.last_index(), idxs.back());

  // loop over mesh points
  int d_idx = 0;
  for ([[maybe_unused]] auto mp : m) {
    check_mp(mp, idxs[d_idx], d_idx, vals[d_idx], m.mesh_hash());
    EXPECT_TRUE(m.is_index_valid(idxs[d_idx]));

    // index <-> data index <-> value, subscript operator, function call operator
    auto cmp = closest_mesh_point_t{triqs::mesh::matsubara_freq{idxs[d_idx], m.beta(), m.statistic()}};
    EXPECT_EQ(m.to_data_index(idxs[d_idx]), d_idx);
    EXPECT_EQ(m.to_data_index(mp), d_idx);
    EXPECT_EQ(m.to_data_index(cmp), d_idx);
    EXPECT_EQ(m.to_index(d_idx), idxs[d_idx]);
    EXPECT_EQ(m.to_index(cmp), idxs[d_idx]);
    EXPECT_COMPLEX_NEAR(m.to_value(idxs[d_idx]), vals[d_idx]);
    check_mp(m[d_idx], idxs[d_idx], d_idx, vals[d_idx], m.mesh_hash());
    check_mp(m(idxs[d_idx]), idxs[d_idx], d_idx, vals[d_idx], m.mesh_hash());
    check_mp(m[cmp], idxs[d_idx], d_idx, vals[d_idx], m.mesh_hash());
    ++d_idx;
  }
  EXPECT_EQ(d_idx, m.size());
  EXPECT_FALSE(m.is_index_valid(d_idx));

  // print
  std::cout << m << std::endl;
}

TEST(TRIQSMesh, ImfreqSizeZero) {
  using namespace triqs::mesh;

  // default constructed imfreq mesh with size 0
  check_size_zero(imfreq{});

  // imfreq meshes with size 0
  check_size_zero(imfreq{10, Fermion, 0});
  check_size_zero(imfreq{10, Boson, 0, imfreq::option::positive_frequencies_only});
}

TEST(TRIQSMesh, ImfreqSizeOne) {
  using std::numbers::pi;
  using namespace triqs::mesh;

  // imfreq mesh with 1 positive and negative Matsubara frequency and fermionic statistics
  auto vals = std::vector<std::complex<double>>(2);
  std::ranges::generate(vals, value_gen{10, Fermion, 1, imfreq::option::all_frequencies});
  auto idxs = std::vector<long>(2);
  std::ranges::generate(idxs, index_gen{Fermion, 1, imfreq::option::all_frequencies});
  check_size_l(imfreq{10, Fermion, 1}, vals, idxs);

  // imfreq mesh with 1 positive Matsubara frequency and fermionic statistics
  vals = std::vector<std::complex<double>>(1);
  std::ranges::generate(vals, value_gen{10, Fermion, 1, imfreq::option::positive_frequencies_only});
  idxs = std::vector<long>(1);
  std::ranges::generate(idxs, index_gen{Fermion, 1, imfreq::option::positive_frequencies_only});
  check_size_l(imfreq{10, Fermion, 1, imfreq::option::positive_frequencies_only}, vals, idxs);

  // imfreq mesh with 1 positive Matsubara frequency and bosonic statistics
  vals = std::vector<std::complex<double>>(1);
  std::ranges::generate(vals, value_gen{10, Boson, 1, imfreq::option::all_frequencies});
  idxs = std::vector<long>(1);
  std::ranges::generate(idxs, index_gen{Boson, 1, imfreq::option::all_frequencies});
  check_size_l(imfreq{10, Boson, 1}, vals, idxs);

  // imfreq mesh with 1 positive Matsubara frequency and bosonic statistics (positive frequencies only)
  vals = std::vector<std::complex<double>>(1);
  std::ranges::generate(vals, value_gen{10, Boson, 1, imfreq::option::positive_frequencies_only});
  idxs = std::vector<long>(1);
  std::ranges::generate(idxs, index_gen{Boson, 1, imfreq::option::positive_frequencies_only});
  check_size_l(imfreq{10, Boson, 1, imfreq::option::positive_frequencies_only}, vals, idxs);
}

TEST(TRIQSMesh, ImfreqSize1024) {
  using std::numbers::pi;
  using namespace triqs::mesh;

  // imfreq mesh with 1024 positive and negative Matsubara frequencies and fermionic statistics
  auto vals = std::vector<std::complex<double>>(2048);
  std::ranges::generate(vals, value_gen{10, Fermion, 1024, imfreq::option::all_frequencies});
  auto idxs = std::vector<long>(2048);
  std::ranges::generate(idxs, index_gen{Fermion, 1024, imfreq::option::all_frequencies});
  check_size_l(imfreq{10, Fermion, 1024}, vals, idxs);

  // imfreq mesh with 1024 positive Matsubara frequencies and fermionic statistics
  vals = std::vector<std::complex<double>>(1024);
  std::ranges::generate(vals, value_gen{10, Fermion, 1024, imfreq::option::positive_frequencies_only});
  idxs = std::vector<long>(1024);
  std::ranges::generate(idxs, index_gen{Fermion, 1024, imfreq::option::positive_frequencies_only});
  check_size_l(imfreq{10, Fermion, 1024, imfreq::option::positive_frequencies_only}, vals, idxs);

  // imfreq mesh with 1024 positive and 1023 negative Matsubara frequencies and bosonic statistics
  vals = std::vector<std::complex<double>>(2047);
  std::ranges::generate(vals, value_gen{10, Boson, 1024, imfreq::option::all_frequencies});
  idxs = std::vector<long>(2047);
  std::ranges::generate(idxs, index_gen{Boson, 1024, imfreq::option::all_frequencies});
  check_size_l(imfreq{10, Boson, 1024}, vals, idxs);

  // imfreq mesh with 1024 positive Matsubara frequencies and bosonic statistics
  vals = std::vector<std::complex<double>>(1024);
  std::ranges::generate(vals, value_gen{10, Boson, 1024, imfreq::option::positive_frequencies_only});
  idxs = std::vector<long>(1024);
  std::ranges::generate(idxs, index_gen{Boson, 1024, imfreq::option::positive_frequencies_only});
  check_size_l(imfreq{10, Boson, 1024, imfreq::option::positive_frequencies_only}, vals, idxs);
}

TEST(TRIQSMesh, ImfreqWithEnergyWindow) {
  using namespace triqs::mesh;

  // imfreq mesh with energy window and fermionic statistics
  auto vals = std::vector<std::complex<double>>(2048);
  std::ranges::generate(vals, value_gen{10, Fermion, 1024, imfreq::option::all_frequencies});
  auto idxs = std::vector<long>(2048);
  std::ranges::generate(idxs, index_gen{Fermion, 1024, imfreq::option::all_frequencies});
  check_size_l(imfreq{10, Fermion, energy_t{vals.back().imag()}, imfreq::option::all_frequencies}, vals, idxs);

  // imfreq mesh with energy window and bosonic statistics
  vals = std::vector<std::complex<double>>(2047);
  std::ranges::generate(vals, value_gen{10, Boson, 1024, imfreq::option::all_frequencies});
  idxs = std::vector<long>(2047);
  std::ranges::generate(idxs, index_gen{Boson, 1024, imfreq::option::all_frequencies});
  check_size_l(imfreq{10, Boson, energy_t{vals.back().imag()}, imfreq::option::all_frequencies}, vals, idxs);
}

TEST(TRIQSMesh, ImfreqHDF5) {
  // create mesh with positive frequencies only
  auto m = triqs::mesh::imfreq{1.2345, triqs::mesh::Boson, 27, triqs::mesh::imfreq::option::positive_frequencies_only};

  // write and read to HDF5
  auto m_r = rw_h5(m, "triqs_mesh_imfreq", "m");
  EXPECT_EQ(m, m_r);

  // create mesh with all frequencies
  auto m2 = triqs::mesh::imfreq{1.2345, triqs::mesh::Fermion, 27};

  // write and read to HDF5
  auto m2_r = rw_h5(m2, "triqs_mesh_imfreq", "m2");
  EXPECT_EQ(m2, m2_r);
}

MAKE_MAIN;
