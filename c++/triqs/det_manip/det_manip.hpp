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
 * @brief Provides a class to manipulate determinants efficiently.
 */

#pragma once

#include "./utils.hpp"
#include "./work_data.hpp"
#include "../utility/callable_traits.hpp"
#include "../utility/first_include.hpp"
#include "../arrays.hpp"

#include <nda/nda.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <ranges>
#include <vector>

namespace triqs::det_manip {

  namespace blas = nda::blas;

  // ================ det_manip implementation =====================

  /**
   * @ingroup triqs-detmanip
   * @brief Manipulate determinants and ratios of determinants for CTQMC solvers.
   *
   * @details The code and the following documentation uses the notation introduced in @ref triqs-detmanip.
   *
   * The determinant \f$ \det(F^{(n)}) \f$ or the underlying matrix \f$ F^{(n)} \f$ is manipulated by performing
   * operations. Most of those operations are split into a `try` and a `complete` function. While the `try` functions
   * only produce intermediate results that are necessary to calculate the ratio of the new to the old determinant, the
   * `complete` functions actually perform the operation and update the matrix and the determinant.
   *
   * The following operations are supported:
   * - Swap two rows (see swap_row()).
   * - Swap two columns (see swap_col()).
   * - Circular shift of rows or columns (see roll_matrix()).
   * - Insert one row and column (see try_insert()).
   * - Insert \f$ k \f$ rows and columns (see try_insert_k() and try_insert2()).
   * - Remove one row and column (see try_remove()).
   * - Remove \f$ k \f$ rows and columns (see try_remove_k() and try_remove2()).
   * - Change one column (see try_change_col()).
   * - Change one row (see try_change_row()).
   * - Change one row and column (see try_change_col_row()).
   * - Build a completely new matrix (see try_refill()).
   *
   * @tparam F Callable object \f$ f \f$ that takes two arguments and returns a real or complex value. It
   * determines the elements of the matrix via \f$ F^{(n)}_{ij} = f(x_i, y_j) \f$.
   */
  template <MatrixBuilder F> class det_manip {
    public:
    using x_type      = detail::get_xarg_t<F>;
    using y_type      = detail::get_yarg_t<F>;
    using value_type  = detail::get_result_t<F>;
    using matrix_type = nda::matrix<value_type>;

    /**
     * @brief Construct a det_manip object with a callable `F` and an initial capacity for the data storages.
     *
     * @details Like for `std::vector`, the capacity grows automatically (by a factor of 2) when needed, but this can
     * yield a performance penalty if it happens too often.
     *
     * @param f Callable `F` object (a copy is stored in the class).
     * @param ncap Initial capacity for the size of the matrix, i.e. the maximum number of rows and columns.
     * @param kcap Initial capacity for the maximum number of rows and columns that can be added or removed in a single
     * operation.
     */
    det_manip(F f, long ncap, long kcap = 1) : f_(std::move(f)) { reserve(ncap, kcap); }

    /**
     * @brief Construct a det_manip object with a callable `F` and two ranges containing the arguments for
     * the matrix builder.
     *
     * @tparam X triqs::det_manip::MatrixBuilderXRange.
     * @tparam Y triqs::det_manip::MatrixBuilderYRange.
     * @param f Callable `F` object (a copy is stored in the class).
     * @param x_rg Range containing the first arguments \f$ \mathbf{x} \f$.
     * @param y_rg Range containing the second arguments \f$ \mathbf{y} \f$.
     */
    template <typename X, typename Y>
      requires(MatrixBuilderXRange<X, F> && MatrixBuilderYRange<Y, F>)
    det_manip(F f, X &&x_rg, Y &&y_rg) // NOLINT (ranges need not be forwarded)
       : f_(std::move(f)), n_(static_cast<long>(std::ranges::size(x_rg))) {
      // check input sizes
      if (n_ != static_cast<long>(std::ranges::size(y_rg)))
        TRIQS_RUNTIME_ERROR << "Error in det_manip::det_manip: Argument ranges have different sizes";

      // early return if the argument ranges are empty
      if (n_ == 0) {
        reserve(30);
        return;
      }

      // reserve memory and fill the data storages
      reserve(n_ * 2);
      set_xy(x_rg, y_rg);

      // determinant and inverse matrix
      auto M_v = M_(nda::range(size()), nda::range(size()));
      nda::for_each(M_v.shape(), [this, &M_v](auto i, auto j) { M_v(i, j) = f_(x_[i], y_[j]); });
      det_ = nda::linalg::det(M_v);
      M_v  = nda::linalg::inv(M_v);
    }

    /**
     * @brief Reserve memory and resize the data storages.
     *
     * @details Like for `std::vector`, this reserves memory for a bigger matrix size. It only reserves/resizes if the
     * requested capacity is larger than the current one. It preserves the matrix \f$ M^{(n)} \f$ but not the temporary
     * working data, so it must NOT be called between a `try_*` function and the corresponding complete_operation().
     *
     * @param new_ncap New capacity for the size of the matrix, i.e. the maximum number of rows and columns.
     * @param new_kcap New capacity for the maximum number of rows and columns that can be added or removed in a single
     * operation. It sizes the working data used by the `try_*_k` functions.
     */
    void reserve(long new_ncap, long new_kcap = 1) {
      if (new_kcap > kmax_tried) {
        kmax_tried = new_kcap;
        if (new_ncap <= ncap_) wk_.resize(ncap_, kmax_tried);
      }
      if (new_ncap > ncap_) {
        ncap_ = 2 * new_ncap;

        matrix_type M_copy(M_);
        M_.resize(ncap_, ncap_);
        auto rg    = nda::range(M_copy.extent(0));
        M_(rg, rg) = M_copy;

        row_perm_.reserve(ncap_);
        col_perm_.reserve(ncap_);
        x_.reserve(ncap_);
        y_.reserve(ncap_);

        w1_.resize(ncap_);
        wk_.resize(ncap_, kmax_tried);
      }
    }

    /**
     * @brief Clear the data storages and reset the matrix to size zero.
     */
    void clear() {
      n_        = 0;
      sign_     = 1;
      det_      = 1;
      last_try_ = try_tag::NoTry;
      row_perm_.clear();
      col_perm_.clear();
      x_.clear();
      y_.clear();
    }

    /**
     * @brief Get the threshold being used when testing for a singular matrix.
     * @details See set_singular_threshold() for details.
     * @return Threshold value.
     */
    [[nodiscard]] double get_singular_threshold() const { return singular_threshold_; }

    /**
     * @brief Set the threshold being used when testing for a singular matrix (default: -1).
     *
     * @details The threshold \f$ \epsilon \f$ determines when a matrix \f$ M \f$ is considered singular. A matrix is
     * considered to be singular if \f$ |\det(M)| < \epsilon \f$.
     *
     * If \f$ \epsilon \f$ is negative, it simply checks if the determinant is not `std::isnormal`.
     *
     * @param threshold Threshold value.
     */
    void set_singular_threshold(double threshold) { singular_threshold_ = threshold; }

    /**
     * @brief Get the number of operations before a consistency check is performed.
     * @details See set_n_operations_before_check() for details.
     * @return Number of operations.
     */
    [[nodiscard]] double get_n_operations_before_check() const { return nops_before_check_; }

    /**
     * @brief Set the number of operations before a consistency check is performed (default: 100).
     * @details See complete_operation(), which triggers the check once the counter exceeds this number.
     * @param n Number of operations.
     */
    void set_n_operations_before_check(uint64_t n) { nops_before_check_ = n; }

    /**
     * @brief Get the precision threshold that determines when to print a warning.
     * @details See set_precision_warning() for details.
     * @return Threshold value.
     */
    [[nodiscard]] double get_precision_warning() const { return precision_warning_; }

    /**
     * @brief Set the precision threshold that determines when to print a warning (default: 1e-8).
     *
     * @details In case we compare two matrices \f$ A \f$ and \f$ B \f$, a warning is printed when \f$ 2 \lVert A - B
     * \rVert >= \epsilon \left( \lVert A \rVert + \lVert B \rVert \right) \f$, where \f$ \lVert \cdot \rVert \f$ is the
     * max norm.
     *
     * In case we compare two scalar values \f$ a \f$ and \f$ b \f$, a warning is printed when \f$ 2 |a - b| >= \epsilon
     * (|a| + |b|) \f$.
     *
     * @param threshold Threshold value.
     */
    void set_precision_warning(double threshold) { precision_warning_ = threshold; }

    /**
     * @brief Get the precision threshold that determines when to throw an exception.
     * @details See set_precision_warning() for details.
     * @return Threshold value.
     */
    [[nodiscard]] double get_precision_error() const { return precision_error_; }

    /**
     * @brief Set the precision threshold that determines when to throw an exception (default: 1e-5).
     * @details See set_precision_warning() for details.
     * @param threshold Threshold value.
     */
    void set_precision_error(double threshold) { precision_error_ = threshold; }

    //----------------------- READ ACCESS TO DATA ----------------------------------

    /**
     * @brief Get the current size of the matrix.
     * @return Number of rows/columns of the matrix.
     */
    [[nodiscard]] long size() const { return n_; }

    /**
     * @brief Get the matrix builder argument \f$ x_i \f$ that determines the elements of the i<sup>th</sup> row in the
     * original matrix \f$ F^{(n)} \f$.
     * @param i Argument index.
     * @return Argument value \f$ x_i \f$.
     */
    x_type const &get_x(long i) const { return x_[row_perm_[i]]; }

    /**
     * @brief Get the matrix builder argument \f$ y_j \f$ that determines the elements of the j<sup>th</sup> column in
     * the original matrix \f$ F^{(n)} \f$.
     * @param j Argument index.
     * @return Argument value \f$ y_j \f$.
     */
    y_type const &get_y(long j) const { return y_[col_perm_[j]]; }

    /**
     * @brief Get a vector with all matrix builder arguments \f$ \mathbf{x} \f$.
     * @details Warning: this is slow, since it creates a new copy and reorders the rows.
     * @return `std::vector` containing the arguments \f$ x_i \f$ in the order of the original matrix \f$ F^{(n)} \f$.
     */
    std::vector<x_type> get_x() const {
      std::vector<x_type> res;
      res.reserve(n_);
      for (long i : range(n_)) res.emplace_back(x_[row_perm_[i]]);
      return res;
    }

    /**
     * @brief Get a vector with all matrix builder arguments \f$ \mathbf{y} \f$.
     * @details Warning: this is slow, since it creates a new copy and reorders the columns.
     * @return `std::vector` containing the arguments \f$ y_j \f$ in the order of the original matrix \f$ F^{(n)} \f$.
     */
    std::vector<y_type> get_y() const {
      std::vector<y_type> res;
      res.reserve(n_);
      for (long i : range(n_)) res.emplace_back(y_[col_perm_[i]]);
      return res;
    }

    /**
     * @brief Get the matrix builder arguments \f$ \mathbf{x} \f$ in the order of the matrix \f$ G^{(n)} \f$.
     *
     * @details Advanced: this is the internal storage order, which differs by some permutation from the order given by
     * the user. Useful for some performance-critical loops, to be used together with the other `*_internal_order`
     * functions.
     *
     * @return `std::vector` containing the arguments \f$ x_i \f$.
     */
    std::vector<x_type> const &get_x_internal_order() const { return x_; }

    /**
     * @brief Get the matrix builder arguments \f$ \mathbf{y} \f$ in the order of the matrix \f$ G^{(n)} \f$.
     * @details See get_x_internal_order() for details.
     * @return `std::vector` containing the arguments \f$ y_j \f$.
     */
    std::vector<y_type> const &get_y_internal_order() const { return y_; }

    /**
     * @brief Get the callable `F` object \f$ f \f$ used as the matrix builder.
     * @return Const reference to the stored callable.
     */
    F const &get_function() const { return f_; }

    /**
     * @brief Get the determinant of the original matrix \f$ F^{(n)} \f$.
     * @return Determinant \f$ \det(F^{(n)}) = s^{(n)} \det(G^{(n)}) \f$.
     */
    value_type determinant() {
      if (is_singular()) regenerate();
      return sign_ * det_;
    }

    /**
     * @brief Get an element of the inverse matrix.
     *
     * @details The inverse matrix is given by
     * \f[
     *   [F^{(n)}]^{-1} = (P^{(n)}_r G^{(n)} P^{(n)}_c)^{-1} = [P^{(n)}_c]^T [G^{(n)}]^{-1} [P^{(n)}_r]^T \; .
     * \f]
     *
     * @param i Row index.
     * @param j Column index.
     * @return The matrix element \f$ [F^{(n)}]^{-1}_{ij} \f$.
     */
    // warning : need to invert the 2 permutations: (AP)^-1= P^-1 A^-1.
    value_type inverse_matrix(int i, int j) const { return M_(col_perm_[i], row_perm_[j]); }

    /**
     * @brief Get the full inverse matrix \f$ [F^{(n)}]^{-1} \f$.
     * @details See inverse_matrix(int, int) for details. Warning: this is slow, since it creates a new copy and
     * reorders the rows/columns.
     * @return The inverse matrix \f$ [F^{(n)}]^{-1} \f$.
     */
    matrix_type inverse_matrix() const {
      matrix_type res(n_, n_);
      for (long i = 0; i < n_; i++)
        for (long j = 0; j < n_; j++) res(i, j) = inverse_matrix(i, j);
      return res;
    }

    /**
     * @brief Get an element of the matrix \f$ M^{(n)} = [G^{(n)}]^{-1} \f$.
     * @details Advanced: uses the internal storage order, see get_x_internal_order().
     * @param i Row index.
     * @param j Column index.
     * @return The matrix element \f$ M^{(n)}_{ij} \f$.
     */
    value_type inverse_matrix_internal_order(int i, int j) const { return M_(i, j); }

    /**
     * @brief Get the full inverse matrix \f$ M^{(n)} = [G^{(n)}]^{-1} \f$.
     * @details Advanced: uses the internal storage order, see get_x_internal_order().
     * @return The inverse matrix \f$ M^{(n)} \f$.
     */
    nda::matrix_const_view<value_type> inverse_matrix_internal_order() const { return M_(range(n_), range(n_)); }

    /**
     * @brief Get the original matrix \f$ F^{(n)} \f$.
     * @details Warning: this is slow, since it creates a new matrix and re-evaluates the matrix builder.
     * @return The original matrix \f$ F^{(n)} \f$.
     */
    matrix_type matrix() const {
      matrix_type res(n_, n_);
      for (long i = 0; i < n_; i++)
        for (long j = 0; j < n_; j++) res(i, j) = f_(get_x(i), get_y(j));
      return res;
    }

    /**
     * @brief For-each implementation for triqs::det_manip::det_manip objects.
     *
     * @details It loops over all elements of the matrix \f$ M^{(n)} \f$ and calls the given callable object for each
     * element together with the corresponding arguments \f$ x_i \f$ and \f$ y_j \f$. The order of iteration is not
     * fixed; it is optimised for memory traversal.
     *
     * @tparam LambdaType Callable type.
     * @param d triqs::det_manip::det_manip object.
     * @param fn Callable object that takes three arguments: \f$ x_i \f$, \f$ y_j \f$, and \f$ M_{ji} \f$.
     */
    template <typename LambdaType> friend void foreach (det_manip const &d, LambdaType const &fn) {
      nda::for_each(std::array{d.n_, d.n_}, [&fn, &d](int i, int j) { return fn(d.x_[i], d.y_[j], d.M_(j, i)); });
    }

    // ------------------------- OPERATIONS -----------------------------------------------

    /**
     * @brief Swap two rows.
     *
     * @details It simply performs the transposition in the row permutation vector and changes the sign \f$ s^{(n)} \f$
     * associated with the permutation matrices. It is very quick: we just change the permutation table internally, and
     * this operation is so simple that it has no `try`/`complete` step.
     *
     * Since we are only changing the matrix \f$ P^{(n)}_r \f$, the matrix \f$ G^{(n)} \f$ and its determinant remain
     * unchanged.
     *
     * @param i Index of the first row to swap.
     * @param j Index of the second row to swap.
     */
    void swap_row(long i, long j) {
      if (i == j) return;
      std::swap(row_perm_[i], row_perm_[j]);
      sign_ = -sign_;
      // we do not need to change the det, or the matrix, just the permutation
    }

    /**
     * @brief Swap two columns.
     *
     * @details It simply performs the transposition in the column permutation vector and changes the sign \f$ s^{(n)}
     * \f$ associated with the permutation matrices. It is very quick: we just change the permutation table internally,
     * and this operation is so simple that it has no `try`/`complete` step.
     *
     * Since we are only changing the matrix \f$ P^{(n)}_c \f$, the matrix \f$ G^{(n)} \f$ and its determinant remain
     * unchanged.
     *
     * @param i Index of the first column to swap.
     * @param j Index of the second column to swap.
     */
    void swap_col(long i, long j) {
      if (i == j) return;
      std::swap(col_perm_[i], col_perm_[j]);
      sign_ = -sign_;
    }

    /**
     * @brief Direction of the roll_matrix() operation.
     *
     * @details It specifies the direction of the circular shift performed on either the rows or columns of the matrix
     * \f$ F^{(n)} \f$. The following directions are supported:
     *
     * - `None`: No roll operation is performed.
     * - `Up`: Roll the rows up (move the first row to the last, cyclically).
     * - `Down`: Roll the rows down (move the last row to the first, cyclically).
     * - `Left`: Roll the columns to the left (move the first column to the last, cyclically).
     * - `Right`: Roll the columns to the right (move the last column to the first, cyclically).
     */
    enum RollDirection { None, Up, Down, Left, Right };

    /**
     * @brief Perform a circular shift permutation on the rows or columns of the matrix \f$ F^{(n)} \f$.
     *
     * @details See RollDirection for the supported directions. This routine is not a `try_*` operation: it does make
     * the modification and does not need to be completed.
     *
     * A circular shift permutation of a finite set is equivalent to \f$ N \f$ transpositions, where \f$ N \f$ is the
     * size of the set. The sign of the permutation is therefore given by \f$ (-1)^{N-1} \f$.
     *
     * @param roll Direction of the roll operation.
     * @return -1 if the roll changes the sign of the determinant, 1 otherwise.
     */
    int roll_matrix(RollDirection roll) {
      long tmp      = 0;
      const long NN = n_;
      switch (roll) {
        case (None): return 1;
        case (Down):
          tmp = row_perm_[n_ - 1];
          for (long i = NN - 2; i >= 0; i--) row_perm_[i + 1] = row_perm_[i];
          row_perm_[0] = tmp;
          break;
        case (Up):
          tmp = row_perm_[0];
          for (long i = 0; i < n_ - 1; i++) row_perm_[i] = row_perm_[i + 1];
          row_perm_[n_ - 1] = tmp;
          break;
        case (Right):
          tmp = col_perm_[n_ - 1];
          for (long i = NN - 2; i >= 0; i--) col_perm_[i + 1] = col_perm_[i];
          col_perm_[0] = tmp;
          break;
        case (Left):
          tmp = col_perm_[0];
          for (long i = 0; i < n_ - 1; i++) col_perm_[i] = col_perm_[i + 1];
          col_perm_[n_ - 1] = tmp;
          break;
        default: assert(0);
      }
      // signature of the cycle of order N : (-1)^(N-1)
      if ((n_ - 1) % 2 == 1) {
        sign_ *= -1;
        return -1;
      }
      return 1;
    }

    /**
     * @brief Try to insert one row and column.
     *
     * @details The row is inserted at position \f$ i \f$ and the column at position \f$ j \f$ in the original matrix
     * \f$ F^{(n)} \f$. Their elements are determined by the given matrix builder arguments \f$ x \f$ and \f$ y \f$.
     *
     * The new column/row will be at column \f$ j \f$, row \f$ i \f$, with \f$ 0 \leq i, j \leq n \f$. The current
     * column \f$ j \f$ (resp. row \f$ i \f$) becomes column \f$ j + 1 \f$ (resp. row \f$ i + 1 \f$). Inserting at
     * \f$ n \f$ simply adds the new row/column at the end.
     *
     * This is a special case of try_insert_k() with \f$ k = 1 \f$.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the elements of the new row.
     * @param y Argument to the matrix builder that determines the elements of the new column.
     * @return Determinant ratio \f$ \det(F^{(n+1)}) / \det(F^{(n)}) \f$.
     */
    value_type try_insert(long i, long j, x_type const &x, y_type const &y) {

      // check input and store it for complete_operation
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(0 <= i and i <= n_);
      TRIQS_ASSERT(0 <= j and j <= n_);
      reserve(n_ + 1);
      last_try_ = try_tag::Insert;
      w1_.i     = i;
      w1_.j     = j;
      w1_.x     = x;
      w1_.y     = y;

      // treat empty matrix separately
      if (n_ == 0) {
        newdet_  = f_(x, y);
        newsign_ = 1;
        return value_type(newdet_);
      }

      // I add the row and col and the end. If the move is rejected,
      // no effect since n_ will not be changed : Minv(i,j) for i,j>=n_ has no meaning.
      for (long l = 0; l < n_; l++) {
        w1_.B(l) = f_(x_[l], y);
        w1_.C(l) = f_(x, y_[l]);
      }
      range RN(n_);
      //w1_.MB(R) = M_(R,R) * w1_.B(R);// OPTIMIZE BELOW
      blas::gemv(1.0, M_(RN, RN), w1_.B(RN), 0.0, w1_.MB(RN));
      w1_.ksi  = f_(x, y) - nda::blas::dot(w1_.C(RN), w1_.MB(RN));
      newdet_  = det_ * w1_.ksi;
      newsign_ = ((i + j) % 2 == 0 ? sign_ : -sign_); // since N-i0 + N-j0  = i0+j0 [2]
      return w1_.ksi * (newsign_ * sign_);            // sign_ is unity, hence 1/sign_ == sign_
    }

    /**
     * @brief Try to insert one row and column, providing the new elements through callables instead of the matrix
     * builder.
     *
     * @details Like try_insert(), but the elements of the new row and column are supplied directly: `fx` gives the new
     * row coefficients \f$ f(x_l, y) \f$, `fy` gives the new column coefficients \f$ f(x, y_l) \f$, and `ksi` is the
     * corner element \f$ f(x, y) \f$ at the intersection of the new row and column.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @tparam Fx Callable type for the new row coefficients.
     * @tparam Fy Callable type for the new column coefficients.
     * @param i Position of the row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param fx Callable returning the new row coefficient for a given argument \f$ x_l \f$.
     * @param fy Callable returning the new column coefficient for a given argument \f$ y_l \f$.
     * @param ksi Corner element at the intersection of the new row and column.
     * @return Determinant ratio \f$ \det(F^{(n+1)}) / \det(F^{(n)}) \f$.
     */
    template <typename Fx, typename Fy> value_type try_insert_from_function(long i, long j, Fx fx, Fy fy, value_type const ksi) {

      // check input and store it for complete_operation
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(0 <= i and i <= n_);
      TRIQS_ASSERT(0 <= j and j <= n_);
      reserve(n_ + 1);
      last_try_ = try_tag::Insert;
      w1_.i     = i;
      w1_.j     = j;

      // treat empty matrix separately
      if (n_ == 0) {
        newdet_  = ksi;
        newsign_ = 1;
        return newdet_;
      }

      // I add the row and col and the end. If the move is rejected,
      // no effect since n_ will not be changed : Minv(i,j) for i,j>=n_ has no meaning.
      for (long l = 0; l < n_; l++) {
        w1_.B(l) = fx(x_[l]);
        w1_.C(l) = fy(y_[l]);
      }
      range RN(n_);
      //w1_.MB(R) = M_(R,R) * w1_.B(R);// OPTIMIZE BELOW
      blas::gemv(1.0, M_(RN, RN), w1_.B(RN), 0.0, w1_.MB(RN));
      w1_.ksi  = ksi - nda::blas::dot(w1_.C(RN), w1_.MB(RN));
      newdet_  = det_ * w1_.ksi;
      newsign_ = ((i + j) % 2 == 0 ? sign_ : -sign_); // since N-i0 + N-j0  = i0+j0 [2]
      return w1_.ksi * (newsign_ * sign_);            // sign_ is unity, hence 1/sign_ == sign_
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the insert operation.
    void complete_insert() {
      // store the new value of x,y. They are seen through the same permutations as rows and cols resp.
      x_.push_back(w1_.x);
      y_.push_back(w1_.y);
      row_perm_.push_back(0);
      col_perm_.push_back(0);

      // special empty case again
      if (n_ == 0) {
        n_       = 1;
        M_(0, 0) = 1 / value_type(newdet_);
        return;
      }

      range RN(n_);
      //w1_.MC(R1) = transpose(M_(R1,R1)) * w1_.C(R1); //OPTIMIZE BELOW
      blas::gemv(1.0, transpose(M_(RN, RN)), w1_.C(RN), 0.0, w1_.MC(RN));
      w1_.MC(n_) = -1;
      w1_.MB(n_) = -1;

      n_++;
      RN = range(n_);

      // keep the real position of the row/col
      // since we insert a col/row, we have first to push the col at the right
      // and then say that col w1_.i is stored in n_, the last col.
      // same for rows
      for (long i = n_ - 2; i >= w1_.i; i--) row_perm_[i + 1] = row_perm_[i];
      row_perm_[w1_.i] = n_ - 1;
      for (long i = n_ - 2; i >= w1_.j; i--) col_perm_[i + 1] = col_perm_[i];
      col_perm_[w1_.j] = n_ - 1;

      // Minv is ok, we need to complete
      w1_.ksi = 1 / w1_.ksi;

      // compute the change to the inverse
      // M += w1_.ksi w1_.MB w1_.MC with BLAS. first put the 0
      M_(RN, n_ - 1) = 0;
      M_(n_ - 1, RN) = 0;
      //M_(R,R) += w1_.ksi* w1_.MB(R) * w1_.MC(R)// OPTIMIZE BELOW
      blas::ger(w1_.ksi, w1_.MB(RN), w1_.MC(RN), M_(RN, RN));
    }

    public:
    //------------------------------------------------------------------------------------------

    /**
     * @brief Try to insert \f$ k \f$ rows and columns.
     *
     * @details The positions of the new rows and columns in the original matrix \f$ F^{(n+k)} \f$ are specified in the
     * tuples \f$ \mathbf{i} \f$ and \f$ \mathbf{j} \f$, respectively. Their elements are determined by the given
     * matrix builder arguments \f$ \mathbf{x} \f$ and \f$ \mathbf{y} \f$.
     *
     * Since we are working with \f$ G^{(n)} \f$, we are free to insert the rows and columns at the bottom and right of
     * the matrix and use the update formulas presented in @ref triqs-detmanip.
     *
     * We use the following order for the rows and columns to be inserted:
     * - The first row (column) in \f$ C \f$ (\f$ B \f$) corresponds to the row (column) with the smallest index in the
     * matrix \f$ F^{(n)} \f$.
     * - The second row (column) in \f$ C \f$ (\f$ B \f$) corresponds to the row (column) with the second smallest index
     * in the matrix \f$ F^{(n)} \f$.
     * - And so on.
     *
     * The expression for the new determinant \f$ \det(G^{(n+k)}) \f$ can be found at @ref triqs-detmanip and the new
     * sign associated with the permutation matrices can be written as
     * \f[
     *   s^{(n+k)} = \det(P^{(n)}_r) \det(P^{(n)}_c) \det(P1) \det(P2) = s^{(n)} \det(P1) \det(P2) \; ,
     * \f]
     * where \f$ P1 \f$ and \f$ P2 \f$ are permutation matrices that move the inserted rows and columns to their
     * respective positions in the original matrix \f$ F^{(n)} \f$.
     *
     * The function returns the ratio
     * \f[
     *   R = \frac{\det(F^{(n+k)})}{\det(F^{(n)})} = \frac{\det(G^{(n+k)}) s^{(n+k)}}{\det(G^{(n)}) s^{(n)}} =
     *   \det(D - C M^{(n)} B) s^{(n+k)} s^{(n)} = \det(S^{-1}) s^{(n+k)} s^{(n)} \; ,
     * \f]
     * where we used the fact that \f$ s^{(n)} = 1 / s^{(n)} \f$.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Positions of the rows to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j Positions of the columns to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param x Arguments to the matrix builder that determine the elements of the new rows.
     * @param y Arguments to the matrix builder that determine the elements of the new columns.
     * @return Determinant ratio \f$ \det(F^{(n+k)}) / \det(F^{(n)}) \f$.
     */
    value_type try_insert_k(std::vector<long> i, std::vector<long> j, std::vector<x_type> x, std::vector<y_type> y) {
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(i.size() == j.size());
      TRIQS_ASSERT(j.size() == x.size());
      TRIQS_ASSERT(x.size() == y.size());

      k_tried = static_cast<long>(i.size());
      reserve(n_ + k_tried, k_tried);
      last_try_ = try_tag::InsertK;

      auto const argsort = [](auto const &vec) {
        std::vector<long> idx(vec.size());
        std::iota(idx.begin(), idx.end(), static_cast<long>(0));
        std::stable_sort(idx.begin(), idx.end(), [&vec](long const lhs, long const rhs) { return vec[lhs] < vec[rhs]; });
        return idx;
      };
      std::vector<long> idx = argsort(i);
      std::vector<long> idy = argsort(j);

      // store it for complete_operation
      for (long l = 0; l < k_tried; ++l) {
        wk_.i[l] = i[idx[l]];
        wk_.x[l] = x[idx[l]];
        wk_.j[l] = j[idy[l]];
        wk_.y[l] = y[idy[l]];
      };

      // check consistency
      for (int l = 0; l < k_tried - 1; ++l) {
        TRIQS_ASSERT(wk_.i[l] != wk_.i[l + 1] and 0 <= wk_.i[l] and wk_.i[l] < n_ + k_tried);
        TRIQS_ASSERT(wk_.j[l] != wk_.j[l + 1] and 0 <= wk_.j[l] and wk_.j[l] < n_ + k_tried);
      }

      // w1_.ksi = Delta(x_,y_) - Cw.MB using BLAS
      for (long m = 0; m < k_tried; ++m) {
        for (long n = 0; n < k_tried; ++n) { wk_.ksi(m, n) = f_(wk_.x[m], wk_.y[n]); }
      }

      // treat empty matrix separately
      if (n_ == 0) {
        newdet_  = wk_.det_ksi(k_tried);
        newsign_ = 1;
        return value_type(newdet_);
      }

      // I add the rows and cols and the end. If the move is rejected,
      // no effect since n_ will not be changed : inv_mat(i,j) for i,j>=n_ has no meaning.
      for (long n = 0; n < n_; n++) {
        for (long l = 0; l < k_tried; ++l) {
          wk_.B(n, l) = f_(x_[n], wk_.y[l]);
          wk_.C(l, n) = f_(wk_.x[l], y_[n]);
        }
      }
      range RN(n_), Rk(k_tried);
      //wk_.MB(RN,Rk) = M_(RN,n_) * wk_.B(RN,Rk); // OPTIMIZE BELOW
      blas::gemm(1.0, M_(RN, RN), wk_.B(RN, Rk), 0.0, wk_.MB(RN, Rk));
      //ksi -= wk_.C (Rk, RN) * wk_.MB(RN, Rk); // OPTIMIZE BELOW
      blas::gemm(-1.0, wk_.C(Rk, RN), wk_.MB(RN, Rk), 1.0, wk_.ksi(Rk, Rk));
      auto ksi     = wk_.det_ksi(k_tried);
      newdet_      = det_ * ksi;
      long idx_sum = 0;
      for (long l = 0; l < k_tried; ++l) { idx_sum += wk_.i[l] + wk_.j[l]; }
      newsign_ = (idx_sum % 2 == 0 ? sign_ : -sign_); // since N-i0 + N-j0 + N + 1 -i1 + N+1 -j1 = i0+j0 [2]
      return ksi * (newsign_ * sign_);                // sign_ is unity, hence 1/sign_ == sign_
    }
    /**
     * @brief Try to insert two rows and columns.
     *
     * @details The rows are inserted at the positions \f$ i_0 \f$ and \f$ i_1 \f$ and the columns at the positions
     * \f$ j_0 \f$ and \f$ j_1 \f$ in the original matrix \f$ F^{(n)} \f$. Their elements are determined by the given
     * matrix builder arguments \f$ x_0 \f$, \f$ x_1 \f$, \f$ y_0 \f$ and \f$ y_1 \f$.
     *
     * It simply calls the more general try_insert_k().
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i0 Position of the first row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param i1 Position of the second row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j0 Position of the first column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j1 Position of the second column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param x0 Argument to the matrix builder that determines the elements of the first new row.
     * @param x1 Argument to the matrix builder that determines the elements of the second new row.
     * @param y0 Argument to the matrix builder that determines the elements of the first new column.
     * @param y1 Argument to the matrix builder that determines the elements of the second new column.
     * @return Determinant ratio \f$ \det(F^{(n+2)}) / \det(F^{(n)}) \f$.
     */
    value_type try_insert2(long i0, long i1, long j0, long j1, x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
      return try_insert_k({i0, i1}, {j0, j1}, {x0, x1}, {y0, y1});
    }

    // ---- Helper: flatten an nda::Array to a std::vector in C-order ----
    private:
    template <nda::Array A> static auto flatten_array(A const &a) {
      auto v    = std::vector<typename A::value_type>(a.size());
      long flat = 0;
      nda::for_each(a.shape(), [&](auto... idx) { v[flat++] = a(idx...); });
      return v;
    }

    public:
    /**
     * @brief Compute independent single-insertion determinant ratios at position \f$ (i, j) \f$ for paired elements of
     * `xs` and `ys`.
     *
     * @details For each pair `(xs[m], ys[m])` it computes the determinant ratio that a single try_insert() at position
     * \f$ (i, j) \f$ would produce, without modifying the internal state. The inputs must be `nda::Array` objects with
     * the same rank and shape, and the result has the same shape.
     *
     * @tparam X `nda::Array` type of the first arguments.
     * @tparam Y `nda::Array` type of the second arguments.
     * @param i Position of the row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param xs Array of first arguments to the matrix builder.
     * @param ys Array of second arguments to the matrix builder.
     * @return Array of determinant ratios with the same shape as `xs` and `ys`.
     */
    template <nda::Array X, nda::Array Y>
      requires(nda::get_rank<X> == nda::get_rank<Y>)
    auto insert_ratios(long i, long j, X const &xs, Y const &ys) const -> nda::array<value_type, nda::get_rank<X>> {
      constexpr int R = nda::get_rank<X>;
      TRIQS_ASSERT(xs.shape() == ys.shape());
      TRIQS_ASSERT(0 <= i and i <= n_);
      TRIQS_ASSERT(0 <= j and j <= n_);

      long nbatch         = xs.size();
      value_type sign_fac = ((i + j) % 2 == 0 ? 1 : -1);
      nda::array<value_type, R> result(xs.shape());

      if (nbatch == 0) return result;

      // Flatten inputs for BLAS
      auto xs_flat = flatten_array(xs);
      auto ys_flat = flatten_array(ys);

      if (n_ == 0) {
        for (long m = 0; m < nbatch; ++m) result.data()[m] = sign_fac * f_(xs_flat[m], ys_flat[m]);
        return result;
      }

      range RN(n_);

      // Build B(n_, nbatch) and C(nbatch, n_) matrices
      nda::matrix<value_type> B(n_, nbatch), C(nbatch, n_), MB(n_, nbatch);
      for (long l = 0; l < n_; ++l)
        for (long m = 0; m < nbatch; ++m) B(l, m) = f_(x_[l], ys_flat[m]);
      for (long m = 0; m < nbatch; ++m)
        for (long l = 0; l < n_; ++l) C(m, l) = f_(xs_flat[m], y_[l]);

      // MB = M_ * B -- single BLAS3 gemm
      blas::gemm(1.0, M_(RN, RN), B, 0.0, MB);

      // Compute each ratio: ksi_m = f_(xs[m], ys[m]) - C[m,:] . MB[:,m]
      for (long m = 0; m < nbatch; ++m) {
        value_type dot = 0;
        for (long l = 0; l < n_; ++l) dot += C(m, l) * MB(l, m);
        result.data()[m] = sign_fac * (f_(xs_flat[m], ys_flat[m]) - dot);
      }

      return result;
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the insert_k operation.
    void complete_insert_k() {

      // store the new value of x,y. They are seen through the same permutations as rows and cols resp.
      for (int l = 0; l < k_tried; ++l) {
        x_.push_back(wk_.x[l]);
        y_.push_back(wk_.y[l]);
        row_perm_.push_back(0);
        col_perm_.push_back(0);
      }

      range Rk(0, k_tried);
      // treat empty matrix separately
      if (n_ == 0) {
        n_         = k_tried;
        M_(Rk, Rk) = nda::linalg::inv(wk_.ksi(Rk, Rk));
        for (long l = 0; l < k_tried; ++l) {
          row_perm_[wk_.i[l]] = l;
          col_perm_[wk_.j[l]] = l;
        }
        return;
      }

      range RN(n_);
      //wk_.MC(Rk,RN) = wk_.C(Rk,RN) * M_(RN,RN);// OPTIMIZE BELOW
      blas::gemm(1.0, wk_.C(Rk, RN), M_(RN, RN), 0.0, wk_.MC(Rk, RN));
      wk_.MC(Rk, range(n_, n_ + k_tried)) = -1; // -identity matrix
      wk_.MB(range(n_, n_ + k_tried), Rk) = -1; // -identity matrix !

      // keep the real position of the row/col
      // since we insert a col/row, we have first to push the col at the right
      // and then say that col wk_.i[0] is stored in n_, the last col.
      // same for rows
      for (int l = 0; l < k_tried; ++l) {
        n_++;
        for (long i = n_ - 2; i >= wk_.i[l]; i--) row_perm_[i + 1] = row_perm_[i];
        row_perm_[wk_.i[l]] = n_ - 1;
        for (long i = n_ - 2; i >= wk_.j[l]; i--) col_perm_[i + 1] = col_perm_[i];
        col_perm_[wk_.j[l]] = n_ - 1;
      }
      RN = range(n_);

      wk_.ksi(Rk, Rk)                 = nda::linalg::inv(wk_.ksi(Rk, Rk));
      M_(RN, range(n_ - k_tried, n_)) = 0;
      M_(range(n_ - k_tried, n_), RN) = 0;
      //M_(RN,RN) += wk_.MB(RN,Rk) * (wk_.ksi(Rk, Rk) * wk_.MC(Rk,RN)); // OPTIMIZE BELOW
      blas::gemm(1.0, wk_.MB(RN, Rk), (wk_.ksi(Rk, Rk) * wk_.MC(Rk, RN)), 1.0, M_(RN, RN));
    }
    // Complete the insert2 operation.
    void complete_insert2() { complete_insert_k(); }

    public:
    //------------------------------------------------------------------------------------------

    /**
     * @brief Try to remove one row and column.
     *
     * @details The row to be removed is at position \f$ i \f$ and the column at position \f$ j \f$ in the original
     * matrix \f$ F^{(n)} \f$.
     *
     * This is a special case of try_remove_k() with \f$ k = 1 \f$.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be removed in the original matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ \det(F^{(n-1)}) / \det(F^{(n)}) \f$.
     */
    value_type try_remove(long i, long j) {
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(0 <= i and i < n_);
      TRIQS_ASSERT(0 <= j and j < n_);
      w1_.i     = i;
      w1_.j     = j;
      last_try_ = try_tag::Remove;
      w1_.jreal = col_perm_[w1_.j];
      w1_.ireal = row_perm_[w1_.i];
      // compute the newdet_
      // first we resolve the w1_.ireal,w1_.jreal, with the permutation of the Minv, then we pick up what
      // will become the 'corner' coefficient, if the move is accepted, after the exchange of row and col.
      w1_.ksi  = M_(w1_.jreal, w1_.ireal);
      auto ksi = w1_.ksi;
      newdet_  = det_ * ksi;
      newsign_ = ((i + j) % 2 == 0 ? sign_ : -sign_);
      return ksi * (newsign_ * sign_); // sign_ is unity, hence 1/sign_ == sign_
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the remove operation.
    void complete_remove() {
      if (n_ == 1) {
        clear();
        return;
      }

      // Move rows and cols to be removed to the end.
      // Adjust the x_ and y_ vector accordingly and
      // swap the associated row_perm_ and col_perm_ elements
      // Remember that for M row/col is interchanged by inversion, transposition.
      range RN(n_);
      if (w1_.ireal != n_ - 1) {
        deep_swap(M_(RN, w1_.ireal), M_(RN, n_ - 1));
        x_[w1_.ireal] = x_[n_ - 1];
        auto iitr     = std::ranges::find(row_perm_, w1_.ireal);
        auto titr     = std::ranges::find(row_perm_, n_ - 1);
        std::swap(*iitr, *titr);
      }
      if (w1_.jreal != n_ - 1) {
        deep_swap(M_(w1_.jreal, RN), M_(n_ - 1, RN));
        y_[w1_.jreal] = y_[n_ - 1];
        auto jitr     = std::ranges::find(col_perm_, w1_.jreal);
        auto titr     = std::ranges::find(col_perm_, n_ - 1);
        std::swap(*jitr, *titr);
      }
      n_--;
      RN = range(n_);

      auto it1 [[maybe_unused]] = std::ranges::remove(row_perm_, n_);
      auto it2 [[maybe_unused]] = std::ranges::remove(col_perm_, n_);

      row_perm_.pop_back();
      col_perm_.pop_back();
      x_.pop_back();
      y_.pop_back();

      // M <- a - d^-1 b c with BLAS
      w1_.ksi = -1 / M_(n_, n_);
      ASSERT(std::isfinite(std::abs(w1_.ksi)));

      //M_(RN,RN) += w1_.ksi, * M_(RN,n_) * M_(n_,RN);
      blas::ger(w1_.ksi, M_(RN, n_), M_(n_, RN), M_(RN, RN));
    }

    public:
    //------------------------------------------------------------------------------------------

    /**
     * @brief Try to remove \f$ k \f$ rows and columns.
     *
     * @details The rows to be removed are specified in the tuple \f$ \mathbf{i} \f$ and the columns in the tuple
     * \f$ \mathbf{j} \f$. The positions are given w.r.t. the original matrix \f$ F^{(n)} \f$. The corresponding
     * positions in the matrix \f$ G^{(n)} \f$ are denoted by \f$ \mathbf{i}_p \f$ and \f$ \mathbf{j}_p \f$,
     * respectively.
     *
     * Since we are working with \f$ G^{(n)} \f$, we are free to first move the rows and columns to the bottom and to
     * the right of the matrix and use the update formulas presented in @ref triqs-detmanip.
     *
     * More specifically, we introduce the matrix
     * \f[
     *   \widetilde{G}^{(n)} = P_1 G^{(n)} P_2 =  \begin{bmatrix} \widetilde{G}^{(n-k)} & B \\ C & D \end{bmatrix} \; ,
     * \f]
     * where \f$ P_1 \f$ and \f$ P_2 \f$ are permutation matrices that swap the rows and columns to be removed
     * (contained in the matrices \f$ B \f$, \f$ C \f$ and \f$ D \f$) with the bottom rows and the right most columns of
     * the matrix. \f$ \widetilde{G}^{(n-k)} \f$ is the resulting matrix after the remove operation.
     *
     * We use the following order for the rows and columns to be removed:
     * - The first row (column) in \f$ C \f$ (\f$ B \f$) corresponds to the row (column) with the smallest index in the
     * matrix \f$ F^{(n)} \f$.
     * - The second row (column) in \f$ C \f$ (\f$ B \f$) corresponds to the row (column) with the second smallest index
     * in the matrix \f$ F^{(n)} \f$.
     * - And so on.
     *
     * The original matrix can be written as
     * \f[
     *   \begin{split}
     *   F^{(n)} &= P^{(n)}_r G^{(n)} P^{(n)}_c = P^{(n)}_r P_1^{-1} [P_1 G^{(n)} P_2] P_2^{-1} P^{(n)}_c =
     *   \widetilde{P}^{(n)}_r \widetilde{G}^{(n)} \widetilde{P}^{(n)}_c \\
     *   &= P_3 \begin{bmatrix} P^{(n-k)}_r & 0 \\ 0 & I \end{bmatrix} \begin{bmatrix} \widetilde{G}^{(n-k)} & B \\ C &
     *   D \end{bmatrix} \begin{bmatrix} P^{(n-k)}_c & 0 \\ 0 & I \end{bmatrix} P_4 \; ,
     *   \end{split}
     * \f]
     * where \f$ P_3 \f$ and \f$ P_4 \f$ are permutation matrices that move the rows and columns in \f$ B \f$, \f$ C \f$
     * and \f$ D \f$ back to their original positions in the matrix \f$ F^{(n)} \f$.
     *
     * We can therefore write the determinant of the resulting matrix \f$ \widetilde{G}^{(n-k)} \f$ in terms of the
     * determinant of the current matrix \f$ G^{(n)} \f$
     * \f[
     *   \det(\widetilde{G}^{(n-k)}) = \det(\widetilde{G}^{(n)}) \det(S) = \det(P_1) \det(G^{(n)}) \det(P_2) \det(S)
     *   \; ,
     * \f]
     * and the new sign \f$ \widetilde{s}^{(n-k)} \f$ in terms of the current sign \f$ s^{(n)} \f$:
     * \f[
     *   \begin{split}
     *   \widetilde{s}^{(n-k)} &= \det(\widetilde{P}^{(n-k)}_r) \det(\widetilde{P}^{(n-k)}_c) =
     *   \det(P_3) \det(\widetilde{P}^{(n)}_r) \det(\widetilde{P}^{(n)}_c) \det(P_4) \\
     *   &= \det(P_3) \det(P^{(n)}_r) \det(P_1) \det(P_2) \det(P^{(n)}_c) \det(P_4) =
     *   s^{(n)} \det(P_1) \det(P_2) \det(P_3) \det(P_4) \; .
     *   \end{split}
     * \f]
     * Here, we used the fact that \f$ \det(P) = \det(P^{-1}) \f$ for a permutation matrix \f$ P \f$.
     *
     * The function returns the ratio
     * \f[
     *   R = \frac{\det(F^{(n-k)})}{\det(F^{(n)})} = \frac{\det(\widetilde{G}^{(n-k)}) \widetilde{s}^{(n-k)}}{
     *   \det(G^{(n)}) s^{(n)}} = \det(S) \det(P_3) \det(P_4) \; .
     * \f]
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Positions of the rows to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j Positions of the columns to be removed in the original matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ \det(F^{(n-k)}) / \det(F^{(n)}) \f$.
     */
    value_type try_remove_k(std::vector<long> i, std::vector<long> j) {

      std::ranges::sort(i);
      std::ranges::sort(j);

      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(n_ >= 2);
      TRIQS_ASSERT(i.size() == j.size());

      k_tried = static_cast<long>(i.size());
      reserve(n_ - k_tried, k_tried);
      last_try_ = try_tag::RemoveK;

      // check inputs
      for (int l = 0; l < k_tried - 1; ++l) {
        TRIQS_ASSERT(i[l] != i[l + 1] and 0 <= i[l] and i[l] < n_);
        TRIQS_ASSERT(j[l] != j[l + 1] and 0 <= j[l] and j[l] < n_);
      }

      for (long l = 0; l < k_tried; ++l) {
        wk_.i[l]     = i[l];
        wk_.j[l]     = j[l];
        wk_.ireal[l] = row_perm_[wk_.i[l]];
        wk_.jreal[l] = col_perm_[wk_.j[l]];
      }

      // compute the newdet_
      for (long l1 = 0; l1 < k_tried; ++l1) {
        for (long l2 = 0; l2 < k_tried; ++l2) { wk_.ksi(l1, l2) = M_(wk_.jreal[l1], wk_.ireal[l2]); }
      }
      auto det_ksi = wk_.det_ksi(k_tried);
      newdet_      = det_ * det_ksi;
      long idx_sum = 0;
      for (long l = 0; l < k_tried; ++l) { idx_sum += wk_.i[l] + wk_.j[l]; }
      newsign_ = (idx_sum % 2 == 0 ? sign_ : -sign_);

      return det_ksi * (newsign_ * sign_); // sign_ is unity, hence 1/sign_ == sign_
    }
    /**
     * @brief Try to remove two rows and two columns.
     *
     * @details The rows to be removed are specified by the indices \f$ i_0 \f$ and \f$ i_1 \f$, and the columns by the
     * indices \f$ j_0 \f$ and \f$ j_1 \f$. The positions are given w.r.t. the original matrix \f$ F^{(n)} \f$.
     *
     * It simply calls the more general try_remove_k().
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i0 Position of the first row to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param i1 Position of the second row to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j0 Position of the first column to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j1 Position of the second column to be removed in the original matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ \det(F^{(n-2)}) / \det(F^{(n)}) \f$.
     */
    value_type try_remove2(long i0, long i1, long j0, long j1) { return try_remove_k({i0, i1}, {j0, j1}); }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the remove_k operation.
    void complete_remove_k() {
      if (n_ == k_tried) {
        clear();
        return;
      } // put the sign_ to 1 also .... Change complete_remove...

      std::vector<long> ireal = wk_.ireal;
      std::vector<long> jreal = wk_.jreal;
      std::sort(ireal.begin(), ireal.begin() + k_tried);
      std::sort(jreal.begin(), jreal.begin() + k_tried);

      // Move rows and cols to be removed to the end, starting from the right.
      // Adjust the x_ and y_ vector accordingly and
      // swap the associated row_perm_ and col_perm_ elements
      // Remember that for M row/col is interchanged by inversion, transposition.
      range RN(n_);
      for (long m = k_tried - 1, target = n_ - 1; m >= 0; --m, --target) {
        if (ireal[m] != target) {
          deep_swap(M_(RN, ireal[m]), M_(RN, target));
          x_[ireal[m]] = x_[target];
          auto iitr    = std::ranges::find(row_perm_, ireal[m]);
          auto titr    = std::ranges::find(row_perm_, target);
          std::swap(*iitr, *titr);
        }
        if (jreal[m] != target) {
          deep_swap(M_(jreal[m], RN), M_(target, RN));
          y_[jreal[m]] = y_[target];
          auto jitr    = std::ranges::find(col_perm_, jreal[m]);
          auto titr    = std::ranges::find(col_perm_, target);
          std::swap(*jitr, *titr);
        }
      }
      n_ -= k_tried;
      RN = range(n_);

      // Clean up removed elements from row_perm_ and col_perm_
      auto gtN = [&](auto i) { return i >= n_; };

      auto it1 [[maybe_unused]] = std::remove_if(row_perm_.begin(), row_perm_.end(), gtN);
      auto it2 [[maybe_unused]] = std::remove_if(col_perm_.begin(), col_perm_.end(), gtN);

      row_perm_.resize(n_);
      col_perm_.resize(n_);
      x_.resize(n_);
      y_.resize(n_);

      // M <- a - d^-1 b c with BLAS
      range Rl(n_, n_ + k_tried), Rk(k_tried);
      wk_.ksi(Rk, Rk) = nda::linalg::inv(M_(Rl, Rl));

      // write explicitely the second product on ksi for speed ?
      //M_(RN,RN) -= M_(RN,Rl) * (wk_.ksi * M_(Rl,RN)); // OPTIMIZE BELOW
      blas::gemm(-1.0, M_(RN, Rl), wk_.ksi(Rk, Rk) * M_(Rl, RN), 1.0, M_(RN, RN));
    }
    // Complete the remove2 operation.
    void complete_remove2() { complete_remove_k(); }

    //------------------------------------------------------------------------------------------
    public:
    /**
     * @brief Try to change one column in the original matrix \f$ F^{(n)} \f$.
     *
     * @details The column to be changed is at position \f$ j \f$ in the original matrix \f$ F^{(n)} \f$. The new
     * elements of the columns are determined by the given matrix builder argument \f$ y \f$.
     *
     * Let \f$ j_p \f$ be the position of the column in the matrix \f$ G^{(n)} \f$. We can write the new matrix as
     * \f[
     *   \widetilde{G}^{(n)} = G^{(n)} + \mathbf{u} \mathbf{v}^T \; ,
     * \f]
     * where \f$ u_i = f(x_i, y) - f(x_i, y_{j_p}) \f$ and \f$ \mathbf{v} = \mathbf{e}_{j_p} \f$ is a cartesian basis
     * vector.
     *
     * The new inverse matrix \f$ \widetilde{M}^{(n)} \f$ is then given by the Sherman-Morrison formula
     * \f[
     *   \widetilde{M}^{(n)} = M^{(n)} - \frac{M^{(n)} \mathbf{u} \mathbf{v}^T M^{(n)}}{1 + \mathbf{v}^T M^{(n)}
     *   \mathbf{u}} \; ,
     * \f]
     * and the new determinant by the matrix determinant lemma
     * \f[
     *   \det(\widetilde{G}^{(n)}) = \det(G^{(n)}) \left( 1 + \mathbf{v}^T M^{(n)} \mathbf{u} \right) = \det(G^{(n)})
     *   \xi \; .
     * \f]
     *
     * The function returns the ratio
     * \f[
     *   R = \frac{\det(\widetilde{F}^{(n)})}{\det(F^{(n)})} =  \left( 1 + \mathbf{v}^T M^{(n)} \mathbf{u} \right) \; .
     * \f]
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param j Position of the column to be changed in the original matrix \f$ F^{(n)} \f$.
     * @param y Argument to the matrix builder that determines the new elements of the column.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
     */
    value_type try_change_col(long j, y_type const &y) {
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(0 <= j and j < n_);
      w1_.j     = j;
      last_try_ = try_tag::ChangeCol;
      w1_.jreal = col_perm_[j];
      w1_.y     = y;

      // Compute the col B.
      for (long i = 0; i < n_; i++) w1_.MC(i) = f_(x_[i], w1_.y) - f_(x_[i], y_[w1_.jreal]);
      range RN(n_);
      //w1_.MB(R) = M_(R,R) * w1_.MC(R);// OPTIMIZE BELOW
      blas::gemv(1.0, M_(RN, RN), w1_.MC(RN), 0.0, w1_.MB(RN));

      // compute the newdet_
      w1_.ksi  = (1 + w1_.MB(w1_.jreal));
      auto ksi = w1_.ksi;
      newdet_  = det_ * ksi;
      newsign_ = sign_;

      return ksi; // newsign_/sign_ is unity
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the change column operation.
    void complete_change_col() {
      range RN(n_);
      y_[w1_.jreal] = w1_.y;

      // modifying M : Mij += w1_.ksi Bi Mnj
      // using Shermann Morrison formula.
      // implemented in 2 times : first Bn=0 so that Mnj is not modified ! and then change Mnj
      // Cf notes : simply multiply by -w1_.ksi
      w1_.ksi           = -1 / w1_.ksi;
      w1_.MB(w1_.jreal) = 0;
      //M_(R,R) += w1_.ksi * w1_.MB(R) * M_(w1_.jreal,R)); // OPTIMIZE BELOW
      blas::ger(w1_.ksi, w1_.MB(RN), M_(w1_.jreal, RN), M_(RN, RN));
      M_(w1_.jreal, RN) *= -w1_.ksi;
    }

    //------------------------------------------------------------------------------------------
    public:
    /**
     * @brief Try to change one row in the original matrix \f$ F^{(n)} \f$.
     *
     * @details The row to be changed is at position \f$ i \f$ in the original matrix \f$ F^{(n)} \f$. The new
     * elements of the row are determined by the given matrix builder argument \f$ x \f$.
     *
     * We follow the same procedure as in try_change_col(), except that we use \f$ v_i = f(x, y_j) - f(x_{i_p}, y_j) \f$
     * and \f$ \mathbf{u} = \mathbf{e}_{i_p} \f$ is a cartesian basis vector.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be changed in the original matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the new elements of the row.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
     */
    value_type try_change_row(long i, x_type const &x) {
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(i < n_);
      w1_.i     = i;
      last_try_ = try_tag::ChangeRow;
      w1_.ireal = row_perm_[i];
      w1_.x     = x;

      // Compute the col B.
      for (long idx = 0; idx < n_; idx++) w1_.MB(idx) = f_(w1_.x, y_[idx]) - f_(x_[w1_.ireal], y_[idx]);
      range RN(n_);
      //w1_.MC(R) = transpose(M_(R,R)) * w1_.MB(R); // OPTIMIZE BELOW
      blas::gemv(1.0, transpose(M_(RN, RN)), w1_.MB(RN), 0.0, w1_.MC(RN));

      // compute the newdet_
      w1_.ksi  = (1 + w1_.MC(w1_.ireal));
      auto ksi = w1_.ksi;
      newdet_  = det_ * ksi;
      newsign_ = sign_;
      return ksi; // newsign_/sign_ is unity
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the change row operation.
    void complete_change_row() {
      range RN(n_);
      x_[w1_.ireal] = w1_.x;

      // modifying M : M ij += w1_.ksi Min Cj
      // using Shermann Morrison formula.
      // impl. Cf case 3
      w1_.ksi           = -1 / w1_.ksi;
      w1_.MC(w1_.ireal) = 0;
      //M_(R,R) += w1_.ksi * M_(R,w1_.ireal) * w1_.MC(R);
      blas::ger(w1_.ksi, M_(RN, w1_.ireal), w1_.MC(RN), M_(RN, RN));
      M_(RN, w1_.ireal) *= -w1_.ksi;
    }

    //------------------------------------------------------------------------------------------
    public:
    /**
     * @brief Try to change one column and one row in the original matrix \f$ F^{(n)} \f$.
     *
     * @details The row and column to be changed are at positions \f$ i \f$ and \f$ j \f$ in the original matrix
     * \f$ F^{(n)} \f$, respectively. The new elements of the row and column are determined by the given matrix builder
     * arguments \f$ x \f$ and \f$ y \f$.
     *
     * Let \f$ i_p \f$ and \f$ j_p \f$ be the positions of the row and the column in the matrix \f$ G^{(n)} \f$. We can
     * write the new matrix as
     * \f[
     *   \widetilde{G}^{(n)} = G^{(n)} + \mathbf{r} \mathbf{s}^T + \mathbf{u} \mathbf{v}^T \; ,
     * \f]
     * where
     * - \f$ \mathbf{r} = \mathbf{e}_{i_p} \f$,
     * - \f$ s_k = f(x, y_k) - f(x_{i_p}, y_k) \f$ except for \f$ s_{j_p} = f(x, y) - f(x_{i_p}, y_{j_p}) \f$,
     * - \f$ u_l = f(x_l, y) - f(x_l, y_{j_p}) \f$ except for \f$ u_{i_p} = 0 \f$, and
     * - \f$ \mathbf{v} = \mathbf{e}_{j_p} \f$.
     *
     * By using the matrix determinant lemma twice and once the Sherman-Morrison formula, we find for the new
     * determinant
     * \f[
     *   \det(\widetilde{G}^{(n)}) = \det(G^{(n)}) \left[ (1 + \mathbf{s}^T M^{(n)} \mathbf{r}) (1 + \mathbf{v}^T
     *   M^{(n)} \mathbf{u}) - M^{(n)}_{j_p i_p} \mathbf{s}^T M^{(n)} \mathbf{u} \right] =
     *   \det(G^{(n)}) \left[ (1 + \alpha)(1 + \beta) - M^{(n)}_{j_p i_p} \gamma \right] =
     *   \det(G^{(n)}) \xi \; .
     * \f]
     *
     * The new inverse matrix \f$ \widetilde{M}^{(n)} \f$ can be obtained by applying the Sherman-Morrison formula
     * twice:
     * \f[
     *   \widetilde{M}^{(n)} = H^{-1} - \frac{H^{-1} \mathbf{u} \mathbf{v}^T H^{-1}}{1 + \mathbf{v}^T H^{-1}
     *   \mathbf{u}} \; ,
     * \f]
     * where
     * \f[
     *   H^{-1} = M^{(n)} - \frac{M^{(n)} \mathbf{r} \mathbf{s}^T M^{(n)}}{1 + \mathbf{s}^T M^{(n)} \mathbf{r}} \; .
     * \f]
     * After some algebra, we find for its elements
     * \f[
     *   \widetilde{M}^{(n)}_{ab} = M^{(n)}_{ab} - \frac{(1 + \alpha) (M^{(n)} \mathbf{u})_a M^{(n)}_{j_p b}}{\xi} +
     *   \frac{M^{(n)}_{j_p i_p} (M^{(n)} \mathbf{u})_a (\mathbf{s}^T M^{(n)})_b}{\xi} +
     *   \frac{\gamma M^{(n)}_{a i_p} M^{(n)}_{j_p b}}{\xi} -
     *   \frac{(1 + \beta) (\mathbf{s}^T M^{(n)})_b M^{(n)}_{a i_p}}{\xi}
     *   \; .
     * \f]
     *
     * The function returns the ratio
     * \f[
     *   R = \frac{\det(\widetilde{F}^{(n)})}{\det(F^{(n)})} = \xi \; .
     * \f]
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be changed in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be changed in the original matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the new elements of the row.
     * @param y Argument to the matrix builder that determines the new elements of the column.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
     */
    value_type try_change_col_row(long i, long j, x_type const &x, y_type const &y) {
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(0 <= i and i < n_);
      TRIQS_ASSERT(0 <= j and j < n_);

      last_try_ = try_tag::ChangeRowCol;
      w1_.i     = i;
      w1_.j     = j;
      w1_.ireal = row_perm_[i];
      w1_.jreal = col_perm_[j];
      w1_.x     = x;
      w1_.y     = y;

      // Compute the col B.
      for (long idx = 0; idx < n_; idx++) { // MC :  delta_x, MB : delta_y
        w1_.MC(idx) = f_(x_[idx], y) - f_(x_[idx], y_[w1_.jreal]);
        w1_.MB(idx) = f_(x, y_[idx]) - f_(x_[w1_.ireal], y_[idx]);
      }
      w1_.MC(w1_.ireal) = f_(x, y) - f_(x_[w1_.ireal], y_[w1_.jreal]);
      w1_.MB(w1_.jreal) = 0;

      range RN(n_);
      // C : X, B : Y
      //w1_.C(R) = M_(R,R) * w1_.MC(R);// OPTIMIZE BELOW
      blas::gemv(1.0, M_(RN, RN), w1_.MC(RN), 0.0, w1_.C(RN));
      //w1_.B(R) = transpose(M_(R,R)) * w1_.MB(R); // OPTIMIZE BELOW
      blas::gemv(1.0, transpose(M_(RN, RN)), w1_.MB(RN), 0.0, w1_.B(RN));

      // compute the det_ratio
      auto Xn        = w1_.C(w1_.jreal);
      auto Yn        = w1_.B(w1_.ireal);
      auto Z         = nda::blas::dot(w1_.MB(RN), w1_.C(RN));
      auto Mnn       = M_(w1_.jreal, w1_.ireal);
      auto det_ratio = (1 + Xn) * (1 + Yn) - Mnn * Z;
      w1_.ksi        = det_ratio;
      newdet_        = det_ * det_ratio;
      newsign_       = sign_;
      return det_ratio; // newsign_/sign_ is unity
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the change row and column operation.
    void complete_change_col_row() {
      range RN(n_);
      x_[w1_.ireal] = w1_.x;
      y_[w1_.jreal] = w1_.y;

      // FIXME : Use blas for this ? Is it better
      auto Xn  = w1_.C(w1_.jreal);
      auto Yn  = w1_.B(w1_.ireal);
      auto Mnn = M_(w1_.jreal, w1_.ireal);

      auto D     = w1_.ksi;       // get back
      auto a     = -(1 + Yn) / D; // D in the notes
      auto b     = -(1 + Xn) / D;
      auto Z     = nda::blas::dot(w1_.MB(RN), w1_.C(RN));
      Z          = Z / D;
      Mnn        = Mnn / D;
      w1_.MB(RN) = M_(w1_.jreal, RN); // Mnj
      w1_.MC(RN) = M_(RN, w1_.ireal); // Min

      for (long i = 0; i < n_; ++i)
        for (long j = 0; j < n_; ++j) {
          auto Xi  = w1_.C(i);
          auto Yj  = w1_.B(j);
          auto Mnj = w1_.MB(j);
          auto Min = w1_.MC(i);
          M_(i, j) += a * Xi * Mnj + b * Min * Yj + Mnn * Xi * Yj + Z * Min * Mnj;
        }
    }

    //------------------------------------------------------------------------------------------
    public:
    /**
     * @brief Try to fill the original matrix \f$ F^{(n)} \f$ with new elements.
     *
     * @details This function tries to build a completely new matrix \f$ \widetilde{F}^{(\widetilde{n})} \f$ using the
     * given matrix builder arguments \f$ \mathbf{x} \f$ and \f$ \mathbf{y} \f$. The new elements are calculated as
     * \f$ \widetilde{F}_{ij} = f(x_i, y_j) \f$.
     *
     * The function returns the ratio
     * \f[
     *   R = \frac{\det(\widetilde{F}^{(\widetilde{n})})}{\det(F^{(n)})} \; .
     * \f]
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @tparam ArgumentContainer1 Container type holding the first arguments.
     * @tparam ArgumentContainer2 Container type holding the second arguments.
     * @param X Container holding the first matrix builder arguments.
     * @param Y Container holding the second matrix builder arguments.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(\widetilde{n})}) / \det(F^{(n)}) \f$.
     */
    template <typename ArgumentContainer1, typename ArgumentContainer2>
    value_type try_refill(ArgumentContainer1 const &X, ArgumentContainer2 const &Y) {
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(X.size() == Y.size());

      last_try_ = try_tag::Refill;

      long s = X.size();
      // treat empty matrix separately
      if (s == 0) {
        wref_.x_values.clear();
        wref_.y_values.clear();
        return 1 / (sign_ * det_);
      }

      wref_.reserve(s);
      wref_.x_values.clear();
      wref_.y_values.clear();
      std::copy(X.begin(), X.end(), std::back_inserter(wref_.x_values));
      std::copy(Y.begin(), Y.end(), std::back_inserter(wref_.y_values));

      for (long i = 0; i < s; ++i)
        for (long j = 0; j < s; ++j) wref_.M(i, j) = f_(wref_.x_values[i], wref_.y_values[j]);
      range R(s);
      newdet_  = nda::linalg::det(wref_.M(R, R));
      newsign_ = 1;

      return newdet_ / (sign_ * det_);
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the refill operation.
    void complete_refill() {
      n_ = wref_.x_values.size();

      // special empty case again
      if (n_ == 0) {
        clear();
        newdet_  = 1;
        newsign_ = 1;
        return;
      }

      reserve(n_);
      std::swap(x_, wref_.x_values);
      std::swap(y_, wref_.y_values);

      row_perm_.resize(n_, 0); // Zero Initialization avoids ASAN false positive
      col_perm_.resize(n_, 0);
      std::iota(row_perm_.begin(), row_perm_.end(), 0);
      std::iota(col_perm_.begin(), col_perm_.end(), 0);

      range RN(n_);
      M_(RN, RN) = nda::linalg::inv(wref_.M(RN, RN));
    }

    public:
    /**
     * @brief Complete the last try-operation.
     *
     * @details It completes the last try-operation by calling the correct completion function depending on the try tag
     * set in the last try function call.
     *
     * If the number of operations exceeds a certain threshold (see set_n_operations_before_check()), the inverse
     * matrix \f$ M^{(n)} \f$, the determinant \f$ \det(G^{(n)}) \f$ and the sign \f$ s^{(n)} \f$ are regenerated using
     * the matrix builder and checked against the stored values.
     *
     * A possible warning is emitted or an exception is thrown if the current objects are not consistent with the
     * regenerated ones (see set_precision_warning() and set_precision_error()).
     */
    void complete_operation() {
      switch (last_try_) {
        case (try_tag::Insert): complete_insert(); break;
        case (try_tag::Remove): complete_remove(); break;
        case (try_tag::ChangeCol): complete_change_col(); break;
        case (try_tag::ChangeRow): complete_change_row(); break;
        case (try_tag::ChangeRowCol): complete_change_col_row(); break;
        case (try_tag::InsertK): complete_insert_k(); break;
        case (try_tag::RemoveK): complete_remove_k(); break;
        case (try_tag::Refill): complete_refill(); break;
        case (try_tag::NoTry): return; break;
        default: TRIQS_RUNTIME_ERROR << "Misuing det_manip"; // Never used?
      }

      det_  = newdet_;
      sign_ = newsign_;
      ++nops_;
      if (nops_ > nops_before_check_) check_mat_inv();
      last_try_ = try_tag::NoTry;
    }

    /**
     * @brief Reject the last try-operation.
     * @details All `try_*` operations have to be either accepted (via complete_operation()) or rejected.
     */
    void reject_last_try() { last_try_ = try_tag::NoTry; }

    // ----------------- A few short cuts   -----------------

    public:
    /**
     * @brief Insert one row and column.
     * @details Wrapper for try_insert() followed by a complete_operation() call. See try_insert() for details.
     * @param i Position of the row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the elements of the new row.
     * @param y Argument to the matrix builder that determines the elements of the new column.
     * @return Determinant ratio \f$ \det(F^{(n+1)}) / \det(F^{(n)}) \f$.
     */
    value_type insert(long i, long j, x_type const &x, y_type const &y) {
      auto r = try_insert(i, j, x, y);
      complete_operation();
      return r;
    }

    /**
     * @brief Insert one row and column at the end of the matrix.
     * @details Same as insert() but with `i` and `j` set to size().
     * @param x Argument to the matrix builder that determines the elements of the new row.
     * @param y Argument to the matrix builder that determines the elements of the new column.
     * @return Determinant ratio \f$ \det(F^{(n+1)}) / \det(F^{(n)}) \f$.
     */
    value_type insert_at_end(x_type const &x, y_type const &y) { return insert(n_, n_, x, y); }

    /**
     * @brief Insert two rows and columns.
     * @details Wrapper for try_insert2() followed by a complete_operation() call. See try_insert2() for details.
     * @param i0,i1 Positions of the rows to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j0,j1 Positions of the columns to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param x0,x1 Arguments to the matrix builder that determine the elements of the new rows.
     * @param y0,y1 Arguments to the matrix builder that determine the elements of the new columns.
     * @return Determinant ratio \f$ \det(F^{(n+2)}) / \det(F^{(n)}) \f$.
     */
    value_type insert2(long i0, long i1, long j0, long j1, x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
      auto r = try_insert2(i0, i1, j0, j1, x0, x1, y0, y1);
      complete_operation();
      return r;
    }

    /**
     * @brief Insert two rows and columns at the end of the matrix.
     * @details Same as insert2() but with `i0` and `j0` set to size() and `i1` and `j1` set to size() + 1.
     * @param x0,x1 Arguments to the matrix builder that determine the elements of the new rows.
     * @param y0,y1 Arguments to the matrix builder that determine the elements of the new columns.
     * @return Determinant ratio \f$ \det(F^{(n+2)}) / \det(F^{(n)}) \f$.
     */
    value_type insert2_at_end(x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
      return insert2(n_, n_ + 1, n_, n_ + 1, x0, x1, y0, y1);
    }

    /**
     * @brief Remove one row and column.
     * @details Wrapper for try_remove() followed by a complete_operation() call. See try_remove() for details.
     * @param i Position of the row to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be removed in the original matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ \det(F^{(n-1)}) / \det(F^{(n)}) \f$.
     */
    value_type remove(long i, long j) {
      auto r = try_remove(i, j);
      complete_operation();
      return r;
    }

    /**
     * @brief Remove the last row and column of the matrix.
     * @details Same as remove() but with `i` and `j` set to size() - 1.
     * @return Determinant ratio \f$ \det(F^{(n-1)}) / \det(F^{(n)}) \f$.
     */
    value_type remove_at_end() { return remove(n_ - 1, n_ - 1); }

    /**
     * @brief Remove two rows and columns.
     * @details Wrapper for try_remove2() followed by a complete_operation() call. See try_remove2() for details.
     * @param i0,i1 Positions of the rows to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j0,j1 Positions of the columns to be removed in the original matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ \det(F^{(n-2)}) / \det(F^{(n)}) \f$.
     */
    value_type remove2(long i0, long i1, long j0, long j1) {
      auto r = try_remove2(i0, i1, j0, j1);
      complete_operation();
      return r;
    }

    /**
     * @brief Remove the last two rows and columns of the matrix.
     * @details Same as remove2() but with `i0` and `j0` set to size() - 1 and `i1` and `j1` set to size() - 2.
     * @return Determinant ratio \f$ \det(F^{(n-2)}) / \det(F^{(n)}) \f$.
     */
    value_type remove2_at_end() { return remove2(n_ - 1, n_ - 2, n_ - 1, n_ - 2); }

    /**
     * @brief Change one column.
     * @details Wrapper for try_change_col() followed by a complete_operation() call. See try_change_col() for details.
     * @param j Position of the column to be changed in the original matrix \f$ F^{(n)} \f$.
     * @param y Argument to the matrix builder that determines the new elements of the column.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
     */
    value_type change_col(long j, y_type const &y) {
      auto r = try_change_col(j, y);
      complete_operation();
      return r;
    }

    /**
     * @brief Change one row.
     * @details Wrapper for try_change_row() followed by a complete_operation() call. See try_change_row() for details.
     * @param i Position of the row to be changed in the original matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the new elements of the row.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
     */
    value_type change_row(long i, x_type const &x) {
      auto r = try_change_row(i, x);
      complete_operation();
      return r;
    }

    /**
     * @brief Change one row and one column.
     * @details Wrapper for try_change_col_row() followed by a complete_operation() call. See try_change_col_row() for
     * details.
     * @param i Position of the row to be changed in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be changed in the original matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the new elements of the row.
     * @param y Argument to the matrix builder that determines the new elements of the column.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
     */
    value_type change_one_row_and_one_col(long i, long j, x_type const &x, y_type const &y) {
      auto r = try_change_col_row(i, j, x, y);
      complete_operation();
      return r;
    }

    /**
     * @brief Regenerate the inverse matrix \f$ M^{(n)} \f$, the determinant \f$ \det(G^{(n)}) \f$ and the sign
     * \f$ s^{(n)} \f$ from scratch using the matrix builder.
     *
     * @details It uses the matrix builder to rebuild the matrix \f$ G^{(n)} \f$, then computes its inverse
     * \f$ M^{(n)} \f$ and its determinant \f$ \det(G^{(n)}) \f$, and recomputes the sign \f$ s^{(n)} \f$ associated
     * with the permutation matrices. This is used to counteract the accumulation of numerical errors after many
     * `try`/`complete` operations.
     *
     * The consistency check against the stored values (see set_precision_warning(), set_precision_error() and
     * set_singular_threshold()) is performed automatically in complete_operation() after a configurable number of
     * operations (see set_n_operations_before_check()); this function itself does not perform that check.
     */
    void regenerate() { _regenerate_with_check(false, 0, 0); }

    /**
     * @brief Write a triqs::det_manip::det_manip object to HDF5.
     *
     * @param fg `h5::group` containing the subgroup to be written to.
     * @param subgroup_name Name of the subgroup.
     * @param g Manipulator object to be written.
     */
    friend void h5_write(h5::group fg, std::string subgroup_name, det_manip const &g) {
      auto gr = fg.create_group(subgroup_name);
      h5_write(gr, "N", g.n_);
      h5_write(gr, "mat_inv", g.M_);
      h5_write(gr, "det", g.det_);
      h5_write(gr, "sign", g.sign_);
      h5_write(gr, "row_num", g.row_perm_);
      h5_write(gr, "col_num", g.col_perm_);
      h5_write(gr, "x_values", g.x_);
      h5_write(gr, "y_values", g.y_);
      h5_write(gr, "n_opts", g.nops_);
      h5_write(gr, "n_opts_max_before_check", g.nops_before_check_);
      h5_write(gr, "singular_threshold", g.singular_threshold_);
    }

    /**
     * @brief Read a triqs::det_manip::det_manip object from HDF5.
     *
     * @param fg `h5::group` containing the subgroup to be read from.
     * @param subgroup_name Name of the subgroup.
     * @param g Manipulator object to be read into.
     */
    friend void h5_read(h5::group fg, std::string subgroup_name, det_manip &g) {
      auto gr = fg.open_group(subgroup_name);
      h5_read(gr, "N", g.n_);
      h5_read(gr, "mat_inv", g.M_);
      g.ncap_     = first_dim(g.M_); // restore ncap_
      g.last_try_ = try_tag::NoTry;
      h5_read(gr, "det", g.det_);
      h5_read(gr, "sign", g.sign_);
      h5_read(gr, "row_num", g.row_perm_);
      h5_read(gr, "col_num", g.col_perm_);
      h5_read(gr, "x_values", g.x_);
      h5_read(gr, "y_values", g.y_);
      h5_read(gr, "n_opts", g.nops_);
      h5_read(gr, "n_opts_max_before_check", g.nops_before_check_);
      h5_read(gr, "singular_threshold", g.singular_threshold_);
    }

    //------------------------------------------------------------------------------------------
    private:
    // Enumerate the different operations supported by the det_manip class that have a try - complete step.
    enum class try_tag { NoTry, Insert, Remove, ChangeCol, ChangeRow, ChangeRowCol, InsertK, RemoveK, Refill };

    // Set the matrix builder arguments to the given ranges and reset the permutation vectors.
    template <typename X, typename Y>
      requires(MatrixBuilderXRange<X, F> && MatrixBuilderYRange<Y, F>)
    void set_xy(X &&x_rg, Y &&y_rg) { // NOLINT (ranges need not be forwarded)
      x_.clear();
      y_.clear();
      row_perm_.clear();
      col_perm_.clear();
      for (long i = 0; auto const &[x, y] : std::views::zip(x_rg, y_rg)) {
        x_.push_back(x);
        y_.push_back(y);
        row_perm_.push_back(i);
        col_perm_.push_back(i);
        ++i;
      }
    }

    // Regenerate the inverse matrix, determinant and sign from the matrix builder, optionally checking the freshly
    // computed values against the stored ones.
    void _regenerate_with_check(bool do_check, double prec_warning, double prec_error) {
      if (n_ == 0) {
        det_  = 1;
        sign_ = 1;
        return;
      }

      range RN(n_);
      matrix_type res(n_, n_);
      for (int i = 0; i < n_; i++)
        for (int j = 0; j < n_; j++) res(i, j) = f_(x_[i], y_[j]);
      det_ = nda::linalg::det(res);

      if (is_singular()) TRIQS_RUNTIME_ERROR << "ERROR in det_manip regenerate: Determinant is singular";
      res = nda::linalg::inv(res);

      if (do_check) { // check that M_ is close to res
        const bool relative = true;
        double r            = max_element(abs(res - M_(RN, RN)));
        double r2           = max_element(abs(res + M_(RN, RN)));
        bool err            = !(r < (relative ? prec_error * r2 : prec_error));
        bool war            = !(r < (relative ? prec_warning * r2 : prec_warning));
        if (err || war) {
          std::cerr << "matrix  = " << matrix() << std::endl;
          std::cerr << "inverse_matrix = " << inverse_matrix() << std::endl;
        }
        if (war)
          std::cerr << "Warning : det_manip deviation above warning threshold "
                    << "check "
                    << "N = " << n_ << "  "
                    << "\n   max(abs(M^-1 - M^-1_true)) = " << r
                    << "\n   precision*max(abs(M^-1 + M^-1_true)) = " << (relative ? prec_warning * r2 : prec_warning) << " " << std::endl;
        if (err) TRIQS_RUNTIME_ERROR << "Error : det_manip deviation above critical threshold !! ";
      }

      // since we have the proper inverse, replace the matrix and the det
      M_(RN, RN) = res;
      nops_      = 0;

      // find the sign (there must be a better way...)
      double s = 1.0;
      nda::matrix<double> m(n_, n_);
      m() = 0.0;
      for (int i = 0; i < n_; i++) m(i, row_perm_[i]) = 1;
      s *= nda::linalg::det(m);
      m() = 0.0;
      for (int i = 0; i < n_; i++) m(i, col_perm_[i]) = 1;
      s *= nda::linalg::det(m);
      sign_ = (s > 0 ? 1 : -1);
    }

    // Regenerate and check the consistency of the stored inverse matrix, determinant and sign.
    void check_mat_inv() { _regenerate_with_check(true, precision_warning_, precision_error_); }

    // Check whether the determinant is considered singular: (singular_threshold_ < 0 ? not
    // std::isnormal(std::abs(det_)) : (std::abs(det_) < singular_threshold_)). See set_singular_threshold().
    [[nodiscard]] bool is_singular() const {
      return (singular_threshold_ < 0 ? not std::isnormal(std::abs(det_)) : (std::abs(det_) < singular_threshold_));
    }

    private:
    // matrix builder: G_{ij} = f_(x_[i], y_[j]) or F_{ij} = f_(x_[row_perm_[i]], y_[col_perm_[j]])
    F f_;
    std::vector<x_type> x_;
    std::vector<y_type> y_;

    // matrix M such that G^{-1} = M_(nda::range(size()), nda::range(size())) and det(G)
    matrix_type M_;
    value_type det_{1};

    // permutation vectors: row (column) i in the original matrix F corresponds to the row (column) row_perm_[i]
    // (col_perm_[i]) in the matrix G
    std::vector<long> row_perm_;
    std::vector<long> col_perm_;
    int sign_{1};

    // working data for the try-complete operations
    detail::work_data_type1<x_type, y_type, value_type> w1_;
    detail::work_data_typek<x_type, y_type, value_type> wk_;
    detail::work_data_type_refill<x_type, y_type, value_type> wref_;
    value_type newdet_{1};
    int newsign_{1};

    // parameters
    std::uint64_t nops_before_check_{100};
    double singular_threshold_{-1};
    double precision_warning_{1.e-8};
    double precision_error_{1.e-5};

    // tag and operation counter
    try_tag last_try_{try_tag::NoTry};
    std::uint64_t nops_{0};

    // sizes of matrices and capacities of their data storages
    long n_{0};
    long ncap_{0};
    long k_tried{0};
    long kmax_tried{1};
  };
} // namespace triqs::det_manip
