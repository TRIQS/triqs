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

#pragma once

#include "./utils.hpp"
#include "./work_data.hpp"
#include <triqs/utility/first_include.hpp>
#include <vector>
#include <iterator>
#include <numeric>
#include <cmath>
#include <triqs/arrays.hpp>
#include <triqs/utility/callable_traits.hpp>
#include <nda/linalg/det.hpp>
#include <nda/linalg/inv.hpp>
#include <fmt/ranges.h>
#include <utility>

namespace triqs::det_manip {

  namespace blas = nda::blas;

  /**
   * @brief Efficient manipulation of determinants and ratios of determinants for CTQMC solvers.
   *
   * @details The weight of a MC configuration in CTQMC solvers is often given in terms of the determinant of one or
   * more matrices such that the acceptance probability is a ratio of these determinants. It is therefore important to
   * be able to efficiently compute these determinants and ratios of determinants.
   *
   * Let \f$ F^{(n)} \f$ be the \f$ n \times n \f$ matrix that we are interested in. We assume that the elements of the
   * matrix can be written as \f$ F^{(n)}_{ij} = f(x_i, y_j) \f$, where \f$ f \f$ is a triqs::det_manip::MatrixBuilder
   * object and \f$ \mathbf{x} \f$ and \f$ \mathbf{y} \f$ are vectors of size \f$ n \f$ that contain the arguments for
   * \f$ f \f$ and therefore determine the contents of the matrix.
   *
   * In the following, we will not work directly with the matrix \f$ F^{(n)} \f$ but with \f$ G^{(n)} \f$ which has its
   * rows and columns permuted w.r.t. \f$ F^{(n)} \f$, i.e. \f$ F^{(n)} = P^{(n)}_r G^{(n)} P^{(n)}_c \f$. \f$ P^{(n)}_r
   * \f$ and \f$ P^{(n)}_c \f$ are permutation matrices that permute the rows and columns of \f$ G^{(n)} \f$ to restore
   * the original matrix. This gives us more flexibility when adding, removing, or modifying rows and columns.
   *
   * Suppose we know the matrix \f$ G^{(n)} \f$ and its determinant \f$ \det(G^{(n)}) \f$ and that we want to add \f$ k
   * \f$ new rows and columns to the matrix. We can write the resulting matrix as
   * \f[
   *   G^{(n+k)} = \begin{bmatrix} G^{(n)} & B \\ C & D \end{bmatrix} =
   *   \begin{bmatrix} [M^{(n)}]^{-1} & B \\ C & D \end{bmatrix} =
   *   [M^{(n+k)}]^{-1} \; .
   * \f]
   * Here, we have introduced the inverse matrix \f$ M^{(n)} = [G^{(n)}]^{-1} \f$ and the matrices \f$ B \f$, \f$ C \f$,
   * and \f$ D \f$ which are of size \f$ n \times k \f$, \f$ k \times n \f$, and \f$ k \times k \f$, respectively. They
   * contain the elements of the new rows and columns.
   *
   * By making use of the block structure of the matrix (see also </a href="https://en.wikipedia.org/wiki/Block_matrix">
   * Wikipedia</a>), the inverse matrix takes the form
   * \f[
   *   M^{(n+k)} = \begin{bmatrix} M^{(n)} + M^{(n)} B (D - C M^{(n)} B)^{-1} C M^{(n)} & -M^{(n)} B (D - C M^{(n)}
   *   B)^{-1} \\ -(D - C M^{(n)} B)^{-1} C M^{(n)} & (D - C M^{(n)} B)^{-1} \end{bmatrix} =
   *   \begin{bmatrix} P & Q \\ R & S \end{bmatrix}  \; ,
   * \f]
   * and its determinant can be computed as
   * \f[
   *   \det(G^{(n+k)}) = \det(G^{(n)}) \det(D - C M^{(n)} B) = \det(G^{(n)}) \det(S^{-1}) \; .
   * \f]
   * Here, \f$ P \f$, \f$ Q \f$, \f$ R \f$, and \f$ S \f$ are matrices of size \f$ n \times n \f$, \f$ n \times k \f$,
   * \f$ k \times n \f$, and \f$ k \times k \f$, respectively.
   *
   * The original matrix \f$ F^{(n)} \f$ and its determinant can be obtained with \f$ F^{(n)} = P^{(n)}_r G^{(n)}
   * P^{(n)}_c \f$ and \f$ \det(F^{(n)}) = \det(P^{(n)}_r) \det(G^{(n)}) det(P^{(n)}_c) = s^{(n)} \det(G^{(n)}) \f$,
   * where \f$ s^{(n)} = \det(P^{(n)}_r) det(P^{(n)}_c) = \pm 1 \f$ is a sign associated with the permutation matrices.
   *
   * So by keeping track of the inverse matrix \f$ M^{(n)} \f$, the determinant \f$ \det(G^{(n)}) \f$ and two additional
   * permutation matrices that specify how the rows and columns have been permuted w.r.t. the original matrix, we can
   * add, remove, or modify (a few) rows and columns fairly efficiently.
   *
   * @tparam F triqs::det_manip::MatrixBuilder.
   */
  template <MatrixBuilder F> class det_manip {
    public:
    /// Type of the first argument that the triqs::det_manip::MatrixBuilder takes.
    using x_type = detail::get_xarg_t<F>;

    /// Type of the second argument that the triqs::det_manip::MatrixBuilder takes.
    using y_type = detail::get_yarg_t<F>;

    /// Type of the result that the triqs::det_manip::MatrixBuilder returns.
    using value_type = detail::get_result_t<F>;

    /// Type of the matrix.
    using matrix_type = nda::matrix<value_type>;

    public:
    /**
     * @brief Construct a det_manip object with a triqs::det_manip::MatrixBuilder and an initial capacity for the data
     * storages.
     *
     * @param f triqs::det_manip::MatrixBuilder object.
     * @param ncap Initial capacity for the size of the matrix, i.e. the maximum number of rows and columns.
     * @param kcap Initial capacity for the maximum number of rows and columns that can be added or removed in a single
     * operation.
     */
    det_manip(F f, long ncap, long kcap = 1) : f_(std::move(f)) { reserve(ncap, kcap); }

    /**
     * @brief Construct a det_manip object with a triqs::det_manip::MatrixBuilder and two ranges containing the
     * arguments for the matrix builder.
     *
     * @tparam X triqs::det_manip::MatrixBuilderXRange.
     * @tparam Y triqs::det_manip::MatrixBuilderYRange.
     * @param f triqs::det_manip::MatrixBuilder object.
     * @param x_rg Range containing the first arguments.
     * @param y_rg Range containing the second arguments.
     */
    template <typename X, typename Y>
      requires(MatrixBuilderXRange<X, F> && MatrixBuilderYRange<Y, F>)
    det_manip(F f, X &&x_rg, Y &&y_rg) : f_(std::move(f)), n_(std::ranges::size(x_rg)) { // NOLINT (ranges need not be forwarded)
      // check input sizes
      if (n_ != std::ranges::size(y_rg)) TRIQS_RUNTIME_ERROR << "Error in det_manip::det_manip: Argument ranges have different sizes";

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
     * @details It only reserves/resizes if the current capacities are smaller than the new capacities.
     *
     * @param new_ncap New capacity for the size of the matrix, i.e. the maximum number of rows and columns.
     * @param new_kcap New capacity for the maximum number of rows and columns that can be added or removed in a single
     * operation.
     */
    void reserve(long new_ncap, long new_kcap = 1) {
      if (new_kcap > kcap_) {
        kcap_ = new_kcap;
        if (new_ncap <= ncap_) wk_.resize(ncap_, kcap_);
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
        wk_.resize(ncap_, kcap_);
      }
    }

    /// Clear the data storages and set the size to zero.
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
     * @brief Set the threshold being used when testing for a singular matrix (default: -1).
     *
     * @details The threshold \f$ \epsilon \f$ determines when a matrix \f$ M \f$ is considered singular. A matrix is
     * considered to be singular if \f$ |\det(M)| < \epsilon \f$.
     *
     * If \f$ \epsilon \f$ is negative, it simply checks if the determinant is not `std::isnormal`.
     *
     * @param eps Threshold value.
     */
    void set_singular_threshold(double eps) { singular_threshold_ = eps; }

    /**
     * @brief Set the number of operations before a consistency check is performed (default: 100).
     * @param nops Number of operations.
     */
    void set_n_operations_before_check(std::uint64_t nops) { nops_before_check_ = nops; }

    /**
     * @brief Set the precision threshold that determines when to print a warning (default: 1e-8).
     *
     * @details In case we compare two matrices \f$ A \f$ and \f$ B \f$, a warning is printed when \f$ 2 \lVert A - B
     * \rVert >= \epsilon \lVert A \rVert + \lVert B \rVert \f$, where \f$ \lVert \cdot \rVert \f$ is the max norm.
     *
     * In case we compare two scalar values \f$ a \f$ and \f$ b \f$, a warning is printed when \f$ 2 |a - b| >= \epsilon
     * (|a| + |b|) \f$.
     *
     * @param eps Threshold value.
     */
    void set_precision_warning(double eps) { precision_warning_ = eps; }

    /**
     * @brief Set the precision threshold that determines when to throw an exception (default: 1e-5).
     * @details See set_precision_warning for details.
     * @param eps Threshold value.
     */
    void set_precision_error(double eps) { precision_error_ = eps; }

    /// Get the current size of the matrix.
    [[nodiscard]] auto size() const { return n_; }

    /// Get current capacity of the data storages.
    [[nodiscard]] auto capacity() const { return M_.shape()[0]; }

    /**
     * @brief Get the matrix builder argument \f$ x_i \f$ that determines the elements of the i<sup>th</sup> row in the
     * original matrix \f$ F^{(n)} \f$.
     *
     * @param i Argument index.
     * @return Argument value \f$ x_i \f$.
     */
    [[nodiscard]] auto const &get_x(long i) const {
      EXPECTS(0 <= i and i < size());
      return x_[row_perm_[i]];
    }

    /**
     * @brief Get the matrix builder argument \f$ y_j \f$ that determines the elements of the j<sup>th</sup> column in
     * the original matrix \f$ F^{(n)} \f$.
     *
     * @param j Argument index.
     * @return Argument value \f$ y_j \f$.
     */
    [[nodiscard]] auto const &get_y(long j) const {
      EXPECTS(0 <= j and j < size());
      return y_[col_perm_[j]];
    }

    /// Get a vector with all matrix builder arguments \f$ \mathbf{x} \f$.
    [[nodiscard]] auto get_x() const {
      std::vector<x_type> res;
      res.reserve(n_);
      for (auto i : range(n_)) res.emplace_back(x_[row_perm_[i]]);
      return res;
    }

    /// Get a vector with all matrix builder arguments \f$ \mathbf{y} \f$.
    [[nodiscard]] auto get_y() const {
      std::vector<y_type> res;
      res.reserve(n_);
      for (auto i : range(n_)) res.emplace_back(y_[col_perm_[i]]);
      return res;
    }

    /**
     * @brief Get the matrix builder arguments \f$ \mathbf{x} \f$ in the order of the matrix \f$ G^{(n)} \f$.
     * @return `std::vector` containing the arguments \f$ x_i \f$.
     */
    [[nodiscard]] auto const &get_x_internal_order() const { return x_; }

    /**
     * @brief Get the matrix builder arguments \f$ \mathbf{y} \f$ in the order of the matrix \f$ G^{(n)} \f$.
     * @return `std::vector` containing the arguments \f$ y_j \f$.
     */
    [[nodiscard]] auto const &get_y_internal_order() const { return y_; }

    /// Get the derminant of the original matrix \f$ F^{(n)} \f$.
    [[nodiscard]] auto determinant() const { return sign_ * det_; }

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
    [[nodiscard]] auto inverse_matrix(int i, int j) const {
      EXPECTS(0 <= i and i < size());
      EXPECTS(0 <= j and j < size());
      return M_(col_perm_[i], row_perm_[j]);
    }

    /// Get the full inverse matrix \f$ [F^{(n)}]^{-1} \f$. See inverse_matrix(int, int) for details.
    [[nodiscard]] auto inverse_matrix() const {
      matrix_type res(size(), size());
      nda::for_each(res.shape(), [this, &res](auto i, auto j) { res(i, j) = this->inverse_matrix(i, j); });
      return res;
    }

    /**
     * @brief Get an element of the matrix \f$ M^{(n)} = [G^{(n)}]^{-1} \f$.
     *
     * @param i Row index.
     * @param j Column index.
     * @return The matrix element \f$ M^{(n)}_{ij} \f$.
     */
    [[nodiscard]] auto inverse_matrix_internal_order(int i, int j) const {
      EXPECTS(0 <= i and i < size());
      EXPECTS(0 <= j and j < size());
      return M_(i, j);
    }

    /// Get the full inverse matrix \f$ M^{(n)} = [G^{(n)}]^{-1} \f$.
    [[nodiscard]] auto inverse_matrix_internal_order() const {
      return nda::matrix_const_view<value_type>{M_(nda::range(size()), nda::range(size()))};
    }

    /// Get the original matrix \f$ F^{(n)} \f$.
    [[nodiscard]] auto matrix() const {
      matrix_type res(size(), size());
      nda::for_each(res.shape(), [this, &res](auto i, auto j) { res(i, j) = f_(get_x(i), get_y(j)); });
      return res;
    }

    /**
     * @brief For each implementation for triqs::det_manip::det_manip objects.
     *
     * @details It loops over all elements of the matrix \f$ M^{(n)} \f$ and calls the given callable object for each
     * element together with the corresponding arguments \f$ x_i \f$ and \f$ y_j \f$.
     *
     * @tparam L Callable type.
     * @param dm triqs::det_manip::det_manip object.
     * @param fn Callable object that takes three arguments: \f$ x_i \f$, \f$ y_j \f$, and \f$ M_{ji} \f$.
     */
    friend void foreach (det_manip const &dm, auto const &fn) {
      nda::for_each(std::array{dm.size(), dm.size()}, [&fn, &dm](auto i, auto j) { return fn(dm.x_[i], dm.y_[j], dm.M_(j, i)); });
    }

    /**
     * @brief Swap two rows.
     *
     * @details It simply performs the transposition in the row permutation vector and changes the sign \f$ s^{(n)} \f$
     * associated with the permutation matrices.
     *
     * Since we are only changing the matrix \f$ P^{(n)}_r \f$, the matrix \f$ G^{(n)} \f$ and its determinant remain
     * unchanged.
     *
     * @param i Index of the first row to swap.
     * @param j Index of the second row to swap.
     */
    void swap_row(long i, long j) {
      EXPECTS(0 <= i and i < size());
      EXPECTS(0 <= j and j < size());
      if (i == j) return;
      std::swap(row_perm_[i], row_perm_[j]);
      sign_ = -sign_;
    }

    /**
     * @brief Swap two columns.
     *
     * @details It simply performs the transposition in the column permutation vector and changes the sign \f$ s^{(n)}
     * \f$ associated with the permutation matrices.
     *
     * Since we are only changing the matrix \f$ P^{(n)}_c \f$, the matrix \f$ G^{(n)} \f$ and its determinant remain
     * unchanged.
     *
     * @param i Index of the first column to swap.
     * @param j Index of the second column to swap.
     */
    void swap_col(long i, long j) {
      EXPECTS(0 <= i and i < size());
      EXPECTS(0 <= j and j < size());
      if (i == j) return;
      std::swap(col_perm_[i], col_perm_[j]);
      sign_ = -sign_;
    }

    /**
     * @brief Direction of the roll operation.
     *
     * @details It specifies the direction of the circular shift performed on either the rows or columns of the matrix
     * \f$ F^{(n)} \f$. The following directions are supported:
     *
     * - `None`: No roll operation is performed.
     * - `Up`: Roll the rows up.
     * - `Down`: Roll the rows down.
     * - `Left`: Roll the columns to the left.
     * - `Right`: Roll the columns to the right.
     */
    enum RollDirection { None, Up, Down, Left, Right };

    /**
     * @brief Perform a circular shift permutation on the rows or columns of the matrix \f$ F^{(n)} \f$.
     *
     * @details See RollDirection for the supported directions.
     *
     * A circular shift permutation of a finite set is equivalent to \f$ N \f$ transpositions, where \f$ N \f$ is the
     * size of the set. The sign of the permutation is therefore given by \f$ (-1)^{N-1} \f$.
     *
     * @param dir Direction of the roll operation.
     * @return -1 if the roll changes the sign of the determinant, 1 otherwise.
     */
    int roll_matrix(RollDirection dir) {
      // early return for matrices of size 0 or 1
      if (size() < 2) return 1;

      // perform the circular shift permutation
      switch (dir) {
        case (None): return 1;
        case (Down): std::ranges::rotate(row_perm_, row_perm_.end() - 1); break;
        case (Up): std::ranges::rotate(row_perm_, row_perm_.begin() + 1); break;
        case (Right): std::ranges::rotate(col_perm_, col_perm_.end() - 1); break;
        case (Left): std::ranges::rotate(col_perm_, col_perm_.begin() + 1); break;
        default: TRIQS_RUNTIME_ERROR << "Error in det_manip::roll_matrix: Invalid roll direction";
      }

      // update the sign and return the sign change
      if ((size() - 1) % 2 == 1) {
        sign_ = -sign_;
        return -1;
      }
      return 1;
    }

    /**
     * @brief Try to insert one row and column.
     *
     * @details The row is inserted at position \f$ i \f$ the column at position \f$ j \f$ in the original matrix
     * \f$ F^{(n)} \f$. Their elements are determined by the given arguments \f$ x \f$ and \f$ y \f$ as well as the
     * triqs::det_manip::MatrixBuilder object \f$ f \f$ together with the current arguments.
     *
     * This is a special case of try_insert_k() with \f$ k = 1 \f$.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the elements of the new row.
     * @param y Argument to the matrix builder that determines the elements of the new column.
     * @return Determinant ratio \f$ det(F^{(n+1)}) / det(F^{(n)}) \f$.
     */
    value_type try_insert(long i, long j, x_type const &x, y_type const &y) {
      // check input arguments and copy them to the working data
      EXPECTS(last_try_ == try_tag::NoTry);
      EXPECTS(0 <= i and i <= n_);
      EXPECTS(0 <= j and j <= n_);
      std::tie(wins_.i, wins_.j, wins_.x, wins_.y) = std::make_tuple(i, j, x, y);

      // set the try tag
      last_try_ = try_tag::Insert;

      // early return if the current matrix is empty
      if (n_ == 0) {
        newdet_  = f_(x, y);
        newsign_ = 1;
        return newdet_;
      }

      // reserve memory for the working data
      if (n_ + 1 > wins_.capacity()) wins_.reserve(2 * (n_ + 1));

      // calculate the new column B and the new row C of the matrix G (except for the element D)
      for (long k = 0; k < size(); ++k) {
        wins_.B(k) = f_(x_[k], y);
        wins_.C(k) = f_(x, y_[k]);
      }

      // calculate S^{-1} = D - C * M * B
      auto rg_n = nda::range(n_);
      nda::blas::gemv(1.0, M_(rg_n, rg_n), wins_.B(rg_n), 0.0, wins_.MB(rg_n));
      wins_.S_inv = f_(x, y) - nda::blas::dot(wins_.C(rg_n), wins_.MB(rg_n));

      // calculate the new determinant = det(G^{(n)}) * S^{-1} and the new sign = old sign * (-1)^{i + j}
      newdet_  = det_ * wins_.S_inv;
      newsign_ = ((i + j) % 2 == 0 ? sign_ : -sign_);

      return wins_.S_inv * newsign_ * sign_;
    }

    private:
    // Complete the insert operation.
    void complete_insert() {
      auto const new_size = n_ + 1;
      auto const old_size = n_;
      ++n_;

      // reserve data storages
      if (new_size > capacity()) reserve(2 * new_size);

      // copy the matrix builder arguments
      x_.push_back(wins_.x);
      y_.push_back(wins_.y);

      // early return if the new matrix is size 1
      if (n_ == 1) {
        M_(0, 0) = 1 / newdet_;
        row_perm_.push_back(0);
        col_perm_.push_back(0);
        return;
      }

      // update the permutation vectors: only rows and cols with k > i or l > j are affected
      row_perm_.push_back(old_size);
      std::rotate(row_perm_.begin() + wins_.i, row_perm_.begin() + old_size, row_perm_.begin() + new_size);
      col_perm_.push_back(old_size);
      std::rotate(col_perm_.begin() + wins_.j, col_perm_.begin() + old_size, col_perm_.begin() + new_size);

      // calculate C^T M by computing its transpose, i.e. M^T C
      auto rg_n = nda::range(old_size);
      nda::blas::gemv(1.0, transpose(M_(rg_n, rg_n)), wins_.C(rg_n), 0.0, wins_.CM(rg_n));

      // calculate M^{(n+1)} using the update formula
      auto rg_n1          = nda::range{new_size};
      auto const S        = 1 / wins_.S_inv;
      wins_.CM(old_size)  = -1;
      wins_.MB(old_size)  = -1;
      M_(rg_n1, old_size) = 0;
      M_(old_size, rg_n1) = 0;
      nda::blas::ger(S, wins_.MB(rg_n1), wins_.CM(rg_n1), M_(rg_n1, rg_n1));
    }

    public:
    /**
     * @brief Try to insert \f$ k \f$ rows and columns.
     *
     * @details The positions of the new rows and columns in the original matrix \f$ F^{(n+k)} \f$ are specified in the
     * tuples \f$ \mathbf{i} \f$ and \f$ \mathbf{j} \f$, respectively. Their elements are determined by the given
     * arguments \f$ \mathbf{x} \f$ and \f$ \mathbf{y} \f$ as well as the triqs::det_manip::MatrixBuilder object \f$ f
     * \f$ together with the current arguments.
     *
     * Since we are working with \f$ G^{(n)} \f$, we are free to insert the rows and columns at the bottom and right of
     * the matrix and use the update formulas presented in triqs::det_manip::det_manip.
     *
     * We use the following order for the rows and columns to be inserted:
     * - The first row (column) in C (B) corresponds to the row (column) with the smallest index in the matrix \f$
     * F^{(n)} \f$.
     * - The second row (column) in C (B) corresponds to the row (column) with the second smallest index in the matrix
     * \f$ F^{(n)} \f$.
     * - And so on.
     *
     * The expression for the new determinant can be found at triqs::det_manip::det_manip and the new sign associated
     * with the permutation matrices can be written as
     * \f[
     *   s^{(n+k)} = \det(P^{(n)}_r) \det(P^{(n)}_c) \det(P1) \det(P2) = s^{(n)} \det(P1) \det(P2) \; ,
     * \f]
     * where \f$ P1 \f$ and \f$ P2 \f$ are the permutation matrices that move the inserted rows and columns to their
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
     * @param x Arguments to the matrix builder that determines the elements of the new rows.
     * @param y Arguments to the matrix builder that determines the elements of the new columns.
     * @return Determinant ratio \f$ det(F^{(n+k)}) / det(F^{(n)}) \f$.
     */
    value_type try_insert_k(std::vector<long> i, std::vector<long> j, std::vector<x_type> x, std::vector<y_type> y) {
      // check input argument sizes
      k_ = static_cast<long>(i.size());
      EXPECTS(last_try_ == try_tag::NoTry);
      EXPECTS(k_ > 0);
      EXPECTS(j.size() == k_);
      EXPECTS(x.size() == k_);
      EXPECTS(y.size() == k_);

      // move the input arguments to the working data
      winsk_.i = std::move(i);
      winsk_.j = std::move(j);
      winsk_.x = std::move(x);
      winsk_.y = std::move(y);

      // sort input arguments and check for duplicates and out-of-bounds indices
      auto comp = [](auto const &a, auto const &b) { return std::get<0>(a) < std::get<0>(b); };
      std::ranges::sort(std::ranges::zip_view(winsk_.i, winsk_.x), comp);
      std::ranges::sort(std::ranges::zip_view(winsk_.j, winsk_.y), comp);
      EXPECTS(std::ranges::adjacent_find(winsk_.i) == winsk_.i.end());
      EXPECTS(winsk_.i.front() >= 0 && winsk_.i.back() < n_ + k_);
      EXPECTS(std::ranges::adjacent_find(winsk_.j) == winsk_.j.end());
      EXPECTS(winsk_.j.front() >= 0 && winsk_.j.back() < n_ + k_);

      // set the try tag
      last_try_ = try_tag::InsertK;

      // reserve memory for the working data
      auto const [n_cap, k_cap] = winsk_.capacity();
      if (n_ + k_ > n_cap || k_ > k_cap) winsk_.reserve(2 * (n_ + k_), k_);

      // build the matrix D as part of S^{-1} = D - C M B
      nda::for_each(std::array{k_, k_}, [this](auto l, auto m) { winsk_.S_inv(l, m) = f_(winsk_.x[l], winsk_.y[m]); });

      // early return if the current matrix is empty
      if (n_ == 0) {
        newdet_  = detail::determinant(winsk_.S_inv, k_);
        newsign_ = 1;
        return newdet_;
      }

      // calculate the new columns B and the new rows C of the matrix G (except for the block matrix D)
      for (long l = 0; l < n_; ++l) {
        for (long m = 0; m < k_; ++m) {
          winsk_.B(l, m) = f_(x_[l], winsk_.y[m]);
          winsk_.C(m, l) = f_(winsk_.x[m], y_[l]);
        }
      }

      // calculate S^{-1} = D - C M B and its determinant
      auto rg_n = nda::range(n_);
      auto rg_k = nda::range(k_);
      nda::blas::gemm(1.0, M_(rg_n, rg_n), winsk_.B(rg_n, rg_k), 0.0, winsk_.MB(rg_n, rg_k));
      nda::blas::gemm(-1.0, winsk_.C(rg_k, rg_n), winsk_.MB(rg_n, rg_k), 1.0, winsk_.S_inv(rg_k, rg_k));
      auto const det_S_inv = detail::determinant(winsk_.S_inv, k_);

      // calculate the new determinant = det(G^{(n)}) * det(S^{-1}) and sign = old sign * (-1)^{\sum_l i_l + j_l}
      newdet_      = det_ * det_S_inv;
      auto idx_sum = std::accumulate(winsk_.i.begin(), winsk_.i.end(), 0l) + std::accumulate(winsk_.j.begin(), winsk_.j.end(), 0l);
      newsign_     = (idx_sum % 2 == 0 ? sign_ : -sign_);

      return det_S_inv * newsign_ * sign_;
    }

    /**
     * @brief Try to insert two rows and columns.
     *
     * @details The rows are inserted at the positions \f$ i_0 \f$ and \f$ i_1 \f$ and the columns at the positions
     * \f$ j_0 \f$ and \f$ j_1 \f$ in the original matrix \f$ F^{(n)} \f$. Their elements are determined by the given
     * arguments \f$ x_0 \f$, \f$ x_1 \f$, \f$ y_0 \f$ and \f$ y_1 \f$ as well as the triqs::det_manip::MatrixBuilder
     * object \f$ f \f$ together with the current arguments.
     *
     * It simply calls the more general try_insert_k().
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i0 Position of the first row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param i1 Position of the second row to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j0 Position of the first column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param j1 Position of the second column to be inserted in the original matrix \f$ F^{(n)} \f$.
     * @param x_0 Argument to the matrix builder that determines the elements of the first new row.
     * @param x_1 Argument to the matrix builder that determines the elements of the second new row.
     * @param y_0 Argument to the matrix builder that determines the elements of the first new column.
     * @param y_1 Argument to the matrix builder that determines the elements of the second new column.
     * @return Determinant ratio \f$ det(F^{(n+2)}) / det(F^{(n)}) \f$.
     */
    value_type try_insert2(long i0, long i1, long j0, long j1, x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
      return try_insert_k({i0, i1}, {j0, j1}, {x0, x1}, {y0, y1});
    }

    private:
    // Complete the insert_k operation.
    void complete_insert_k() {
      auto const new_size = n_ + k_;
      auto const old_size = n_;

      // reserve data storages
      if (new_size > capacity()) reserve(2 * new_size);

      // append to matrix builder arguments and permutation vectors
      std::ranges::copy(winsk_.x, std::back_inserter(x_));
      std::ranges::copy(winsk_.y, std::back_inserter(y_));
      std::ranges::copy(std::ranges::iota_view(old_size, new_size), std::back_inserter(row_perm_));
      std::ranges::copy(std::ranges::iota_view(old_size, new_size), std::back_inserter(col_perm_));

      // early return if the old matrix was empty
      auto rg_k = nda::range(k_);
      if (old_size == 0) {
        n_             = new_size;
        M_(rg_k, rg_k) = nda::linalg::inv(winsk_.S_inv(rg_k, rg_k));
        return;
      }

      // update the permutation vectors
      for (auto l : rg_k) {
        ++n_;
        std::rotate(row_perm_.begin() + winsk_.i[l], row_perm_.begin() + n_ - 1, row_perm_.begin() + n_);
        std::rotate(col_perm_.begin() + winsk_.j[l], col_perm_.begin() + n_ - 1, col_perm_.begin() + n_);
      }

      // calculate the matrix product C M and the matrix S
      auto rg_n = nda::range(old_size);
      nda::blas::gemm(1.0, winsk_.C(rg_k, rg_n), M_(rg_n, rg_n), 0.0, winsk_.CM(rg_k, rg_n));
      nda::linalg::inv_in_place(winsk_.S_inv(rg_k, rg_k)); // S_inv contains S now

      // calculate M^{(n+k)} using the update formula
      auto rg_nk               = nda::range(new_size);
      auto rg_n_nk             = nda::range(old_size, new_size);
      winsk_.CM(rg_k, rg_n_nk) = -1;
      winsk_.MB(rg_n_nk, rg_k) = -1;
      M_(rg_nk, rg_n_nk)       = 0;
      M_(rg_n_nk, rg_nk)       = 0;
      nda::blas::gemm(1.0, winsk_.MB(rg_nk, rg_k), (winsk_.S_inv(rg_k, rg_k) * winsk_.CM(rg_k, rg_nk)), 1.0, M_(rg_nk, rg_nk));
    }

    // Complete the insert2 operation.
    void complete_insert2() { complete_insert_k(); }

    public:
    /**
     * @brief Try to remove one row and column.
     *
     * @details The row to be removed is at position \f$ \mathbf{i} \f$ and the column at position \f$ \mathbf{j} \f$ in
     * the original matrix \f$ F^{(n)} \f$.
     *
     * This is a special case of try_remove_k() with \f$ k = 1 \f$.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be removed in the original matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ det(F^{(n-1)}) / det(F^{(n)}) \f$.
     */
    value_type try_remove(long i, long j) {
      // check input arguments and copy them to the working data
      EXPECTS(last_try_ == try_tag::NoTry);
      EXPECTS(i >= 0 and i < n_);
      EXPECTS(j >= 0 and j < n_);
      std::tie(wrem_.i, wrem_.j, wrem_.ip, wrem_.jp) = std::make_tuple(i, j, row_perm_[i], col_perm_[j]);

      // set the try tag
      last_try_ = try_tag::Remove;

      // calculate the signs associated with P1, P2, P3 and P4
      int s_p1p2 = (wrem_.ip == n_ - 1 ? 1 : -1);
      s_p1p2     = (wrem_.jp == n_ - 1 ? s_p1p2 : -s_p1p2);
      int s_p3p4 = ((i + j) % 2 == 0 ? 1 : -1);

      // set the diagonal element S
      wrem_.S = M_(wrem_.jp, wrem_.ip);

      // calculate the new determinant and sign
      newdet_  = det_ * wrem_.S * s_p1p2;
      newsign_ = sign_ * s_p1p2 * s_p3p4;

      return wrem_.S * s_p3p4;
    }

    private:
    // Complete the remove operation.
    void complete_remove() {
      // early return if the resulting matrix is empty
      if (n_ == 1) {
        clear();
        return;
      }

      // perform the P1 and P2 permutations by swapping the row and column to be removed with the last row and column
      auto rg_n = nda::range{n_};
      if (wrem_.ip != n_ - 1) {
        // for M, we have to apply P1^T to the columns
        deep_swap(M_(rg_n, wrem_.ip), M_(rg_n, n_ - 1));
        // update the x arguments and the row permutation vector
        x_[wrem_.ip] = x_[n_ - 1];
        auto it1     = std::ranges::find(row_perm_, wrem_.ip);
        auto it2     = std::ranges::find(row_perm_, n_ - 1);
        std::swap(*it1, *it2);
      }
      if (wrem_.jp != n_ - 1) {
        // for M, we have to apply P2^T to the rows
        deep_swap(M_(wrem_.jp, rg_n), M_(n_ - 1, rg_n));
        // update the y arguments and the column permutation vector
        y_[wrem_.jp] = y_[n_ - 1];
        auto it1     = std::ranges::find(col_perm_, wrem_.jp);
        auto it2     = std::ranges::find(col_perm_, n_ - 1);
        std::swap(*it1, *it2);
      }

      // update the size of the matrix
      --n_;
      rg_n = nda::range{n_};

      // remove elements from the row and column permutation vectors and from the x and y arguments
      std::ignore = std::ranges::remove(row_perm_, n_);
      std::ignore = std::ranges::remove(col_perm_, n_);
      row_perm_.pop_back();
      col_perm_.pop_back();
      x_.pop_back();
      y_.pop_back();

      // calculate -S^{-1}
      auto mS_inv = -1 / wrem_.S;
      ASSERT(std::isfinite(std::abs(w1_.ksi)));

      // solve P = \widetilde{M}^{(n-1)} + \widetilde{M}^{(n-1)} B S C \widetilde{M}^{(n-1)} for \widetilde{M}^{(n-1)}
      // by using the fact that we know -\widetilde{M}^{(n-1)} B S, -S C \widetilde{M}^{(n-1)} and S^{-1}
      blas::ger(mS_inv, M_(rg_n, n_), M_(n_, rg_n), M_(rg_n, rg_n));
    }

    public:
    /**
     * @brief Try to remove \f$ k \f$ rows and columns.
     *
     * @details The rows to be removed are specified in the tuple \f$ \mathbf{i} \f$ and the columns in the tuple
     * \f$ \mathbf{j} \f$. The positions are given w.r.t. the original matrix \f$ F^{(n)} \f$. The corresponding
     * positions in the matrix \f$ G^{(n)} \f$ are denoted by \f$ \mathbf{i}_p \f$ and \f$ \mathbf{j}_p \f$,
     * respectively.
     *
     * Since we are working with \f$ G^{(n)} \f$, we are free to first move the rows and columns to the bottom and to
     * the right of the matrix and use the update formulas presented in triqs::det_manip::det_manip.
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
     * - The first row (column) in C (B) corresponds to the row (column) with the smallest index in the matrix \f$
     * F^{(n)} \f$.
     * - The second row (column) in C (B) corresponds to the row (column) with the second smallest index in the matrix
     * \f$ F^{(n)} \f$.
     * - And so on.
     *
     * The original matrix can be written as
     * \f[
     *   F^{(n)} = P^{(n)}_r G^{(n)} P^{(n)}_c = P^{(n)}_r P_1^{-1} [P_1 G^{(n)} P_2] P_2^{-1} P^{(n)}_c =
     *   \widetilde{P}^{(n)}_r \widetilde{G}^{(n)} \widetilde{P}^{(n)}_c =
     *   P_3 \begin{bmatrix} P^{(n-k)}_r & 0 \\ 0 & I \end{bmatrix} \begin{bmatrix} \widetilde{G}^{(n-k)} & B \\ C & D
     *   \end{bmatrix} \begin{bmatrix} P^{(n-k)}_c & 0 \\ 0 & I \end{bmatrix} P_4 \; ,
     * \f]
     * where \f$ P_3 \f$ and \f$ P_4 \f$ are permutation matrices that move the rows and columns in \f$ B \f$, \f$ C \f$
     * and \f$ D \f$ back to their original positions in the matrix \f$ F^{(n)} \f$.
     *
     * We can therefore write the determinant of the resulting matrix \f $ \widetilde{G}^{(n-k)} \f$ in terms of the
     * determinant of the current matrix \f$ G^{(n)} \f$
     * \f[
     *   \det(\widetilde{G}^{(n-k)}) = \det(\widetilde{G}^{(n)}) \det(S) = \det(P_1) \det(G^{(n)}) \det(P_2) \det(S)
     *   \; ,
     * \f]
     * and the new sign \f$ \widetilde{s}^{(n-k)} \f$ in terms of the current sign \f$ s^{(n)} \f$:
     * \f[
     *   \widetilde{s}^{(n-k)} = \det(\widetilde{P}^{(n-k)}_r) \det(\widetilde{P}^{(n-k)}_c) =
     *   \det(P_3) \det(\widetilde{P}^{(n)}_r) \det(\widetilde{P}^{(n)}_c) \det(P_4) =
     *   \det(P_3) \det(P^{(n)}_r) \det(P_1) \det(P_2) \det(P^{(n)}_c) \det(P_4) =
     *   s^{(n)} \det(P_1) \det(P_2) \det(P_3) \det(P_4) \; .
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
     * @return Determinant ratio \f$ det(F^{(n-k)}) / det(F^{(n)}) \f$.
     */
    value_type try_remove_k(std::vector<long> i, std::vector<long> j) {
      // check input argument sizes
      k_ = static_cast<long>(i.size());
      EXPECTS(last_try_ == try_tag::NoTry);
      EXPECTS(k_ > 0 && k_ <= n_);
      EXPECTS(j.size() == k_);

      // sort and check input arguments
      std::ranges::sort(i);
      std::ranges::sort(j);
      EXPECTS(std::ranges::adjacent_find(i) == i.end() && i.front() >= 0 && i.back() < n_);
      EXPECTS(std::ranges::adjacent_find(j) == j.end() && j.front() >= 0 && j.back() < n_);

      // set the try tag
      last_try_ = try_tag::RemoveK;

      // reserve memory for the working data
      wremk_.reserve(k_);

      // move input arguments to the working data and get the corresponding row/column positions in the matrix G
      wremk_.i = std::move(i);
      wremk_.j = std::move(j);
      for (long l = 0; l < k_; ++l) {
        wremk_.ip[l] = row_perm_[wremk_.i[l]];
        wremk_.jp[l] = col_perm_[wremk_.j[l]];
      }

      // compute the signs of the permutations P1, P2, P3, P4 and set the matrix S
      int s_p1p2   = 1;
      long idx_sum = 0;
      long target  = n_ - k_;
      for (long l = 0; l < k_; ++l) {
        // the combined sign of P3 and P4 is simply (-1)^{\sum i_k + j_k}
        idx_sum += wremk_.i[l] + wremk_.j[l];

        // check if the current position of the row in G is where we want it
        if (wremk_.ip[l] != target) {
          // if not, P1 has to swap it with the corresponding row
          s_p1p2 = -s_p1p2;
          // we have to take care of the case where the row is swapped with another row that we want to remove
          auto it = std::find(wremk_.ip.begin() + l + 1, wremk_.ip.begin() + k_, target);
          if (it != wremk_.ip.begin() + k_) {
            std::swap(wremk_.ip[l], *it);
          } else {
            wremk_.ip[l] = target;
          }
        }

        // check if the current position of the column in G is where we want it
        if (wremk_.jp[l] != target) {
          // if not, P2 has to swap it with the corresponding column
          s_p1p2 = -s_p1p2;
          // we have to take care of the case where the column is swapped with another column that we want to remove
          auto it = std::find(wremk_.jp.begin() + l + 1, wremk_.jp.begin() + k_, target);
          if (it != wremk_.jp.begin() + k_) {
            std::swap(wremk_.jp[l], *it);
          } else {
            wremk_.jp[l] = target;
          }
        }
        ++target;

        // set the elements of the matrix S
        for (long m = 0; m < k_; ++m) { wremk_.S(l, m) = M_(col_perm_[wremk_.j[l]], row_perm_[wremk_.i[m]]); }
      }
      int s_p3p4 = (idx_sum % 2 == 0 ? 1 : -1);

      // compute the new determinant and sign
      auto det_S = detail::determinant(wremk_.S, k_);
      newdet_    = det_ * det_S * s_p1p2;
      newsign_   = sign_ * s_p1p2 * s_p3p4;

      return det_S * s_p3p4;
    }

    /**
     * @brief Try to remove two rows and two columns.
     *
     * @details The rows to be removed are specified by the indices \f$ i_0 \f$ and \f$ i_1 \f$, and the columns by the
     * indices \f$ j_0 \f$ and \f$ j_1 \f$. The positions are given w.r.t. the original matrix \f$ F^{(n)} \f$.
     *
     * It simply calls the more general try_remove_k().
     *
     * @param i0 Position of the first row to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param i1 Position of the second row to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j0 Position of the first column to be removed in the original matrix \f$ F^{(n)} \f$.
     * @param j1 Position of the second column to be removed in the original matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ det(F^{(n-2)}) / det(F^{(n)}) \f$.
     */
    value_type try_remove2(long i0, long i1, long j0, long j1) { return try_remove_k({i0, i1}, {j0, j1}); }

    private:
    // Complete the remove_k operation.
    void complete_remove_k() {
      // early return if the resulting matrix is empty
      if (n_ == k_) {
        clear();
        return;
      }

      // perform the P1 and P2 permutations by swapping the rows and columns accordingly
      auto rg_n = nda::range{n_};
      for (long m = 0, target = n_ - k_; m < k_; ++m, ++target) {
        if (row_perm_[wremk_.i[m]] != target) {
          // for M, we have to apply P1^T to the columns
          deep_swap(M_(rg_n, row_perm_[wremk_.i[m]]), M_(rg_n, target));
          // update the x arguments and the row permutation vector
          x_[row_perm_[wremk_.i[m]]] = x_[target];
          auto it1                   = std::ranges::find(row_perm_, row_perm_[wremk_.i[m]]);
          auto it2                   = std::ranges::find(row_perm_, target);
          std::swap(*it1, *it2);
        }
        if (col_perm_[wremk_.j[m]] != target) {
          // for M, we have to apply P2^T to the rows
          deep_swap(M_(col_perm_[wremk_.j[m]], rg_n), M_(target, rg_n));
          // update the y arguments and the column permutation vector
          y_[col_perm_[wremk_.j[m]]] = y_[target];
          auto jitr                  = std::ranges::find(col_perm_, col_perm_[wremk_.j[m]]);
          auto titr                  = std::ranges::find(col_perm_, target);
          std::swap(*jitr, *titr);
        }
      }

      // update the size of the matrix
      n_ -= k_;
      rg_n = nda::range{n_};

      // remove elements from the row and column permutation vectors and from the x and y arguments
      auto ge_n   = [this](auto i) { return i >= n_; };
      std::ignore = std::ranges::remove_if(row_perm_, ge_n);
      std::ignore = std::ranges::remove_if(col_perm_, ge_n);
      row_perm_.resize(n_);
      col_perm_.resize(n_);
      x_.resize(n_);
      y_.resize(n_);

      // calculate S^{-1}
      auto rg_k    = nda::range{k_};
      auto rg_n_nk = nda::range{n_, n_ + k_};
      nda::linalg::inv_in_place(wremk_.S(rg_k, rg_k));

      // solve P = \widetilde{M}^{(n-k)} + \widetilde{M}^{(n-k)} B S C \widetilde{M}^{(n-k)} for \widetilde{M}^{(n-k)}
      // by using the fact that we know -\widetilde{M}^{(n-k)} B S, -S C \widetilde{M}^{(n-k)} and S^{-1}
      blas::gemm(-1.0, M_(rg_n, rg_n_nk), wremk_.S(rg_k, rg_k) * M_(rg_n_nk, rg_n), 1.0, M_(rg_n, rg_n));
    }

    // Complete the remove2 operation.
    void complete_remove2() { complete_remove_k(); }

    public:
    /**
       * Consider the change the column j and the corresponding y.
       *
       * Returns the ratio of det Minv_new / det Minv.
       * This routine does NOT make any modification. It has to be completed with complete_operation().
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
      //w1.MB(R) = mat_inv(R,R) * w1.MC(R);// OPTIMIZE BELOW
      blas::gemv(1.0, M_(RN, RN), w1_.MC(RN), 0.0, w1_.MB(RN));

      // compute the newdet
      w1_.ksi  = (1 + w1_.MB(w1_.jreal));
      auto ksi = w1_.ksi;
      newdet_  = det_ * ksi;
      newsign_ = sign_;

      return ksi; // newsign/sign is unity
    }
    //------------------------------------------------------------------------------------------
    private:
    void complete_change_col() {
      range RN(n_);
      y_[w1_.jreal] = w1_.y;

      // modifying M : Mij += w1.ksi Bi Mnj
      // using Shermann Morrison formula.
      // implemented in 2 times : first Bn=0 so that Mnj is not modified ! and then change Mnj
      // Cf notes : simply multiply by -w1.ksi
      w1_.ksi           = -1 / w1_.ksi;
      w1_.MB(w1_.jreal) = 0;
      //mat_inv(R,R) += w1.ksi * w1.MB(R) * mat_inv(w1.jreal,R)); // OPTIMIZE BELOW
      blas::ger(w1_.ksi, w1_.MB(RN), M_(w1_.jreal, RN), M_(RN, RN));
      M_(w1_.jreal, RN) *= -w1_.ksi;
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
      TRIQS_ASSERT(last_try_ == try_tag::NoTry);
      TRIQS_ASSERT(i < n_);
      w1_.i     = i;
      last_try_ = try_tag::ChangeRow;
      w1_.ireal = row_perm_[i];
      w1_.x     = x;

      // Compute the col B.
      for (long idx = 0; idx < n_; idx++) w1_.MB(idx) = f_(w1_.x, y_[idx]) - f_(x_[w1_.ireal], y_[idx]);
      range RN(n_);
      //w1.MC(R) = transpose(mat_inv(R,R)) * w1.MB(R); // OPTIMIZE BELOW
      blas::gemv(1.0, transpose(M_(RN, RN)), w1_.MB(RN), 0.0, w1_.MC(RN));

      // compute the newdet
      w1_.ksi  = (1 + w1_.MC(w1_.ireal));
      auto ksi = w1_.ksi;
      newdet_  = det_ * ksi;
      newsign_ = sign_;
      return ksi; // newsign/sign is unity
    }
    //------------------------------------------------------------------------------------------
    private:
    void complete_change_row() {
      range RN(n_);
      x_[w1_.ireal] = w1_.x;

      // modifying M : M ij += w1.ksi Min Cj
      // using Shermann Morrison formula.
      // impl. Cf case 3
      w1_.ksi           = -1 / w1_.ksi;
      w1_.MC(w1_.ireal) = 0;
      //mat_inv(R,R) += w1.ksi * mat_inv(R,w1.ireal) * w1.MC(R);
      blas::ger(w1_.ksi, M_(RN, w1_.ireal), w1_.MC(RN), M_(RN, RN));
      M_(RN, w1_.ireal) *= -w1_.ksi;
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
      //w1.C(R) = mat_inv(R,R) * w1.MC(R);// OPTIMIZE BELOW
      blas::gemv(1.0, M_(RN, RN), w1_.MC(RN), 0.0, w1_.C(RN));
      //w1.B(R) = transpose(mat_inv(R,R)) * w1.MB(R); // OPTIMIZE BELOW
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
      return det_ratio; // newsign/sign is unity
    }
    //------------------------------------------------------------------------------------------
    private:
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
       * Refill determinant with new values
       *
       * New values are calculated as f(x_i, y_i)
       *
       * Returns the ratio of det Minv_new / det Minv.
       *
       * This routine does NOT make any modification. It has to be completed with complete_operation().
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
       *  Finish the move of the last try_xxx called.
       *  Throws if no try_xxx has been done or if the last operation was complete_operation.
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
      if (nops_ > nops_before_check_) regenerate_and_check();
      last_try_ = try_tag::NoTry;
    }

    /**
       *  Reject the previous try_xxx called.
       *  All try_xxx have to be either accepted (complete_operation) or rejected.
       */
    void reject_last_try() { last_try_ = try_tag::NoTry; }

    // ----------------- A few short cuts   -----------------

    public:
    /// Insert (try_insert + complete)
    value_type insert(long i, long j, x_type const &x, y_type const &y) {
      auto r = try_insert(i, j, x, y);
      complete_operation();
      return r;
    }

    /// Insert_at_end (try_insert + complete)
    value_type insert_at_end(x_type const &x, y_type const &y) { return insert(n_, n_, x, y); }

    /// Insert2 (try_insert2 + complete)
    value_type insert2(long i0, long i1, long j0, long j1, x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
      auto r = try_insert2(i0, i1, j0, j1, x0, x1, y0, y1);
      complete_operation();
      return r;
    }

    /// Insert2_at_end (try_insert2 + complete)
    value_type insert2_at_end(x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
      return insert2(n_, n_ + 1, n_, n_ + 1, x0, x1, y0, y1);
    }

    /// Remove (try_remove + complete)
    value_type remove(long i, long j) {
      auto r = try_remove(i, j);
      complete_operation();
      return r;
    }

    /// Remove_at_end (try_remove + complete)
    value_type remove_at_end() { return remove(n_ - 1, n_ - 1); }

    /// Remove2 (try_remove2 + complete)
    value_type remove2(long i0, long i1, long j0, long j1) {
      auto r = try_remove2(i0, i1, j0, j1);
      complete_operation();
      return r;
    }

    /// Remove2_at_end (try_remove2 + complete)
    value_type remove2_at_end() { return remove2(n_ - 1, n_ - 2, n_ - 1, n_ - 2); }

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

    /**
     * @brief Regenerate the inverse matrix, determinant and sign and check the consistency of the current
     * values/objects.
     *
     * @details It uses the matrix builder to regenerate the matrix \f$ G^{(n)} \f$. Then it calculates its inverse
     * \f$ M^{(n)} \f$ with `nda::linalg::inv_in_place` and its determinant \f$ \det(G^{(n)}) \f$ with 
     * `nda::linalg::det`. The sign \f$ s^{(n)} \f$ associated with the permutation matrices is also recalculated.
     *
     * If the stored objects are not consistent with the regenerated ones, a warning is emitted or an exception is
     * thrown.
     *
     * See also set_precision_warning, set_precision_error and set_singular_threshold.
     */
    void regenerate_and_check() {
      nops_ = 0;

      // lambda to write a complex or real number to a string
      auto str = [](auto x) {
        if constexpr (std::same_as<std::decay_t<decltype(x)>, std::complex<double>>)
          return fmt::format("({},{})", std::real(x), std::imag(x));
        else
          return fmt::format("{}", x);
      };

      // early return if the matrix is empty
      if (size() == 0) {
        // empty matrices always have its determinant and sign set to exactly 1
        if (std::abs(det_ - 1) > 1e-14)
          TRIQS_RUNTIME_ERROR << fmt::format("Error in det_manip::regenerate_and_check: Determinant of empty matrix: {} != 1\n", str(det_));
        if (sign_ != 1) TRIQS_RUNTIME_ERROR << fmt::format("Error in det_manip::regenerate_and_check: Sign of empty matrix: {} != 1\n", sign_);
        return;
      }

      // regenerate G and its determinant
      auto mat = matrix_type{size(), size()};
      nda::for_each(mat.shape(), [this, &mat](auto i, auto j) { mat(i, j) = f_(x_[i], y_[j]); });
      auto const det_G = nda::linalg::det(mat);

      // check G and compare determinants
      if (is_singular(det_G))
        TRIQS_RUNTIME_ERROR << fmt::format("Error in det_manip::regenerate_and_check: Matrix G is singular: det(G) = {}\n", str(det_G));
      auto const det_diff = detail::rel_diff(det_, det_G);
      if (det_diff >= precision_warning_)
        fmt::print(stderr, "Warning in det_manip::regenerate_and_check: Inconsistent determinants: {} != {}\n", str(det_), str(det_G));
      if (det_diff >= precision_error_)
        TRIQS_RUNTIME_ERROR << fmt::format("Error in det_manip::regenerate_and_check: Inconsistent determinants: {} != {}\n", str(det_), str(det_G));
      det_ = det_G;

      // check the inverse matrix
      nda::linalg::inv_in_place(mat);
      auto M_v            = M_(nda::range(size()), nda::range(size()));
      auto const mat_diff = detail::rel_diff(mat, M_v);
      if (mat_diff >= precision_warning_)
        fmt::print(stderr, "Warning in det_manip::regenerate_and_check: Inconsistent matrices: relative difference = {}\n", mat_diff);
      if (mat_diff >= precision_error_)
        TRIQS_RUNTIME_ERROR << fmt::format("Error in det_manip::regenerate_and_check: Inconsistent matrices: relative difference = {}\n", mat_diff);
      M_v = mat;

      // regenerate and check the sign of the permutation matrices
      double exp_sign = 1.0;
      auto P          = nda::matrix<double>::zeros(size(), size());
      for (int i = 0; i < size(); i++) P(i, row_perm_[i]) = 1;
      exp_sign *= nda::linalg::det(P);
      P() = 0.0;
      for (int i = 0; i < size(); i++) P(i, col_perm_[i]) = 1;
      exp_sign *= nda::linalg::det(P);
      if ((exp_sign > 0) != (sign_ > 0))
        TRIQS_RUNTIME_ERROR << fmt::format("Error in det_manip::regenerate_and_check: Inconsistent signs: {} != {}\n", sign_, exp_sign);
      sign_ = (exp_sign > 0 ? 1 : -1);
    }

    /// Write into HDF5
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

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, det_manip &g) {
      auto gr = fg.open_group(subgroup_name);
      h5_read(gr, "N", g.n_);
      h5_read(gr, "mat_inv", g.M_);
      g.ncap_     = first_dim(g.M_); // restore Nmax
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

    // Check if the given determinant is considered to be singular.
    [[nodiscard]] bool is_singular(value_type det) const {
      return (singular_threshold_ < 0 ? not std::isnormal(std::abs(det)) : (std::abs(det) < singular_threshold_));
    }

    private:
    // matrix builder: G_{ij} = f_(x_[i], y_[j]) or F_{ij} = f_(x_[row_perm_[i]], y_[col_perm_[j]])
    F f_;
    std::vector<x_type> x_;
    std::vector<y_type> y_;

    // matrix M such that G^{-1} = M(nda::range(size()), nda::range(size())) and det(G)
    matrix_type M_;
    value_type det_{1};

    // permutation vectors: row (column) i in the original matrix F corresponds to the row (column) row_perm[i]
    // (col_perm[i]) in the matrix G
    std::vector<long> row_perm_;
    std::vector<long> col_perm_;
    int sign_{1};

    // working data for the try-complete operations
    detail::work_data_insert<x_type, y_type, value_type> wins_;
    detail::work_data_insert_k<x_type, y_type, value_type> winsk_;
    detail::work_data_remove<value_type> wrem_;
    detail::work_data_remove_k<value_type> wremk_;
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
    long k_{0};
    long kcap_{1};
  };
} // namespace triqs::det_manip
