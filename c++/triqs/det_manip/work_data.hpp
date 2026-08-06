// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
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
// Authors: Michel Ferrero, JaksaVucicevic, Igor Krivenko, Henri Menke, Laura Messio, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell

/**
 * @file
 * @brief Provides the working data types used by triqs::det_manip::det_manip.
 */

#pragma once

#include <nda/nda.hpp>

#include <utility>
#include <vector>

namespace triqs::det_manip::detail {

  // Working data used when inserting 1 row and column.
  //
  // - x and y: MatrixBuilder arguments for the new row and column.
  // - i and j: Positions of the new row and column in the original matrix F^{(n)}.
  // - S_inv: Inverse of the new diagonal element of the inverse matrix M, i.e. S^{-1} = D - C M^{(n)} B.
  // - B and C: New column and row of the matrix G excluding the diagonal element D.
  // - MB and CM: Products of the current inverse matrix M and the new column B and row C, respectively.
  template <typename X, typename Y, typename T> struct work_data_insert {
    X x;
    Y y;
    long i, j;
    T S_inv;
    nda::vector<T> B, C, MB, CM;

    // Get current capacity of the data storages.
    [[nodiscard]] auto capacity() const { return B.size(); }

    // Reserve memory and resize the data storages if needed.
    void reserve(long cap) {
      if (cap > capacity()) {
        B.resize(cap);
        C.resize(cap);
        MB.resize(cap);
        CM.resize(cap);
      }
    }
  };

  // Working data used when inserting k rows and columns.
  //
  // - x and y: MatrixBuilder arguments for the new rows and columns.
  // - i and j: Positions of the new rows and columns in the original matrix F^{(n)}.
  // - S_inv: Inverse of the block matrix S, i.e. S^{-1} = D - C M^{(n)} B (= D for M^{(0)}).
  // - B and C: New columns and rows of the matrix G excluding the block matrix D.
  // - MB and CM: Products of the current inverse matrix M and B and C, respectively.
  template <typename X, typename Y, typename T> struct work_data_insert_k {
    std::vector<X> x;
    std::vector<Y> y;
    std::vector<long> i, j;
    nda::matrix<T> S_inv, B, C, MB, CM;

    // Get current capacity of the data storages.
    [[nodiscard]] auto capacity() const { return std::make_pair(B.shape()[0], B.shape()[1]); }

    // Reserve memory and resize the data storages if needed.
    void reserve(long cap, long k) {
      auto const [n_cap, k_cap] = capacity();
      if (cap > n_cap || k > k_cap) {
        S_inv.resize(k, k);
        B.resize(cap, k);
        C.resize(k, cap);
        MB.resize(cap, k);
        CM.resize(k, cap);
      }
    }
  };

  // Working data used when removing 1 row and column.
  //
  // - i and j: Positions of the row and column in the original matrix F^{(n)}.
  // - ip and jp: Positions of the row and column in the matrix G^{(n)}.
  // - S: Diagonal element of \widetilde{M}^{(n)}.
  template <typename T> struct work_data_remove {
    long i, j, ip, jp;
    T S;
  };

  // Working data used when removing k rows and columns.
  //
  // - i and j: Positions of the rows and columns in the original matrix F^{(n)}.
  // - ip and jp: Positions of the rows and columns in the matrix G^{(n)}.
  // - S: Block matrix of \widetilde{M}^{(n)}.
  template <typename T> struct work_data_remove_k {
    std::vector<long> i, j, ip, jp;
    nda::matrix<T> S;

    // Get current capacity of the data storages.
    [[nodiscard]] auto capacity() const { return S.shape()[0]; }

    // Reserve memory and resize the data storages if needed.
    void reserve(long cap) {
      if (cap > capacity()) {
        ip.resize(cap);
        jp.resize(cap);
        S.resize(cap, cap);
      }
    }
  };

  // Working data used when changing the elements of a column.
  //
  // - y: MatrixBuilder argument for the new column.
  // - j: Position of the column in the original matrix F^{(n)} that is being changed.
  // - jp: Position of the column in the matrix G^{(n)}.
  // - u: Vector used in the Sherman-Morrison formula containing the difference between the new and the old column.
  // - Mu: Product of the current inverse matrix M and the vector u.
  // - vTM: Product of the vector v^T with the current inverse matrix M, i.e. the jp-th row of the matrix M.
  // - xi: Factor appearing in the matrix determinant lemma, i.e. xi = (1 + v^T M u).
  template <typename Y, typename T> struct work_data_change_col {
    Y y;
    long j, jp;
    nda::vector<T> u, Mu, vTM;
    T xi;

    // Get current capacity of the data storages.
    [[nodiscard]] auto capacity() const { return u.size(); }

    // Reserve memory and resize the data storages if needed.
    void reserve(long cap) {
      if (cap > capacity()) {
        u.resize(cap);
        Mu.resize(cap);
        vTM.resize(cap);
      }
    }
  };

