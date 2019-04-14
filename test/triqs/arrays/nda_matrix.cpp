#include "./nda_test_common.hpp"

/// Matrix specific tests

template <typename T> class immutable_diagonal_matrix_view : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) {
  array_view<T, 1> data;

  public:
  immutable_diagonal_matrix_view(array_view<T, 1> v) : data(v) {}

  // the ImmutableMatrix concept
  using value_type = T;
  T operator()(size_t i, size_t j) const { return (i == j ? data(i) : 0); }

  using domain_type = indexmaps::cuboid::domain_t<2>;
  domain_type domain() const {
    auto s = data.shape()[0];
    return mini_vector<size_t, 2>(s, s);
  }

  //
  friend std::ostream &operator<<(std::ostream &out, immutable_diagonal_matrix_view const &d) { return out << "diagonal_matrix " << d.data; }
};

// ===============================================================

TEST(Array, DiagonalMatrix) {

  auto a = array<int, 1>{1, 2, 3, 4};
  auto d = immutable_diagonal_matrix_view<int>(a);

  EXPECT_ARRAY_NEAR(matrix<int>(2 * d), matrix<double>{{2, 0, 0, 0}, {0, 4, 0, 0}, {0, 0, 6, 0}, {0, 0, 0, 8}});
  EXPECT_ARRAY_NEAR(matrix<int>(d * d), matrix<double>{{1, 0, 0, 0}, {0, 4, 0, 0}, {0, 0, 9, 0}, {0, 0, 0, 16}});

  int sum = 0;
  foreach (d, [&](int i, int j) { sum += d(i, j); })
    ;
  EXPECT_EQ(sum, 10);
}

// ===============================================================

TEST(Matrix, TransposeDagger) {

  const int N = 5;

  triqs::arrays::matrix<double> A(N, N, FORTRAN_LAYOUT);
  triqs::arrays::matrix<std::complex<double>> B(N, N);

  // A *=A;; should be rejected by compiler

  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j) {
      A(i, j) = i + 2 * j + 1;
      B(i, j) = i + 2.5 * j + (i - 0.8 * j) * 1_j;
    }

  auto at = A.transpose();
  auto ad = dagger(A);
  auto bt = B.transpose();
  auto bd = dagger(B);

  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j) {
      EXPECT_COMPLEX_NEAR(at(i, j), A(j, i));
      EXPECT_COMPLEX_NEAR(ad(i, j), A(j, i));
      EXPECT_COMPLEX_NEAR(bd(i, j), std::conj(B(j, i)));
      EXPECT_COMPLEX_NEAR(bt(i, j), B(j, i));
    }
}
MAKE_MAIN
