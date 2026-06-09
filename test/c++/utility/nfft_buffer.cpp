#include <triqs/utility/nfft/buffer.hpp>
#include <random>
#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
#include <triqs/mesh/matsubara_freq.hpp>
#include <triqs/test_tools/gfs.hpp>

using namespace triqs::utility;
using namespace triqs::utility::nfft;

// Check that int holds at least 4 Bytes
static_assert(sizeof(int) >= sizeof(int32_t), " Error: sizeof(int) < 4 Byte ");

// Tolerance for FINUFFT in tests: tighter than the default 1e-8 to allow 1e-10 comparison thresholds
constexpr double test_tol = 1e-12;

/********************* Fixture Common to all TEST_F ********************/
class Nfft : public ::testing::Test {

  protected:
  // parameters
  double beta    = 20.0;
  int n_iw       = 100;

  // gf containers common to multiple tests
  std::array<long, 2> shape;
  gf<imfreq, matrix_valued> giw_exact;

  void SetUp() override {
    shape = std::array<long, 2>{1, 1};

    // Init exact reference gf
    nda::clef::placeholder<0> iw_;
    giw_exact = gf<imfreq, matrix_valued>{{beta, Fermion, n_iw}, shape};
    giw_exact(iw_) << 1.0 / (iw_ - 1.0);
  }

  // function to be transformed
  double f_tau(double tau) { return -std::exp(-tau) / (1 + std::exp(-beta)); } // bosonic: std::exp(-tau) (std::exp(-beta) - 1)
};

/********************* EQUIDISTANT TRANSFORM ********************/
TEST_F(Nfft, Equid) { // NOLINT

  // Parameters
  int n_tau    = 10000;
  int buf_size = n_tau;

  // Create container for Gf from equid nfft
  auto giw_nfft_equid = gf<imfreq, matrix_valued>{{beta, Fermion, n_iw}, shape};

  // nfft_buffer
  buffer_t<1> buf_equid(giw_nfft_equid.data()(range::all, 0, 0), buf_size, beta);

  // Generate data with equidistant \tau-grid (care for weights at beginning and end)
  buf_equid.push_back({0.0}, 0.5 * f_tau(0.0));
  for (int i = 1; i < n_tau - 1; ++i) {
    double tau = beta * i / (n_tau - 1);
    buf_equid.push_back({tau}, f_tau(tau));
  }
  buf_equid.push_back({beta - 1e-10}, 0.5 * f_tau(beta - 1e-10));

  // normalize, and care for half-points at 0^+ and \beta^-
  giw_nfft_equid *= beta / (n_tau - 1);

  // Generate Gf with fftw
  auto gtau    = gf<imtime, matrix_valued>{{beta, Fermion, n_tau}, shape};
  auto iw_mesh = make_adjoint_mesh(gtau.mesh(), n_iw);
  for (auto tau : gtau.mesh()) gtau[tau] = f_tau(tau);
  auto giw_fftw = make_gf_from_fourier(gtau, iw_mesh, make_zero_tail(gtau));

  // Compare to exact and fftw
  EXPECT_GF_NEAR(giw_nfft_equid, giw_exact, 1e-4); // Same order of fftw discretization error
  EXPECT_GF_NEAR(giw_nfft_equid, giw_fftw, 1e-7);  // Only small deviation due to truncation/oversampling factor (see Fig.3 Notes Josef)

  // -- Now check multiple transforms

  // Create container for Gf from multi nfft
  auto giw_nfft_multi = gf<imfreq, matrix_valued>{{beta, Fermion, n_iw}, shape};

  // nfft_buffer with size 4000 = 2 * buf_size / 5
  buffer_t<1> buf_multi(giw_nfft_multi.data()(range::all, 0, 0), 2 * buf_size / 5, beta);

  // Generate data with equidistant \tau-grid (care for weights at beginning and end)
  // Buffer performs multiple transforms as buf_size < n_tau
  buf_multi.push_back({0.0}, 0.5 * f_tau(0.0));
  for (int i = 1; i < n_tau - 1; ++i) {
    double tau = beta * i / (n_tau - 1);
    buf_multi.push_back({tau}, f_tau(tau));
  }
  buf_multi.push_back({beta - 1e-10}, 0.5 * f_tau(beta - 1e-10));

  // Care to flush remaining points since 10000 is not an even multiple of 4000
  buf_multi.flush();

  // normalize, and care for half-points at 0^+ and \beta^-
  giw_nfft_multi *= beta / (n_tau - 1);

  // Compare one-shot with multi-nfft
  EXPECT_GF_NEAR(giw_nfft_multi, giw_nfft_equid, 1e-6); // Small difference from FINUFFT internal precision at tol=1e-8

  // Write to file
  // h5::file h5file("nfft.equid.h5", 'w');
  // h5_write(h5file "giw_nfft_equid", giw_nfft_equid);
  // h5_write(h5file "giw_fftw", giw_fftw);
  // h5_write(h5file "giw_exact", giw_exact);
  // h5_write(h5file "giw_nfft_multi", giw_nfft_multi);
}

/*********************  NON-EQUIDISTANT TRANSFORM ********************/
TEST_F(Nfft, Rng) { // NOLINT

  // Parameters
  int n_tau    = 1e+6;
  int buf_size = n_tau;

  // std random generator
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  // Create container for Gf from nfft
  auto giw_nfft_rng = gf<imfreq, matrix_valued>{{beta, Fermion, n_iw}, shape};

  // nfft_buffer
  n_tau    = 1e+6;
  buf_size = n_tau;
  buffer_t<1> buf_rng(giw_nfft_rng.data()(range::all, 0, 0), buf_size, beta);

  // Generate values at random tau points
  for (int i = 0; i < n_tau; ++i) {
    double tau = double(distribution(generator)) * beta;
    buf_rng.push_back({tau}, f_tau(tau));
  }

  // normalize
  giw_nfft_rng *= beta / n_tau;

  // Compare
  EXPECT_GF_NEAR(giw_nfft_rng, giw_exact, 1e-2); // Expect a Monte-Carlo Error of order 1/sqrt(n_tau)

  // Write to file
  // h5::file h5file("nfft.rng.h5", 'w');
  // h5_write(h5file "giw_nfft_rng", giw_nfft_rng);
  // h5_write(h5file "giw_exact", giw_exact);
}

