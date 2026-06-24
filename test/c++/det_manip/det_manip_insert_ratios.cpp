//     https://www.gnu.org/licenses/gpl-3.0.txt

#include <triqs/det_manip/det_manip.hpp>
#include <triqs/det_manip/det_manip_basic.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <nda/linalg/det.hpp>
#include <iostream>
#include <cmath>

struct fun {
  using result_type   = double;
  using argument_type = double;

  double operator()(double x, double y) const {
    const double pi   = acos(-1.0);
    const double beta = 10.0;
    const double epsi = 0.1;
    double tau        = x - y;
    bool s            = (tau > 0);
    tau               = (s ? tau : beta + tau);
    double r          = epsi + tau / beta * (1 - 2 * epsi);
    return -2 * (pi / beta) / std::sin(pi * r);
  }
};

// Tolerance for batch insert_ratios vs sequential try_insert. On a well-conditioned matrix (see
// MIN_INSERT_RATIO) the gemm and gemv paths agree to ~4e-9; 1e-7 leaves margin for other BLAS.
const double PRECISION = 1.e-7;

template <typename T1, typename T2> void assert_close(T1 const &A, T2 const &B, double precision, std::string const &msg = "") {
  double diff  = std::abs(A - B);
  double scale = std::max(std::abs(double(A)), std::abs(double(B)));
  if (diff > precision * std::max(scale, 1.0))
    TRIQS_RUNTIME_ERROR << "assert_close error: " << A << " vs " << B << " diff=" << diff << " reldiff=" << diff / std::max(scale, 1e-30) << " "
                        << msg;
}

// Reject build insertions with det-ratio below this (as a real MC would, acceptance ~ |ratio|^2):
// otherwise the random kernel can drive det to ~1e-41, corrupting the maintained inverse so that
// batch-vs-sequential ratios are pure roundoff noise. Not det_manip's abs(det) singular_threshold.
const double MIN_INSERT_RATIO = 1.e-3;

// Build up a well-conditioned det_manip of the given size, rejecting near-singular insertions.
template <typename DM> void build_det(DM &D, int target_size, triqs::mc_tools::random_generator &RNG) {
  for (long attempts = 0; D.size() < target_size; ++attempts) {
    if (attempts > 1000L * target_size) TRIQS_RUNTIME_ERROR << "build_det: too few non-singular insertions";
    double x = RNG(10.0);
    double y = RNG(10.0);
    if (std::abs(D.try_insert(D.size(), D.size(), x, y)) > MIN_INSERT_RATIO)
      D.complete_operation();
    else
      D.reject_last_try();
  }
}

// Build two det_manips (optimized + basic) with the same well-conditioned points, rejecting
// near-singular insertions so the incrementally-maintained inverse stays comparable to a fresh one.
template <typename DM1, typename DM2>
void build_det_pair(DM1 &D, DM2 &Db, int target_size, triqs::mc_tools::random_generator &RNG) {
  for (long attempts = 0; D.size() < target_size; ++attempts) {
    if (attempts > 1000L * target_size) TRIQS_RUNTIME_ERROR << "build_det_pair: too few non-singular insertions";
    double x = RNG(10.0);
    double y = RNG(10.0);
    if (std::abs(D.try_insert(D.size(), D.size(), x, y)) > MIN_INSERT_RATIO) {
      D.complete_operation();
      Db.insert(Db.size(), Db.size(), x, y);
    } else {
      D.reject_last_try();
    }
  }
}

// Helper: fill an nda::array<double, 1> with random values
nda::array<double, 1> random_array1(long K, triqs::mc_tools::random_generator &RNG, double range = 10.0) {
  nda::array<double, 1> a(K);
  for (long m = 0; m < K; ++m) a(m) = RNG(range);
  return a;
}

