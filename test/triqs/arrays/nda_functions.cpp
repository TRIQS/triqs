#include "./nda_test_common.hpp"

TEST(matrix, vstack) {

  matrix<double> A1 = {{1, 2}, {3, 4}};
  matrix<double> A2 = {{5, 6}, {7, 8}};

  auto B = vstack(A1, A2);

  EXPECT_EQ(B(2, 0), 5);
  EXPECT_EQ(B(3, 1), 8);

  EXPECT_EQ(B(range(0, 2), range(0, 2)), A1);
  EXPECT_EQ(B(range(2, 4), range(0, 2)), A2);

  EXPECT_EQ(B.shape(), make_shape(4, 2));
}

//================================================

// inversion tensor (by element)
TEST(NDA, InverseTensor) {

  array<dcomplex, 3> a(2, 2, 2);
  a() = 0;
  a() = a() + 2;

  a() = inverse(a);

  array<dcomplex, 3> r(2, 2, 2);
  r() = 0.5;

  EXPECT_ARRAY_NEAR(r, a);
}

placeholder<0> i_;
placeholder<1> j_;
placeholder<2> k_;
placeholder<3> l_;

//================================================

TEST(GroupIndices, v1) {
  array<int, 4> A(2, 2, 2, 2);
  A(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;
  group_indices_view(A, {0, 1}, {2, 3});
}

// ------------------------------------

TEST(GroupIndices, ProdInverse) {

  // more complex : inversing a tensor product of matrices...
  matrix<double> B(2, 2), C(3, 3), Binv, Cinv;
  C(i_, j_) << 1.7 / (3.4 * i_ - 2.3 * j_ + 1);
  B(i_, j_) << 2 * i_ + j_;
  Binv = inverse(B);
  Cinv = inverse(C);

  {
    array<double, 4> A(2, 3, 2, 3);
    A(i_, j_, k_, l_) << B(i_, k_) * C(j_, l_);
    auto M = make_matrix_view(group_indices_view(A, {0, 1}, {2, 3}));
    M      = inverse(M);
    array<double, 4> R(A.shape());
    R(i_, j_, k_, l_) << Binv(i_, k_) * Cinv(j_, l_);
    EXPECT_ARRAY_NEAR(R, A, 5.e-15);
  }

  {
    array<double, 4> A(2, 3, 2, 3, make_memory_layout(1, 0, 3, 2));
    A(i_, j_, k_, l_) << B(i_, k_) * C(j_, l_);
    auto M = make_matrix_view(group_indices_view(A, {0, 1}, {2, 3}));
    M      = inverse(M);
    array<double, 4> R(A.shape());
    R(i_, j_, k_, l_) << Binv(i_, k_) * Cinv(j_, l_);
    EXPECT_ARRAY_NEAR(R, A, 5.e-15);
  }

  {
    array<double, 4> A(2, 2, 3, 3, make_memory_layout(0, 2, 1, 3));
    A(i_, k_, j_, l_) << B(i_, k_) * C(j_, l_);
    auto M = make_matrix_view(group_indices_view(A, {0, 2}, {1, 3}));
    M      = inverse(M);
    array<double, 4> R(A.shape());
    R(i_, k_, j_, l_) << Binv(i_, k_) * Cinv(j_, l_);
    EXPECT_ARRAY_NEAR(R, A, 5.e-15);
  }
}

//================================================

TEST(Array, SwapIndex) {

  array<double, 4> A(1, 2, 3, 4);

  triqs::clef::placeholder<0> i_;
  triqs::clef::placeholder<1> j_;
  triqs::clef::placeholder<2> k_;
  triqs::clef::placeholder<3> l_;

  A(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  auto S = swap_index_view(A, 0, 2);

  array<double, 4> B(3, 2, 1, 4);
  B(k_, j_, i_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  EXPECT_EQ(S, B());
  EXPECT_EQ(first_dim(A), third_dim(S));
  EXPECT_EQ(first_dim(S), third_dim(A));
}

//================================================

//FIXME : move in lib
namespace triqs::arrays {

  template <typename V, int R, typename... I> array_view<V, sizeof...(I)> reinterpret(array<V, R> const &a, I... index) {
    return {{make_shape(index...)}, a.storage()};
  }

  template <typename V, int R, bool B, typename... I> array_view<V, sizeof...(I)> reinterpret_array_view(array_view<V, R, B> const &a, I... index) {
    if (!has_contiguous_data(a)) TRIQS_RUNTIME_ERROR << "reinterpretation failure : data of the view are not contiguous";
    return {{make_shape(index...)}, a.storage()};
  }

} // namespace triqs::arrays

TEST(NDA, reinterpretView) {
  array<long, 1> A = {1, 2, 3, 4, 5, 6};
  EXPECT_ARRAY_NEAR(reinterpret(A, 2, 3), array<long, 2>{{1, 2, 3}, {4, 5, 6}});
}

MAKE_MAIN
