// Copyright (c) 2022-2023 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Nils Wentzell

#include <triqs/mesh/imfreq.hpp>
#include <triqs/mesh/refreq.hpp>
#include <triqs/mesh/tail_fitter.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <cmath>
#include <complex>
#include <numbers>
#include <optional>
#include <vector>

// Functors for the function f(z) = 1 / (z - a) with some constant number or array a.
// The large-z expansion of f(z) is f(z) = 1 / z - a / z^2 + a^2 / z^3 - a^3 / z^4 + ...
struct scalar_f {
  std::complex<double> a;
  [[nodiscard]] auto operator()(auto z) const { return 1 / (z - a); }
  [[nodiscard]] auto expansion_coeffs(long n) const {
    auto M = nda::array<std::complex<double>, 1>::zeros(n);
    for (auto i : nda::range(1, n)) M(i) = std::pow(a, i - 1);
    return M;
  }
};

struct array_f {
  nda::array<std::complex<double>, 2> a;
  [[nodiscard]] auto operator()(auto z) const { return 1 / (z - a); }
  [[nodiscard]] auto expansion_coeffs(long n) const {
    auto M = nda::array<std::complex<double>, 3>::zeros(n, a.shape()[0], a.shape()[1]);
    for (auto i : nda::range(1, n)) M(i, nda::ellipsis{}) = nda::pow(a, static_cast<double>(i - 1));
    return M;
  }
};

// Test the tail fitter for a scalar-valued function.
template <bool do_hermitian>
void test_scalar(scalar_f const &f, auto const &m, double r, int n_max, std::optional<int> max_order = {}, double tol = 1e-5) {
  // tail fitter object
  auto tf = triqs::mesh::tail_fitter{r, n_max, max_order};

  // data array containing the function values on the mesh points
  auto data = nda::array<std::complex<double>, 1>(m.size());
  nda::for_each(data.shape(), [&](auto d) { data(d) = f(m[d].value()); });

  // perform and check the tail fit for different numbers of known moments
  auto n_cmp  = (max_order ? *max_order / 2 : 5);
  auto coeffs = f.expansion_coeffs(n_cmp);
  for (auto i : nda::range(n_cmp)) {
    if constexpr (do_hermitian) {
      auto [tail, err] =
         tf.template fit_hermitian<0>(m, nda::make_array_const_view(data), true, nda::make_array_const_view(coeffs(nda::range(i))), 1);
      EXPECT_ARRAY_NEAR(coeffs(nda::range(n_cmp)), tail(nda::range(n_cmp)), tol);
      // coefficients have to be real in this case
      EXPECT_ARRAY_NEAR(nda::zeros<double>(n_cmp), nda::imag(tail(nda::range(n_cmp))), 1e-14);
    } else {
      auto [tail, err] = tf.template fit<0>(m, nda::make_array_const_view(data), true, nda::make_array_const_view(coeffs(nda::range(i))));
      EXPECT_ARRAY_NEAR(coeffs(nda::range(n_cmp)), tail(nda::range(n_cmp)), tol);
    }
  }
}

// Test the tail fitter for a matrix-valued function.
template <bool do_hermitian>
void test_array(array_f const &f, auto const &m, double r, int n_max, std::optional<int> max_order = {}, double tol = 1e-5) {
  // tail fitter object
  auto tf = triqs::mesh::tail_fitter{r, n_max, max_order};

  // data array containing the function values on the mesh points
  auto data = nda::array<std::complex<double>, 3>(m.size(), f.a.shape(0), f.a.shape(1));
  for (auto i : nda::range(m.size())) data(i, nda::ellipsis{}) = f(m[i]);

  // perform and check the tail fit for different numbers of known moments
  auto n_cmp  = (max_order ? *max_order / 2 : 5);
  auto coeffs = f.expansion_coeffs(n_cmp);
  for (auto i : nda::range(n_cmp)) {
    auto el = nda::ellipsis{};
    if constexpr (do_hermitian) {
      auto [tail, err] = tf.template fit_hermitian<0>(m, nda::make_array_const_view(data), true,
                                                      nda::make_array_const_view(coeffs(nda::range(i), el)), f.a.extent(1));
      EXPECT_ARRAY_NEAR(coeffs(nda::range(n_cmp), el), tail(nda::range(n_cmp), el), tol);
      // coefficient matrices have to be hermitian in this case
      for (auto j : nda::range(n_cmp)) { EXPECT_ARRAY_NEAR(nda::dagger(tail(j, el)), tail(j, el), 1e-14); }
    } else {
      auto [tail, err] = tf.template fit<0>(m, nda::make_array_const_view(data), true, nda::make_array_const_view(coeffs(nda::range(i), el)));
      EXPECT_ARRAY_NEAR(coeffs(nda::range(n_cmp), el), tail(nda::range(n_cmp), el), tol);
    }
  }
}

