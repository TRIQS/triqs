#include "./nda_test_common.hpp"
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include "triqs/arrays/blas_lapack/gtsv.hpp"
#include <triqs/arrays/blas_lapack/stev.hpp>
#include <triqs/arrays/blas_lapack/gelss.hpp>
#include <triqs/arrays/blas_lapack/gesvd.hpp>
#include <triqs/arrays/linalg/eigenelements.hpp>
#include <triqs/utility/numeric_ops.hpp>
#include <triqs/utility/complex_ops.hpp>

template <typename T, typename O1, typename O2, typename O3> void test_matmul(O1 o1, O2 o2, O3 o3, bool all = false) {
  matrix<T> M1(2, 3, o1);
  matrix<T> M2(3, 4, o2);
  matrix<T> M3(o3), M4;
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) { M1(i, j) = i + j; }
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 4; ++j) { M2(i, j) = 1 + i - j; }

  M3 = M1 * M2; //matmul(M1,M2);

  M4   = M3;
  M4() = 0;
  for (int i = 0; i < 2; ++i)
    for (int k = 0; k < 3; ++k)
      for (int j = 0; j < 4; ++j) M4(i, j) += M1(i, k) * M2(k, j);
  EXPECT_ARRAY_NEAR(M4, M3, 1.e-13);

  // recheck gemm_generic
  blas::gemm_generic(1, M1, M2, 0, M4);
  EXPECT_ARRAY_NEAR(M4, M3, 1.e-13);
}

template <typename T> void all_test_matmul() {
  test_matmul<T>(C_LAYOUT, C_LAYOUT, C_LAYOUT);
  test_matmul<T>(C_LAYOUT, C_LAYOUT, FORTRAN_LAYOUT);
  test_matmul<T>(C_LAYOUT, FORTRAN_LAYOUT, FORTRAN_LAYOUT);
  test_matmul<T>(C_LAYOUT, FORTRAN_LAYOUT, C_LAYOUT);
  test_matmul<T>(FORTRAN_LAYOUT, FORTRAN_LAYOUT, FORTRAN_LAYOUT);
  test_matmul<T>(FORTRAN_LAYOUT, C_LAYOUT, FORTRAN_LAYOUT);
  test_matmul<T>(FORTRAN_LAYOUT, FORTRAN_LAYOUT, C_LAYOUT);
  test_matmul<T>(FORTRAN_LAYOUT, C_LAYOUT, C_LAYOUT);
}

TEST(Matmul, Double) { all_test_matmul<double>(); }
TEST(Matmul, Complex) { all_test_matmul<std::complex<double>>(); }
TEST(Matmul, Int) { all_test_matmul<long>(); }

//-------------------------------------------------------------

TEST(Matmul, Promotion) {
  matrix<double> C, D, A = {{1.0, 2.3}, {3.1, 4.3}};
  matrix<int> B     = {{1, 2}, {3, 4}};
  matrix<double> Bd = {{1, 2}, {3, 4}};

  C = A * B;
  D = A * Bd;
  EXPECT_ARRAY_NEAR(A * B, A * Bd, 1.e-13);
}
//-------------------------------------------------------------

TEST(Matmul, Cache) {
  // testing with view for possible cache issue

  array<std::complex<double>, 3> TMPALL(2, 2, 5);
  TMPALL() = -1;
  matrix_view<std::complex<double>> TMP(TMPALL(range(), range(), 2));
  matrix<std::complex<double>> M1(2, 2), Res(2, 2);
  M1()      = 0;
  M1(0, 0)  = 2;
  M1(1, 1)  = 3.2;
  Res()     = 0;
  Res(0, 0) = 8;
  Res(1, 1) = 16.64;
  TMP()     = 0;
  TMP()     = M1 * (M1 + 2.0);
  EXPECT_ARRAY_NEAR(TMP(), Res, 1.e-13);
}
//-------------------------------------------------------------

