// Copyright (c) 2025 Simons Foundation
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

#include <triqs/gfs.hpp>
#include <triqs/test_tools/gfs.hpp>
#include <nda/nda.hpp>

using namespace triqs::gfs;
using namespace triqs::mesh;

static constexpr double beta = 10.0, w_max = 1.0, eps = 1e-10, tol = 1e-10;

// Initialize scalar DLR2D coefficients with simple linear pattern
void init_scalar_coefs(gf<dlr2d, scalar_valued> &g) {
  for (long i = 0; i < g.mesh().size(); ++i) { g.data()(i) = {0.1 * (i + 1), -0.05 * i}; }
}

// Initialize matrix DLR2D coefficients
void init_matrix_coefs(gf<dlr2d, matrix_valued> &g, double scale = 1.0) {
  for (long i = 0; i < g.mesh().size(); ++i) {
    g.data()(i, 0, 0) = {scale * 0.1 * (i + 1), -0.05 * i};
    g.data()(i, 0, 1) = {0.02 * i, -0.01 * scale * (i + 1)};
    g.data()(i, 1, 0) = {-0.02 * i, 0.01 * scale * (i + 1)};
    g.data()(i, 1, 1) = {scale * 0.05 * (i + 2), -0.03 * i};
  }
}

// Verify that g_dlr2d evaluated at the imfreq grid points matches g_iw
void check_evaluate_at_grid(gf_const_view<dlr2d, scalar_valued> g_dlr2d, gf_const_view<dlr2d_imfreq, scalar_valued> g_iw) {
  for (auto const &mp : g_iw.mesh()) {
    auto [iw1, iw2] = mp.value();
    auto eval_val   = g_dlr2d(std::make_pair(iw1, iw2));
    auto ref_val    = g_iw.data()(mp.data_index());
    EXPECT_NEAR(std::abs(eval_val - ref_val), 0.0, tol)
       << "Mismatch at grid point " << mp.data_index() << " (n1=" << iw1.n << ", n2=" << iw2.n << ")";
  }
}

// Test roundtrip: coefficients -> imfreq -> coefficients -> imfreq (scalar-valued)
TEST(DLR2D, ScalarValuedRoundtrip) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d = gf<dlr2d, scalar_valued>{dlr2d{beta, w_max, eps, channel}};
    init_scalar_coefs(g_dlr2d);

    auto g_iw      = make_gf_dlr2d_imfreq(g_dlr2d);
    auto g_iw_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(g_iw));
    EXPECT_GF_NEAR(g_iw, g_iw_back, tol);
  }
}

// Test roundtrip for matrix-valued GF
TEST(DLR2D, MatrixValuedRoundtrip) {
  for (auto channel : {PP, PH}) {
    auto mesh_coef = dlr2d{beta, w_max, eps, channel};
    auto g_dlr2d   = gf<dlr2d, matrix_valued>{mesh_coef, {2, 2}};
    init_matrix_coefs(g_dlr2d);

    auto g_iw      = make_gf_dlr2d_imfreq(g_dlr2d);
    auto g_iw_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(g_iw));
    EXPECT_GF_NEAR(g_iw, g_iw_back, tol);
  }
}

// Test roundtrip for block GF
TEST(DLR2D, BlockGfRoundtrip) {
  for (auto channel : {PP, PH}) {
    auto mesh_coef  = dlr2d{beta, w_max, eps, channel};
    auto g_template = gf<dlr2d, matrix_valued>{mesh_coef, {2, 2}};
    auto bg_dlr2d   = make_block_gf({"up", "down"}, {g_template, g_template});

    for (int b = 0; b < 2; ++b) { init_matrix_coefs(bg_dlr2d[b], b + 1); }

    auto bg_iw      = make_gf_dlr2d_imfreq(bg_dlr2d);
    auto bg_iw_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(bg_iw));
    for (int b = 0; b < 2; ++b) { EXPECT_GF_NEAR(bg_iw[b], bg_iw_back[b], 2e-10); }
  }
}

// Test that zero coefficients give zero imfreq values
TEST(DLR2D, ZeroCoefficients) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d     = gf<dlr2d, scalar_valued>{dlr2d{beta, w_max, eps, channel}};
    g_dlr2d.data()() = 0.0;

    auto g_iw = make_gf_dlr2d_imfreq(g_dlr2d);
    for (long i = 0; i < g_iw.mesh().size(); ++i) { EXPECT_NEAR(std::abs(g_iw.data()(i)), 0.0, 1e-14); }
  }
}

