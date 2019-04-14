#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/arrays.hpp>
#include <cmath>
#include <limits>
#include <triqs/arrays.hpp>
#include <triqs/arrays/asserts.hpp>

using namespace triqs::arrays;

// ==============================================================

// a little non copyable object
struct A {
  int i        = 2;
  A() = default;
  A(A const &) = delete;
  A(A &&)      = default;
  A &operator=(A const &) = delete;
  A &operator=(A &&) = default;

  A(int j) : i(j) { std::cerr << " constructing A : " << i << "\n"; }
};

//---------------------

TEST(NDA, NonDefaultConstructible) {

  array<A, 2> a(make_shape(2, 2), [](int i, int j) { return i + 10 * j; });
  // array<A,2> a( {2,2}, [](int i, int j) { return i+ 10*j;});
  array<A, 1> a1(make_shape(2), [](int i) { return i; });

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) { EXPECT_EQ(a(i, j).i, i + 10 * j); }

  // copy fails to compile
  //auto b= a;

  // view is ok
  auto b = a();

  //a.resize(3,3); // does not compile
}

// ==============================================================

TEST(NDA, array_of_non_copyable) {
  std::vector<array<A, 1>> a(2);
  a.emplace_back(2);
}


// ==============================================================

struct S {
  double x = 0, y = 0;
  int i = 0;
  S()   = default;
  S(double x, double y, int k) : x(x), y(y), i(k) {}
};

TEST(NDA, non_numeric1) {

  array<S, 2> A(2, 2);

  S s0{1.0, 2.0, 3};
  int p = 0;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j, ++p) A(i, j) = S{double(i), double(j), p};

  A() = s0;
  p   = 0;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j, ++p) EXPECT_EQ(A(i, j).i, 3);
}

// ---------------------------------------

TEST(NDA, array_of_array) {

  array<array<double, 1>, 2> a(2, 2);
  array<double, 1> a0{1,2,3};

  a() = a0;

  EXPECT_EQ(a(1,0), a0);
}

// ---------------------------------------

TEST(NDA, matrix_of_function) {

  matrix<std::function<double(double)>> F(2, 2);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) {
      auto s  = i + j;
      F(i, j) = [s](int i) { return i + s; };
    }

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) EXPECT_EQ(F(i, j)(0), i + j);
}

// ==============================================================




MAKE_MAIN