TEST(Matmul, Alias) {

  auto _ = range{};

  array<dcomplex, 3> A(10, 2, 2);
  A() = -1;

  A(4, _, _) = 1;
  A(5, _, _) = 2;

  matrix_view<dcomplex> M1 = A(4, _, _);
  matrix_view<dcomplex> M2 = A(5, _, _);

  M1 = M1 * M2;

  EXPECT_ARRAY_NEAR(M1, matrix<dcomplex>{{4, 4}, {4, 4}});
  EXPECT_ARRAY_NEAR(M2, matrix<dcomplex>{{2, 2}, {2, 2}});

  matrix<double> B1(2, 2), B2(2, 2);
  B1() = 2;
  B2() = 3;

  B1 = make_clone(B1) * B2;
  EXPECT_ARRAY_NEAR(B1, matrix<double>{{6, 0}, {0, 6}});
}

// ==============================================================

TEST(Matvecmul, Double) {

  matrix<double> A(5, 5, FORTRAN_LAYOUT), Ac(5, 5);
  vector<double> MC(5), MB(5);

  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j) A(i, j) = i + 2 * j + 1;

  Ac = A;

  MC() = 1;
  MB() = 0;
  range R(1, 3);

  MB(R) = A(R, R) * MC(R);

  EXPECT_ARRAY_NEAR(MB, vector<double>{0, 10, 12, 0, 0});

  matrix_view<double> Acw = A.transpose();

  auto MB_w = MB(R); // view !

  blas::gemv(1, A(R, R), MC(R), 0, MB_w);
  EXPECT_ARRAY_NEAR(MB, vector<double>{0, 10, 12, 0, 0});

  blas::gemv(1, Ac(R, R), MC(R), 0, MB_w);
  EXPECT_ARRAY_NEAR(MB, vector<double>{0, 10, 12, 0, 0});

  blas::gemv(1, Acw(R, R), MC(R), 0, MB_w);
  EXPECT_ARRAY_NEAR(MB, vector<double>{0, 9, 13, 0, 0});
}

// ==============================================================

TEST(Matvecmul, Promotion) {

  matrix<int> Ai   = {{1, 2}, {3, 4}};
  matrix<double> A = {{1, 2}, {3, 4}};
  vector<int> Ci, B     = {1, 1};
  vector<double> Cd, Bd = {1, 1};

  Cd = A * B;
  Ci = Ai * B;

  EXPECT_ARRAY_NEAR(Cd, Ci, 1.e-13);
}

// ==============================================================

TEST(NDA, MatrixInverse) {

  matrix<double> W(3, 3, FORTRAN_LAYOUT), Wi(3, 3, FORTRAN_LAYOUT), A(FORTRAN_LAYOUT);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) W(i, j) = (i > j ? i + 2.5 * j : i * 0.8 - j);

  auto Wkeep = W;

  EXPECT_NEAR(determinant(W), -7.8, 1.e-12);

  Wi = inverse(W);
  EXPECT_NEAR(determinant(Wi), -1 / 7.8, 1.e-12);

  matrix<double> should_be_one(W * Wi);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) EXPECT_NEAR(std::abs(should_be_one(i, j)), (i == j ? 1 : 0), 1.e-13);

  // involution
  W = inverse(W);
  A = inverse(W);
  EXPECT_ARRAY_NEAR(A, Wkeep, 1.e-10);

  matrix_view<double> V(W(range(0, 3, 2), range(0, 3, 2)));
  matrix<double> Vi = inverse(V);
  matrix<double> Viref{{-5.0, 4.0}, {24.5, -27.4}};
  EXPECT_ARRAY_NEAR(Vi, Viref, 1.e-12);

  // testing against "manual" call of bindings
  triqs::arrays::vector<int> ipiv2(3);
  ipiv2() = 0;
  lapack::getrf(Wi, ipiv2);
  lapack::getri(Wi, ipiv2);
  EXPECT_ARRAY_NEAR(Wi, Wkeep, 1.e-12);
}
// ========================= tridiag matrix STEV  =====================================

template <typename T> void check_eig(matrix<T> M, matrix_view<T> vectors, vector_view<double> values) {
  auto _ = range();
  for (auto i : range(0, first_dim(M))) { EXPECT_ARRAY_NEAR(M * vectors(i, _), values(i) * vectors(i, _), 1.e-13); }
}