// Helper: fill an nda::array<double, 2> with random values
nda::array<double, 2> random_array2(long M, long N, triqs::mc_tools::random_generator &RNG, double range = 10.0) {
  nda::array<double, 2> a(M, N);
  for (long i = 0; i < M; ++i)
    for (long j = 0; j < N; ++j) a(i, j) = RNG(range);
  return a;
}

void test_rank1_batch_vs_sequential() {
  std::cerr << "=== test_rank1_batch_vs_sequential ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 12345);
  build_det(D, 20, RNG);

  long K  = 30;
  auto xs = random_array1(K, RNG);
  auto ys = random_array1(K, RNG);

  auto batch = D.insert_ratios(0, 0, xs, ys);

  for (long m = 0; m < K; ++m) {
    auto ratio = D.try_insert(0, 0, xs(m), ys(m));
    D.reject_last_try();
    assert_close(batch(m), ratio, PRECISION, "rank1 m=" + std::to_string(m));
  }
  std::cerr << "PASSED" << std::endl;
}

void test_rank1_state_unchanged() {
  std::cerr << "=== test_rank1_state_unchanged ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 54321);
  build_det(D, 15, RNG);

  auto det_before  = D.determinant();
  auto inv_before  = D.inverse_matrix();
  auto size_before = D.size();

  auto xs = nda::array<double, 1>{1.0, 2.0, 3.0};
  auto ys = nda::array<double, 1>{4.0, 5.0, 6.0};
  D.insert_ratios(0, 0, xs, ys);

  if (D.size() != size_before) TRIQS_RUNTIME_ERROR << "Size changed!";
  assert_close(D.determinant(), det_before, 1.e-12, "det changed");
  auto inv_after = D.inverse_matrix();
  for (int i = 0; i < size_before; ++i)
    for (int j = 0; j < size_before; ++j) assert_close(inv_after(i, j), inv_before(i, j), 1.e-12, "inv changed");
  std::cerr << "PASSED" << std::endl;
}

void test_rank1_empty_matrix() {
  std::cerr << "=== test_rank1_empty_matrix ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);

  auto xs    = nda::array<double, 1>{1.0, 2.0, 3.0};
  auto ys    = nda::array<double, 1>{4.0, 5.0, 6.0};
  auto batch = D.insert_ratios(0, 0, xs, ys);

  for (long m = 0; m < 3; ++m) {
    auto ratio = D.try_insert(0, 0, xs(m), ys(m));
    D.reject_last_try();
    assert_close(batch(m), ratio, 1.e-14, "rank1 empty m=" + std::to_string(m));
  }
  std::cerr << "PASSED" << std::endl;
}

void test_rank1_single_point() {
  std::cerr << "=== test_rank1_single_point ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 99999);
  build_det(D, 10, RNG);

  auto xs    = nda::array<double, 1>{5.0};
  auto ys    = nda::array<double, 1>{3.0};
  auto batch = D.insert_ratios(0, 0, xs, ys);
  auto ratio = D.try_insert(0, 0, xs(0), ys(0));
  D.reject_last_try();
  assert_close(batch(0), ratio, PRECISION, "rank1 single");
  std::cerr << "PASSED" << std::endl;
}

void test_rank1_empty_batch() {
  std::cerr << "=== test_rank1_empty_batch ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 11111);
  build_det(D, 5, RNG);

  nda::array<double, 1> xs(0), ys(0);
  auto batch = D.insert_ratios(0, 0, xs, ys);
  if (batch.size() != 0) TRIQS_RUNTIME_ERROR << "Expected empty result";
  std::cerr << "PASSED" << std::endl;
}

void test_cross_validate_rank1() {
  std::cerr << "=== test_cross_validate_rank1 ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::det_manip::det_manip_basic<fun> Db(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 33333);

  build_det_pair(D, Db, 15, RNG);

  long K  = 10;
  auto xs = random_array1(K, RNG);
  auto ys = random_array1(K, RNG);

  auto batch_opt   = D.insert_ratios(0, 0, xs, ys);
  auto batch_basic = Db.insert_ratios(0, 0, xs, ys);

  for (long m = 0; m < K; ++m) assert_close(batch_opt(m), batch_basic(m), 1.e-4, "cross rank1 m=" + std::to_string(m));
  std::cerr << "PASSED" << std::endl;
}

