#include "./nda_test_common.hpp"

// ====================== VIEW ========================================

TEST(NDA, ViewBasic) {
  array<long, 3> a(3, 3, 4);

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 4; ++k) a(i, j, k) = i + 10 * j + 100 * k;

  auto v = a(_, 1, 2);

  EXPECT_EQ(v.shape(), (myshape_t<1>{3}));

  EXPECT_EQ(a(1, 1, 2), 1 + 10 * 1 + 100 * 2);
  
  a(1,1,2) = -28;
  EXPECT_EQ(v(1), a(1, 1, 2));
}
// ----------------------------------

TEST(NDA, Ellipsis) {
  array<long, 3> A(2, 3, 4);
  A() = 7;

  EXPECT_ARRAY_NEAR(A(0, ___), A(0, _, _), 1.e-15);

  array<long, 4> B(2, 3, 4, 5);
  B() = 8;

  EXPECT_ARRAY_NEAR(B(0, ___, 3), B(0, _, _, 3), 1.e-15);
  EXPECT_ARRAY_NEAR(B(0, ___, 2, 3), B(0, _, 2, 3), 1.e-15);
  EXPECT_ARRAY_NEAR(B(___, 2, 3), B(_, _, 2, 3), 1.e-15);
}

// ----------------------------------

template <typename ArrayType> auto sum0(ArrayType const &A) {
  array<typename ArrayType::value_type, ArrayType::rank - 1> res = A(0, ___);
  for (size_t u = 1; u < A.shape()[0]; ++u) res += A(u, ___);
  return res;
}

TEST(NDA, Ellipsis2) {
  array<double, 2> A(5, 2);
  A() = 2;
  array<double, 3> B(5, 2, 3);
  B() = 3;
  EXPECT_ARRAY_NEAR(sum0(A), array<double, 1>{10, 10}, 1.e-15);
  EXPECT_ARRAY_NEAR(sum0(B), array<double, 2>{{15, 15, 15}, {15, 15, 15}}, 1.e-15);
}

// ==============================================================

TEST(NDA, ConstView) {

  array<long, 2> A(2, 3);
  A() = 98;

  auto f2 = [](array_const_view<long, 2> const &a) {};

  f2(A());

  array_const_view<long, 2> Vc = A();

//#define SHOULD_NOT_COMPILE
#ifdef SHOULD_NOT_COMPILE
  {
    const array<long, 1> A = {1, 2, 3, 4};

    // None of this should compile
    A(0)              = 2;
    A()(0)            = 2;
    A(range(0, 2))(0) = 10;
  }
#endif
}

// ==============================================================

TEST(NDA, Bug2) {

  array<double, 3> A(10, 2, 2);
  A() = 0;

  A(4, range(), range()) = 1;
  A(5, range(), range()) = 2;

  matrix_view<double> M1 = A(4, range(), range());
  matrix_view<double> M2 = A(5, range(), range());

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{1, 1}, {1, 1}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{2, 2}, {2, 2}});

  M1 = M2;

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{2, 2}, {2, 2}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{2, 2}, {2, 2}});
}

// ==============================================================

TEST(NDA, View) {

  array<long, 2> A(2, 3);
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) A(i, j) = 10 * i + j;

  array_view<long, 2> AA(A);

  std::stringstream fs1, fs2;
  fs1 << A;
  fs2 << AA;
  EXPECT_EQ(fs1.str(), fs2.str());
  EXPECT_EQ(AA(0, 0), 0);

  array_view<long, 1> SL1(A(0, range(0, 3)));
  array_view<long, 1> SL2(A(1, range(0, 2)));
  array_view<long, 1> SL3(A(1, range(1, 3)));
  array_view<long, 1> SL4(A(range(0, 2), 0));
  array_view<long, 1> SL5(A(range(0, 2), 1));

  EXPECT_EQ_ARRAY(SL1, (array<long, 1>{0, 1, 2}));
  EXPECT_EQ_ARRAY(SL2, (array<long, 1>{10, 11}));
  EXPECT_EQ_ARRAY(SL3, (array<long, 1>{11, 12}));
  EXPECT_EQ_ARRAY(SL4, (array<long, 1>{0, 10}));
  EXPECT_EQ_ARRAY(SL5, (array<long, 1>{1, 11}));
}

// ==============================================================