/*********************  2D NFFT TRANSFORM ********************/
TEST_F(Nfft, 2D) { // NOLINT

  // Parameters
  int n_tau    = 6 * n_iw + 1;
  int buf_size = n_tau * n_tau;

  // Create container for Gf from nfft
  auto giw_nfft_2d = gf<prod<imfreq, imfreq>>{{{beta, Fermion, n_iw}, {beta, Fermion, n_iw}}, shape};

  // nfft_buffer
  buffer_t<2> buf_2d(slice_target_to_scalar(giw_nfft_2d, 0, 0).data(), buf_size, beta);

  // ==== Generate 2d data with equidistant \tau-grids (care for weights at edges and corners)
  // Corner Points with weight 0.25
  buf_2d.push_back({0.0, 0.0}, 0.25 * f_tau(0.0) * f_tau(0.0));
  buf_2d.push_back({0.0, beta - 1e-10}, 0.25 * f_tau(0.0) * f_tau(beta - 1e-10));
  buf_2d.push_back({beta - 1e-10, 0.0}, 0.25 * f_tau(beta - 1e-10) * f_tau(0.0));
  buf_2d.push_back({beta - 1e-10, beta - 1e-10}, 0.25 * f_tau(beta - 1e-10) * f_tau(beta - 1e-10));
  for (int i = 1; i < n_tau - 1; ++i) {
    double tau_i = beta * i / (n_tau - 1);
    for (int j = 1; j < n_tau - 1; ++j) {
      double tau_j = beta * j / (n_tau - 1);
      // Core points with full weight
      buf_2d.push_back({tau_i, tau_j}, f_tau(tau_i) * f_tau(tau_j));
    }
    // Edge points with weight 0.5
    buf_2d.push_back({tau_i, 0.0}, 0.5 * f_tau(tau_i) * f_tau(0.0));
    buf_2d.push_back({tau_i, beta - 1e-10}, 0.5 * f_tau(tau_i) * f_tau(beta - 1e-10));
  }
  for (int j = 1; j < n_tau - 1; ++j) {
    double tau_j = beta * j / (n_tau - 1);
    // Edge points with weight 0.5
    buf_2d.push_back({0, tau_j}, 0.5 * f_tau(0) * f_tau(tau_j));
    buf_2d.push_back({beta - 1e-10, tau_j}, 0.5 * f_tau(beta - 1e-10) * f_tau(tau_j));
  }
  // ====

  // normalize, care for half-points at 0^+ and \beta^-
  giw_nfft_2d *= beta * beta / (n_tau - 1) / (n_tau - 1);

  // === Generate 2d Gf with fftw
  // Create 1d giw from fftw
  auto gtau2   = gf<imtime, matrix_valued>{{beta, Fermion, n_tau}, shape};
  auto iw_mesh = make_adjoint_mesh(gtau2.mesh(), n_iw);
  for (auto tau : gtau2.mesh()) gtau2[tau] = f_tau(tau);
  auto giw_fftw = make_gf_from_fourier(gtau2, iw_mesh, make_zero_tail(gtau2));
  // Create giw_fftw_2d from product of giw_fftw
  auto giw_fftw_2d = gf<prod<imfreq, imfreq>>{{{beta, Fermion, n_iw}, {beta, Fermion, n_iw}}, shape};
  for (auto iw1 : giw_fftw.mesh())
    for (auto iw2 : giw_fftw.mesh()) giw_fftw_2d[iw1, iw2] = giw_fftw[iw1] * giw_fftw[iw2];

  // Init exact reference gf
  nda::clef::placeholder<0> iw1_;
  nda::clef::placeholder<1> iw2_;
  auto giw_exact_2d = gf<prod<imfreq, imfreq>>{{{beta, Fermion, n_iw}, {beta, Fermion, n_iw}}, shape};
  giw_exact_2d(iw1_, iw2_) << 1.0 / (iw1_ - 1.0) / (iw2_ - 1.0);

  // Compare
  EXPECT_GF_NEAR(giw_nfft_2d, giw_exact_2d, 1e-2); // Same order of fftw discretization error
  EXPECT_GF_NEAR(giw_nfft_2d, giw_fftw_2d, 1e-8);  // Only small deviation due to truncation/oversampling factor (see Fig.3 Notes Josef)

  // Write to file
  // h5::file h5file("nfft.2d.h5", 'w');
  // h5_write(h5file "arr_nfft_2d", giw_nfft_2d.data());
  // h5_write(h5file "arr_fftw_2d", giw_fftw_2d.data());
  // h5_write(h5file "arr_exact_2d", giw_exact_2d.data());
}

/********************* TYPE 3: Analytical 1D ********************/
TEST_F(Nfft, Type3_Analytical_1D) { // NOLINT

  int n_tau    = 10000;
  int buf_size = n_tau;

  // Non-uniform target frequencies: a subset of Matsubara frequencies
  std::vector<int> n_indices = {0, 1, 3, 5, 10, 20, 50, 99, -1, -3, -10, -50, -100};
  int64_t n_targets          = n_indices.size();

  // Build target matsubara_freq vector
  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) target_mf.push_back(mesh::matsubara_freq(n_indices[k], beta, mesh::Fermion));

  // Output vector
  nda::vector<dcomplex> fiw_out(n_targets);
  fiw_out = 0;

  // Type 3 nfft buffer
  buffer_t<1> buf(fiw_out, target_mf, buf_size, test_tol, type_t::type3);

  // Generate equidistant tau data with trapezoidal weights
  buf.push_back({0.0}, 0.5 * f_tau(0.0));
  for (int i = 1; i < n_tau - 1; ++i) {
    double tau = beta * i / (n_tau - 1);
    buf.push_back({tau}, f_tau(tau));
  }
  buf.push_back({beta - 1e-10}, 0.5 * f_tau(beta - 1e-10));
  buf.flush();

  // Normalize
  fiw_out *= beta / (n_tau - 1);

  // Compare against exact G(iw) = 1/(iw - 1)
  for (int64_t k = 0; k < n_targets; ++k) {
    dcomplex iw    = dcomplex(0, (2 * n_indices[k] + 1) * M_PI / beta);
    dcomplex exact = 1.0 / (iw - 1.0);
    EXPECT_LT(std::abs(fiw_out(k) - exact), 1e-4) << "Failed at n=" << n_indices[k];
  }
}

