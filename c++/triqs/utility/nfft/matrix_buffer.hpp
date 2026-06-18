// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "common.hpp"
#include <map>

namespace triqs::utility::nfft {

  /**
   * Matrix-valued NFFT buffer for product-grid sources on a 2D non-uniform target mesh.
   *
   * Computes the matrix-valued 2D DFT:
   *   output(t, u, v) += sum_{j: u_x[j]==u, i: u_y[i]==v} V(j,i) * exp(i*wx_t*x[j]) * exp(i*wy_t*y[i])
   *
   * by factoring the 2D transform into two 1D operations, exploiting:
   *   1. The product-grid structure of the source points (x[j], y[i])
   *   2. The small number of unique target frequencies per dimension
   *   3. The orbital scattering structure (u_x, u_y)
   *
   * See doc/implementation_notes/matrix_buffer.pdf for the derivation.
   *
   * @tparam Layout  nda layout policy for the 3D output view (n_mesh_points, n_orb, n_orb)
   */
  template <typename Layout = nda::C_layout> struct matrix_buffer_t {

    using output_view_t = nda::array_view<dcomplex, 3, Layout>;

    /**
     * Construct a matrix buffer for a 2D non-uniform target mesh.
     *
     * @param output_data  3D view (n_mesh_points, n_orb, n_orb) into the output gf data.
     *                     Results are accumulated (+=) into this view.
     * @param target_mf    Target Matsubara frequency pairs {iw_x, iw_y} for each mesh point.
     *                     Same format as buffer_t<2>.
     * @param n_orb        Orbital dimension (number of rows = columns of the matrix).
     */
    matrix_buffer_t(output_view_t output_data, std::vector<std::array<target_mf_t, 2>> const &target_mf, int n_orb)
       : output_(std::move(output_data)), n_orb_(n_orb) {

      long n_mp = static_cast<long>(target_mf.size());
      beta_     = target_mf[0][0].beta;

      // Helper: assign a dense index to a Matsubara n-index, tracking unique odd exponents (2n+1).
      auto assign_idx = [](long n, std::map<long, int> &idx_map, std::vector<double> &odd_exponents) {
        auto [it, inserted] = idx_map.try_emplace(n, static_cast<int>(odd_exponents.size()));
        if (inserted) odd_exponents.push_back(static_cast<double>(2 * n + 1));
        return it->second;
      };

      std::map<long, int> nx_to_idx, ny_to_idx;
      mp_wx_idx_.resize(n_mp);
      std::vector<int> mp_wy_idx(n_mp);

      for (long mp = 0; mp < n_mp; ++mp) {
        mp_wx_idx_[mp] = assign_idx(target_mf[mp][0].n, nx_to_idx, unique_nx_);
        mp_wy_idx[mp]  = assign_idx(target_mf[mp][1].n, ny_to_idx, unique_ny_);
      }

      // Group mesh points by their wy frequency index
      groups_by_wy_.resize(unique_ny_.size());
      for (long mp = 0; mp < n_mp; ++mp) groups_by_wy_[mp_wy_idx[mp]].push_back(static_cast<int>(mp));
    }

    matrix_buffer_t(matrix_buffer_t const &)            = delete;
    matrix_buffer_t(matrix_buffer_t &&)                 = default;
    matrix_buffer_t &operator=(matrix_buffer_t const &) = delete;
    matrix_buffer_t &operator=(matrix_buffer_t &&)      = default;

    /**
     * Product-grid push: compute the factored 2D DFT and accumulate into output.
     * Executes immediately (no buffering).
     *
     * @param x    Dim-0 source coordinates (length kx)
     * @param u_x  Dim-0 orbital indices in [0, n_orb) (length kx)
     * @param kx   Number of dim-0 source points
     * @param y    Dim-1 source coordinates (length ky)
     * @param u_y  Dim-1 orbital indices in [0, n_orb) (length ky)
     * @param ky   Number of dim-1 source points
     * @param V    Value matrix of shape (kx, ky)
     */
    void push_product(const double *x, const int *u_x, long kx, const double *y, const int *u_y, long ky, nda::matrix_const_view<dcomplex> V) {

      int n_wy = static_cast<int>(unique_ny_.size());

      // Step 1: Precompute exponential tables in transposed layout for cache-friendly access.
      auto ExT = make_exp_table_T(x, kx, unique_nx_);
      auto EyT = make_exp_table_T(y, ky, unique_ny_);

      // Group y-source indices by orbital
      std::vector<std::vector<long>> orb_cols(n_orb_);
      for (long i = 0; i < ky; ++i) orb_cols[u_y[i]].push_back(i);

      // Step 2: For each orbital v, gather relevant columns and GEMM into R_all_T(v).
      // R_all_T(v)(wy_idx, j) = sum_{i: u_y[i]==v} EyT(wy_idx, i) * V(j, i)
      nda::array<nda::matrix<dcomplex>, 1> R_all_T(n_orb_);
      for (int v = 0; v < n_orb_; ++v) {
        auto &cols = orb_cols[v];
        long nc    = static_cast<long>(cols.size());
        R_all_T(v) = nda::matrix<dcomplex>(n_wy, kx);
        if (nc == 0) { R_all_T(v)() = 0; continue; }

        if (nc == ky) {
          // All columns belong to this orbital — GEMM directly without copying
          nda::blas::gemm(dcomplex(1), EyT, nda::transpose(V), dcomplex(0), R_all_T(v));
        } else {
          // Gather V and EyT columns for this orbital into contiguous matrices for GEMM
          auto Vg   = nda::matrix<dcomplex>(kx, nc);
          auto EyTg = nda::matrix<dcomplex>(n_wy, nc);
          for (long ii = 0; ii < nc; ++ii) {
            long i = cols[ii];
            for (long j = 0; j < kx; ++j) Vg(j, ii) = V(j, i);
            for (int a = 0; a < n_wy; ++a) EyTg(a, ii) = EyT(a, i);
          }
          nda::blas::gemm(dcomplex(1), EyTg, nda::transpose(Vg), dcomplex(0), R_all_T(v));
        }
      }

      // Step 3: Accumulate output. Both ExT and R_all_T have contiguous j-access.
      constexpr long S = static_cast<long>(cbatch::size);

      auto const &out_strides    = output_.indexmap().strides();
      long out_stride_mp         = out_strides[0];
      long out_stride_row        = out_strides[1];
      long out_stride_col        = out_strides[2];
      auto *__restrict__ out_ptr = output_.data();

      std::vector<dcomplex const *> R_all_T_ptr(n_orb_);
      for (int v = 0; v < n_orb_; ++v) R_all_T_ptr[v] = R_all_T(v).data();

      long kx_simd = kx - (kx % S);

      for (int wy_idx = 0; wy_idx < n_wy; ++wy_idx) {
        for (int mp : groups_by_wy_[wy_idx]) {
          int wx_idx                = mp_wx_idx_[mp];
          auto *__restrict__ ex_col = ExT.data() + static_cast<long>(wx_idx) * kx;
          auto *__restrict__ out_mp = out_ptr + mp * out_stride_mp;

          if (n_orb_ == 1) {
            // n_orb=1: no orbital scatter, SIMD dot product over all kx sources
            auto *__restrict__ r_ptr = R_all_T_ptr[0] + static_cast<long>(wy_idx) * kx;
            cbatch acc{};
            long j = 0;
            for (; j < kx_simd; j += S)
              acc = xsimd::fma(cbatch::load_unaligned(ex_col + j), cbatch::load_unaligned(r_ptr + j), acc);
            dcomplex dot = xsimd::reduce_add(acc);
            for (; j < kx; ++j) dot += ex_col[j] * r_ptr[j];
            out_mp[0] += dot;
          } else {
            // n_orb>1: scalar loop with contiguous data access (no permutation overhead)
            for (long j = 0; j < kx; ++j) {
              dcomplex ex                = ex_col[j];
              auto *__restrict__ out_row = out_mp + u_x[j] * out_stride_row;
              for (int v = 0; v < n_orb_; ++v)
                out_row[v * out_stride_col] += ex * R_all_T_ptr[v][static_cast<long>(wy_idx) * kx + j];
            }
          }
        }
      }
    }

    private:
    // Build exponential table in transposed layout: ET(a, j) = exp(i * (2*n_a+1) * pi * tau_j / beta).
    // SIMD-vectorizes over source points j (contiguous writes per frequency row).
    // Returns (n_w, k) matrix for cache-friendly column access in the accumulation loop.
    nda::matrix<dcomplex> make_exp_table_T(const double *tau, long k, std::vector<double> const &odd_exp) const {
      using dbatch = xsimd::batch<double>;
      constexpr long S = static_cast<long>(dbatch::size);

      double pi_over_beta = M_PI / beta_;
      int n_w             = static_cast<int>(odd_exp.size());
      long k_simd         = k - (k % S);
      auto ET             = nda::matrix<dcomplex>(n_w, k);

      for (int a = 0; a < n_w; ++a) {
        auto *__restrict__ row = ET.data() + a * k;
        double freq_a = odd_exp[a] * pi_over_beta;

        long j = 0;
        for (; j < k_simd; j += S) {
          dbatch angles = dbatch(freq_a) * dbatch::load_unaligned(tau + j);
          auto [sin_v, cos_v] = triqs::utility::math::sincos<12>(angles);
          for (long s = 0; s < S; ++s) row[j + s] = {cos_v.get(s), sin_v.get(s)};
        }
        for (; j < k; ++j) row[j] = cis<12>(freq_a * tau[j]);
      }
      return ET;
    }

    output_view_t output_;
    int n_orb_   = 0;
    double beta_ = 0;

    // Precomputed target mesh structure for factored DFT
    // Store odd exponents (2n+1) as doubles for direct SIMD sincos evaluation
    std::vector<double> unique_nx_, unique_ny_;
    std::vector<int> mp_wx_idx_;
    std::vector<std::vector<int>> groups_by_wy_;

  };

} // namespace triqs::utility::nfft
