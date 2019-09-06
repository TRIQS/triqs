/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017-2018, N. Wentzell, O. Parcollet
 * Copyright (C) 2019 The Simons Foundation
 *    author : N. Wentzell
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include <gtest/gtest.h>
#include <itertools/itertools.hpp>

#include <array>
#include <vector>
#include <numeric>

using namespace itertools;

// An uncopyable int
struct _int {
  int i;
  _int(int u) : i(u){};
  _int(_int const &) = delete;
  _int(_int &&)      = default;

  operator int() const { return i; }
  friend std::ostream &operator<<(std::ostream &out, _int const &x) { return out << x.i; }
};

TEST(Itertools, Zip) {

  std::array<_int, 6> V1{6, 5, 4, 3, 2, 1};
  std::vector<int> V2{1, 2, 3, 4, 5, 6};

  // Zip both vectors and sum
  for (auto [x, y] : zip(V1, V2)) { EXPECT_TRUE(7 - y == x); }

  // Change the values of the second vector to the first
  for (auto [x, y] : zip(V1, V2)) { y = x.i; }
  EXPECT_TRUE(std::equal(V2.begin(), V2.end(), V1.begin()));
}

TEST(Itertools, Enumerate) {

  std::vector<int> V{6, 5, 4, 3, 2, 1};

  // Enumerate the elements of the vector
  for (auto [j, x] : enumerate(V)) {
    EXPECT_TRUE(j + x == 6);
    EXPECT_TRUE(x == V[j]);
  }

  // Enumerate and change values
  std::vector<int> V_compare{0, 1, 2, 3, 4, 5};
  for (auto [j, x] : enumerate(V)) { x = j; }
  EXPECT_TRUE(std::equal(V.begin(), V.end(), V_compare.begin()));

  std::array<_int, 6> W{6, 5, 4, 3, 2, 1};

  // Enumerate the elements of the array
  for (auto [j, x] : enumerate(W)) {
    EXPECT_TRUE(6 - j == x);
    EXPECT_TRUE(W[j] == x);
  }
}

TEST(Itertools, Transform) {

  std::vector<int> V{1, 2, 3, 4, 5, 6};

  // Square the elements of the vector
  auto l = [](int i) { return i * i; };

  int i = 0;
  for (auto x : transform(V, l)) {
    ++i;
    EXPECT_TRUE(i * i == x);
  }

  std::array<_int, 6> W{1, 2, 3, 4, 5, 6};

  // Square the elements of the array
  i = 0;
  for (auto x : transform(W, l)) {
    ++i;
    EXPECT_TRUE(i * i == x);
  }
}

TEST(Itertools, Product) {

  std::vector<int> V1{0, 1, 2, 3, 4};
  std::array<_int, 5> V2{0, 1, 2, 3, 4};
  for (auto [x, y] : product(V1, V2)) { std::cout << "[" << x << "," << y << "]\n"; }

  // Check that we can alter the values
  std::vector<int> V3{1, 2, 3, 4};
  std::vector<int> V4{1, 1, 1, 1};
  for (auto [x, y] : product(V3, V4)) { y *= x; }
  EXPECT_EQ(V4, std::vector<int>(4, 1 * 2 * 3 * 4));
}

TEST(Itertools, Slice) {

  for (long N : range(1, 6)) {
    for (auto start_idx : range(N)) {
      for (auto M : range(1, 6)) {
        auto sliced  = slice(range(N), start_idx, M);
        long sum     = std::accumulate(sliced.cbegin(), sliced.cend(), 0);
        long end_idx = std::max(std::min(M, N), start_idx);
        EXPECT_EQ(sum, end_idx * (end_idx - 1) / 2 - start_idx * (start_idx - 1) / 2);
      }
    }
  }
}

TEST(Itertools, Make_Product) {

  constexpr int N = 4;

  std::array<range, N> range_arr{range(1), range(2), range(3), range(4)};

  int count = 0;
  for (auto [i, j, k, l] : make_product(range_arr)) ++count;

  EXPECT_EQ(count, 1 * 2 * 3 * 4);
}

TEST(Itertools, Multi) {

  std::vector<int> V{1, 2, 3, 4, 5, 6};

  // Build enumerate from transform and compare
  auto l = [n = 0](auto x) mutable { return std::make_tuple(n++, x); };

  for (auto [x1, x2] : zip(transform(V, l), enumerate(V))) { EXPECT_TRUE(x1 == x2); }

  // Chain enumerate and transform
  for (auto [i, x] : enumerate(transform(V, l))) { std::cout << i << "  [" << std::get<0>(x) << ", " << std::get<1>(x) << "]\n"; }
}

TEST(Itertools, Product_Range) {

  long res = 0;
  for (auto [i, j, k] : product_range(5, 5, 5)) res += i * j * k;
  EXPECT_EQ(res, 1000);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