/********************* TYPE 3: Random 1D with brute-force check ********************/
TEST_F(Nfft, Type3_Random_1D) { // NOLINT

  int n_tau    = 5000;
  int buf_size = n_tau;

  std::default_random_engine gen(42);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Non-uniform target frequencies
  std::vector<int> n_indices = {0, 2, 7, -4, -15, 30};
  int64_t n_targets          = n_indices.size();
  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) target_mf.push_back(mesh::matsubara_freq(n_indices[k], beta, mesh::Fermion));

  // Generate random tau values and strengths
  std::vector<double> taus(n_tau);
  std::vector<dcomplex> vals(n_tau);
  for (int i = 0; i < n_tau; ++i) {
    taus[i] = dist(gen) * beta;
    vals[i] = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
  }

  // Type 3 nfft buffer
  nda::vector<dcomplex> fiw_out(n_targets);
  fiw_out = 0;
  buffer_t<1> buf(fiw_out, target_mf, buf_size, test_tol, type_t::type3);

  for (int i = 0; i < n_tau; ++i) buf.push_back({taus[i]}, vals[i]);
  buf.flush();

  // Brute-force reference: f_k = sum_j c_j * exp(i * omega_k * tau_j)
  for (int64_t k = 0; k < n_targets; ++k) {
    double omega_k = std::imag(dcomplex(target_mf[k]));
    dcomplex ref   = 0;
    for (int j = 0; j < n_tau; ++j) ref += vals[j] * std::exp(dcomplex(0, omega_k * taus[j]));
    EXPECT_LT(std::abs(fiw_out(k) - ref), 1e-9 * std::abs(ref) + 1e-9) << "Failed at k=" << k;
  }
}

/********************* TYPE 3: Analytical 2D ********************/
TEST_F(Nfft, Type3_Analytical_2D) { // NOLINT

  int n_tau    = 601;
  int buf_size = n_tau * n_tau;

  // Non-uniform 2D target frequencies: subset of (omega_i, omega_j) pairs
  std::vector<int> n1_list = {0, 1, 5, -1, -5};
  std::vector<int> n2_list = {0, 2, -3, 10};
  int64_t n_targets        = n1_list.size() * n2_list.size();

  std::vector<std::array<mesh::matsubara_freq, 2>> target_mf;
  target_mf.reserve(n_targets);
  for (int n1 : n1_list)
    for (int n2 : n2_list)
      target_mf.push_back({mesh::matsubara_freq(n1, beta, mesh::Fermion), mesh::matsubara_freq(n2, beta, mesh::Fermion)});

  nda::vector<dcomplex> fiw_out(n_targets);
  fiw_out = 0;

  buffer_t<2> buf(fiw_out, target_mf, buf_size, test_tol, type_t::type3);

  // 2D equidistant tau with trapezoidal weights
  auto weight = [&](int i, int n) { return (i == 0 || i == n - 1) ? 0.5 : 1.0; };
  for (int i = 0; i < n_tau; ++i) {
    double tau_i = (i == n_tau - 1) ? beta - 1e-10 : beta * i / (n_tau - 1);
    for (int j = 0; j < n_tau; ++j) {
      double tau_j = (j == n_tau - 1) ? beta - 1e-10 : beta * j / (n_tau - 1);
      buf.push_back({tau_i, tau_j}, weight(i, n_tau) * weight(j, n_tau) * f_tau(tau_i) * f_tau(tau_j));
    }
  }
  buf.flush();

  fiw_out *= beta * beta / (n_tau - 1) / (n_tau - 1);

  // Compare against exact G(iw1, iw2) = 1/(iw1 - 1) * 1/(iw2 - 1)
  int64_t idx = 0;
  for (int n1 : n1_list)
    for (int n2 : n2_list) {
      dcomplex iw1   = dcomplex(0, (2 * n1 + 1) * M_PI / beta);
      dcomplex iw2   = dcomplex(0, (2 * n2 + 1) * M_PI / beta);
      dcomplex exact = 1.0 / (iw1 - 1.0) / (iw2 - 1.0);
      EXPECT_LT(std::abs(fiw_out(idx) - exact), 1e-2) << "Failed at n1=" << n1 << " n2=" << n2;
      ++idx;
    }
}

/********************* TYPE 3 vs TYPE 1: Consistency 1D ********************/
TEST_F(Nfft, Type3_vs_Type1_1D) { // NOLINT

  int n_tau    = 10000;
  int buf_size = n_tau;

  std::default_random_engine gen(123);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Type 1 buffer with 2*n_iw uniform Matsubara frequencies
  auto giw_type1 = gf<imfreq, matrix_valued>{{beta, Fermion, n_iw}, shape};
  buffer_t<1> buf1(giw_type1.data()(range::all, 0, 0), buf_size, beta, test_tol);

  // Type 3 buffer targeting all uniform Matsubara frequencies omega_n = (2n+1)*pi/beta, n = -n_iw,...,n_iw-1
  int64_t n_targets = 2 * n_iw;
  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) {
    int n = static_cast<int>(k) - n_iw;
    target_mf.push_back(mesh::matsubara_freq(n, beta, mesh::Fermion));
  }

  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Push same random data to both
  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf1.push_back({tau}, fv);
    buf3.push_back({tau}, fv);
  }
  buf1.flush();
  buf3.flush();

  // Compare outputs
  auto type1_data = giw_type1.data()(range::all, 0, 0);
  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - type1_data(k)), 1e-9); }
}

/********************* TYPE 3 vs TYPE 1: Consistency 2D ********************/
TEST_F(Nfft, Type3_vs_Type1_2D) { // NOLINT

  int small_niw = 10;
  int n_tau     = 5000;
  int buf_size  = n_tau;

  std::default_random_engine gen(456);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Type 1: 2D uniform grid
  auto giw_type1 = gf<prod<imfreq, imfreq>>{{{beta, Fermion, small_niw}, {beta, Fermion, small_niw}}, shape};
  buffer_t<2> buf1(slice_target_to_scalar(giw_type1, 0, 0).data(), buf_size, beta, test_tol);

  // Type 3: all (omega_n1, omega_n2) pairs for n1,n2 in [-small_niw, small_niw)
  int64_t n_per_dim = 2 * small_niw;
  int64_t n_targets = n_per_dim * n_per_dim;
  std::vector<std::array<mesh::matsubara_freq, 2>> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k1 = 0; k1 < n_per_dim; ++k1)
    for (int64_t k2 = 0; k2 < n_per_dim; ++k2) {
      int n1 = static_cast<int>(k1) - small_niw;
      int n2 = static_cast<int>(k2) - small_niw;
      target_mf.push_back({mesh::matsubara_freq(n1, beta, mesh::Fermion), mesh::matsubara_freq(n2, beta, mesh::Fermion)});
    }

  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<2> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  for (int i = 0; i < n_tau; ++i) {
    double tau1 = dist(gen) * beta;
    double tau2 = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf1.push_back({tau1, tau2}, fv);
    buf3.push_back({tau1, tau2}, fv);
  }
  buf1.flush();
  buf3.flush();

  // Compare: type 3 output stored in row-major order (k1, k2) matches type 1 grid indexing
  auto type1_data = slice_target_to_scalar(giw_type1, 0, 0).data();
  int64_t idx     = 0;
  for (int64_t k1 = 0; k1 < n_per_dim; ++k1)
    for (int64_t k2 = 0; k2 < n_per_dim; ++k2) { EXPECT_LT(std::abs(fiw_type3(idx++) - type1_data(k1, k2)), 1e-9); }
}