// Test both channels with DLR-representable data
TEST(DLR2D, DifferentChannels) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d = gf<dlr2d, scalar_valued>{dlr2d{beta, w_max, eps, channel}};
    init_scalar_coefs(g_dlr2d);

    auto g_iw      = make_gf_dlr2d_imfreq(g_dlr2d);
    auto g_iw_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(g_iw));
    EXPECT_GF_NEAR(g_iw, g_iw_back, tol);
  }
}

// Test evaluation at grid points (scalar-valued, both channels)
TEST(DLR2D, ScalarEvaluateAtGridPoints) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d = gf<dlr2d, scalar_valued>{dlr2d{beta, w_max, eps, channel}};
    init_scalar_coefs(g_dlr2d);
    check_evaluate_at_grid(g_dlr2d, make_gf_dlr2d_imfreq(g_dlr2d));
  }
}

// Test evaluation at grid points (matrix-valued)
TEST(DLR2D, MatrixEvaluateAtGridPoints) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d = gf<dlr2d, matrix_valued>{dlr2d{beta, w_max, eps, channel}, {2, 2}};
    init_matrix_coefs(g_dlr2d);

    auto g_iw = make_gf_dlr2d_imfreq(g_dlr2d);
    for (auto const &mp : g_iw.mesh()) {
      auto [iw1, iw2] = mp.value();
      auto eval_mat   = g_dlr2d(std::make_pair(iw1, iw2));
      for (int a = 0; a < 2; ++a) {
        for (int b = 0; b < 2; ++b) {
          EXPECT_NEAR(std::abs(eval_mat(a, b) - g_iw.data()(mp.data_index(), a, b)), 0.0, tol) << "Mismatch at element (" << a << "," << b << ")";
        }
      }
    }
  }
}

// Test exact roundtrip via evaluate: coefficients -> imfreq -> coefficients -> evaluate
TEST(DLR2D, ExactRoundtripViaEvaluate) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d = gf<dlr2d, scalar_valued>{dlr2d{beta, w_max, eps, channel}};
    init_scalar_coefs(g_dlr2d);

    auto g_iw = make_gf_dlr2d_imfreq(g_dlr2d);
    check_evaluate_at_grid(make_gf_dlr2d(g_iw), g_iw);
  }
}

// Test make_gf_imfreq matches point-by-point evaluation (scalar)
TEST(DLR2D, MakeGfImfreqScalar) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d = gf<dlr2d, scalar_valued>{dlr2d{beta, w_max, eps, channel}};
    init_scalar_coefs(g_dlr2d);

    auto g_full = make_gf_imfreq(g_dlr2d);

    for (auto iw1 : std::get<0>(g_full.mesh()))
      for (auto iw2 : std::get<1>(g_full.mesh())) {
        auto eval = g_dlr2d(std::pair{iw1.value(), iw2.value()});
        EXPECT_NEAR(std::abs(eval - g_full[iw1, iw2]), 0.0, tol);
      }
  }
}

// Test make_gf_imfreq matches point-by-point evaluation (matrix)
TEST(DLR2D, MakeGfImfreqMatrix) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d = gf<dlr2d, matrix_valued>{dlr2d{beta, w_max, eps, channel}, {2, 2}};
    init_matrix_coefs(g_dlr2d);

    auto g_full = make_gf_imfreq(g_dlr2d);

    for (auto iw1 : std::get<0>(g_full.mesh()))
      for (auto iw2 : std::get<1>(g_full.mesh())) {
        auto eval = g_dlr2d(std::pair{iw1.value(), iw2.value()});
        EXPECT_ARRAY_NEAR(eval, g_full[iw1, iw2], tol);
      }
  }
}

// Test make_gf_imfreq with explicit n_iw larger than default
TEST(DLR2D, MakeGfImfreqLargerMesh) {
  for (auto channel : {PP, PH}) {
    auto g_dlr2d = gf<dlr2d, scalar_valued>{dlr2d{beta, w_max, eps, channel}};
    init_scalar_coefs(g_dlr2d);

    long n_iw   = dlr2d_imfreq{g_dlr2d.mesh()}.max_n() + 5;
    auto g_full = make_gf_imfreq(g_dlr2d, n_iw);

    for (auto iw1 : std::get<0>(g_full.mesh()))
      for (auto iw2 : std::get<1>(g_full.mesh())) {
        auto eval = g_dlr2d(std::pair{iw1.value(), iw2.value()});
        EXPECT_NEAR(std::abs(eval - g_full[iw1, iw2]), 0.0, tol);
      }
  }
}

MAKE_MAIN;