  // Working data used when changing the elements of a row.
  //
  // - x: MatrixBuilder argument for the new row.
  // - i: Position of the row in the original matrix F^{(n)} that is being changed.
  // - ip: Position of the row in the matrix G^{(n)}.
  // - vT: Vector used in the Sherman-Morrison formula containing the difference between the new and the old row.
  // - vTM: Product of the vector v^T with the current inverse matrix M.
  // - Mu: Product of the current inverse matrix M and the vector u, i.e. the ip-th column of the matrix M.
  // - xi: Factor appearing in the matrix determinant lemma, i.e. xi = (1 + v^T M u).
  template <typename X, typename T> struct work_data_change_row {
    X x;
    long i, ip;
    nda::vector<T> vT, vTM, Mu;
    T xi;

    // Get current capacity of the data storages.
    [[nodiscard]] auto capacity() const { return vT.size(); }

    // Reserve memory and resize the data storages if needed.
    void reserve(long cap) {
      if (cap > capacity()) {
        vT.resize(cap);
        vTM.resize(cap);
        Mu.resize(cap);
      }
    }
  };

  // Working data used when changing the elements of a row and a column.
  //
  // - x and y: MatrixBuilder arguments for the new row/column.
  // - i and j: Positions of the row/column in the original matrix F^{(n)} that are being changed.
  // - ip and jp: Positions of the row/column in the matrix G^{(n)}.
  // - sT: Vector used in the Sherman-Morrison formula containing the difference between the new and the old row.
  // - u: Vector used in the Sherman-Morrison formula containing the difference between the new and the old column.
  // - sTM: Product of the vector s^T with the current inverse matrix M.
  // - Mu: Product of the current inverse matrix M and the vector u.
  // - mT_jp and m_ip: jp-th row and ip-th column of the matrix M.
  // - gamma: Scalar s^T M u.
  // - xi: Factor appearing in the matrix determinant lemma, i.e. xi = (1 + alpha)(1 + beta) - M_{jp ip} gamma.
  template <typename X, typename Y, typename T> struct work_data_change_col_row {
    X x;
    Y y;
    long i, j, ip, jp;
    nda::vector<T> sT, u, sTM, Mu, mT_jp, m_ip;
    T gamma, xi;

    // Get current capacity of the data storages.
    [[nodiscard]] auto capacity() const { return sT.size(); }

    // Reserve memory and resize the data storages if needed.
    void reserve(long cap) {
      if (cap > capacity()) {
        sT.resize(cap);
        u.resize(cap);
        sTM.resize(cap);
        Mu.resize(cap);
        mT_jp.resize(cap);
        m_ip.resize(cap);
      }
    }
  };

  // Working data used when refilling the full matrix.
  //
  // - x and y: New MatrixBuilder arguments.
  // - G: New matrix G built from the new arguments.
  template <typename X, typename Y, typename T> struct work_data_refill {
    std::vector<X> x;
    std::vector<Y> y;
    nda::matrix<T> G;

    // Get current capacity of the data storages.
    [[nodiscard]] auto capacity() const { return G.shape()[0]; }

    // Get the size of the new matrix G.
    [[nodiscard]] auto size() const { return static_cast<long>(x.size()); }

    // Reserve memory and resize the data storages if needed.
    void reserve(long cap) {
      if (cap > capacity()) {
        x.reserve(cap);
        y.reserve(cap);
        G.resize(cap, cap);
      }
    }
  };

  // Calculate the determinant of the leading k x k block of the matrix M, with fast paths for small k.
  template <typename T> T determinant(nda::matrix<T> const &M, long k) {
    switch (k) {
      case 0: return 1;
      case 1: return M(0, 0);
      case 2: return M(0, 0) * M(1, 1) - M(1, 0) * M(0, 1);
      case 3: // Rule of Sarrus
        return M(0, 0) * M(1, 1) * M(2, 2) + M(0, 1) * M(1, 2) * M(2, 0) + M(0, 2) * M(1, 0) * M(2, 1) - M(2, 0) * M(1, 1) * M(0, 2)
           - M(2, 1) * M(1, 2) * M(0, 0) - M(2, 2) * M(1, 0) * M(0, 1);
      default: return nda::linalg::det(M(nda::range(k), nda::range(k)));
    }
  }

} // namespace triqs::det_manip::detail