TEST(TRIQSMesh, TailFitterIndicesForFitFermionStatistics) {
  using std::numbers::pi;

  // construct a mesh
  auto m = triqs::mesh::imfreq{pi, triqs::mesh::Fermion, 10};

  // construct a tail fitter object with a tail window of 3 points and maximum 3 points in the tail
  auto r1  = 0.3;
  auto n1  = 3;
  auto tf1 = triqs::mesh::tail_fitter{r1, n1};
  EXPECT_EQ(tf1.get_tail_fraction(), r1);
  EXPECT_EQ(tf1.n_pts_in_tail(m), 3);
  EXPECT_EQ(tf1.get_tail_fit_indices(m), (std::vector<long>{-10, 9, -9, 8, -8, 7}));

  // construct a tail fitter object with a tail window of 5 points and maximum 3 points in the tail
  auto r2  = 0.5;
  auto n2  = 3;
  auto tf2 = triqs::mesh::tail_fitter{r2, n2};
  EXPECT_EQ(tf2.get_tail_fraction(), r2);
  EXPECT_EQ(tf2.n_pts_in_tail(m), 3);
  EXPECT_EQ(tf2.get_tail_fit_indices(m), (std::vector<long>{-10, 9, -8, 7, -6, 5}));
}

TEST(TRIQSMesh, TailFitterIndicesForFitBosonStatistics) {
  using std::numbers::pi;

  // construct a mesh
  auto m = triqs::mesh::imfreq{pi, triqs::mesh::Boson, 10};

  // construct a tail fitter object with a tail window of 3 points and maximum 3 points in the tail
  auto r1  = 0.3;
  auto n1  = 3;
  auto tf1 = triqs::mesh::tail_fitter{r1, n1};
  EXPECT_EQ(tf1.get_tail_fraction(), r1);
  EXPECT_EQ(tf1.n_pts_in_tail(m), 3);
  EXPECT_EQ(tf1.get_tail_fit_indices(m), (std::vector<long>{-9, 9, -8, 8, -7, 7}));

  // construct a tail fitter object with a tail window of 5 points and maximum 3 points in the tail
  auto r2  = 0.5;
  auto n2  = 3;
  auto tf2 = triqs::mesh::tail_fitter{r2, n2};
  EXPECT_EQ(tf2.get_tail_fraction(), r2);
  EXPECT_EQ(tf2.n_pts_in_tail(m), 3);
  EXPECT_EQ(tf2.get_tail_fit_indices(m), (std::vector<long>{-9, 9, -7, 7, -5, 5}));
}

TEST(TRIQSMesh, TailFitterImfreqScalarValuedFunction) {
  // imfreq mesh
  double const beta = 10;
  long const n_iw   = 100;
  auto m            = triqs::mesh::imfreq{beta, triqs::mesh::Fermion, n_iw};

  // test tail fitter for different
  test_scalar<false>(scalar_f{1.0}, m, 0.3, 30);
  test_scalar<false>(scalar_f{1.0}, m, 0.3, 30, 6);
  test_scalar<false>(scalar_f{{1.0, 0.4}}, m, 0.3, 30);
  test_scalar<false>(scalar_f{{1.0, 0.4}}, m, 0.3, 30, 6);
  test_scalar<false>(scalar_f{{-2.0, 1.1234}}, m, 0.3, 30);
  test_scalar<false>(scalar_f{{-2.0, 1.1234}}, m, 0.3, 30, 6);
}

