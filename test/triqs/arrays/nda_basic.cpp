#include "./nda_test_common.hpp"

static_assert(!std::is_pod<triqs::arrays::array<long, 2>>::value, "POD pb");
static_assert(is_scalar_for<int, matrix<std::complex<double>>>::type::value == 1, "oops");

// ==============================================================

TEST(NDA, Create1) {
  array<long, 2> A(3, 3);
  EXPECT_EQ(A.shape(), (myshape_t<2>{3, 3}));
}

// -------------------------------------

TEST(NDA, Resize) {

  array<long, 2> A;
  A.resize(make_shape(3, 3));
  EXPECT_EQ(A.shape(), (myshape_t<2>{3, 3}));

  matrix<double> M;
  M.resize(3, 3);

  EXPECT_EQ(M.shape(), (myshape_t<2>{3, 3}));

  vector<double> V;
  V.resize(10);

  EXPECT_EQ(V.shape(), (myshape_t<1>{10}));
}

// ==============================================================

TEST(NDA, InitList) {

  // 1d
  array<double, 1> A = {1, 2, 3, 4};

  EXPECT_EQ(A.shape(), (myshape_t<1>{4}));

  for (int i = 0; i < 4; ++i) EXPECT_EQ(A(i), i + 1);

  // 2d
  array<double, 2> B = {{1, 2}, {3, 4}, {5, 6}};

  EXPECT_EQ(B.shape(), (myshape_t<2>{3, 2}));
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 2; ++j) EXPECT_EQ(B(i, j), j + 2 * i + 1);

  // matrix
  matrix<double> M = {{1, 2}, {3, 4}, {5, 6}};
  EXPECT_EQ(M.shape(), (myshape_t<2>{3, 2}));

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 2; ++j) EXPECT_EQ(M(i, j), j + 2 * i + 1);
}

// ==============================================================

TEST(NDA, MoveConstructor) {
  array<double, 1> A(3);
  A() = 9;

  array<double, 1> B(std::move(A));

  EXPECT_TRUE(A.is_empty());
  EXPECT_EQ(B.shape(), (myshape_t<1>{3}));
  for (int i = 0; i < 3; ++i) EXPECT_EQ(B(i), 9);
}

// ==============================================================

TEST(NDA, MoveAssignment) {

  array<double, 1> A(3);
  A() = 9;

  array<double, 1> B;
  B = std::move(A);

  EXPECT_TRUE(A.is_empty());
  EXPECT_EQ(B.shape(), (myshape_t<1>{3}));
  for (int i = 0; i < 3; ++i) EXPECT_EQ(B(i), 9);
}

// ===================== SWAP =========================================

TEST(NDA, Swap) {
  auto V = vector<double>{3, 3, 3};
  auto W = vector<double>{4, 4, 4, 4};

  swap(V, W);

  // V , W are swapped
  EXPECT_EQ(V, (vector<double>{4, 4, 4, 4}));
  EXPECT_EQ(W, (vector<double>{3, 3, 3}));
}

// ----------------------------------

TEST(NDA, StdSwap) { // same are triqs swap for regular types

  auto V = vector<double>{3, 3, 3};
  auto W = vector<double>{4, 4, 4, 4};

  std::swap(V, W);

  // THIS Should not compile : deleted function
  //auto VV = V(range(0, 2));
  //auto WW = W(range(0, 2));
  //std::swap(VV, WW);

  // V , W are swapped
  EXPECT_EQ(V, (vector<double>{4, 4, 4, 4}));
  EXPECT_EQ(W, (vector<double>{3, 3, 3}));
}

// ----------------------------------

TEST(NDA, SwapView) {

  auto V = vector<double>{3, 3, 3};
  auto W = vector<double>{4, 4, 4, 4};

  // swap the view, not the vectors. Views are pointers
  // FIXME should we keep this behaviour ?
  auto VV = V(range(0, 2));
  auto WW = W(range(0, 2));
  swap(VV, WW);

  // V, W unchanged
  EXPECT_EQ(V, (vector<double>{3, 3, 3}));
  EXPECT_EQ(W, (vector<double>{4, 4, 4, 4}));

  // VV, WW swapped
  EXPECT_EQ(WW, (vector<double>{3, 3}));
  EXPECT_EQ(VV, (vector<double>{4, 4}));
}


// ==============================================================

TEST(NDA, Print) {
  array<long, 2> A(2, 3), B;

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) A(i, j) = 10 * i + j;

  EXPECT_PRINT("\n[[0,1,2]\n [10,11,12]]", A);
}


// ==============================================================

TEST(NDA, AssignVectorArray) {

  vector<double> V;
  array<double, 1> Va(5);
  for (int i = 0; i < 5; ++i) Va(i) = i + 2;

  V = Va / 2.0;
  EXPECT_ARRAY_NEAR(V, array<double, 1>{1.0, 1.5, 2.0, 2.5, 3.0});
  EXPECT_ARRAY_NEAR(Va, array<double, 1>{2, 3, 4, 5, 6});

  V = Va;
  EXPECT_ARRAY_NEAR(V, array<double, 1>{2, 3, 4, 5, 6});
  EXPECT_ARRAY_NEAR(Va, array<double, 1>{2, 3, 4, 5, 6});
}

// ===========   Cross construction  ===================================================

TEST(Array, CrossConstruct1) {
  vector<int> Vi(3);
  Vi() = 3;
  vector<double> Vd(Vi);
  EXPECT_ARRAY_NEAR(Vd, Vi);
}

// ------------------
TEST(Array, CrossConstruct2) {

  array<long, 2> A(2, 3);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) A(i, j) = 10 * i + j;

  std::vector<array<long, 2>> V(3, A);

  std::vector<array_view<long, 2>> W;
  for (auto &x : V) W.push_back(x);

  std::vector<array_view<long, 2>> W2(W);

  for (int i = 1; i < 3; ++i) V[i] *= i;

  for (int i = 1; i < 3; ++i) EXPECT_ARRAY_NEAR(W2[i], i * A);
}
// =============================================================

TEST(NDA, ConvertibleCR) {

  array<double, 2> A(2, 2);
  array<dcomplex, 2> B(2, 2);

  //A = B; // should not compile
  B = A;

  // can convert an array of double to an array of complex
  static_assert(std::is_convertible<array<double, 2>, array<dcomplex, 2>>::value, "oops");

  // can not do the reverse !
  static_assert(!std::is_convertible<array<dcomplex, 2>, array<double, 2>>::value, "oops");
}

MAKE_MAIN