/********************* TYPE 3 vs TYPE 1: Consistency 3D ********************/
TEST_F(Nfft, Type3_vs_Type1_3D) { // NOLINT

  int small_niw = 5;
  int n_tau     = 5000;
  int buf_size  = n_tau;

  std::default_random_engine gen(789);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Type 1: 3D uniform grid
  auto giw_type1 =
     gf<prod<imfreq, imfreq, imfreq>>{{{beta, Fermion, small_niw}, {beta, Fermion, small_niw}, {beta, Fermion, small_niw}}, shape};
  buffer_t<3> buf1(slice_target_to_scalar(giw_type1, 0, 0).data(), buf_size, beta, test_tol);

  // Type 3: all (omega_n1, omega_n2, omega_n3) triples
  int64_t n_per_dim = 2 * small_niw;
  int64_t n_targets = n_per_dim * n_per_dim * n_per_dim;
  std::vector<std::array<mesh::matsubara_freq, 3>> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k1 = 0; k1 < n_per_dim; ++k1)
    for (int64_t k2 = 0; k2 < n_per_dim; ++k2)
      for (int64_t k3 = 0; k3 < n_per_dim; ++k3) {
        int n1 = static_cast<int>(k1) - small_niw;
        int n2 = static_cast<int>(k2) - small_niw;
        int n3 = static_cast<int>(k3) - small_niw;
        target_mf.push_back({mesh::matsubara_freq(n1, beta, mesh::Fermion), mesh::matsubara_freq(n2, beta, mesh::Fermion),
                             mesh::matsubara_freq(n3, beta, mesh::Fermion)});
      }

  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<3> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  for (int i = 0; i < n_tau; ++i) {
    double tau1 = dist(gen) * beta;
    double tau2 = dist(gen) * beta;
    double tau3 = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf1.push_back({tau1, tau2, tau3}, fv);
    buf3.push_back({tau1, tau2, tau3}, fv);
  }
  buf1.flush();
  buf3.flush();

  // Compare
  auto type1_data = slice_target_to_scalar(giw_type1, 0, 0).data();
  int64_t idx     = 0;
  for (int64_t k1 = 0; k1 < n_per_dim; ++k1)
    for (int64_t k2 = 0; k2 < n_per_dim; ++k2)
      for (int64_t k3 = 0; k3 < n_per_dim; ++k3) { EXPECT_LT(std::abs(fiw_type3(idx++) - type1_data(k1, k2, k3)), 1e-9); }
}

/********************* TYPE1_GATHER vs TYPE 3: Consistency 1D ********************/
TEST_F(Nfft, Type1Gather_vs_Type3_1D) { // NOLINT

  int n_tau    = 10000;
  int buf_size = n_tau;

  std::default_random_engine gen(777);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Non-uniform targets (subset of Matsubara frequencies)
  std::vector<int> n_indices = {0, 1, 3, 5, 10, 20, 50, 99, -1, -3, -10, -50, -100};
  int64_t n_targets          = n_indices.size();

  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) target_mf.push_back(mesh::matsubara_freq(n_indices[k], beta, mesh::Fermion));

  // Type 3 reference
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Type1 gather
  nda::vector<dcomplex> fiw_gather(n_targets);
  fiw_gather = 0;
  buffer_t<1> bufg(fiw_gather, target_mf, buf_size, test_tol, type_t::type1_gather);

  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau}, fv);
    bufg.push_back({tau}, fv);
  }
  buf3.flush();
  bufg.flush();

  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - fiw_gather(k)), 1e-9); }
}

/********************* TYPE1_GATHER vs TYPE 3: Consistency 2D ********************/
TEST_F(Nfft, Type1Gather_vs_Type3_2D) { // NOLINT

  int small_niw = 10;
  int n_tau     = 5000;
  int buf_size  = n_tau;

  std::default_random_engine gen(888);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  int64_t n_per_dim = 2 * small_niw;
  int64_t n_targets = n_per_dim * n_per_dim;
  std::vector<std::array<mesh::matsubara_freq, 2>> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k1 = 0; k1 < n_per_dim; ++k1)
    for (int64_t k2 = 0; k2 < n_per_dim; ++k2) {
      int n1 = static_cast<int>(k1) - small_niw;
      int n2 = static_cast<int>(k2) - small_niw;
      target_mf.push_back({mesh::matsubara_freq(n1, beta, mesh::Fermion), mesh::matsubara_freq(n2, beta, mesh::Fermion)});
    }

  // Type 3 reference
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<2> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Type1 gather
  nda::vector<dcomplex> fiw_gather(n_targets);
  fiw_gather = 0;
  buffer_t<2> bufg(fiw_gather, target_mf, buf_size, test_tol, type_t::type1_gather);

  for (int i = 0; i < n_tau; ++i) {
    double tau1 = dist(gen) * beta;
    double tau2 = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau1, tau2}, fv);
    bufg.push_back({tau1, tau2}, fv);
  }
  buf3.flush();
  bufg.flush();

  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - fiw_gather(k)), 1e-9); }
}