TEST(TRIQSMesh, TailFitterRefreqScalarValuedFunction) {
  // refreq mesh
  double const w_max = 60;
  long const n_w     = 201;
  auto m             = triqs::mesh::refreq{-w_max, w_max, n_w};

  // test tail fitter
  test_scalar<false>(scalar_f{1.0}, m, 0.3, 30);
  test_scalar<false>(scalar_f{1.0}, m, 0.3, 30, 6);
  test_scalar<false>(scalar_f{{1.0, 0.4}}, m, 0.3, 30);
  test_scalar<false>(scalar_f{{1.0, 0.4}}, m, 0.3, 30, 6);
  test_scalar<false>(scalar_f{{-2.0, 1.1234}}, m, 0.3, 30);
  test_scalar<false>(scalar_f{{-2.0, 1.1234}}, m, 0.3, 30, 6);
}

TEST(TRIQSMesh, TailFitterImfreqMatrixValuedFunction) {
  using namespace std::complex_literals;
  // imfreq mesh
  double const beta = 10;
  long const n_iw   = 100;
  auto m            = triqs::mesh::imfreq{beta, triqs::mesh::Boson, n_iw};

  // test tail fitter
  auto A = nda::array<std::complex<double>, 2>{{1.0}};
  test_array<false>(array_f{A}, m, 0.3, 30);
  test_array<false>(array_f{A}, m, 0.3, 30, 6);
  auto B = nda::array<std::complex<double>, 2>{{1.0 + 0.4i, -1.0 - 0.9i}, {0.02 + 1.3i, 0.8 + 0.2i}};
  test_array<false>(array_f{B}, m, 0.3, 30);
  test_array<false>(array_f{B}, m, 0.3, 30, 6);
  auto C = nda::array<std::complex<double>, 2>{{0.5, -1.0 - 0.3i}, {-1.0 + 0.3i, 1.8}};
  test_array<false>(array_f{C}, m, 0.3, 30);
  test_array<false>(array_f{C}, m, 0.3, 30, 6);

  // test hermitian tail fitter
  test_array<true>(array_f{C}, m, 0.3, 30);
  test_array<true>(array_f{C}, m, 0.3, 30, 6);
  auto D = nda::array<std::complex<double>, 2>{{-0.2, 0.3i}, {0.2, -0.3i}};
  test_array<false>(array_f{D}, m, 0.3, 30);
  test_array<false>(array_f{D}, m, 0.3, 30, 6);
}

TEST(TRIQSMesh, TailFitterRefreqMatrixValuedFunction) {
  using namespace std::complex_literals;
  // refreq mesh
  double const w_max = 60;
  long const n_w     = 201;
  auto m             = triqs::mesh::refreq{-w_max, w_max, n_w};

  // test tail fitter
  auto A = nda::array<std::complex<double>, 2>{{1.0}};
  test_array<false>(array_f{A}, m, 0.3, 30);
  test_array<false>(array_f{A}, m, 0.3, 30, 6);
  auto B = nda::array<std::complex<double>, 2>{{1.0 + 0.4i, -1.0 - 0.9i}, {0.02 + 1.3i, 0.8 + 0.2i}};
  test_array<false>(array_f{B}, m, 0.3, 30);
  test_array<false>(array_f{B}, m, 0.3, 30, 6);
  auto C = nda::array<std::complex<double>, 2>{{0.5, -1.0 - 0.3i}, {-1.0 + 0.3i, 1.8}};
  test_array<false>(array_f{C}, m, 0.3, 30);
  test_array<false>(array_f{C}, m, 0.3, 30, 6);
}

MAKE_MAIN;