template <typename Md, typename Me> void test(Md d, Me e) {
  using value_t = typename Me::value_type;
  using triqs::utility::conj;

  matrix<value_t> A(first_dim(d), first_dim(d));
  assign_foreach(A, [&](size_t i, size_t j) { return j == i ? d(i) : j == i + 1 ? e(i) : j == i - 1 ? conj(e(j)) : .0; });

  int size = first_dim(d);
  for (int init_size : {0, size, 2 * size}) {
    lapack::tridiag_worker<triqs::is_complex<value_t>::value> w(init_size);
    w(d, e);
    check_eig(A, w.vectors(), w.values());
  }
}

TEST(tridiag, real) {
  vector<double> d(5);
  vector<double> e(4);
  assign_foreach(d, [](size_t i) { return 3.2 * i + 2.3; });
  assign_foreach(e, [](size_t i) { return 2.4 * (i + 1.82) + 0.78; });
  e(1) = .0;
  test(d, e);
}

TEST(tridiag, complex) {
  vector<double> d(5);
  vector<dcomplex> e(4);
  assign_foreach(d, [](size_t i) { return 3.2 * i + 2.3; });
  assign_foreach(e, [](size_t i) { return 2.4 * (i + 1.82) + 0.78 * dcomplex(0, 1.0); });
  e(1) = .0;
  test(d, e);
}

//=======================================  gtsv=====================================

TEST(blas_lapack, dgtsv) {

  vector<double> DL = {4, 3, 2, 1};    // sub-diagonal elements
  vector<double> D  = {1, 2, 3, 4, 5}; // diagonal elements
  vector<double> DU = {1, 2, 3, 4};    // super-diagonal elements

  vector<double> B1 = {6, 2, 7, 4, 5};  // RHS column 1
  vector<double> B2 = {1, 3, 8, 9, 10}; // RHS column 2
  matrix<double> B(5, 2, FORTRAN_LAYOUT);
  B(range(), 0) = B1;
  B(range(), 1) = B2;

  // reference solutions
  vector<double> ref_sol_1 = {43.0 / 33.0, 155.0 / 33.0, -208.0 / 33.0, 130.0 / 33.0, 7.0 / 33.0};
  vector<double> ref_sol_2 = {-28.0 / 33.0, 61.0 / 33.0, 89.0 / 66.0, -35.0 / 66.0, 139.0 / 66.0};
  matrix<double> ref_sol(5, 2, FORTRAN_LAYOUT);
  ref_sol(range(), 0) = ref_sol_1;
  ref_sol(range(), 1) = ref_sol_2;

  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B1);
    EXPECT_ARRAY_NEAR(B1, ref_sol_1);
  }
  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B2);
    EXPECT_ARRAY_NEAR(B2, ref_sol_2);
  }
  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B);
    EXPECT_ARRAY_NEAR(B, ref_sol);
  }
}

//---------------------------------------------------------

TEST(blas_lapack, cgtsv) {
  using dcomplex = std::complex<double>;

  vector<dcomplex> DL = {-4_j, -3_j, -2_j, -1_j}; // sub-diagonal elements
  vector<dcomplex> D  = {1, 2, 3, 4, 5};          // diagonal elements
  vector<dcomplex> DU = {1_j, 2_j, 3_j, 4_j};     // super-diagonal elements

  vector<dcomplex> B1 = {6 + 0_j, 2_j, 7 + 0_j, 4_j, 5 + 0_j}; // RHS column 1
  vector<dcomplex> B2 = {1_j, 3 + 0_j, 8_j, 9 + 0_j, 10_j};    // RHS column 2
  matrix<dcomplex> B(5, 2, FORTRAN_LAYOUT);
  B(range(), 0) = B1;
  B(range(), 1) = B2;

  // reference solutions
  vector<dcomplex> ref_sol_1 = {137.0 / 33.0 + 0_j, -61_j / 33.0, 368.0 / 33.0 + 0_j, 230_j / 33.0, -13.0 / 33.0 + 0_j};
  vector<dcomplex> ref_sol_2 = {-35_j / 33.0, 68.0 / 33.0 + 0_j, -103_j / 66.0, 415.0 / 66.0 + 0_j, 215_j / 66.0};
  matrix<dcomplex> ref_sol(5, 2, FORTRAN_LAYOUT);
  ref_sol(range(), 0) = ref_sol_1;
  ref_sol(range(), 1) = ref_sol_2;

  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B1);
    EXPECT_ARRAY_NEAR(B1, ref_sol_1);
  }
  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B2);
    EXPECT_ARRAY_NEAR(B2, ref_sol_2);
  }
  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B);
    EXPECT_ARRAY_NEAR(B, ref_sol);
  }
}