/********************* TYPE1_GATHER vs TYPE 3: DLR2D mesh ********************/
TEST_F(Nfft, Type1Gather_vs_Type3_DLR2D) { // NOLINT

  int n_tau    = 5000;
  int buf_size = n_tau;

  std::default_random_engine gen(999);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  mesh::dlr2d_imfreq dlr2d_mesh{beta, /*dlr_wmax=*/1.0, /*dlr_eps=*/1e-6, mesh::PH};
  int64_t n_targets = dlr2d_mesh.size();

  std::vector<std::array<mesh::matsubara_freq, 2>> target_mf;
  target_mf.reserve(n_targets);
  for (long d = 0; d < n_targets; ++d) {
    auto [n1, n2] = dlr2d_mesh.to_index(d);
    target_mf.push_back({mesh::matsubara_freq(n2, beta, mesh::Fermion), mesh::matsubara_freq(n1, beta, mesh::Fermion)});
  }

  // Type 3 reference
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<2> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Type1 gather
  nda::vector<dcomplex> fiw_gather(n_targets);
  fiw_gather = 0;
  buffer_t<2> bufg(fiw_gather, target_mf, buf_size, test_tol, type_t::type1_gather);

  for (int i = 0; i < n_tau; ++i) {
    double tau1 = dist(gen) * beta;
    double tau2 = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau1, tau2}, fv);
    bufg.push_back({tau1, tau2}, fv);
  }
  buf3.flush();
  bufg.flush();

  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - fiw_gather(k)), 1e-9); }
}

/********************* DIRECT: Analytical 1D ********************/
void run_direct_analytical_1d(type_t type, double beta, auto f_tau) {

  int n_tau    = 10000;
  int buf_size = n_tau;

  // Non-uniform target frequencies: a subset of Matsubara frequencies
  std::vector<int> n_indices = {0, 1, 3, 5, 10, 20, 50, 99, -1, -3, -10, -50, -100};
  int64_t n_targets          = n_indices.size();

  // Build target matsubara_freq vector
  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) target_mf.push_back(mesh::matsubara_freq(n_indices[k], beta, mesh::Fermion));

  // Output vector
  nda::vector<dcomplex> fiw_out(n_targets);
  fiw_out = 0;

  buffer_t<1> buf(fiw_out, target_mf, buf_size, test_tol,type);

  // Generate equidistant tau data with trapezoidal weights
  buf.push_back({0.0}, 0.5 * f_tau(0.0));
  for (int i = 1; i < n_tau - 1; ++i) {
    double tau = beta * i / (n_tau - 1);
    buf.push_back({tau}, f_tau(tau));
  }
  buf.push_back({beta - 1e-10}, 0.5 * f_tau(beta - 1e-10));
  buf.flush();

  // Normalize
  fiw_out *= beta / (n_tau - 1);

  // Compare against exact G(iw) = 1/(iw - 1)
  for (int64_t k = 0; k < n_targets; ++k) {
    dcomplex iw    = dcomplex(0, (2 * n_indices[k] + 1) * M_PI / beta);
    dcomplex exact = 1.0 / (iw - 1.0);
    EXPECT_LT(std::abs(fiw_out(k) - exact), 1e-4) << "Failed at n=" << n_indices[k];
  }
}

TEST_F(Nfft, DirectType1_Analytical_1D) { run_direct_analytical_1d(type_t::direct_type1, beta, [&](double tau) { return f_tau(tau); }); }
// Bitwise kernel removed
//TEST_F(Nfft, DirectBitwise_Analytical_1D) {
//  run_direct_analytical_1d(type_t::direct_bitwise, beta, [&](double tau) { return f_tau(tau); });
//}
TEST_F(Nfft, DirectType3_Analytical_1D) {
  run_direct_analytical_1d(type_t::direct_type3, beta, [&](double tau) { return f_tau(tau); });
}
TEST_F(Nfft, DirectChain_Analytical_1D) {
  run_direct_analytical_1d(type_t::direct_chain, beta, [&](double tau) { return f_tau(tau); });
}

/********************* DIRECT: Analytical 2D ********************/
void run_direct_analytical_2d(type_t type, double beta, auto f_tau) {

  int n_tau    = 601;
  int buf_size = n_tau * n_tau;

  // Non-uniform 2D target frequencies: subset of (omega_i, omega_j) pairs
  std::vector<int> n1_list = {0, 1, 5, -1, -5};
  std::vector<int> n2_list = {0, 2, -3, 10};
  int64_t n_targets        = n1_list.size() * n2_list.size();

  std::vector<std::array<mesh::matsubara_freq, 2>> target_mf;
  target_mf.reserve(n_targets);
  for (int n1 : n1_list)
    for (int n2 : n2_list)
      target_mf.push_back({mesh::matsubara_freq(n1, beta, mesh::Fermion), mesh::matsubara_freq(n2, beta, mesh::Fermion)});

  nda::vector<dcomplex> fiw_out(n_targets);
  fiw_out = 0;

  buffer_t<2> buf(fiw_out, target_mf, buf_size, test_tol,type);

  // 2D equidistant tau with trapezoidal weights
  auto weight = [&](int i, int n) { return (i == 0 || i == n - 1) ? 0.5 : 1.0; };
  for (int i = 0; i < n_tau; ++i) {
    double tau_i = (i == n_tau - 1) ? beta - 1e-10 : beta * i / (n_tau - 1);
    for (int j = 0; j < n_tau; ++j) {
      double tau_j = (j == n_tau - 1) ? beta - 1e-10 : beta * j / (n_tau - 1);
      buf.push_back({tau_i, tau_j}, weight(i, n_tau) * weight(j, n_tau) * f_tau(tau_i) * f_tau(tau_j));
    }
  }
  buf.flush();

  fiw_out *= beta * beta / (n_tau - 1) / (n_tau - 1);

  // Compare against exact G(iw1, iw2) = 1/(iw1 - 1) * 1/(iw2 - 1)
  int64_t idx = 0;
  for (int n1 : n1_list)
    for (int n2 : n2_list) {
      dcomplex iw1   = dcomplex(0, (2 * n1 + 1) * M_PI / beta);
      dcomplex iw2   = dcomplex(0, (2 * n2 + 1) * M_PI / beta);
      dcomplex exact = 1.0 / (iw1 - 1.0) / (iw2 - 1.0);
      EXPECT_LT(std::abs(fiw_out(idx) - exact), 1e-2) << "Failed at n1=" << n1 << " n2=" << n2;
      ++idx;
    }
}

TEST_F(Nfft, DirectType1_Analytical_2D) { run_direct_analytical_2d(type_t::direct_type1, beta, [&](double tau) { return f_tau(tau); }); }
TEST_F(Nfft, DirectChain_Analytical_2D) {
  run_direct_analytical_2d(type_t::direct_chain, beta, [&](double tau) { return f_tau(tau); });
}
TEST_F(Nfft, DirectType3_Analytical_2D) {
  run_direct_analytical_2d(type_t::direct_type3, beta, [&](double tau) { return f_tau(tau); });
}

