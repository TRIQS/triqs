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

#include <triqs/mesh.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <iterator>
#include <ranges>

// Custom mesh for testing.
struct my_mesh {
  using mesh_point_t = long;
  long sz{10};
  [[nodiscard]] mesh_point_t operator[](long data_index) const { return data_index; }
  [[nodiscard]] long size() const { return sz; }
  [[nodiscard]] auto begin() const { return triqs::mesh::mesh_iterator<my_mesh>{.mesh_ptr = this, .data_index = 0}; }
  [[nodiscard]] auto end() const { return triqs::mesh::mesh_iterator<my_mesh>{.mesh_ptr = this, .data_index = size()}; }
  [[nodiscard]] auto mesh_hash() const { return sz; }
};

// Test random access concept.
static_assert(std::random_access_iterator<triqs::mesh::mesh_iterator<my_mesh>>);
static_assert(std::ranges::random_access_range<my_mesh>);
static_assert(std::ranges::random_access_range<triqs::mesh::brzone>);
static_assert(std::ranges::random_access_range<triqs::mesh::cyclat>);
static_assert(std::ranges::random_access_range<triqs::mesh::discrete>);
static_assert(std::ranges::random_access_range<triqs::mesh::dlr_imtime>);
static_assert(std::ranges::random_access_range<triqs::mesh::dlr_imfreq>);
static_assert(std::ranges::random_access_range<triqs::mesh::dlr>);
static_assert(std::ranges::random_access_range<triqs::mesh::imfreq>);
static_assert(std::ranges::random_access_range<triqs::mesh::imtime>);
static_assert(std::ranges::random_access_range<triqs::mesh::legendre>);
static_assert(std::ranges::random_access_range<triqs::mesh::refreq>);
static_assert(std::ranges::random_access_range<triqs::mesh::retime>);

TEST(TRIQS, MeshIteratorZeroSize) {
  my_mesh mesh{0};
  EXPECT_EQ(mesh.begin(), mesh.end());
  int i = 0;
  for ([[maybe_unused]] auto m : mesh) ++i;
  EXPECT_EQ(i, 0);
}

TEST(TRIQS, MeshIteratorComparisons) {
  auto check_comparisons = [](auto &&rg) {
    auto check_ordering = [](auto &&it, auto &&it2) {
      EXPECT_TRUE(it < it2);
      EXPECT_TRUE(it <= it2);
      EXPECT_TRUE(it2 > it);
      EXPECT_TRUE(it2 >= it);
    };
    auto it  = rg.begin();
    auto it2 = rg.end();
    EXPECT_TRUE(it == rg.begin());
    EXPECT_TRUE(it2 == rg.end());
    EXPECT_TRUE(it != it2);
    check_ordering(it, it2);
    for (; it != it2; ++it, --it2) check_ordering(it, it2);
    EXPECT_TRUE(it == it2);
    for (; it != rg.end(); it++);
    for (; it2 != rg.begin(); it2--);
    EXPECT_TRUE(it == rg.end());
    EXPECT_TRUE(it2 == rg.begin());
    EXPECT_TRUE(it != it2);
    check_ordering(it2, it);
  };
  check_comparisons(my_mesh(2));
  check_comparisons(my_mesh(6));
  check_comparisons(my_mesh(10));
}

TEST(TRIQS, MeshIteratorRandomAccessOperations) {
  auto mesh     = my_mesh(10);
  auto it_begin = mesh.begin();
  auto it_end   = mesh.end();
  auto it       = mesh.begin();
  for (int i = 0; i < mesh.size(); ++i, ++it) {
    EXPECT_EQ(it_begin[i], *it);
    EXPECT_EQ(it_begin + i, it);
    EXPECT_EQ(i + it_begin, it);
    EXPECT_EQ(it_end - (mesh.size() - i), it);
    EXPECT_EQ(it - it_begin, i);
    EXPECT_EQ(it_end - it, mesh.size() - i);

    auto it_tmp = it_begin;
    it_tmp += i;
    EXPECT_EQ(it_tmp, it);
    it_tmp = it_end;
    it_tmp -= mesh.size() - i;
    EXPECT_EQ(it_tmp, it);
  }
}

MAKE_MAIN;
