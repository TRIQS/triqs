#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/arrays.hpp>
#include <cmath>
#include <limits>
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/asserts.hpp>
#include <vector>
#include <map>
#include <algorithm>

template <typename T> using vector_t = triqs::arrays::vector<T>;

// A few examples paying with STL containers and algorithms

// ==============================================================

// We can put arrays in simple containers (there are regular types)
TEST(STL, Containers) {

  array<long, 2> A(2, 3);
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) A(i, j) = 10 * i + j;

  std::vector<array<long, 2>> VV;
  VV.push_back(A);

  std::map<std::string, array<long, 2>> MAP;
  MAP["1"] = A;
}

// ==============================================================

// STL algorithms works on nda::vector
TEST(STL, Vector) {

  std::vector<int> V(10);
  for (unsigned int i = 0; i < 10; ++i) V[i] = 10 + i;

  // Trying to put a vector in an array

  array<int, 1> B(V.size()), C(V.size());
  // copy to B. Iterators on array are STL compliant so STL algorithms work.
  std::copy(V.begin(), V.end(), B.begin());

  // change B
  for (int i = 0; i < 10; ++i) B(i) *= 2;

  // come back !
  std::copy(B.begin(), B.end(), V.begin());

  for (unsigned int i = 0; i < 10; ++i) EXPECT_EQ(V[i], 2 * (10 + i));
}

// ==============================================================

TEST(STL, Algo1) {

  array<int, 1> B{1, 34, 2, 6, 23, 189, 8, 4};

  auto te = [](int x) { return (x < 25); };

  EXPECT_EQ(std::count_if(B.begin(), B.end(), te), 6);
  EXPECT_EQ(*std::max_element(B.begin(), B.end()), 189);
  EXPECT_EQ(std::max_element(B.begin(), B.end()).indices()[0], 5);

  std::replace_if(B.begin(), B.end(), te, 0);

  EXPECT_EQ(B, (array<int, 1>{0, 34, 0, 0, 0, 189, 0, 0}));
}

// ==============================================================

// NB Old bug, no issue
TEST(STL, Bugxxx) {

  {
    vector_t<double> a = {1, 3, 2}, b = {2, 3, 1};
    EXPECT_TRUE(std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()));
    EXPECT_FALSE(std::lexicographical_compare(b.begin(), b.end(), a.begin(), a.end()));
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
  }
  {
    vector_t<double> a = {1, 3, 2}, b = {1, 2, 10};
    EXPECT_FALSE(std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()));
    EXPECT_TRUE(std::lexicographical_compare(b.begin(), b.end(), a.begin(), a.end()));
    EXPECT_TRUE(b < a);
    EXPECT_FALSE(a < b);
  }
  {
    vector_t<int> a = {1, 3, 2}, b = a;
    EXPECT_FALSE(a < b);
  }
}

// ==============================================================

MAKE_MAIN