/********************* DIRECT vs TYPE 3: Consistency 1D ********************/
void run_direct_vs_type3_1d(type_t direct_type, double beta, int n_iw) {

  int n_tau    = 10000;
  int buf_size = n_tau;

  std::default_random_engine gen(111);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Target all uniform Matsubara frequencies
  int64_t n_targets = 2 * n_iw;
  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) {
    int n = static_cast<int>(k) - n_iw;
    target_mf.push_back(mesh::matsubara_freq(n, beta, mesh::Fermion));
  }

  // Type 3 buffer
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Direct buffer
  nda::vector<dcomplex> fiw_direct(n_targets);
  fiw_direct = 0;
  buffer_t<1> bufd(fiw_direct, target_mf, buf_size, test_tol,direct_type);

  // Push same random data to both
  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau}, fv);
    bufd.push_back({tau}, fv);
  }
  buf3.flush();
  bufd.flush();

  // Compare outputs
  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - fiw_direct(k)), 1e-9); }
}

TEST_F(Nfft, DirectType1_vs_Type3_1D) { run_direct_vs_type3_1d(type_t::direct_type1, beta, n_iw); }
// Bitwise kernel removed
//TEST_F(Nfft, DirectBitwise_vs_Type3_1D) { run_direct_vs_type3_1d(type_t::direct_bitwise, beta, n_iw); }
TEST_F(Nfft, DirectType3_vs_Type3_1D) { run_direct_vs_type3_1d(type_t::direct_type3, beta, n_iw); }
TEST_F(Nfft, DirectChain_vs_Type3_1D) { run_direct_vs_type3_1d(type_t::direct_chain, beta, n_iw); }

/********************* DIRECT vs TYPE 3: Consistency 2D ********************/
void run_direct_vs_type3_2d(type_t direct_type, double beta) {

  int small_niw = 10;
  int n_tau     = 5000;
  int buf_size  = n_tau;

  std::default_random_engine gen(222);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Build 2D target
  int64_t n_per_dim = 2 * small_niw;
  int64_t n_targets = n_per_dim * n_per_dim;
  std::vector<std::array<mesh::matsubara_freq, 2>> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k1 = 0; k1 < n_per_dim; ++k1)
    for (int64_t k2 = 0; k2 < n_per_dim; ++k2) {
      int n1 = static_cast<int>(k1) - small_niw;
      int n2 = static_cast<int>(k2) - small_niw;
      target_mf.push_back({mesh::matsubara_freq(n1, beta, mesh::Fermion), mesh::matsubara_freq(n2, beta, mesh::Fermion)});
    }

  // Type 3 buffer
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<2> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Direct buffer
  nda::vector<dcomplex> fiw_direct(n_targets);
  fiw_direct = 0;
  buffer_t<2> bufd(fiw_direct, target_mf, buf_size, test_tol,direct_type);

  for (int i = 0; i < n_tau; ++i) {
    double tau1 = dist(gen) * beta;
    double tau2 = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau1, tau2}, fv);
    bufd.push_back({tau1, tau2}, fv);
  }
  buf3.flush();
  bufd.flush();

  // Compare
  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - fiw_direct(k)), 1e-9); }
}

TEST_F(Nfft, DirectType1_vs_Type3_2D) { run_direct_vs_type3_2d(type_t::direct_type1, beta); }
TEST_F(Nfft, DirectChain_vs_Type3_2D) { run_direct_vs_type3_2d(type_t::direct_chain, beta); }
TEST_F(Nfft, DirectType3_vs_Type3_2D) { run_direct_vs_type3_2d(type_t::direct_type3, beta); }

/********************* STRIDED OUTPUT: Direct vs Type3 with strided view ********************/
// Reproduces the M_iw.cpp pattern: output is a strided slice of a 3D array.
void run_direct_vs_type3_strided(type_t direct_type, double beta, int n_iw) {

  int n_tau    = 10000;
  int buf_size = n_tau;
  int bl_size  = 4; // matrix dimension, giving stride = bl_size^2 = 16

  std::default_random_engine gen(333);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Target Matsubara frequencies
  int64_t n_targets = 2 * n_iw;
  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) {
    int n = static_cast<int>(k) - n_iw;
    target_mf.push_back(mesh::matsubara_freq(n, beta, mesh::Fermion));
  }

  // Type 3 with contiguous output (reference)
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Direct with strided output: slice of a 3D array (n_targets, bl_size, bl_size)
  nda::array<dcomplex, 3> M_data(n_targets, bl_size, bl_size);
  M_data            = 0;
  auto strided_view = M_data(nda::range::all, 1, 2); // stride = bl_size * bl_size = 16
  buffer_t<1> bufd(strided_view, target_mf, buf_size, test_tol,direct_type);

  // Push same random data to both
  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau}, fv);
    bufd.push_back({tau}, fv);
  }
  buf3.flush();
  bufd.flush();

  // Compare strided output against contiguous type3 reference
  for (int64_t k = 0; k < n_targets; ++k) {
    EXPECT_LT(std::abs(fiw_type3(k) - strided_view(k)), 1e-9) << "Strided output mismatch at k=" << k << " (stride=" << bl_size * bl_size << ")";
  }
}

TEST_F(Nfft, DirectType1_Strided) { run_direct_vs_type3_strided(type_t::direct_type1, beta, n_iw); }
// Bitwise kernel removed
//TEST_F(Nfft, DirectBitwise_Strided) { run_direct_vs_type3_strided(type_t::direct_bitwise, beta, n_iw); }
TEST_F(Nfft, DirectChain_Strided) { run_direct_vs_type3_strided(type_t::direct_chain, beta, n_iw); }
TEST_F(Nfft, DirectType3_Strided) { run_direct_vs_type3_strided(type_t::direct_type3, beta, n_iw); }
TEST_F(Nfft, Automatic_Strided) { run_direct_vs_type3_strided(type_t::automatic, beta, n_iw); }

