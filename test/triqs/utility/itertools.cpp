/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017 by O. Parcollet, N. Wentzell
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
// Only in C++17 mode
#include <triqs/test_tools/arrays.hpp>
#if __cplusplus > 201402L

#include <array>
#include <triqs/utility/itertools.hpp>
#include <vector>

using namespace triqs::utility;

// An uncopyable int
struct _int {
  int i;
  _int(int u) : i(u){};
  _int(_int const&) = delete;
  _int(_int&&)      = default;

  operator int() const { return i; }
  friend std::ostream& operator<<(std::ostream& out, _int const& x) { return out << x.i; }
};

TEST(Itertools, Zip) {

  std::array<_int, 6> V1{6, 5, 4, 3, 2, 1};
  std::vector<int> V2{1, 2, 3, 4, 5, 6};

  // Zip both vectors and sum
  for (auto[x, y] : zip(V1, V2)) {
    EXPECT_TRUE(7 - y == x);
  }

  // Change the values of the second vector to the first
  for (auto[x, y] : zip(V1, std::ref(V2))) {
    y = x.i;
  }
  EXPECT_TRUE(std::equal(V2.begin(), V2.end(), V1.begin()));
}

TEST(Itertools, Enumerate) {

  std::vector<int> V{6, 5, 4, 3, 2, 1};

  // Enumerate the elements of the vector
  for (auto[j, x] : enumerate(V)) {
    EXPECT_TRUE(j + x == 6);
    EXPECT_TRUE(x == V[j]);
  }

  // Enumerate and change values
  std::vector<int> V_compare{0, 1, 2, 3, 4, 5};
  for (auto[j, x] : enumerate(std::ref(V))) {
    x = j;
  }
  EXPECT_TRUE(std::equal(V.begin(), V.end(), V_compare.begin()));

  std::array<_int, 6> W{6, 5, 4, 3, 2, 1};

  // Enumerate the elements of the array
  for (auto[j, x] : enumerate(W)) {
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

  for (auto[x, y] : prod(V1, V2)) {
    std::cout << "[" << x << "," << y << "]\n";
  }
}

TEST(Itertools, Multi) {

  std::vector<int> V{1, 2, 3, 4, 5, 6};

  // Build enumerate from transform and compare
  auto l = [n = 0](auto x) mutable { return std::tuple<int, decltype(x)>{n++, x}; };

  for (auto[x1, x2] : zip(transform(V, l), enumerate(V))) {
    EXPECT_TRUE(x1 == x2);
  }

  // Chain enumerate and transform
  for (auto[i, x] : enumerate(transform(V, l))) {
    std::cout << i << " " << x << "\n";
  }
}
#endif

MAKE_MAIN;