// ---- Tests for rank-2 arrays (batching across two dimensions) ----

void test_rank2_array_insert_ratios() {
  std::cerr << "=== test_rank2_array_insert_ratios ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 11223);
  build_det(D, 20, RNG);

  long M = 5, E = 8;
  auto xs = random_array2(M, E, RNG);
  auto ys = random_array2(M, E, RNG);

  auto batch = D.insert_ratios(0, 0, xs, ys);
  static_assert(nda::get_rank<decltype(batch)> == 2);
  TRIQS_ASSERT(batch.shape() == (std::array<long, 2>{M, E}));

  // Cross-validate against sequential calls
  for (long i = 0; i < M; ++i)
    for (long j = 0; j < E; ++j) {
      auto ratio = D.try_insert(0, 0, xs(i, j), ys(i, j));
      D.reject_last_try();
      assert_close(batch(i, j), ratio, PRECISION, "rank2 array (" + std::to_string(i) + "," + std::to_string(j) + ")");
    }
  std::cerr << "PASSED" << std::endl;
}

void test_rank2_array_insert_ratios_cross_validate() {
  std::cerr << "=== test_rank2_array_insert_ratios_cross_validate ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::det_manip::det_manip_basic<fun> Db(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 44556);

  build_det_pair(D, Db, 15, RNG);

  long M = 4, E = 6;
  auto xs = random_array2(M, E, RNG);
  auto ys = random_array2(M, E, RNG);

  auto batch_opt   = D.insert_ratios(0, 0, xs, ys);
  auto batch_basic = Db.insert_ratios(0, 0, xs, ys);

  for (long i = 0; i < M; ++i)
    for (long j = 0; j < E; ++j)
      assert_close(batch_opt(i, j), batch_basic(i, j), 1.e-2, "cross rank2 array (" + std::to_string(i) + "," + std::to_string(j) + ")");
  std::cerr << "PASSED" << std::endl;
}

// insert_ratios at positions with i+j odd: exercises the sign_fac = -1 branch.
void test_rank1_nonzero_position() {
  std::cerr << "=== test_rank1_nonzero_position ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 31415);
  build_det(D, 20, RNG);

  long K  = 12;
  auto xs = random_array1(K, RNG);
  auto ys = random_array1(K, RNG);

  std::array<long, 5> is = {1, 0, 2, 5, 1}; // i+j = 1(odd), 1(odd), 5(odd), 7(odd), 2(even control)
  std::array<long, 5> js = {0, 1, 3, 2, 1};
  for (int t = 0; t < 5; ++t) {
    long i = is[t], j = js[t];
    auto batch = D.insert_ratios(i, j, xs, ys);
    for (long m = 0; m < K; ++m) {
      auto ratio = D.try_insert(i, j, xs(m), ys(m));
      D.reject_last_try();
      assert_close(batch(m), ratio, PRECISION, "nonzero pos i=" + std::to_string(i) + " j=" + std::to_string(j) + " m=" + std::to_string(m));
    }
  }
  std::cerr << "PASSED" << std::endl;
}

int main() {
  // Original rank-1 tests
  test_rank1_batch_vs_sequential();
  test_rank1_state_unchanged();
  test_rank1_empty_matrix();
  test_rank1_single_point();
  test_rank1_empty_batch();
  test_cross_validate_rank1();

  // Rank-2 array tests
  test_rank2_array_insert_ratios();
  test_rank2_array_insert_ratios_cross_validate();

  // Position-dependent sign factor (odd parity)
  test_rank1_nonzero_position();

  std::cerr << "\nAll tests PASSED." << std::endl;
  return 0;
}