/********************* DLR-SCALE: Direct vs Type3 with large Matsubara indices ********************/
// Uses DLR-like parameters (beta=100, large indices up to |2n+1|=1501) to exercise
// deep power-of-two tables and NAF decompositions.
void run_direct_vs_type3_dlr(type_t direct_type) {

  double dlr_beta = 100.0;
  int n_tau       = 5000;
  int buf_size    = n_tau;

  std::default_random_engine gen(444);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // DLR-like target frequencies with large indices (mimics plaquette: beta=100, wmax=10, eps=1e-10)
  std::vector<int> n_indices = {-751, -368, -232, -162, -124, -97, -79, -66, -48, -40, -34, -26, -22, -19, -14, -12, -10,
                                -9,   -8,   -7,   -6,   -5,   -4,  -3,  -2,  -1,  0,   1,   2,   3,   4,   5,   6,   7,
                                8,    9,    12,   15,   18,   25,  33,  39,  47,  63,  79,  102, 134, 183, 384, 685};
  int64_t n_targets          = static_cast<int64_t>(n_indices.size());

  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int n : n_indices) target_mf.push_back(mesh::matsubara_freq(n, dlr_beta, mesh::Fermion));

  // Type 3 (reference)
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Direct
  nda::vector<dcomplex> fiw_direct(n_targets);
  fiw_direct = 0;
  buffer_t<1> bufd(fiw_direct, target_mf, buf_size, test_tol,direct_type);

  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * dlr_beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau}, fv);
    bufd.push_back({tau}, fv);
  }
  buf3.flush();
  bufd.flush();

  for (int64_t k = 0; k < n_targets; ++k) {
    EXPECT_LT(std::abs(fiw_type3(k) - fiw_direct(k)), 1e-9)
       << "DLR-scale mismatch at n=" << n_indices[k] << " (|2n+1|=" << std::abs(2 * n_indices[k] + 1) << ")";
  }
}

TEST_F(Nfft, DirectType1_DLR) { run_direct_vs_type3_dlr(type_t::direct_type1); }
// Bitwise kernel removed
//TEST_F(Nfft, DirectBitwise_DLR) { run_direct_vs_type3_dlr(type_t::direct_bitwise); }
TEST_F(Nfft, DirectChain_DLR) { run_direct_vs_type3_dlr(type_t::direct_chain); }
TEST_F(Nfft, DirectType3_DLR) { run_direct_vs_type3_dlr(type_t::direct_type3); }
TEST_F(Nfft, Automatic_DLR) { run_direct_vs_type3_dlr(type_t::automatic); }

/********************* ODD BUFFER COUNT: Exercises scalar tail path ********************/
// Uses a buffer size that forces flushes with odd element counts (buf_counter % simd_size != 0),
// triggering the scalar tail code path in all direct kernels.
void run_direct_vs_type3_odd_flush(type_t direct_type, double beta) {

  int n_tau    = 307; // prime number, guarantees odd buf_counter on flush
  int buf_size = 101; // prime buffer size, forces multiple partial flushes

  std::default_random_engine gen(555);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  std::vector<int> n_indices = {0, 1, 5, -1, -10, 50, -50};
  int64_t n_targets          = static_cast<int64_t>(n_indices.size());

  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int n : n_indices) target_mf.push_back(mesh::matsubara_freq(n, beta, mesh::Fermion));

  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  nda::vector<dcomplex> fiw_direct(n_targets);
  fiw_direct = 0;
  buffer_t<1> bufd(fiw_direct, target_mf, buf_size, test_tol,direct_type);

  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau}, fv);
    bufd.push_back({tau}, fv);
  }
  buf3.flush();
  bufd.flush();

  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - fiw_direct(k)), 1e-9) << "Odd-flush mismatch at n=" << n_indices[k]; }
}

TEST_F(Nfft, DirectType1_OddFlush) { run_direct_vs_type3_odd_flush(type_t::direct_type1, beta); }
// Bitwise kernel removed
//TEST_F(Nfft, DirectBitwise_OddFlush) { run_direct_vs_type3_odd_flush(type_t::direct_bitwise, beta); }
TEST_F(Nfft, DirectChain_OddFlush) { run_direct_vs_type3_odd_flush(type_t::direct_chain, beta); }
TEST_F(Nfft, DirectType3_OddFlush) { run_direct_vs_type3_odd_flush(type_t::direct_type3, beta); }
TEST_F(Nfft, Automatic_OddFlush) { run_direct_vs_type3_odd_flush(type_t::automatic, beta); }

/********************* AUTOMATIC: Dispatch correctness ********************/
// Verifies automatic mode matches type3 for both small and large buffer counts.
TEST_F(Nfft, Automatic_vs_Type3_1D) { run_direct_vs_type3_1d(type_t::automatic, beta, n_iw); }

/********************* COMBINED: Strided + DLR-scale + odd flush ********************/
// Combines all three failure modes: strided output, large indices, and odd buffer count.
TEST_F(Nfft, DirectType3_Strided_DLR_OddFlush) { // NOLINT

  double dlr_beta = 100.0;
  int n_tau       = 307;
  int buf_size    = 101;
  int bl_size     = 4;

  std::default_random_engine gen(666);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // DLR-like targets
  std::vector<int> n_indices = {-751, -368, -97, -48, -7, -1, 0, 1, 7, 102, 384, 685};
  int64_t n_targets          = static_cast<int64_t>(n_indices.size());

  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int n : n_indices) target_mf.push_back(mesh::matsubara_freq(n, dlr_beta, mesh::Fermion));

  // Type 3 contiguous reference
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // NAF with strided output
  nda::array<dcomplex, 3> M_data(n_targets, bl_size, bl_size);
  M_data            = 0;
  auto strided_view = M_data(nda::range::all, 2, 3);
  buffer_t<1> bufd(strided_view, target_mf, buf_size, test_tol, type_t::automatic);

  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * dlr_beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau}, fv);
    bufd.push_back({tau}, fv);
  }
  buf3.flush();
  bufd.flush();

  for (int64_t k = 0; k < n_targets; ++k) {
    EXPECT_LT(std::abs(fiw_type3(k) - strided_view(k)), 1e-9) << "Combined strided+DLR+odd mismatch at n=" << n_indices[k];
  }
}

