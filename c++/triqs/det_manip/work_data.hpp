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

#include <vector>

namespace triqs::det_manip::detail {

  // ================ Work Data Types =====================

  // Working data for single-row/column operations (insert, remove, change_col, change_row, change_col_row).
  //
  // - x and y: matrix builder arguments for the new/changed row and column.
  // - i and j: positions of the row and column in the original matrix F^{(n)}.
  // - ireal and jreal: positions of the row and column in the matrix G^{(n)}.
  // - B and C: new column and row of the matrix G^{(n)} (also reused as scratch by the change operations).
  // - MB and MC: products M^{(n)} B and C M^{(n)} of the current inverse matrix with the new column/row.
  // - ksi: determinant-ratio factor det(G^{new}) / det(G^{(n)}) (= newdet / det).
  template <typename x_type, typename y_type, typename value_type> struct work_data_type1 {
    x_type x;
    y_type y;
    long i, j, ireal, jreal;
    nda::vector<value_type> MB, MC, B, C;
    value_type ksi;

    // Resize the working vectors for a matrix of size N.
    void resize(long N) {
      MB.resize(N);
      MC.resize(N);
      B.resize(N);
      C.resize(N);
    }
  };

  // Working data for multiple-row/column operations (insert_k, insert2, remove_k, remove2).
  //
  // - x and y: matrix builder arguments for the k new rows and columns.
  // - i and j: positions of the rows and columns in the original matrix F^{(n)}.
  // - ireal and jreal: positions of the rows and columns in the matrix G^{(n)}.
  // - B and C: new columns and rows of the matrix G^{(n)}.
  // - MB and MC: products M^{(n)} B and C M^{(n)} of the current inverse matrix with the new columns/rows.
  // - ksi: k x k block used to form the determinant ratio (see det_ksi()).
  template <typename x_type, typename y_type, typename value_type> struct work_data_typek {
    std::vector<x_type> x;
    std::vector<y_type> y;
    std::vector<long> i, j, ireal, jreal;
    nda::matrix<value_type> MB, MC, B, C, ksi;

    // Resize the working data for a matrix of size N and up to k inserted/removed rows and columns.
    void resize(long N, long k) {
      if (k < 2) return;
      x.resize(k);
      y.resize(k);
      i.resize(k);
      j.resize(k);
      ireal.resize(k);
      jreal.resize(k);
      MB.resize(N, k);
      MC.resize(k, N);
      B.resize(N, k);
      C.resize(k, N);
      ksi.resize(k, k);
    }

    // Determinant of the leading k x k block of ksi, with fast paths for k = 2 and the k = 3 rule of Sarrus.
    value_type det_ksi(long k) const {
      if (k == 2) {
        return ksi(0, 0) * ksi(1, 1) - ksi(1, 0) * ksi(0, 1);
      } else if (k == 3) {
        return                                 // Rule of Sarrus
           ksi(0, 0) * ksi(1, 1) * ksi(2, 2) + //
           ksi(0, 1) * ksi(1, 2) * ksi(2, 0) + //
           ksi(0, 2) * ksi(1, 0) * ksi(2, 1) - //
           ksi(2, 0) * ksi(1, 1) * ksi(0, 2) - //
           ksi(2, 1) * ksi(1, 2) * ksi(0, 0) - //
           ksi(2, 2) * ksi(1, 0) * ksi(0, 1);  //
      } else {
        auto Rk = nda::range(k);
        return nda::linalg::det(ksi(Rk, Rk));
      };
    }
  };

  // Working data for the refill operation.
  //
  // - x_values and y_values: new matrix builder arguments.
  // - M: new matrix G built from the new arguments (later inverted in place).
  template <typename x_type, typename y_type, typename value_type> struct work_data_type_refill {
    std::vector<x_type> x_values;
    std::vector<y_type> y_values;
    nda::matrix<value_type> M;

    // Reserve memory and resize the data storages for a matrix of size N.
    void reserve(long N) {
      x_values.reserve(N);
      y_values.reserve(N);
      M.resize(N, N);
    }
  };

} // namespace triqs::det_manip::detail