TEST(NDA, View3) {

  // ---------
  auto print_in_indexmap_order = [](auto const &a) {
    std::stringstream out;

    // FIXME : better loop
    using A = std::decay_t<decltype(a)>;
    for (typename A::indexmap_type::iterator it(a.indexmap()); it; ++it) {
      auto i = it.indices();
      out << a(i[0], i[1], i[2]) << " ";
    }
    return out.str();
  };

  //-------------
  auto f = [](auto &A) {
    for (int i = 0; i < 2; ++i)
      for (int j = 0; j < 3; ++j)
        for (int k = 0; k < 4; ++k) A(i, j, k) = 100 * (i + 1) + 10 * (j + 1) + (k + 1);

    auto _ = range{};

    EXPECT_EQ_ARRAY(A(0, _, _), (array<long, 2>{{111, 112, 113, 114}, {121, 122, 123, 124}, {131, 132, 133, 134}}));
    EXPECT_EQ_ARRAY(A(1, _, _), (array<long, 2>{{211, 212, 213, 214}, {221, 222, 223, 224}, {231, 232, 233, 234}}));
    EXPECT_EQ_ARRAY(A(_, 0, _), (array<long, 2>{{111, 112, 113, 114}, {211, 212, 213, 214}}));
    EXPECT_EQ_ARRAY(A(_, _, 1), (array<long, 2>{{112, 122, 132}, {212, 222, 232}}));
    EXPECT_EQ_ARRAY(A(_, 0, 1), (array<long, 1>{112, 212}));
  };

  //-------------
  // permutation in triqs::arrays
  array<long, 3> A0(2, 3, 4);
  array<long, 3> A1(2, 3, 4, make_memory_layout(2, 1, 0));
  array<long, 3> A2(2, 3, 4, make_memory_layout(2, 0, 1));
  array<long, 3> A3(2, 3, 4, make_memory_layout(0, 1, 2));
  array<long, 3> A4(2, 3, 4, FORTRAN_LAYOUT);

  f(A0);
  f(A1);
  f(A2);
  f(A3);
  f(A4);

  EXPECT_EQ(print_in_indexmap_order(A0), "111 112 113 114 121 122 123 124 131 132 133 134 211 212 213 214 221 222 223 224 231 232 233 234 ");
  EXPECT_EQ(print_in_indexmap_order(A1), "111 112 113 114 121 122 123 124 131 132 133 134 211 212 213 214 221 222 223 224 231 232 233 234 ");
  EXPECT_EQ(print_in_indexmap_order(A2), "111 112 113 114 121 122 123 124 131 132 133 134 211 212 213 214 221 222 223 224 231 232 233 234 ");
  EXPECT_EQ(print_in_indexmap_order(A3), "111 112 113 114 121 122 123 124 131 132 133 134 211 212 213 214 221 222 223 224 231 232 233 234 ");
  EXPECT_EQ(print_in_indexmap_order(A4), "111 112 113 114 121 122 123 124 131 132 133 134 211 212 213 214 221 222 223 224 231 232 233 234 ");

  //EXPECT_EQ(print_in_indexmap_order(A1), "111 211 121 221 131 231 112 212 122 222 132 232 113 213 123 223 133 233 114 214 124 224 134 234 ");
  //EXPECT_EQ(print_in_indexmap_order(A2), "111 121 131 211 221 231 112 122 132 212 222 232 113 123 133 213 223 233 114 124 134 214 224 234 ");
  //EXPECT_EQ(print_in_indexmap_order(A3), "111 112 113 114 121 122 123 124 131 132 133 134 211 212 213 214 221 222 223 224 231 232 233 234 ");
  //EXPECT_EQ(print_in_indexmap_order(A4), "111 211 121 221 131 231 112 212 122 222 132 232 113 213 123 223 133 233 114 214 124 224 134 234 ");
}


// ==============================================================

// old issue
TEST(NDA, IssueXXX) {

  array<double, 3> A(10, 2, 2);
  A() = 0;

  A(4, range(), range()) = 1;
  A(5, range(), range()) = 2;

  matrix_view<double> M1 = A(4, range(), range());
  matrix_view<double> M2 = A(5, range(), range());

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{1, 1}, {1, 1}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{2, 2}, {2, 2}});

  M1 = M2;

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{2, 2}, {2, 2}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{2, 2}, {2, 2}});
}



MAKE_MAIN