/********************* kernel vs TYPE 3: DLR2D mesh ********************/
void run_vs_type3_dlr2d(type_t test_type, double beta, bool compressgrid = false) {

  int n_tau    = 5000;
  int buf_size = n_tau;

  std::default_random_engine gen(333);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Build DLR2D mesh and extract target matsubara_freq
  mesh::dlr2d_imfreq dlr2d_mesh{beta, /*dlr_wmax=*/1.0, /*dlr_eps=*/1e-6, mesh::PH, compressgrid};
  int64_t n_targets = dlr2d_mesh.size();

  std::vector<std::array<mesh::matsubara_freq, 2>> target_mf;
  target_mf.reserve(n_targets);
  for (long d = 0; d < n_targets; ++d) {
    auto [n1, n2] = dlr2d_mesh.to_index(d);
    target_mf.push_back({mesh::matsubara_freq(n2, beta, mesh::Fermion), mesh::matsubara_freq(n1, beta, mesh::Fermion)});
  }

  // Type 3 buffer (reference)
  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<2> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  // Test buffer
  nda::vector<dcomplex> fiw_test(n_targets);
  fiw_test = 0;
  buffer_t<2> buft(fiw_test, target_mf, buf_size, test_tol, test_type);

  for (int i = 0; i < n_tau; ++i) {
    double tau1 = dist(gen) * beta;
    double tau2 = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf3.push_back({tau1, tau2}, fv);
    buft.push_back({tau1, tau2}, fv);
  }
  buf3.flush();
  buft.flush();

  // Compare
  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - fiw_test(k)), 1e-9); }
}

TEST_F(Nfft, DirectType1_DLR2D) { run_vs_type3_dlr2d(type_t::direct_type1, beta); }
TEST_F(Nfft, DirectChain_DLR2D) { run_vs_type3_dlr2d(type_t::direct_chain, beta); }
TEST_F(Nfft, DirectType3_DLR2D) { run_vs_type3_dlr2d(type_t::direct_type3, beta); }
TEST_F(Nfft, Automatic_DLR2D) { run_vs_type3_dlr2d(type_t::automatic, beta); }

TEST_F(Nfft, DirectType3_DLR2D_CG) { run_vs_type3_dlr2d(type_t::direct_type3, beta, /*compressgrid=*/true); }
TEST_F(Nfft, Automatic_DLR2D_CG) { run_vs_type3_dlr2d(type_t::automatic, beta, /*compressgrid=*/true); }

/********************* TYPE 1 AUTOMATIC DISPATCH: 1D ********************/
// Verifies that type1 with automatic dispatch matches type3 reference (which uses a different code path).
TEST_F(Nfft, Type1_AutoDispatch_1D) { // NOLINT

  int n_tau    = 10000;
  int buf_size = n_tau;

  std::default_random_engine gen(1234);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Type 1 buffer (now with automatic dispatch)
  auto giw_type1 = gf<imfreq, matrix_valued>{{beta, Fermion, n_iw}, shape};
  buffer_t<1> buf1(giw_type1.data()(range::all, 0, 0), buf_size, beta, test_tol);

  // Type 3 buffer targeting the same uniform frequencies as reference
  int64_t n_targets = 2 * n_iw;
  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) {
    int n = static_cast<int>(k) - n_iw;
    target_mf.push_back(mesh::matsubara_freq(n, beta, mesh::Fermion));
  }

  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf1.push_back({tau}, fv);
    buf3.push_back({tau}, fv);
  }
  buf1.flush();
  buf3.flush();

  auto type1_data = giw_type1.data()(range::all, 0, 0);
  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - type1_data(k)), 1e-9); }
}

/********************* TYPE 1 AUTOMATIC DISPATCH: 2D ********************/
TEST_F(Nfft, Type1_AutoDispatch_2D) { // NOLINT

  int small_niw = 10;
  int n_tau     = 5000;
  int buf_size  = n_tau;

  std::default_random_engine gen(5678);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  auto giw_type1 = gf<prod<imfreq, imfreq>>{{{beta, Fermion, small_niw}, {beta, Fermion, small_niw}}, shape};
  buffer_t<2> buf1(slice_target_to_scalar(giw_type1, 0, 0).data(), buf_size, beta, test_tol);

  int64_t n_per_dim = 2 * small_niw;
  int64_t n_targets = n_per_dim * n_per_dim;
  std::vector<std::array<mesh::matsubara_freq, 2>> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k1 = 0; k1 < n_per_dim; ++k1)
    for (int64_t k2 = 0; k2 < n_per_dim; ++k2) {
      int n1 = static_cast<int>(k1) - small_niw;
      int n2 = static_cast<int>(k2) - small_niw;
      target_mf.push_back({mesh::matsubara_freq(n1, beta, mesh::Fermion), mesh::matsubara_freq(n2, beta, mesh::Fermion)});
    }

  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<2> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  for (int i = 0; i < n_tau; ++i) {
    double tau1 = dist(gen) * beta;
    double tau2 = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf1.push_back({tau1, tau2}, fv);
    buf3.push_back({tau1, tau2}, fv);
  }
  buf1.flush();
  buf3.flush();

  auto type1_data = slice_target_to_scalar(giw_type1, 0, 0).data();
  int64_t idx     = 0;
  for (int64_t k1 = 0; k1 < n_per_dim; ++k1)
    for (int64_t k2 = 0; k2 < n_per_dim; ++k2) { EXPECT_LT(std::abs(fiw_type3(idx++) - type1_data(k1, k2)), 1e-9); }
}

/********************* TYPE 1 AUTOMATIC DISPATCH: Small buffer forcing partial flushes ********************/
TEST_F(Nfft, Type1_AutoDispatch_SmallFlush) { // NOLINT

  int n_tau    = 1000;
  int buf_size = 37; // small prime buffer size forces many partial flushes, exercising NAF path

  std::default_random_engine gen(9012);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  auto giw_type1 = gf<imfreq, matrix_valued>{{beta, Fermion, n_iw}, shape};
  buffer_t<1> buf1(giw_type1.data()(range::all, 0, 0), buf_size, beta, test_tol);

  int64_t n_targets = 2 * n_iw;
  std::vector<mesh::matsubara_freq> target_mf;
  target_mf.reserve(n_targets);
  for (int64_t k = 0; k < n_targets; ++k) target_mf.push_back(mesh::matsubara_freq(static_cast<int>(k) - n_iw, beta, mesh::Fermion));

  nda::vector<dcomplex> fiw_type3(n_targets);
  fiw_type3 = 0;
  buffer_t<1> buf3(fiw_type3, target_mf, buf_size, test_tol, type_t::type3);

  for (int i = 0; i < n_tau; ++i) {
    double tau  = dist(gen) * beta;
    dcomplex fv = dcomplex(dist(gen) - 0.5, dist(gen) - 0.5);
    buf1.push_back({tau}, fv);
    buf3.push_back({tau}, fv);
  }
  buf1.flush();
  buf3.flush();

  auto type1_data = giw_type1.data()(range::all, 0, 0);
  for (int64_t k = 0; k < n_targets; ++k) { EXPECT_LT(std::abs(fiw_type3(k) - type1_data(k)), 1e-9); }
}

MAKE_MAIN;