// ================================================================================

TEST(blas_lapack, svd) {

  auto A = matrix<dcomplex>{{{1, 1, 1}, {2, 3, 4}, {3, 5, 2}, {4, 2, 5}, {5, 4, 3}}};
  int M  = first_dim(A);
  int N  = second_dim(A);

  auto U  = matrix<dcomplex>(M, M);
  auto VT = matrix<dcomplex>(N, N);

  auto S = vector<double>(std::min(M, N));

  lapack::gesvd(A, S, U, VT);

  auto S_Mat = A;
  S_Mat()    = 0.0;
  for (int i : range(std::min(M, N))) S_Mat(i, i) = S(i);

  EXPECT_ARRAY_NEAR(A, U * S_Mat * VT, 1e-14);
}

// ================================================================================

TEST(blas_lapack, gelss) {

  // Cf. http://www.netlib.org/lapack/explore-html/d3/d77/example___d_g_e_l_s__colmajor_8c_source.html
  auto A = matrix<dcomplex>{{{1, 1, 1}, {2, 3, 4}, {3, 5, 2}, {4, 2, 5}, {5, 4, 3}}};
  auto B = matrix<dcomplex>{{{-10, -3}, {12, 14}, {14, 12}, {16, 16}, {18, 16}}};

  int M    = first_dim(A);
  int N    = second_dim(A);
  int NRHS = second_dim(B);

  auto S = vector<double>(std::min(M, N));

  auto gelss_new    = lapack::gelss_cache<dcomplex>{A};
  auto [x_1, eps_1] = gelss_new(B);

  int i;
  lapack::gelss(A, B, S, 1e-18, i);
  auto x_2 = B(range(N), range(NRHS));

  auto x_exact = matrix<dcomplex>{{{2, 1}, {1, 1}, {1, 2}}};

  EXPECT_ARRAY_NEAR(x_exact, x_1, 1e-14);
  EXPECT_ARRAY_NEAR(x_exact, x_2, 1e-14);
}
// ================================================================================

TEST(eigenelements, test1) {

  auto test = [](auto && A) {
    auto w = linalg::eigenelements(make_clone(A));
    check_eig(A, w.second(), w.first());
  };

  {
    matrix<double> A(3, 3);

    for (int i = 0; i < 3; ++i)
      for (int j = 0; j <= i; ++j) {
        A(i, j) = (i > j ? i + 2 * j : i - j);
        A(j, i) = A(i, j);
      }
    test(A);
    
    A()     = 0;
    A(0, 1) = 1;
    A(1, 0) = 1;
    A(2, 2) = 8;
    A(0, 2) = 2;
    A(2, 0) = 2;

    test(A);

    A()     = 0;
    A(0, 1) = 1;
    A(1, 0) = 1;
    A(2, 2) = 8;

    test(A);
  }
  { // the complex case

    matrix<dcomplex> M(2, 2);

    M(0, 0) = 1;
    M(0, 1) = 1.0_j;
    M(1, 0) = -1.0_j;
    M(1, 1) = 2;

    test(M);
  }

  { // the complex case

    matrix<dcomplex> M(2, 2, FORTRAN_LAYOUT);

    M(0, 0) = 1;
    M(0, 1) = 1.0_j;
    M(1, 0) = -1.0_j;
    M(1, 1) = 2;

    test(M);
  }
}
MAKE_MAIN
