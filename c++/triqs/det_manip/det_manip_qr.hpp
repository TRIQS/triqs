// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2016 Igor Krivenko
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Michel Ferrero, JaksaVucicevic, Igor Krivenko, Laura Messio, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell

#pragma once

#include <triqs/utility/first_include.hpp>
#include <triqs/utility/callable_traits.hpp>
#include <triqs/arrays.hpp>
#include <itertools/itertools.hpp>

#include "./lapack_qr.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <vector>

namespace triqs::det_manip {

  namespace blas = nda::blas;

  // ---------------------------------------------------------------------------------------------------
  //
  //  det_manip_qr : a QR-based, numerically more robust alternative to det_manip, with the same interface.
  //
  //  The default det_manip tracks an explicit inverse updated by the Sherman-Morrison-Woodbury formula
  //  without pivoting, which can drift. det_manip_qr instead obtains the determinant and inverse from an
  //  orthogonal column-pivoted QR factorization (A P = Q R, nda::lapack::geqp3) instead of LU. Orthogonal
  //  transforms have a bounded growth factor, so the determinant is backward stable and a singular
  //  configuration is detected cleanly (an exact zero on the R diagonal).
  //
  //  The factorization (explicit Q, R and the column pivot) is held as member data, like w2dynamics'
  //  UPDATABLE_QR, so the determinant and the inverse share a single factorization (the inverse uses
  //  nda::lapack::trtrs : A^{-1} = P R^{-1} Q^H). The helpers householder_det_factor / perm_sign live in
  //  det_manip/lapack_qr.hpp. This works identically for real and complex value types.
  //
  // ---------------------------------------------------------------------------------------------------
    template <typename FunctionType> class det_manip_qr {
      // ---------------------------------------------------------------------------------------------------
      // Type Checks and Definitions
      private:
      using f_tr = utility::callable_traits<FunctionType>;
      static_assert(f_tr::arity == 2, "det_manip_qr : the function must take two arguments !");

      using int_type = std::ptrdiff_t;
      using range    = itertools::range;

      public:
      using x_type     = typename f_tr::template decay_arg_t<0>;
      using y_type     = typename f_tr::template decay_arg_t<1>;
      using value_type = typename f_tr::result_type;
      using det_type   = value_type;
      static_assert(std::is_floating_point_v<value_type> || nda::is_complex_v<value_type>,
                    "det_manip_qr : the function must return a floating number or a complex number");

      using matrix_type  = nda::matrix<value_type>;
      using fmatrix_type = nda::matrix<value_type, nda::F_layout>; // Fortran layout, required by the LAPACK QR routines

      // ---------------------------------------------------------------------------------------------------
      // Data
      private:
      enum {
        NoTry,
        Insert,
        Remove,
        ChangeCol,
        ChangeRow,
        ChangeRowCol,
        Insert2  = 10,
        Remove2  = 11,
        InsertK  = 12,
        RemoveK  = 13,
        Refill   = 20
      } last_try = NoTry; // keep in memory the last operation not completed

      FunctionType f;

      // Serialized data:

      uint64_t n_opts = 0; // count the number of operation

      long Nmax = 0;
      long N    = 0;

      matrix_type mat;
      det_type det = 1.0;

      std::vector<x_type> x_values;
      std::vector<y_type> y_values;

      // Temporary objects: not serialized
      matrix_type mat_new;
      det_type det_new = 1.0;

      mutable matrix_type mat_inverse;
      mutable bool mat_inverse_is_valid = false;

      // Cached QR factorization of the current matrix : A P = Q R (cf. w2dynamics UPDATABLE_QR :
      // qbuf / rbuf / jperm / det_q). Held as member data so the determinant and the inverse share a
      // single factorization. The `_new` members hold the candidate built during a try_*, swapped in by
      // complete_operation, exactly like mat / mat_new.
      fmatrix_type q_mat, r_mat;
      nda::vector<int> jpvt; // 1-based column pivot returned by geqp3
      det_type det_q = 1;    // det(Q)
      fmatrix_type q_new, r_new;
      nda::vector<int> jpvt_new;
      det_type det_q_new = 1;

      // Temporary work data

      struct work_data_type1 {
        x_type x;
        y_type y;
        long i;
        long j;
      } w1;

      struct work_data_type2 {
        x_type x[2];
        y_type y[2];
        long i[2], j[2];
      } w2;

      struct work_data_typek {
        std::vector<x_type> x;
        std::vector<y_type> y;
        std::vector<long> i, j;
        long k = 0;
        void resize(long new_k) {
          k = new_k;
          x.resize(k);
          y.resize(k);
          i.resize(k);
          j.resize(k);
        }
      } wk;

      struct work_data_type_refill {
        std::vector<x_type> x_values;
        std::vector<y_type> y_values;
        void clear() {
          x_values.clear();
          y_values.clear();
        }
        void reserve(size_t s) {
          x_values.reserve(s);
          y_values.reserve(s);
        }
      } w_refill;

      // ---------------------------------------------------------------------------------------------------
      // Data Transfer: HDF5
      private:
      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, det_manip_qr const &g) {
        auto gr = fg.create_group(subgroup_name);
        h5_write(gr, "n_opts", g.n_opts);
        h5_write(gr, "N", g.N);
        h5_write(gr, "mat", g.mat);
        h5_write(gr, "det", g.det);
        h5_write(gr, "x_values", g.x_values);
        h5_write(gr, "y_values", g.y_values);
      }

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, det_manip_qr &g) {
        auto gr = fg.open_group(subgroup_name);
        h5_read(gr, "n_opts", g.n_opts);
        h5_read(gr, "N", g.N);
        h5_read(gr, "mat", g.mat);
        g.Nmax     = first_dim(g.mat); // restore Nmax
        g.last_try = NoTry;
        h5_read(gr, "det", g.det);
        h5_read(gr, "x_values", g.x_values);
        h5_read(gr, "y_values", g.y_values);
      }

      public:
      /**
     * Like for std::vector, reserve memory for a bigger size.
     * Preserves only the matrix, not the temporary working vectors/matrices, so do NOT use it
     * between a try_XXX and a complete_operation
     *
     * @param new_size The new size of the reserved memory
     * @param new_k Unused, for API compatibility with det_manip
     */
      void reserve(long new_size, long new_k = 1) {
        (void)new_k; // unused, for API compatibility
        if (new_size <= Nmax) return;
        matrix_type Mcopy(mat);
        long N0 = Nmax;
        Nmax    = new_size;
        mat.resize(Nmax, Nmax);
        mat_new.resize(Nmax, Nmax);
        mat_inverse.resize(Nmax, Nmax);
        mat(range(0, N0), range(0, N0)) = Mcopy; // keep the content of mat_inv ---> into the lib ?
        x_values.reserve(Nmax);
        y_values.reserve(Nmax);
        mat_inverse_is_valid = false;
      }

      /**
     * @brief Constructor.
     *
     * @param F         The function (NB : a copy is made of the F object in this class).
     * @param init_size The maximum size of the matrix before a resize (like reserve in std::vector).
     *                  Like std::vector, resize is automatic (by a factor 2) but can yield a performance penalty
     *                  if it happens too often.
     */
      det_manip_qr(FunctionType F, long init_size) : f(std::move(F)) {
        reserve(init_size);
        mat() = 0;
      }

      /** @brief Constructor.
     *
     * @param F         The function (NB : a copy is made of the F object in this class).
     * @tparam ArgumentContainer
     * @param X, Y : container for X,Y.
     */
      template <typename ArgumentContainer1, typename ArgumentContainer2>
      det_manip_qr(FunctionType F, ArgumentContainer1 const &X, ArgumentContainer2 const &Y) : f(std::move(F)) {
        if (X.size() != Y.size()) TRIQS_RUNTIME_ERROR << " X.size != Y.size";
        N = X.size();
        if (N == 0) {
          det = 1;
          reserve(30);
          return;
        }
        if (N > Nmax) reserve(2 * N); // put some margin..
        std::copy(X.begin(), X.end(), std::back_inserter(x_values));
        std::copy(Y.begin(), Y.end(), std::back_inserter(y_values));

        build_matrix();
        compute_determinant();
      }

      det_manip_qr(det_manip_qr const &)        = default;
      det_manip_qr(det_manip_qr &&rhs) noexcept = default;
      det_manip_qr &operator=(const det_manip_qr &) = delete;
      // det_manip_qr &operator                           = default;

      /// Put to size 0 : like a vector
      void clear() {
        N        = 0;
        det      = 1;
        last_try = NoTry;
        x_values.clear();
        y_values.clear();
        q_mat = fmatrix_type{};
        r_mat = fmatrix_type{};
        jpvt  = nda::vector<int>{};
        det_q = 1;
      }
      //----------------------- Computations ----------------------------------

      matrix_type build_matrix_scratch() {
        TRIQS_ASSERT(x_values.size() == N)
        TRIQS_ASSERT(y_values.size() == N)

        matrix_type res(N, N);

        for (long i = 0; i < N; i++) {
          for (long j = 0; j < N; j++) { res(i, j) = f(x_values[i], y_values[j]); }
        }

        return res;
      }

      private:
      void build_matrix() {
        TRIQS_ASSERT(x_values.size() == N)
        TRIQS_ASSERT(y_values.size() == N)
        if (N > Nmax) reserve(N); // TODO: no extra margin

        for (long i = 0; i < N; i++) {
          for (long j = 0; j < N; j++) { mat(i, j) = f(x_values[i], y_values[j]); }
        }
      }

      // Column-pivoted QR factorization (A P = Q R) of an N x N view, using LAPACK via nda. Fills Q (explicit,
      // Fortran layout), R (upper triangular), the pivot piv (1-based) and detq = det(Q); returns
      // det(A) = prod_i R(i,i) * det(Q) * det(P). geqp3 is backward stable and gives an exact zero for singular A.
      det_type factorize(nda::matrix_const_view<value_type> M, fmatrix_type &Q, fmatrix_type &R, nda::vector<int> &piv, det_type &detq) {
        long const n = M.extent(0);
        if (n == 0) {
          Q    = fmatrix_type{};
          R    = fmatrix_type{};
          piv  = nda::vector<int>{};
          detq = 1;
          return det_type{1};
        }

        Q = fmatrix_type{M}; // copy A into the Q buffer (geqp3 then orgqr/ungqr overwrite it in place)
        piv.resize(n);
        piv() = 0; // all columns free
        nda::vector<value_type> tau(n);
        nda::lapack::geqp3(Q, piv, tau);

        R.resize(n, n); // extract R before orgqr/ungqr overwrites Q with the explicit Q
        R() = 0;
        for (long j = 0; j < n; ++j)
          for (long i = 0; i <= j; ++i) R(i, j) = Q(i, j);

        det_type det_r = 1;
        for (long i = 0; i < n; ++i) det_r *= Q(i, i);
        detq = 1;
        for (long i = 0; i < n; ++i) detq *= householder_det_factor<value_type>(tau(i));

        if constexpr (nda::is_complex_v<value_type>)
          nda::lapack::ungqr(Q, tau);
        else
          nda::lapack::orgqr(Q, tau);

        return det_r * detq * det_type(perm_sign(piv));
      }

      // Reset the candidate factorization to empty (used by the N -> 0 special cases that set det_new directly).
      void clear_candidate_factorization() {
        q_new     = fmatrix_type{};
        r_new     = fmatrix_type{};
        jpvt_new  = nda::vector<int>{};
        det_q_new = 1;
      }

      void compute_determinant() {
        range R(0, N);
        det = factorize(mat(R, R), q_mat, r_mat, jpvt, det_q);
      }

      void compute_inverse() const {
        if (mat_inverse_is_valid) return;
        if (N == 0) {
          mat_inverse_is_valid = true;
          return;
        }
        // A^{-1} = P R^{-1} Q^H, reusing the cached factorization (no geqp3 here).
        fmatrix_type qh(N, N); // Q^H
        for (long i = 0; i < N; ++i)
          for (long j = 0; j < N; ++j) {
            if constexpr (nda::is_complex_v<value_type>)
              qh(i, j) = std::conj(q_mat(j, i));
            else
              qh(i, j) = q_mat(j, i);
          }
        nda::lapack::trtrs('U', 'N', 'N', r_mat, qh); // qh <- R^{-1} Q^H
        for (long k = 0; k < N; ++k)
          for (long j = 0; j < N; ++j) mat_inverse(jpvt(k) - 1, j) = qh(k, j); // row jpvt(k)-1 of A^{-1} is row k of X
        mat_inverse_is_valid = true;
      }

      //----------------------- READ ACCESS TO DATA ----------------------------------
      public:
      // Current size of the matrix
      auto size() const { return N; }

      // Get copies of x,y or individual values
      auto get_x() const { return x_values; }
      auto get_y() const { return y_values; }

      x_type const &get_x(long i) const { return x_values[i]; } // TODO: replace with at? Why const ref?
      y_type const &get_y(long j) const { return y_values[j]; } // TODO: replace with at? Why const ref?

      auto const &get_x_internal_order() const { return x_values; }
      auto const &get_y_internal_order() const { return y_values; }


      /// Returns the function f
      FunctionType const &get_function() const { return f; }

      /** det M of the current state of the matrix.  */
      auto determinant() { return det; }

      /** Returns M^{-1} */
      nda::matrix_const_view<value_type> inverse_matrix() const {
        compute_inverse();
        range R(0, N);
        return mat_inverse(R, R);
      }

      value_type inverse_matrix(int i, int j) const {
        compute_inverse();
        return mat_inverse(i, j);
      }

      nda::matrix_const_view<value_type> inverse_matrix_internal_order() const { return inverse_matrix(); }

      value_type inverse_matrix_internal_order(int i, int j) const { return inverse_matrix(i, j); }

      /** Returns M */

      matrix_type matrix() const {
        range R(0, N);
        return mat(R, R);
      }

      value_type matrix(int i, int j) const { return mat(i, j); }

      // Given a lambda func : x,y,M, it calls func(x_i,y_j,M_ji) for all i,j
      // Order of iteration is NOT fixed, it is optimised (for memory traversal)
      template <typename LambdaType> friend void foreach (det_manip_qr const &d, LambdaType const &func) {
        d.compute_inverse();
        nda::for_each(std::array{long(d.N), long(d.N)}, [&func, &d](int i, int j) { return func(d.x_values[i], d.y_values[j], d.mat_inverse(j, i)); });
      }

      // ------------------------- OPERATIONS -----------------------------------------------

      /**
     * Insert operation at column j0 and row i0.
     *
     * The operation consists in adding :
     *
     *    * a column  f(x_i,    y_{j0})
     *    * and a row f(x_{i0}, x_j)
     *
     * The new column/row will be at col j0, row i0.
     *
     * 0 <= i0,j0 <= N, where N is the current size of the matrix.
     * The current column j0 (resp. row i0) will become column j0+1 (resp. row i0+1).
     * Inserting at N simply add the new col at the end.

     * Returns the ratio of det Minv_new / det Minv.
     *
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     *
     * @param i 
     * @param j
     * @category Operations
     */
      value_type try_insert(long i, long j, x_type const &x, y_type const &y) {
        // check input and store it for complete_operation
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i <= N);
        TRIQS_ASSERT(j <= N);
        TRIQS_ASSERT(i >= 0);
        TRIQS_ASSERT(j >= 0);
        // TRIQS_ASSERT(N > 0); -- fix
        w1.i = i;
        w1.j = j;
        w1.x = x;
        w1.y = y;

        if (N == Nmax) reserve(2 * Nmax);
        last_try = Insert;

        range Row_A__(0, i);
        range Row_B_0(i, N);
        range Row_B_1 = Row_B_0 + std::ptrdiff_t{1};

        range Col_A__(0, j);
        range Col_B_0(j, N);
        range Col_B_1 = Col_B_0 + std::ptrdiff_t{1};

        mat_new(Row_A__, Col_A__) = mat(Row_A__, Col_A__);
        mat_new(Row_A__, Col_B_1) = mat(Row_A__, Col_B_0);
        mat_new(Row_B_1, Col_A__) = mat(Row_B_0, Col_A__);
        mat_new(Row_B_1, Col_B_1) = mat(Row_B_0, Col_B_0);

        for (auto k : Row_A__) { mat_new(k, j) = f(x_values[k], y); }
        for (auto k : Row_B_0) { mat_new(k + 1, j) = f(x_values[k], y); }

        for (auto k : Col_A__) { mat_new(i, k) = f(x, y_values[k]); }
        for (auto k : Col_B_0) { mat_new(i, k + 1) = f(x, y_values[k]); }

        mat_new(i, j) = f(x, y);

        range R(0, N + 1);
        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }

      //------------------------------------------------------------------------------------------
      private:
      void complete_insert() {
        N++;
        x_values.insert(begin(x_values) + w1.i, w1.x);
        y_values.insert(begin(y_values) + w1.j, w1.y);
        std::swap(mat, mat_new);
      }

      public:
      //------------------------------------------------------------------------------------------

      /**
     * Double Insert operation at colum j0,j1 and row i0,i1.
     *
     * The operation consists in adding :
     *    * two columns  f(x_i,    y_{j0}), f(x_i,    y_{j1})
     *    * and two rows f(x_{i0}, x_j),    f(x_{i1}, x_j)
     * The new colums/rows will be at col j0, j1, row i0, i1.
     *
     * 0 <= i0,i1,j0,j1 <= N+1, where N is the current size of the matrix.
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     * @category Operations
     */

      value_type try_insert2(long i0, long i1, long j0, long j1, x_type const &x0_, x_type const &x1_, y_type const &y0_, y_type const &y1_) {

        // first make sure i0<i1 and j0<j1
        x_type const &x0((i0 < i1) ? x0_ : x1_);
        x_type const &x1((i0 < i1) ? x1_ : x0_);
        y_type const &y0((j0 < j1) ? y0_ : y1_);
        y_type const &y1((j0 < j1) ? y1_ : y0_);
        if (i0 > i1) std::swap(i0, i1);
        if (j0 > j1) std::swap(j0, j1);

        // check input and store it for complete_operation
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i0 != i1);
        TRIQS_ASSERT(j0 != j1);
        TRIQS_ASSERT(i0 <= N);
        TRIQS_ASSERT(j0 <= N);
        TRIQS_ASSERT(i0 >= 0);
        TRIQS_ASSERT(j0 >= 0);
        TRIQS_ASSERT(i1 <= N + 1);
        TRIQS_ASSERT(j1 <= N + 1);
        TRIQS_ASSERT(i1 >= 0);
        TRIQS_ASSERT(j1 >= 0);

        i1--;
        j1--;

        if (N >= Nmax - 1) reserve(2 * Nmax);
        last_try = Insert2;
        w2.i[0]  = i0;
        w2.i[1]  = i1;
        w2.j[0]  = j0;
        w2.j[1]  = j1;
        w2.x[0]  = x0;
        w2.x[1]  = x1;
        w2.y[0]  = y0;
        w2.y[1]  = y1;

        // std::cerr << i0 << "," << i1 << "," << j0 << "," << j1 << " : " << x0 << "," << x1 << "," << y0 << "," << y1 << std::endl;

        range Row_A__(0, i0);
        range Row_B_0(i0, i1);
        range Row_B_1 = Row_B_0 + std::ptrdiff_t{1};
        range Row_C_0(i1, N);
        range Row_C_1 = Row_C_0 + std::ptrdiff_t{2};

        range Col_A__(0, j0);
        range Col_B_0(j0, j1);
        range Col_B_1 = Col_B_0 + std::ptrdiff_t{1};
        range Col_C_0(j1, N);
        range Col_C_1 = Col_C_0 + std::ptrdiff_t{2};

        mat_new(Row_A__, Col_A__) = mat(Row_A__, Col_A__);
        mat_new(Row_A__, Col_B_1) = mat(Row_A__, Col_B_0);
        mat_new(Row_A__, Col_C_1) = mat(Row_A__, Col_C_0);

        mat_new(Row_B_1, Col_A__) = mat(Row_B_0, Col_A__);
        mat_new(Row_B_1, Col_B_1) = mat(Row_B_0, Col_B_0);
        mat_new(Row_B_1, Col_C_1) = mat(Row_B_0, Col_C_0);

        mat_new(Row_C_1, Col_A__) = mat(Row_C_0, Col_A__);
        mat_new(Row_C_1, Col_B_1) = mat(Row_C_0, Col_B_0);
        mat_new(Row_C_1, Col_C_1) = mat(Row_C_0, Col_C_0);

        // Note: need to shift i1/j1 below by +1 to adjust for first row/column

        for (auto k : Row_A__) {
          mat_new(k, j0)     = f(x_values[k], y0);
          mat_new(k, j1 + 1) = f(x_values[k], y1);
        }
        for (auto k : Row_B_0) {
          mat_new(k + 1, j0)     = f(x_values[k], y0);
          mat_new(k + 1, j1 + 1) = f(x_values[k], y1);
        }

        for (auto k : Row_C_0) {
          mat_new(k + 2, j0)     = f(x_values[k], y0);
          mat_new(k + 2, j1 + 1) = f(x_values[k], y1);
        }

        for (auto k : Col_A__) {
          mat_new(i0, k)     = f(x0, y_values[k]);
          mat_new(i1 + 1, k) = f(x1, y_values[k]);
        }
        for (auto k : Col_B_0) {
          mat_new(i0, k + 1)     = f(x0, y_values[k]);
          mat_new(i1 + 1, k + 1) = f(x1, y_values[k]);
        }
        for (auto k : Col_C_0) {
          mat_new(i0, k + 2)     = f(x0, y_values[k]);
          mat_new(i1 + 1, k + 2) = f(x1, y_values[k]);
        }

        mat_new(i0, j0)         = f(x0, y0);
        mat_new(i0, j1 + 1)     = f(x0, y1);
        mat_new(i1 + 1, j0)     = f(x1, y0);
        mat_new(i1 + 1, j1 + 1) = f(x1, y1);

        range R(0, N + 2);
        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }

      //------------------------------------------------------------------------------------------
      private:
      void complete_insert2() {
        N += 2;
        x_values.insert(begin(x_values) + w2.i[1], w2.x[1]);
        x_values.insert(begin(x_values) + w2.i[0], w2.x[0]);
        y_values.insert(begin(y_values) + w2.j[1], w2.y[1]);
        y_values.insert(begin(y_values) + w2.j[0], w2.y[0]);
        std::swap(mat, mat_new);
      }

      public:
      //------------------------------------------------------------------------------------------

      /**
     * Insert k rows and columns at positions given by vectors i and j.
     *
     * The operation consists in adding k columns and k rows at specified positions.
     * The new columns/rows will be at positions given by i[l] and j[l].
     *
     * 0 <= i[l], j[l] <= N+l, where N is the current size of the matrix.
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     * @category Operations
     */
      value_type try_insert_k(std::vector<long> i, std::vector<long> j, std::vector<x_type> x, std::vector<y_type> y) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i.size() == j.size());
        TRIQS_ASSERT(j.size() == x.size());
        TRIQS_ASSERT(x.size() == y.size());

        long k = i.size();
        wk.resize(k);

        // Sort indices to ensure proper insertion order
        auto argsort = [](auto const &vec) {
          std::vector<long> idx(vec.size());
          std::iota(idx.begin(), idx.end(), 0L);
          std::stable_sort(idx.begin(), idx.end(), [&vec](long lhs, long rhs) { return vec[lhs] < vec[rhs]; });
          return idx;
        };
        std::vector<long> idx = argsort(i);
        std::vector<long> idy = argsort(j);

        // Store sorted values for complete_operation
        for (long l = 0; l < k; ++l) {
          wk.i[l] = i[idx[l]];
          wk.x[l] = x[idx[l]];
          wk.j[l] = j[idy[l]];
          wk.y[l] = y[idy[l]];
        }

        // Check consistency: each position must be valid at insertion time
        // After sorting, wk.i[l] must be <= N + l (valid for size N + l)
        for (long l = 0; l < k; ++l) {
          TRIQS_ASSERT(0 <= wk.i[l] && wk.i[l] <= N + l);
          TRIQS_ASSERT(0 <= wk.j[l] && wk.j[l] <= N + l);
        }
        for (long l = 0; l < k - 1; ++l) {
          TRIQS_ASSERT(wk.i[l] != wk.i[l + 1]);
          TRIQS_ASSERT(wk.j[l] != wk.j[l + 1]);
        }

        if (N + k > Nmax) reserve(2 * (N + k));
        last_try = InsertK;

        // Build new matrix with k inserted rows/columns
        // Create mapping from old indices to new indices
        std::vector<long> row_map(N), col_map(N);
        {
          long offset = 0;
          long insert_idx = 0;
          for (long old_idx = 0; old_idx < N; ++old_idx) {
            while (insert_idx < k && wk.i[insert_idx] <= old_idx + offset) {
              ++offset;
              ++insert_idx;
            }
            row_map[old_idx] = old_idx + offset;
          }
        }
        {
          long offset = 0;
          long insert_idx = 0;
          for (long old_idx = 0; old_idx < N; ++old_idx) {
            while (insert_idx < k && wk.j[insert_idx] <= old_idx + offset) {
              ++offset;
              ++insert_idx;
            }
            col_map[old_idx] = old_idx + offset;
          }
        }

        // Copy existing matrix elements to their new positions
        for (long old_i = 0; old_i < N; ++old_i) {
          for (long old_j = 0; old_j < N; ++old_j) {
            mat_new(row_map[old_i], col_map[old_j]) = mat(old_i, old_j);
          }
        }

        // Fill in new rows
        for (long l = 0; l < k; ++l) {
          long new_row = wk.i[l];
          // Elements from existing columns
          for (long old_j = 0; old_j < N; ++old_j) {
            mat_new(new_row, col_map[old_j]) = f(wk.x[l], y_values[old_j]);
          }
          // Elements from new columns
          for (long m = 0; m < k; ++m) {
            mat_new(new_row, wk.j[m]) = f(wk.x[l], wk.y[m]);
          }
        }

        // Fill in new columns (for existing rows)
        for (long l = 0; l < k; ++l) {
          long new_col = wk.j[l];
          for (long old_i = 0; old_i < N; ++old_i) {
            mat_new(row_map[old_i], new_col) = f(x_values[old_i], wk.y[l]);
          }
        }

        range R(0, N + k);
        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }

      //------------------------------------------------------------------------------------------
      private:
      void complete_insert_k() {
        long k = wk.k;
        N += k;
        // Insert in forward order: position wk.i[l] is valid for size N-k+l
        for (long l = 0; l < k; ++l) {
          x_values.insert(begin(x_values) + wk.i[l], wk.x[l]);
          y_values.insert(begin(y_values) + wk.j[l], wk.y[l]);
        }
        std::swap(mat, mat_new);
      }

      public:
      //------------------------------------------------------------------------------------------

      /**
     * Consider the removal the colj0 and row i0 from the matrix.
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_remove(long i, long j) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i < N);
        TRIQS_ASSERT(j < N);
        TRIQS_ASSERT(i >= 0);
        TRIQS_ASSERT(j >= 0);
        w1.i     = i;
        w1.j     = j;
        last_try = Remove;

        // Treat N = 1 specially -> goes to zero
        if (N == 1){
          det_new = 1.0;
          clear_candidate_factorization();
          return det_new / det;
        }

        range Row_A__(0, i);
        range Row_B_0(i + 1, N);
        range Row_B_1 = Row_B_0 + std::ptrdiff_t{-1};

        range Col_A__(0, j);
        range Col_B_0(j + 1, N);
        range Col_B_1 = Col_B_0 + std::ptrdiff_t{-1};

        mat_new(Row_A__, Col_A__) = mat(Row_A__, Col_A__);
        mat_new(Row_A__, Col_B_1) = mat(Row_A__, Col_B_0);
        mat_new(Row_B_1, Col_A__) = mat(Row_B_0, Col_A__);
        mat_new(Row_B_1, Col_B_1) = mat(Row_B_0, Col_B_0);

        range R(0, N - 1);
        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }
      //------------------------------------------------------------------------------------------
      private:
      void complete_remove() {
        N--;
        x_values.erase(begin(x_values) + w1.i);
        y_values.erase(begin(y_values) + w1.j);
        std::swap(mat, mat_new);
      }

      public:
      //------------------------------------------------------------------------------------------

      /**
     * Double Removal operation of cols j0,j1 and rows i0,i1
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_remove2(long i0, long i1, long j0, long j1) {

        // first make sure i0<i1 and j0<j1
        if (i0 > i1) std::swap(i0, i1);
        if (j0 > j1) std::swap(j0, j1);

        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(N >= 2);
        TRIQS_ASSERT(i0 != i1);
        TRIQS_ASSERT(j0 != j1);
        TRIQS_ASSERT(i0 < N);
        TRIQS_ASSERT(j0 < N);
        TRIQS_ASSERT(i0 >= 0);
        TRIQS_ASSERT(j0 >= 0);
        TRIQS_ASSERT(i1 < N + 1);
        TRIQS_ASSERT(j1 < N + 1);
        TRIQS_ASSERT(i1 >= 0);
        TRIQS_ASSERT(j1 >= 0);

        last_try = Remove2;

        w2.i[0] = std::min(i0, i1);
        w2.i[1] = std::max(i0, i1);
        w2.j[0] = std::min(j0, j1);
        w2.j[1] = std::max(j0, j1);

        // Treat N = 2 specially -> goes to N = 0
        if (N == 2){
          det_new = 1.0;
          clear_candidate_factorization();
          return det_new / det;
        }

        range Row_A__(0, i0);
        range Row_B_0(i0 + 1, i1);
        range Row_B_1 = Row_B_0 + std::ptrdiff_t{-1};
        range Row_C_0(i1 + 1, N);
        range Row_C_1 = Row_C_0 + std::ptrdiff_t{-2};

        range Col_A__(0, j0);
        range Col_B_0(j0 + 1, j1);
        range Col_B_1 = Col_B_0 + std::ptrdiff_t{-1};
        range Col_C_0(j1 + 1, N);
        range Col_C_1 = Col_C_0 + std::ptrdiff_t{-2};

        mat_new(Row_A__, Col_A__) = mat(Row_A__, Col_A__);
        mat_new(Row_A__, Col_B_1) = mat(Row_A__, Col_B_0);
        mat_new(Row_A__, Col_C_1) = mat(Row_A__, Col_C_0);

        mat_new(Row_B_1, Col_A__) = mat(Row_B_0, Col_A__);
        mat_new(Row_B_1, Col_B_1) = mat(Row_B_0, Col_B_0);
        mat_new(Row_B_1, Col_C_1) = mat(Row_B_0, Col_C_0);

        mat_new(Row_C_1, Col_A__) = mat(Row_C_0, Col_A__);
        mat_new(Row_C_1, Col_B_1) = mat(Row_C_0, Col_B_0);
        mat_new(Row_C_1, Col_C_1) = mat(Row_C_0, Col_C_0);

        range R(0, N - 2);
        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }
      //------------------------------------------------------------------------------------------
      private:
      void complete_remove2() {
        N -= 2;

        // Note: need to erase in correct order!
        x_values.erase(begin(x_values) + w2.i[1]);
        x_values.erase(begin(x_values) + w2.i[0]);
        y_values.erase(begin(y_values) + w2.j[1]);
        y_values.erase(begin(y_values) + w2.j[0]);

        std::swap(mat, mat_new);
      }

      public:
      //------------------------------------------------------------------------------------------

      /**
     * Remove k rows and columns at positions given by vectors i and j.
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_remove_k(std::vector<long> i, std::vector<long> j) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i.size() == j.size());
        TRIQS_ASSERT(N >= static_cast<long>(i.size()));

        long k = i.size();
        wk.resize(k);

        // Sort indices in descending order for proper removal
        std::sort(i.begin(), i.end());
        std::sort(j.begin(), j.end());

        // Check consistency
        for (long l = 0; l < k - 1; ++l) {
          TRIQS_ASSERT(i[l] != i[l + 1] && 0 <= i[l] && i[l] < N);
          TRIQS_ASSERT(j[l] != j[l + 1] && 0 <= j[l] && j[l] < N);
        }
        if (k > 0) {
          TRIQS_ASSERT(0 <= i[k - 1] && i[k - 1] < N);
          TRIQS_ASSERT(0 <= j[k - 1] && j[k - 1] < N);
        }

        // Store sorted indices for complete_operation
        for (long l = 0; l < k; ++l) {
          wk.i[l] = i[l];
          wk.j[l] = j[l];
        }

        last_try = RemoveK;

        // Treat N = k specially -> goes to N = 0
        if (N == k) {
          det_new = 1.0;
          clear_candidate_factorization();
          return det_new / det;
        }

        // Build set of indices to keep
        std::vector<long> row_keep, col_keep;
        row_keep.reserve(N - k);
        col_keep.reserve(N - k);
        {
          long remove_idx = 0;
          for (long idx = 0; idx < N; ++idx) {
            if (remove_idx < k && idx == wk.i[remove_idx]) {
              ++remove_idx;
            } else {
              row_keep.push_back(idx);
            }
          }
        }
        {
          long remove_idx = 0;
          for (long idx = 0; idx < N; ++idx) {
            if (remove_idx < k && idx == wk.j[remove_idx]) {
              ++remove_idx;
            } else {
              col_keep.push_back(idx);
            }
          }
        }

        // Copy remaining elements to new matrix
        for (long new_i = 0; new_i < static_cast<long>(row_keep.size()); ++new_i) {
          for (long new_j = 0; new_j < static_cast<long>(col_keep.size()); ++new_j) {
            mat_new(new_i, new_j) = mat(row_keep[new_i], col_keep[new_j]);
          }
        }

        range R(0, N - k);
        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }

      //------------------------------------------------------------------------------------------
      private:
      void complete_remove_k() {
        long k = wk.k;
        N -= k;

        // Erase in reverse order (largest index first) to maintain correct positions
        for (long l = k - 1; l >= 0; --l) {
          x_values.erase(begin(x_values) + wk.i[l]);
          y_values.erase(begin(y_values) + wk.j[l]);
        }

        std::swap(mat, mat_new);
      }

      //------------------------------------------------------------------------------------------
      public:
      /**
     * Consider the change the column j and the corresponding y.
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_change_col(long j, y_type const &y) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(j < N);
        TRIQS_ASSERT(j >= 0);
        w1.j     = j;
        last_try = ChangeCol;
        w1.y     = y;

        range R(0, N);
        mat_new(R, R) = mat(R, R);
        for (auto k : R) { mat_new(k, j) = f(x_values[k], y); }

        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }

      //------------------------------------------------------------------------------------------
      private:
      void complete_change_col() {
        y_values[w1.j] = w1.y;
        std::swap(mat, mat_new);
      }

      //------------------------------------------------------------------------------------------
      public:
      /**
     * Consider the change the row i and the corresponding x.
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_change_row(long i, x_type const &x) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i < N);
        TRIQS_ASSERT(i >= 0);
        w1.i     = i;
        last_try = ChangeRow;
        w1.x     = x;

        range R(0, N);
        mat_new(R, R) = mat(R, R);
        for (auto k : R) { mat_new(i, k) = f(x, y_values[k]); }

        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }
      //------------------------------------------------------------------------------------------
      private:
      void complete_change_row() {
        x_values[w1.i] = w1.x;
        std::swap(mat, mat_new);
      }

      //------------------------------------------------------------------------------------------
      public:
      /**
     * Consider the change the row i and column j and the corresponding x and y
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_change_col_row(long i, long j, x_type const &x, y_type const &y) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(j < N);
        TRIQS_ASSERT(j >= 0);
        TRIQS_ASSERT(i < N);
        TRIQS_ASSERT(i >= 0);

        last_try = ChangeRowCol;
        w1.i     = i;
        w1.j     = j;
        w1.x     = x;
        w1.y     = y;

        range R(0, N);
        mat_new(R, R) = mat(R, R);

        for (auto k : R) {
          mat_new(i, k) = f(x, y_values[k]);
          mat_new(k, j) = f(x_values[k], y);
        }
        mat_new(i, j) = f(x, y);

        det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);

        return det_new / det;
      }

      //------------------------------------------------------------------------------------------
      private:
      void complete_change_col_row() {
        x_values[w1.i] = w1.x;
        y_values[w1.j] = w1.y;
        std::swap(mat, mat_new);
      }

      //------------------------------------------------------------------------------------------
      public:
      /**
       * Refill determinant with new values
       * New values are calculated as f(x_i, y_i)
       * Returns the ratio of det Minv_new / det Minv.
       * This routine does NOT make any modification. It has to be completed with complete_operation().
       */
      template <typename ArgumentContainer1, typename ArgumentContainer2>
      value_type try_refill(ArgumentContainer1 const &X, ArgumentContainer2 const &Y) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(X.size() == Y.size());

        long s = X.size();
        if (s > Nmax) {
          w_refill.reserve(2 * s);
          reserve(2 * s);
        }
        last_try = Refill;

        w_refill.clear();
        if (s == 0) { // treat empty matrix separately
          det_new = 1.0;
          clear_candidate_factorization();
        } else {
          std::copy(X.begin(), X.end(), std::back_inserter(w_refill.x_values));
          std::copy(Y.begin(), Y.end(), std::back_inserter(w_refill.y_values));
          for (long i = 0; i < s; ++i)
            for (long j = 0; j < s; ++j) mat_new(i, j) = f(w_refill.x_values[i], w_refill.y_values[j]);

          range R(0, s);
          det_new = factorize(mat_new(R, R), q_new, r_new, jpvt_new, det_q_new);
        }

        return det_new / det;
      }

      //------------------------------------------------------------------------------------------
      private:
      void complete_refill() {
        N = w_refill.x_values.size();

        // special empty case again
        if (N == 0) {
          clear();
          return;
        }

        std::swap(x_values, w_refill.x_values);
        std::swap(y_values, w_refill.y_values);
        std::swap(mat, mat_new);
      }

      //------------------------------------------------------------------------------------------

      public:
      /**
     *  Finish the move of the last try_xxx called.
     *  Throws if no try_xxx has been done or if the last operation was complete_operation.
     */
      void complete_operation() {
        switch (last_try) {
          case (Insert): complete_insert(); break;
          case (Remove): complete_remove(); break;
          case (ChangeCol): complete_change_col(); break;
          case (ChangeRow): complete_change_row(); break;
          case (ChangeRowCol): complete_change_col_row(); break;
          case (Insert2): complete_insert2(); break;
          case (Remove2): complete_remove2(); break;
          case (InsertK): complete_insert_k(); break;
          case (RemoveK): complete_remove_k(); break;
          case (Refill): complete_refill(); break;
          case (NoTry): return; break;
          default: TRIQS_RUNTIME_ERROR << "Misusing det_manip_qr"; // Never used?
        }

        // Swap in the candidate factorization (built during the try_*), like mat <- mat_new.
        std::swap(q_mat, q_new);
        std::swap(r_mat, r_new);
        std::swap(jpvt, jpvt_new);
        det_q = det_q_new;

        mat_inverse_is_valid = false;
        det                  = det_new;
        ++n_opts;

        last_try = NoTry;
      }

      /**
     *  Reject the previous try_xxx called.
     *  All try_xxx have to be either accepted (complete_operation) or rejected.
     */
      void reject_last_try() { last_try = NoTry; }

      // ----------------- A few short cuts   -----------------

      public:
      /// Insert (try_insert + complete)
      value_type insert(long i, long j, x_type const &x, y_type const &y) {
        auto r = try_insert(i, j, x, y);
        complete_operation();
        return r;
      }

      /// Insert_at_end (try_insert + complete)
      value_type insert_at_end(x_type const &x, y_type const &y) { return insert(N, N, x, y); }

      /// Insert2 (try_insert2 + complete)
      value_type insert2(long i0, long i1, long j0, long j1, x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
        auto r = try_insert2(i0, i1, j0, j1, x0, x1, y0, y1);
        complete_operation();
        return r;
      }

      /// Insert2_at_end (try_insert2 + complete)
      value_type insert2_at_end(x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
        return insert2(N, N + 1, N, N + 1, x0, x1, y0, y1);
      }

      /// Remove (try_remove + complete)
      value_type remove(long i, long j) {
        auto r = try_remove(i, j);
        complete_operation();
        return r;
      }

      /// Remove_at_end (try_remove + complete)
      value_type remove_at_end() { return remove(N - 1, N - 1); }

      /// Remove2 (try_remove2 + complete)
      value_type remove2(long i0, long i1, long j0, long j1) {
        auto r = try_remove2(i0, i1, j0, j1);
        complete_operation();
        return r;
      }

      /// Remove2_at_end (try_remove2 + complete)
      value_type remove2_at_end() { return remove2(N - 1, N - 2, N - 1, N - 2); }

      /// change_col (try_change_col + complete)
      value_type change_col(long j, y_type const &y) {
        auto r = try_change_col(j, y);
        complete_operation();
        return r;
      }

      /// change_row (try_change_row + complete)
      value_type change_row(long i, x_type const &x) {
        auto r = try_change_row(i, x);
        complete_operation();
        return r;
      }

      value_type change_one_row_and_one_col(long i, long j, x_type const &x, y_type const &y) {
        auto r = try_change_col_row(i, j, x, y);
        complete_operation();
        return r;
      }

      ///
      enum RollDirection { None, Up, Down, Left, Right };

      /**
       * "Cyclic Rolling" of the determinant.
       *
       * Right : Move the Nth col to the first col cyclically.
       * Left  : Move the first col to the Nth, cyclically.
       * Up    : Move the first row to the Nth, cyclically.
       * Down  : Move the Nth row to the first row cyclically.
       *
       * Returns -1 is the roll changes the sign of the det, 1 otherwise
       * NB : this routine is not a try_xxx : it DOES make the modification and does not need to be completed...
       * WHY is it like this ???? : try_roll : return det +1/-1.
       */
      int roll_matrix(RollDirection roll) {
	switch (roll) {
	  case (None): return 1;
	  case (Down): std::rotate(begin(x_values), end(x_values) - 1, end(x_values)); break;
	  case (Up): std::rotate(begin(x_values), begin(x_values) + 1, end(x_values)); break;
	  case (Right): std::rotate(begin(y_values), end(y_values) - 1, end(y_values)); break;
	  case (Left): std::rotate(begin(y_values), begin(y_values) + 1, end(y_values)); break;
	  default: assert(0);
	}
	for (long i = 0; i < N; ++i)
	  for (long j = 0; j < N; ++j) mat(i, j) = f(x_values[i], y_values[j]);
        // signature of the cycle of order N : (-1)^(N-1)
        if ((N - 1) % 2 == 1) {
	  det = -det;
	  return -1;
	}
        return 1;
      }
  };

} // namespace triqs::det_manip
