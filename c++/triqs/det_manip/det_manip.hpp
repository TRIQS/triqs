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

#include "../utility/callable_traits.hpp"
#include "../utility/first_include.hpp"
#include "../arrays.hpp"

#include <nda/nda.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <vector>

namespace triqs::det_manip {

  namespace blas = nda::blas;

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
        auto Rk = range(k);
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
   * @tparam FunctionType Callable object \f$ f \f$ that takes two arguments and returns a real or complex value. It
   * determines the elements of the matrix via \f$ F^{(n)}_{ij} = f(x_i, y_j) \f$.
   */
  template <typename FunctionType> class det_manip {
    private:
    using f_tr = utility::callable_traits<FunctionType>;
    static_assert(f_tr::arity == 2, "det_manip : the function must take two arguments !");

    public:
    using x_type     = typename f_tr::template decay_arg_t<0>;
    using y_type     = typename f_tr::template decay_arg_t<1>;
    using value_type = typename f_tr::result_type;
    using det_type   = value_type;
    static_assert(std::is_floating_point_v<value_type> || nda::is_complex_v<value_type>,
                  "det_manip : the function must return a floating number or a complex number");

    using matrix_type = nda::matrix<value_type>;

    protected: // the data
    FunctionType f;

    det_type det{1};
    long Nmax{0}, N{0};
    long kmax_tried{1}, k_tried{0};
    enum {
      NoTry,
      Insert,
      Remove,
      ChangeCol,
      ChangeRow,
      ChangeRowCol,
      InsertK,
      RemoveK,
      Refill
    } last_try = NoTry; // keep in memory the last operation not completed
    std::vector<long> row_num, col_num;
    std::vector<x_type> x_values;
    std::vector<y_type> y_values;
    int sign = 1;
    matrix_type mat_inv;
    uint64_t n_opts                  = 0;   // count the number of operation
    uint64_t n_opts_max_before_check = 100; // max number of ops before the test of deviation of the det, M^-1 is performed.
    double singular_threshold = -1;    // the test to see if the matrix is singular is abs(det) > singular_threshold. If <0, it is !isnormal(abs(det))
    double precision_warning  = 1.e-8; // bound for warning message in check for singular matrix
    double precision_error    = 1.e-5; // bound for throwing error in check for singular matrix

    /**
     * @brief Write a triqs::det_manip::det_manip object to HDF5.
     *
     * @param fg `h5::group` containing the subgroup to be written to.
     * @param subgroup_name Name of the subgroup.
     * @param g Manipulator object to be written.
     */
    friend void h5_write(h5::group fg, std::string subgroup_name, det_manip const &g) {
      auto gr = fg.create_group(subgroup_name);
      h5_write(gr, "N", g.N);
      h5_write(gr, "mat_inv", g.mat_inv);
      h5_write(gr, "det", g.det);
      h5_write(gr, "sign", g.sign);
      h5_write(gr, "row_num", g.row_num);
      h5_write(gr, "col_num", g.col_num);
      h5_write(gr, "x_values", g.x_values);
      h5_write(gr, "y_values", g.y_values);
      h5_write(gr, "n_opts", g.n_opts);
      h5_write(gr, "n_opts_max_before_check", g.n_opts_max_before_check);
      h5_write(gr, "singular_threshold", g.singular_threshold);
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
      h5_read(gr, "N", g.N);
      h5_read(gr, "mat_inv", g.mat_inv);
      g.Nmax     = first_dim(g.mat_inv); // restore Nmax
      g.last_try = NoTry;
      h5_read(gr, "det", g.det);
      h5_read(gr, "sign", g.sign);
      h5_read(gr, "row_num", g.row_num);
      h5_read(gr, "col_num", g.col_num);
      h5_read(gr, "x_values", g.x_values);
      h5_read(gr, "y_values", g.y_values);
      h5_read(gr, "n_opts", g.n_opts);
      h5_read(gr, "n_opts_max_before_check", g.n_opts_max_before_check);
      h5_read(gr, "singular_threshold", g.singular_threshold);
    }

    private:
    work_data_type1<x_type, y_type, value_type> w1;
    work_data_typek<x_type, y_type, value_type> wk;
    work_data_type_refill<x_type, y_type, value_type> w_refill;
    det_type newdet{1};
    int newsign{1};

    private: // for the move constructor, I need to separate the swap since f may not be defaulted constructed
    void swap_but_f(det_manip &rhs) noexcept {
      using std::swap;
#define SW(a) swap(this->a, rhs.a)
      SW(det);
      SW(Nmax);
      SW(N);
      SW(last_try);
      SW(row_num);
      SW(col_num);
      SW(x_values);
      SW(y_values);
      SW(sign);
      SW(mat_inv);
      SW(n_opts);
      SW(n_opts_max_before_check);
      SW(w1);
      SW(wk);
      SW(newdet);
      SW(newsign);
#undef SW
    }

    friend void swap(det_manip &lhs, det_manip &rhs) noexcept {
      using std::swap;
      swap(lhs.f, rhs.f);
      lhs.swap_but_f(rhs);
    }

    public:
    /**
     * @brief Reserve memory and resize the data storages.
     *
     * @details Like for `std::vector`, this reserves memory for a bigger matrix size. It only reserves/resizes if the
     * requested capacity is larger than the current one. It preserves the matrix \f$ M^{(n)} \f$ but not the temporary
     * working data, so it must NOT be called between a `try_*` function and the corresponding complete_operation().
     *
     * @param new_N New capacity for the size of the matrix, i.e. the maximum number of rows and columns.
     * @param new_k Maximum number of rows and columns inserted or removed in a single operation. It sizes the working
     * data used by the `try_*_k` functions.
     */
    void reserve(long new_N, long new_k = 1) {
      if (new_k > kmax_tried) {
        kmax_tried = new_k;
        if (new_N <= Nmax) wk.resize(Nmax, kmax_tried);
      }
      if (new_N > Nmax) {
        Nmax = 2 * new_N;

        matrix_type mcpy(mat_inv);
        mat_inv.resize(Nmax, Nmax);
        auto Rcpy           = range(mcpy.extent(0));
        mat_inv(Rcpy, Rcpy) = mcpy;

        row_num.reserve(Nmax);
        col_num.reserve(Nmax);
        x_values.reserve(Nmax);
        y_values.reserve(Nmax);

        w1.resize(Nmax);
        wk.resize(Nmax, kmax_tried);
      }
    }

    /**
     * @brief Get the threshold being used when testing for a singular matrix.
     * @details See set_singular_threshold() for details.
     * @return Threshold value.
     */
    [[nodiscard]] double get_singular_threshold() const { return singular_threshold; }

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
    void set_singular_threshold(double threshold) { singular_threshold = threshold; }

    /**
     * @brief Get the number of operations before a consistency check is performed.
     * @details See set_n_operations_before_check() for details.
     * @return Number of operations.
     */
    [[nodiscard]] double get_n_operations_before_check() const { return n_opts_max_before_check; }

    /**
     * @brief Set the number of operations before a consistency check is performed (default: 100).
     * @details See complete_operation(), which triggers the check once the counter exceeds this number.
     * @param n Number of operations.
     */
    void set_n_operations_before_check(uint64_t n) { n_opts_max_before_check = n; }

    /**
     * @brief Get the precision threshold that determines when to print a warning.
     * @details See set_precision_warning() for details.
     * @return Threshold value.
     */
    [[nodiscard]] double get_precision_warning() const { return precision_warning; }

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
    void set_precision_warning(double threshold) { precision_warning = threshold; }

    /**
     * @brief Get the precision threshold that determines when to throw an exception.
     * @details See set_precision_warning() for details.
     * @return Threshold value.
     */
    [[nodiscard]] double get_precision_error() const { return precision_error; }

    /**
     * @brief Set the precision threshold that determines when to throw an exception (default: 1e-5).
     * @details See set_precision_warning() for details.
     * @param threshold Threshold value.
     */
    void set_precision_error(double threshold) { precision_error = threshold; }

    /**
     * @brief Construct a det_manip object with a callable `FunctionType` and an initial capacity for the data storages.
     *
     * @details Like for `std::vector`, the capacity grows automatically (by a factor of 2) when needed, but this can
     * yield a performance penalty if it happens too often.
     *
     * @param F Callable `FunctionType` object (a copy is stored in the class).
     * @param init_size Initial capacity for the size of the matrix, i.e. the maximum number of rows and columns.
     */
    det_manip(FunctionType F, long init_size) : f(std::move(F)) {
      reserve(init_size);
      mat_inv() = 0;
    }

    /**
     * @brief Construct a det_manip object with a callable `FunctionType` and two containers holding the arguments for
     * the matrix builder.
     *
     * @tparam ArgumentContainer1 Container type holding the first arguments.
     * @tparam ArgumentContainer2 Container type holding the second arguments.
     * @param F Callable `FunctionType` object (a copy is stored in the class).
     * @param X Container holding the first arguments \f$ \mathbf{x} \f$.
     * @param Y Container holding the second arguments \f$ \mathbf{y} \f$.
     */
    template <typename ArgumentContainer1, typename ArgumentContainer2>
    det_manip(FunctionType F, ArgumentContainer1 const &X, ArgumentContainer2 const &Y) : f(std::move(F)) {
      if (X.size() != Y.size()) TRIQS_RUNTIME_ERROR << " X.size != Y.size";
      N = X.size();
      if (N == 0) {
        det = 1;
        reserve(30);
        return;
      }
      reserve(N);
      std::copy(X.begin(), X.end(), std::back_inserter(x_values));
      std::copy(Y.begin(), Y.end(), std::back_inserter(y_values));
      mat_inv() = 0;
      for (long i = 0; i < N; ++i) {
        row_num.push_back(i);
        col_num.push_back(i);
        for (long j = 0; j < N; ++j) mat_inv(i, j) = f(x_values[i], y_values[j]);
      }
      range RN(N);
      det             = nda::linalg::det(mat_inv(RN, RN));
      mat_inv(RN, RN) = nda::linalg::inv(mat_inv(RN, RN));
    }

    // Special member functions. All of them should be defaulted.
    det_manip(det_manip const &) = default;
    det_manip(det_manip &&rhs) noexcept : f(std::move(rhs.f)) {
      this->swap_but_f(rhs);
    } // f need not have a default constructor and we dont swap the temp data...
    //det_manip& operator=(const det_manip&) = default;
    det_manip &operator=(const det_manip &) = delete;
    det_manip &operator=(det_manip &&rhs) noexcept {
      assert((last_try == NoTry) && (rhs.last_try == NoTry));
      swap(*this, rhs);
      return *this;
    }

    /**
     * @brief Clear the data storages and reset the matrix to size zero.
     */
    void clear() {
      N        = 0;
      sign     = 1;
      det      = 1;
      last_try = NoTry;
      row_num.clear();
      col_num.clear();
      x_values.clear();
      y_values.clear();
    }

    //----------------------- READ ACCESS TO DATA ----------------------------------

    /**
     * @brief Get the current size of the matrix.
     * @return Number of rows/columns of the matrix.
     */
    [[nodiscard]] long size() const { return N; }

    /**
     * @brief Get the matrix builder argument \f$ x_i \f$ that determines the elements of the i<sup>th</sup> row in the
     * original matrix \f$ F^{(n)} \f$.
     * @param i Argument index.
     * @return Argument value \f$ x_i \f$.
     */
    x_type const &get_x(long i) const { return x_values[row_num[i]]; }

    /**
     * @brief Get the matrix builder argument \f$ y_j \f$ that determines the elements of the j<sup>th</sup> column in
     * the original matrix \f$ F^{(n)} \f$.
     * @param j Argument index.
     * @return Argument value \f$ y_j \f$.
     */
    y_type const &get_y(long j) const { return y_values[col_num[j]]; }

    /**
     * @brief Get a vector with all matrix builder arguments \f$ \mathbf{x} \f$.
     * @details Warning: this is slow, since it creates a new copy and reorders the rows.
     * @return `std::vector` containing the arguments \f$ x_i \f$ in the order of the original matrix \f$ F^{(n)} \f$.
     */
    std::vector<x_type> get_x() const {
      std::vector<x_type> res;
      res.reserve(N);
      for (long i : range(N)) res.emplace_back(x_values[row_num[i]]);
      return res;
    }

    /**
     * @brief Get a vector with all matrix builder arguments \f$ \mathbf{y} \f$.
     * @details Warning: this is slow, since it creates a new copy and reorders the columns.
     * @return `std::vector` containing the arguments \f$ y_j \f$ in the order of the original matrix \f$ F^{(n)} \f$.
     */
    std::vector<y_type> get_y() const {
      std::vector<y_type> res;
      res.reserve(N);
      for (long i : range(N)) res.emplace_back(y_values[col_num[i]]);
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
    std::vector<x_type> const &get_x_internal_order() const { return x_values; }

    /**
     * @brief Get the matrix builder arguments \f$ \mathbf{y} \f$ in the order of the matrix \f$ G^{(n)} \f$.
     * @details See get_x_internal_order() for details.
     * @return `std::vector` containing the arguments \f$ y_j \f$.
     */
    std::vector<y_type> const &get_y_internal_order() const { return y_values; }

    /**
     * @brief Get the callable `FunctionType` object \f$ f \f$ used as the matrix builder.
     * @return Const reference to the stored callable.
     */
    FunctionType const &get_function() const { return f; }

    /**
     * @brief Get the determinant of the original matrix \f$ F^{(n)} \f$.
     * @return Determinant \f$ \det(F^{(n)}) = s^{(n)} \det(G^{(n)}) \f$.
     */
    det_type determinant() {
      if (is_singular()) regenerate();
      return sign * det;
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
    value_type inverse_matrix(int i, int j) const { return mat_inv(col_num[i], row_num[j]); }

    /**
     * @brief Get the full inverse matrix \f$ [F^{(n)}]^{-1} \f$.
     * @details See inverse_matrix(int, int) for details. Warning: this is slow, since it creates a new copy and
     * reorders the rows/columns.
     * @return The inverse matrix \f$ [F^{(n)}]^{-1} \f$.
     */
    matrix_type inverse_matrix() const {
      matrix_type res(N, N);
      for (long i = 0; i < N; i++)
        for (long j = 0; j < N; j++) res(i, j) = inverse_matrix(i, j);
      return res;
    }

    /**
     * @brief Get an element of the matrix \f$ M^{(n)} = [G^{(n)}]^{-1} \f$.
     * @details Advanced: uses the internal storage order, see get_x_internal_order().
     * @param i Row index.
     * @param j Column index.
     * @return The matrix element \f$ M^{(n)}_{ij} \f$.
     */
    value_type inverse_matrix_internal_order(int i, int j) const { return mat_inv(i, j); }

    /**
     * @brief Get the full inverse matrix \f$ M^{(n)} = [G^{(n)}]^{-1} \f$.
     * @details Advanced: uses the internal storage order, see get_x_internal_order().
     * @return The inverse matrix \f$ M^{(n)} \f$.
     */
    nda::matrix_const_view<value_type> inverse_matrix_internal_order() const { return mat_inv(range(N), range(N)); }

    /**
     * @brief Get the original matrix \f$ F^{(n)} \f$.
     * @details Warning: this is slow, since it creates a new matrix and re-evaluates the matrix builder.
     * @return The original matrix \f$ F^{(n)} \f$.
     */
    matrix_type matrix() const {
      matrix_type res(N, N);
      for (long i = 0; i < N; i++)
        for (long j = 0; j < N; j++) res(i, j) = f(get_x(i), get_y(j));
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
      nda::for_each(std::array{d.N, d.N}, [&fn, &d](int i, int j) { return fn(d.x_values[i], d.y_values[j], d.mat_inv(j, i)); });
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
      std::swap(row_num[i], row_num[j]);
      sign = -sign;
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
      std::swap(col_num[i], col_num[j]);
      sign = -sign;
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
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(0 <= i and i <= N);
      TRIQS_ASSERT(0 <= j and j <= N);
      reserve(N + 1);
      last_try = Insert;
      w1.i     = i;
      w1.j     = j;
      w1.x     = x;
      w1.y     = y;

      // treat empty matrix separately
      if (N == 0) {
        newdet  = f(x, y);
        newsign = 1;
        return value_type(newdet);
      }

      // I add the row and col and the end. If the move is rejected,
      // no effect since N will not be changed : Minv(i,j) for i,j>=N has no meaning.
      for (long l = 0; l < N; l++) {
        w1.B(l) = f(x_values[l], y);
        w1.C(l) = f(x, y_values[l]);
      }
      range RN(N);
      //w1.MB(R) = mat_inv(R,R) * w1.B(R);// OPTIMIZE BELOW
      blas::gemv(1.0, mat_inv(RN, RN), w1.B(RN), 0.0, w1.MB(RN));
      w1.ksi  = f(x, y) - nda::blas::dot(w1.C(RN), w1.MB(RN));
      newdet  = det * w1.ksi;
      newsign = ((i + j) % 2 == 0 ? sign : -sign); // since N-i0 + N-j0  = i0+j0 [2]
      return w1.ksi * (newsign * sign);            // sign is unity, hence 1/sign == sign
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
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(0 <= i and i <= N);
      TRIQS_ASSERT(0 <= j and j <= N);
      reserve(N + 1);
      last_try = Insert;
      w1.i     = i;
      w1.j     = j;

      // treat empty matrix separately
      if (N == 0) {
        newdet  = ksi;
        newsign = 1;
        return newdet;
      }

      // I add the row and col and the end. If the move is rejected,
      // no effect since N will not be changed : Minv(i,j) for i,j>=N has no meaning.
      for (long l = 0; l < N; l++) {
        w1.B(l) = fx(x_values[l]);
        w1.C(l) = fy(y_values[l]);
      }
      range RN(N);
      //w1.MB(R) = mat_inv(R,R) * w1.B(R);// OPTIMIZE BELOW
      blas::gemv(1.0, mat_inv(RN, RN), w1.B(RN), 0.0, w1.MB(RN));
      w1.ksi  = ksi - nda::blas::dot(w1.C(RN), w1.MB(RN));
      newdet  = det * w1.ksi;
      newsign = ((i + j) % 2 == 0 ? sign : -sign); // since N-i0 + N-j0  = i0+j0 [2]
      return w1.ksi * (newsign * sign);            // sign is unity, hence 1/sign == sign
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the insert operation.
    void complete_insert() {
      // store the new value of x,y. They are seen through the same permutations as rows and cols resp.
      x_values.push_back(w1.x);
      y_values.push_back(w1.y);
      row_num.push_back(0);
      col_num.push_back(0);

      // special empty case again
      if (N == 0) {
        N             = 1;
        mat_inv(0, 0) = 1 / value_type(newdet);
        return;
      }

      range RN(N);
      //w1.MC(R1) = transpose(mat_inv(R1,R1)) * w1.C(R1); //OPTIMIZE BELOW
      blas::gemv(1.0, transpose(mat_inv(RN, RN)), w1.C(RN), 0.0, w1.MC(RN));
      w1.MC(N) = -1;
      w1.MB(N) = -1;

      N++;
      RN = range(N);

      // keep the real position of the row/col
      // since we insert a col/row, we have first to push the col at the right
      // and then say that col w1.i is stored in N, the last col.
      // same for rows
      for (long i = N - 2; i >= w1.i; i--) row_num[i + 1] = row_num[i];
      row_num[w1.i] = N - 1;
      for (long i = N - 2; i >= w1.j; i--) col_num[i + 1] = col_num[i];
      col_num[w1.j] = N - 1;

      // Minv is ok, we need to complete
      w1.ksi = 1 / w1.ksi;

      // compute the change to the inverse
      // M += w1.ksi w1.MB w1.MC with BLAS. first put the 0
      mat_inv(RN, N - 1) = 0;
      mat_inv(N - 1, RN) = 0;
      //mat_inv(R,R) += w1.ksi* w1.MB(R) * w1.MC(R)// OPTIMIZE BELOW
      blas::ger(w1.ksi, w1.MB(RN), w1.MC(RN), mat_inv(RN, RN));
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
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(i.size() == j.size());
      TRIQS_ASSERT(j.size() == x.size());
      TRIQS_ASSERT(x.size() == y.size());

      k_tried = static_cast<long>(i.size());
      reserve(N + k_tried, k_tried);
      last_try = InsertK;

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
        wk.i[l] = i[idx[l]];
        wk.x[l] = x[idx[l]];
        wk.j[l] = j[idy[l]];
        wk.y[l] = y[idy[l]];
      };

      // check consistency
      for (int l = 0; l < k_tried - 1; ++l) {
        TRIQS_ASSERT(wk.i[l] != wk.i[l + 1] and 0 <= wk.i[l] and wk.i[l] < N + k_tried);
        TRIQS_ASSERT(wk.j[l] != wk.j[l + 1] and 0 <= wk.j[l] and wk.j[l] < N + k_tried);
      }

      // w1.ksi = Delta(x_values,y_values) - Cw.MB using BLAS
      for (long m = 0; m < k_tried; ++m) {
        for (long n = 0; n < k_tried; ++n) { wk.ksi(m, n) = f(wk.x[m], wk.y[n]); }
      }

      // treat empty matrix separately
      if (N == 0) {
        newdet  = wk.det_ksi(k_tried);
        newsign = 1;
        return value_type(newdet);
      }

      // I add the rows and cols and the end. If the move is rejected,
      // no effect since N will not be changed : inv_mat(i,j) for i,j>=N has no meaning.
      for (long n = 0; n < N; n++) {
        for (long l = 0; l < k_tried; ++l) {
          wk.B(n, l) = f(x_values[n], wk.y[l]);
          wk.C(l, n) = f(wk.x[l], y_values[n]);
        }
      }
      range RN(N), Rk(k_tried);
      //wk.MB(RN,Rk) = mat_inv(RN,N) * wk.B(RN,Rk); // OPTIMIZE BELOW
      blas::gemm(1.0, mat_inv(RN, RN), wk.B(RN, Rk), 0.0, wk.MB(RN, Rk));
      //ksi -= wk.C (Rk, RN) * wk.MB(RN, Rk); // OPTIMIZE BELOW
      blas::gemm(-1.0, wk.C(Rk, RN), wk.MB(RN, Rk), 1.0, wk.ksi(Rk, Rk));
      auto ksi     = wk.det_ksi(k_tried);
      newdet       = det * ksi;
      long idx_sum = 0;
      for (long l = 0; l < k_tried; ++l) { idx_sum += wk.i[l] + wk.j[l]; }
      newsign = (idx_sum % 2 == 0 ? sign : -sign); // since N-i0 + N-j0 + N + 1 -i1 + N+1 -j1 = i0+j0 [2]
      return ksi * (newsign * sign);               // sign is unity, hence 1/sign == sign
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
      TRIQS_ASSERT(0 <= i and i <= N);
      TRIQS_ASSERT(0 <= j and j <= N);

      long nbatch         = xs.size();
      value_type sign_fac = ((i + j) % 2 == 0 ? 1 : -1);
      nda::array<value_type, R> result(xs.shape());

      if (nbatch == 0) return result;

      // Flatten inputs for BLAS
      auto xs_flat = flatten_array(xs);
      auto ys_flat = flatten_array(ys);

      if (N == 0) {
        for (long m = 0; m < nbatch; ++m) result.data()[m] = sign_fac * f(xs_flat[m], ys_flat[m]);
        return result;
      }

      range RN(N);

      // Build B(N, nbatch) and C(nbatch, N) matrices
      nda::matrix<value_type> B(N, nbatch), C(nbatch, N), MB(N, nbatch);
      for (long l = 0; l < N; ++l)
        for (long m = 0; m < nbatch; ++m) B(l, m) = f(x_values[l], ys_flat[m]);
      for (long m = 0; m < nbatch; ++m)
        for (long l = 0; l < N; ++l) C(m, l) = f(xs_flat[m], y_values[l]);

      // MB = mat_inv * B -- single BLAS3 gemm
      blas::gemm(1.0, mat_inv(RN, RN), B, 0.0, MB);

      // Compute each ratio: ksi_m = f(xs[m], ys[m]) - C[m,:] . MB[:,m]
      for (long m = 0; m < nbatch; ++m) {
        value_type dot = 0;
        for (long l = 0; l < N; ++l) dot += C(m, l) * MB(l, m);
        result.data()[m] = sign_fac * (f(xs_flat[m], ys_flat[m]) - dot);
      }

      return result;
    }

    /// Compute independent rank-2 insertion det-ratios at positions (i0, i1, j0, j1).
    /// Paired args must have equal rank: get_rank<X0> == get_rank<Y0>, get_rank<X1> == get_rank<Y1>.
    /// Paired args must have equal shape: x0s.shape() == y0s.shape(), x1s.shape() == y1s.shape().
    /// When ranks differ between pairs, lower-rank args are broadcast along extra leading dims.
    /// Result rank = max(get_rank<X0>, get_rank<X1>). Read-only: does not modify internal state.
    template <nda::Array X0, nda::Array X1, nda::Array Y0, nda::Array Y1>
      requires(nda::get_rank<X0> == nda::get_rank<Y0>) && (nda::get_rank<X1> == nda::get_rank<Y1>)
    auto insert2_ratios(long i0, long i1, long j0, long j1, X0 const &x0s, X1 const &x1s, Y0 const &y0s, Y1 const &y1s) const
        -> nda::array<value_type, std::max(nda::get_rank<X0>, nda::get_rank<X1>)> {
      constexpr int R0   = nda::get_rank<X0>;
      constexpr int R1   = nda::get_rank<X1>;
      constexpr int Rmax = std::max(R0, R1);

      TRIQS_ASSERT(x0s.shape() == y0s.shape());
      TRIQS_ASSERT(x1s.shape() == y1s.shape());
      TRIQS_ASSERT(0 <= i0 and i0 <= N + 1);
      TRIQS_ASSERT(0 <= i1 and i1 <= N + 1);
      TRIQS_ASSERT(0 <= j0 and j0 <= N + 1);
      TRIQS_ASSERT(0 <= j1 and j1 <= N + 1);
      TRIQS_ASSERT(i0 != i1);
      TRIQS_ASSERT(j0 != j1);

      // sign_fac from positions. Additional (-1)^(swap_x + swap_y) corrects for
      // computing ksi in unsorted (original pair) order rather than sorted order.
      long idx_sum     = i0 + i1 + j0 + j1;
      bool swap_x      = (i0 > i1);
      bool swap_y      = (j0 > j1);
      int sort_sign    = ((int(swap_x) + int(swap_y)) % 2 == 0) ? 1 : -1;
      value_type sign_fac = (idx_sum % 2 == 0 ? sort_sign : -sort_sign);

      if constexpr (R0 == R1) {
        // Same rank: all arrays have the same size, no broadcasting
        long nbatch = x0s.size();
        TRIQS_ASSERT(x0s.size() == x1s.size());

        // Flatten inputs -- keep original pair order (no sorting)
        auto fx0 = flatten_array(x0s);
        auto fx1 = flatten_array(x1s);
        auto fy0 = flatten_array(y0s);
        auto fy1 = flatten_array(y1s);

        nda::array<value_type, Rmax> result(x0s.shape());

        // Empty batch: nothing to compute and a zero-size gemm below would be invalid.
        if (nbatch == 0) return result;

        if (N == 0) {
          for (long m = 0; m < nbatch; ++m)
            result.data()[m] = sign_fac * (f(fx0[m], fy0[m]) * f(fx1[m], fy1[m]) - f(fx0[m], fy1[m]) * f(fx1[m], fy0[m]));
          return result;
        }

        range RN(N);
        // B0 from y0s (pair 0), B1 from y1s (pair 1) -- unsorted
        nda::matrix<value_type> B0(N, nbatch), B1(N, nbatch), C0(nbatch, N), C1(nbatch, N), MB0(N, nbatch), MB1(N, nbatch);
        for (long l = 0; l < N; ++l)
          for (long m = 0; m < nbatch; ++m) {
            B0(l, m) = f(x_values[l], fy0[m]);
            B1(l, m) = f(x_values[l], fy1[m]);
          }
        for (long m = 0; m < nbatch; ++m)
          for (long l = 0; l < N; ++l) {
            C0(m, l) = f(fx0[m], y_values[l]);
            C1(m, l) = f(fx1[m], y_values[l]);
          }

        blas::gemm(1.0, mat_inv(RN, RN), B0, 0.0, MB0);
        blas::gemm(1.0, mat_inv(RN, RN), B1, 0.0, MB1);

        for (long m = 0; m < nbatch; ++m) {
          value_type dot00 = 0, dot01 = 0, dot10 = 0, dot11 = 0;
          for (long l = 0; l < N; ++l) {
            dot00 += C0(m, l) * MB0(l, m);
            dot01 += C0(m, l) * MB1(l, m);
            dot10 += C1(m, l) * MB0(l, m);
            dot11 += C1(m, l) * MB1(l, m);
          }
          auto ksi00 = f(fx0[m], fy0[m]) - dot00;
          auto ksi01 = f(fx0[m], fy1[m]) - dot01;
          auto ksi10 = f(fx1[m], fy0[m]) - dot10;
          auto ksi11 = f(fx1[m], fy1[m]) - dot11;
          result.data()[m] = sign_fac * (ksi00 * ksi11 - ksi01 * ksi10);
        }
        return result;
      } else if constexpr (R0 > R1) {
        // Broadcast: pair 0 (x0s, y0s) has shape (M_dims..., common_dims...), pair 1 has shape (common_dims...)
        auto shape0 = x0s.shape();
        auto shape1 = x1s.shape();
        for (int d = 0; d < R1; ++d)
          TRIQS_ASSERT(shape0[R0 - R1 + d] == shape1[d]);

        // Empty batch: avoid the division by Nc below and the zero-size gemm.
        if (x0s.size() == 0) return nda::array<value_type, R0>(shape0);

        long Nc = x1s.size();
        long M  = x0s.size() / Nc;
        TRIQS_ASSERT(x0s.size() % Nc == 0);

        // Flatten inputs
        auto fx0 = flatten_array(x0s);
        auto fx1 = flatten_array(x1s);
        auto fy0 = flatten_array(y0s);
        auto fy1 = flatten_array(y1s);

        nda::array<value_type, R0> result(shape0);

        if (N == 0) {
          for (long m = 0; m < M; ++m)
            for (long n = 0; n < Nc; ++n) {
              long mn = m * Nc + n;
              result.data()[mn] =
                  sign_fac * (f(fx0[mn], fy0[mn]) * f(fx1[n], fy1[n]) - f(fx0[mn], fy1[n]) * f(fx1[n], fy0[mn]));
            }
          return result;
        }

        range RN(N);

        // Pair 0 (full): B0(N, M*Nc), C0(M*Nc, N)
        long nbatch0 = M * Nc;
        nda::matrix<value_type> B0(N, nbatch0), C0(nbatch0, N), MB0(N, nbatch0);
        for (long l = 0; l < N; ++l)
          for (long m = 0; m < nbatch0; ++m) B0(l, m) = f(x_values[l], fy0[m]);
        for (long m = 0; m < nbatch0; ++m)
          for (long l = 0; l < N; ++l) C0(m, l) = f(fx0[m], y_values[l]);

        // Pair 1 (broadcast): B1(N, Nc), C1(Nc, N)
        nda::matrix<value_type> B1(N, Nc), C1(Nc, N), MB1(N, Nc);
        for (long l = 0; l < N; ++l)
          for (long n = 0; n < Nc; ++n) B1(l, n) = f(x_values[l], fy1[n]);
        for (long n = 0; n < Nc; ++n)
          for (long l = 0; l < N; ++l) C1(n, l) = f(fx1[n], y_values[l]);

        blas::gemm(1.0, mat_inv(RN, RN), B0, 0.0, MB0);
        blas::gemm(1.0, mat_inv(RN, RN), B1, 0.0, MB1);

        // Pre-compute ksi11 for each n (independent of m)
        std::vector<value_type> ksi11(Nc);
        for (long n = 0; n < Nc; ++n) {
          value_type dot11 = 0;
          for (long l = 0; l < N; ++l) dot11 += C1(n, l) * MB1(l, n);
          ksi11[n] = f(fx1[n], fy1[n]) - dot11;
        }

        for (long m = 0; m < M; ++m)
          for (long n = 0; n < Nc; ++n) {
            long mn = m * Nc + n;
            value_type dot00 = 0, dot01 = 0, dot10 = 0;
            for (long l = 0; l < N; ++l) {
              dot00 += C0(mn, l) * MB0(l, mn);
              dot01 += C0(mn, l) * MB1(l, n);
              dot10 += C1(n, l) * MB0(l, mn);
            }
            auto ksi00 = f(fx0[mn], fy0[mn]) - dot00;
            auto ksi01 = f(fx0[mn], fy1[n]) - dot01;
            auto ksi10 = f(fx1[n], fy0[mn]) - dot10;
            result.data()[mn] = sign_fac * (ksi00 * ksi11[n] - ksi01 * ksi10);
          }

        return result;
      } else {
        // R0 < R1: swap pairs and indices, recurse into R0 > R1 branch.
        // Swapping pairs (0↔1) AND indices (i0↔i1, j0↔j1) is equivalent to
        // relabeling which pair is 0 and which is 1, giving the same det-ratio.
        return insert2_ratios(i1, i0, j1, j0, x1s, x0s, y1s, y0s);
      }
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the insert_k operation.
    void complete_insert_k() {

      // store the new value of x,y. They are seen through the same permutations as rows and cols resp.
      for (int l = 0; l < k_tried; ++l) {
        x_values.push_back(wk.x[l]);
        y_values.push_back(wk.y[l]);
        row_num.push_back(0);
        col_num.push_back(0);
      }

      range Rk(0, k_tried);
      // treat empty matrix separately
      if (N == 0) {
        N               = k_tried;
        mat_inv(Rk, Rk) = nda::linalg::inv(wk.ksi(Rk, Rk));
        for (long l = 0; l < k_tried; ++l) {
          row_num[wk.i[l]] = l;
          col_num[wk.j[l]] = l;
        }
        return;
      }

      range RN(N);
      //wk.MC(Rk,RN) = wk.C(Rk,RN) * mat_inv(RN,RN);// OPTIMIZE BELOW
      blas::gemm(1.0, wk.C(Rk, RN), mat_inv(RN, RN), 0.0, wk.MC(Rk, RN));
      wk.MC(Rk, range(N, N + k_tried)) = -1; // -identity matrix
      wk.MB(range(N, N + k_tried), Rk) = -1; // -identity matrix !

      // keep the real position of the row/col
      // since we insert a col/row, we have first to push the col at the right
      // and then say that col wk.i[0] is stored in N, the last col.
      // same for rows
      for (int l = 0; l < k_tried; ++l) {
        N++;
        for (long i = N - 2; i >= wk.i[l]; i--) row_num[i + 1] = row_num[i];
        row_num[wk.i[l]] = N - 1;
        for (long i = N - 2; i >= wk.j[l]; i--) col_num[i + 1] = col_num[i];
        col_num[wk.j[l]] = N - 1;
      }
      RN = range(N);

      wk.ksi(Rk, Rk)                     = nda::linalg::inv(wk.ksi(Rk, Rk));
      mat_inv(RN, range(N - k_tried, N)) = 0;
      mat_inv(range(N - k_tried, N), RN) = 0;
      //mat_inv(RN,RN) += wk.MB(RN,Rk) * (wk.ksi(Rk, Rk) * wk.MC(Rk,RN)); // OPTIMIZE BELOW
      blas::gemm(1.0, wk.MB(RN, Rk), (wk.ksi(Rk, Rk) * wk.MC(Rk, RN)), 1.0, mat_inv(RN, RN));
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
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(0 <= i and i < N);
      TRIQS_ASSERT(0 <= j and j < N);
      w1.i     = i;
      w1.j     = j;
      last_try = Remove;
      w1.jreal = col_num[w1.j];
      w1.ireal = row_num[w1.i];
      // compute the newdet
      // first we resolve the w1.ireal,w1.jreal, with the permutation of the Minv, then we pick up what
      // will become the 'corner' coefficient, if the move is accepted, after the exchange of row and col.
      w1.ksi   = mat_inv(w1.jreal, w1.ireal);
      auto ksi = w1.ksi;
      newdet   = det * ksi;
      newsign  = ((i + j) % 2 == 0 ? sign : -sign);
      return ksi * (newsign * sign); // sign is unity, hence 1/sign == sign
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the remove operation.
    void complete_remove() {
      if (N == 1) {
        clear();
        return;
      }

      // Move rows and cols to be removed to the end.
      // Adjust the x_values and y_values vector accordingly and
      // swap the associated row_num and col_num elements
      // Remember that for M row/col is interchanged by inversion, transposition.
      range RN(N);
      if (w1.ireal != N - 1) {
        deep_swap(mat_inv(RN, w1.ireal), mat_inv(RN, N - 1));
        x_values[w1.ireal] = x_values[N - 1];
        auto iitr          = std::ranges::find(row_num, w1.ireal);
        auto titr          = std::ranges::find(row_num, N - 1);
        std::swap(*iitr, *titr);
      }
      if (w1.jreal != N - 1) {
        deep_swap(mat_inv(w1.jreal, RN), mat_inv(N - 1, RN));
        y_values[w1.jreal] = y_values[N - 1];
        auto jitr          = std::ranges::find(col_num, w1.jreal);
        auto titr          = std::ranges::find(col_num, N - 1);
        std::swap(*jitr, *titr);
      }
      N--;
      RN = range(N);

      auto it1 [[maybe_unused]] = std::ranges::remove(row_num, N);
      auto it2 [[maybe_unused]] = std::ranges::remove(col_num, N);

      row_num.pop_back();
      col_num.pop_back();
      x_values.pop_back();
      y_values.pop_back();

      // M <- a - d^-1 b c with BLAS
      w1.ksi = -1 / mat_inv(N, N);
      ASSERT(std::isfinite(std::abs(w1.ksi)));

      //mat_inv(RN,RN) += w1.ksi, * mat_inv(RN,N) * mat_inv(N,RN);
      blas::ger(w1.ksi, mat_inv(RN, N), mat_inv(N, RN), mat_inv(RN, RN));
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

      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(N >= 2);
      TRIQS_ASSERT(i.size() == j.size());

      k_tried = static_cast<long>(i.size());
      reserve(N - k_tried, k_tried);
      last_try = RemoveK;

      // check inputs
      for (int l = 0; l < k_tried - 1; ++l) {
        TRIQS_ASSERT(i[l] != i[l + 1] and 0 <= i[l] and i[l] < N);
        TRIQS_ASSERT(j[l] != j[l + 1] and 0 <= j[l] and j[l] < N);
      }

      for (long l = 0; l < k_tried; ++l) {
        wk.i[l]     = i[l];
        wk.j[l]     = j[l];
        wk.ireal[l] = row_num[wk.i[l]];
        wk.jreal[l] = col_num[wk.j[l]];
      }

      // compute the newdet
      for (long l1 = 0; l1 < k_tried; ++l1) {
        for (long l2 = 0; l2 < k_tried; ++l2) { wk.ksi(l1, l2) = mat_inv(wk.jreal[l1], wk.ireal[l2]); }
      }
      auto det_ksi = wk.det_ksi(k_tried);
      newdet       = det * det_ksi;
      long idx_sum = 0;
      for (long l = 0; l < k_tried; ++l) { idx_sum += wk.i[l] + wk.j[l]; }
      newsign = (idx_sum % 2 == 0 ? sign : -sign);

      return det_ksi * (newsign * sign); // sign is unity, hence 1/sign == sign
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
      if (N == k_tried) {
        clear();
        return;
      } // put the sign to 1 also .... Change complete_remove...

      std::vector<long> ireal = wk.ireal;
      std::vector<long> jreal = wk.jreal;
      std::sort(ireal.begin(), ireal.begin() + k_tried);
      std::sort(jreal.begin(), jreal.begin() + k_tried);

      // Move rows and cols to be removed to the end, starting from the right.
      // Adjust the x_values and y_values vector accordingly and
      // swap the associated row_num and col_num elements
      // Remember that for M row/col is interchanged by inversion, transposition.
      range RN(N);
      for (long m = k_tried - 1, target = N - 1; m >= 0; --m, --target) {
        if (ireal[m] != target) {
          deep_swap(mat_inv(RN, ireal[m]), mat_inv(RN, target));
          x_values[ireal[m]] = x_values[target];
          auto iitr          = std::ranges::find(row_num, ireal[m]);
          auto titr          = std::ranges::find(row_num, target);
          std::swap(*iitr, *titr);
        }
        if (jreal[m] != target) {
          deep_swap(mat_inv(jreal[m], RN), mat_inv(target, RN));
          y_values[jreal[m]] = y_values[target];
          auto jitr          = std::ranges::find(col_num, jreal[m]);
          auto titr          = std::ranges::find(col_num, target);
          std::swap(*jitr, *titr);
        }
      }
      N -= k_tried;
      RN = range(N);

      // Clean up removed elements from row_num and col_num
      auto gtN = [&](auto i) { return i >= N; };

      auto it1 [[maybe_unused]] = std::remove_if(row_num.begin(), row_num.end(), gtN);
      auto it2 [[maybe_unused]] = std::remove_if(col_num.begin(), col_num.end(), gtN);

      row_num.resize(N);
      col_num.resize(N);
      x_values.resize(N);
      y_values.resize(N);

      // M <- a - d^-1 b c with BLAS
      range Rl(N, N + k_tried), Rk(k_tried);
      wk.ksi(Rk, Rk) = nda::linalg::inv(mat_inv(Rl, Rl));

      // write explicitely the second product on ksi for speed ?
      //mat_inv(RN,RN) -= mat_inv(RN,Rl) * (wk.ksi * mat_inv(Rl,RN)); // OPTIMIZE BELOW
      blas::gemm(-1.0, mat_inv(RN, Rl), wk.ksi(Rk, Rk) * mat_inv(Rl, RN), 1.0, mat_inv(RN, RN));
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
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(0 <= j and j < N);
      w1.j     = j;
      last_try = ChangeCol;
      w1.jreal = col_num[j];
      w1.y     = y;

      // Compute the col B.
      for (long i = 0; i < N; i++) w1.MC(i) = f(x_values[i], w1.y) - f(x_values[i], y_values[w1.jreal]);
      range RN(N);
      //w1.MB(R) = mat_inv(R,R) * w1.MC(R);// OPTIMIZE BELOW
      blas::gemv(1.0, mat_inv(RN, RN), w1.MC(RN), 0.0, w1.MB(RN));

      // compute the newdet
      w1.ksi   = (1 + w1.MB(w1.jreal));
      auto ksi = w1.ksi;
      newdet   = det * ksi;
      newsign  = sign;

      return ksi; // newsign/sign is unity
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the change column operation.
    void complete_change_col() {
      range RN(N);
      y_values[w1.jreal] = w1.y;

      // modifying M : Mij += w1.ksi Bi Mnj
      // using Shermann Morrison formula.
      // implemented in 2 times : first Bn=0 so that Mnj is not modified ! and then change Mnj
      // Cf notes : simply multiply by -w1.ksi
      w1.ksi          = -1 / w1.ksi;
      w1.MB(w1.jreal) = 0;
      //mat_inv(R,R) += w1.ksi * w1.MB(R) * mat_inv(w1.jreal,R)); // OPTIMIZE BELOW
      blas::ger(w1.ksi, w1.MB(RN), mat_inv(w1.jreal, RN), mat_inv(RN, RN));
      mat_inv(w1.jreal, RN) *= -w1.ksi;
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
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(i < N);
      w1.i     = i;
      last_try = ChangeRow;
      w1.ireal = row_num[i];
      w1.x     = x;

      // Compute the col B.
      for (long idx = 0; idx < N; idx++) w1.MB(idx) = f(w1.x, y_values[idx]) - f(x_values[w1.ireal], y_values[idx]);
      range RN(N);
      //w1.MC(R) = transpose(mat_inv(R,R)) * w1.MB(R); // OPTIMIZE BELOW
      blas::gemv(1.0, transpose(mat_inv(RN, RN)), w1.MB(RN), 0.0, w1.MC(RN));

      // compute the newdet
      w1.ksi   = (1 + w1.MC(w1.ireal));
      auto ksi = w1.ksi;
      newdet   = det * ksi;
      newsign  = sign;
      return ksi; // newsign/sign is unity
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the change row operation.
    void complete_change_row() {
      range RN(N);
      x_values[w1.ireal] = w1.x;

      // modifying M : M ij += w1.ksi Min Cj
      // using Shermann Morrison formula.
      // impl. Cf case 3
      w1.ksi          = -1 / w1.ksi;
      w1.MC(w1.ireal) = 0;
      //mat_inv(R,R) += w1.ksi * mat_inv(R,w1.ireal) * w1.MC(R);
      blas::ger(w1.ksi, mat_inv(RN, w1.ireal), w1.MC(RN), mat_inv(RN, RN));
      mat_inv(RN, w1.ireal) *= -w1.ksi;
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
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(0 <= i and i < N);
      TRIQS_ASSERT(0 <= j and j < N);

      last_try = ChangeRowCol;
      w1.i     = i;
      w1.j     = j;
      w1.ireal = row_num[i];
      w1.jreal = col_num[j];
      w1.x     = x;
      w1.y     = y;

      // Compute the col B.
      for (long idx = 0; idx < N; idx++) { // MC :  delta_x, MB : delta_y
        w1.MC(idx) = f(x_values[idx], y) - f(x_values[idx], y_values[w1.jreal]);
        w1.MB(idx) = f(x, y_values[idx]) - f(x_values[w1.ireal], y_values[idx]);
      }
      w1.MC(w1.ireal) = f(x, y) - f(x_values[w1.ireal], y_values[w1.jreal]);
      w1.MB(w1.jreal) = 0;

      range RN(N);
      // C : X, B : Y
      //w1.C(R) = mat_inv(R,R) * w1.MC(R);// OPTIMIZE BELOW
      blas::gemv(1.0, mat_inv(RN, RN), w1.MC(RN), 0.0, w1.C(RN));
      //w1.B(R) = transpose(mat_inv(R,R)) * w1.MB(R); // OPTIMIZE BELOW
      blas::gemv(1.0, transpose(mat_inv(RN, RN)), w1.MB(RN), 0.0, w1.B(RN));

      // compute the det_ratio
      auto Xn        = w1.C(w1.jreal);
      auto Yn        = w1.B(w1.ireal);
      auto Z         = nda::blas::dot(w1.MB(RN), w1.C(RN));
      auto Mnn       = mat_inv(w1.jreal, w1.ireal);
      auto det_ratio = (1 + Xn) * (1 + Yn) - Mnn * Z;
      w1.ksi         = det_ratio;
      newdet         = det * det_ratio;
      newsign        = sign;
      return det_ratio; // newsign/sign is unity
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the change row and column operation.
    void complete_change_col_row() {
      range RN(N);
      x_values[w1.ireal] = w1.x;
      y_values[w1.jreal] = w1.y;

      // FIXME : Use blas for this ? Is it better
      auto Xn  = w1.C(w1.jreal);
      auto Yn  = w1.B(w1.ireal);
      auto Mnn = mat_inv(w1.jreal, w1.ireal);

      auto D    = w1.ksi;        // get back
      auto a    = -(1 + Yn) / D; // D in the notes
      auto b    = -(1 + Xn) / D;
      auto Z    = nda::blas::dot(w1.MB(RN), w1.C(RN));
      Z         = Z / D;
      Mnn       = Mnn / D;
      w1.MB(RN) = mat_inv(w1.jreal, RN); // Mnj
      w1.MC(RN) = mat_inv(RN, w1.ireal); // Min

      for (long i = 0; i < N; ++i)
        for (long j = 0; j < N; ++j) {
          auto Xi  = w1.C(i);
          auto Yj  = w1.B(j);
          auto Mnj = w1.MB(j);
          auto Min = w1.MC(i);
          mat_inv(i, j) += a * Xi * Mnj + b * Min * Yj + Mnn * Xi * Yj + Z * Min * Mnj;
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
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(X.size() == Y.size());

      last_try = Refill;

      long s = X.size();
      // treat empty matrix separately
      if (s == 0) {
        w_refill.x_values.clear();
        w_refill.y_values.clear();
        return 1 / (sign * det);
      }

      w_refill.reserve(s);
      w_refill.x_values.clear();
      w_refill.y_values.clear();
      std::copy(X.begin(), X.end(), std::back_inserter(w_refill.x_values));
      std::copy(Y.begin(), Y.end(), std::back_inserter(w_refill.y_values));

      for (long i = 0; i < s; ++i)
        for (long j = 0; j < s; ++j) w_refill.M(i, j) = f(w_refill.x_values[i], w_refill.y_values[j]);
      range R(s);
      newdet  = nda::linalg::det(w_refill.M(R, R));
      newsign = 1;

      return newdet / (sign * det);
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the refill operation.
    void complete_refill() {
      N = w_refill.x_values.size();

      // special empty case again
      if (N == 0) {
        clear();
        newdet  = 1;
        newsign = 1;
        return;
      }

      reserve(N);
      std::swap(x_values, w_refill.x_values);
      std::swap(y_values, w_refill.y_values);

      row_num.resize(N, 0); // Zero Initialization avoids ASAN false positive
      col_num.resize(N, 0);
      std::iota(row_num.begin(), row_num.end(), 0);
      std::iota(col_num.begin(), col_num.end(), 0);

      range RN(N);
      mat_inv(RN, RN) = nda::linalg::inv(w_refill.M(RN, RN));
    }

    //------------------------------------------------------------------------------------------
    private:
    // Regenerate the inverse matrix, determinant and sign from the matrix builder, optionally checking the freshly
    // computed values against the stored ones.
    void _regenerate_with_check(bool do_check, double prec_warning, double prec_error) {
      if (N == 0) {
        det  = 1;
        sign = 1;
        return;
      }

      range RN(N);
      matrix_type res(N, N);
      for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) res(i, j) = f(x_values[i], y_values[j]);
      det = nda::linalg::det(res);

      if (is_singular()) TRIQS_RUNTIME_ERROR << "ERROR in det_manip regenerate: Determinant is singular";
      res = nda::linalg::inv(res);

      if (do_check) { // check that mat_inv is close to res
        const bool relative = true;
        double r            = max_element(abs(res - mat_inv(RN, RN)));
        double r2           = max_element(abs(res + mat_inv(RN, RN)));
        bool err            = !(r < (relative ? prec_error * r2 : prec_error));
        bool war            = !(r < (relative ? prec_warning * r2 : prec_warning));
        if (err || war) {
          std::cerr << "matrix  = " << matrix() << std::endl;
          std::cerr << "inverse_matrix = " << inverse_matrix() << std::endl;
        }
        if (war)
          std::cerr << "Warning : det_manip deviation above warning threshold "
                    << "check "
                    << "N = " << N << "  "
                    << "\n   max(abs(M^-1 - M^-1_true)) = " << r
                    << "\n   precision*max(abs(M^-1 + M^-1_true)) = " << (relative ? prec_warning * r2 : prec_warning) << " " << std::endl;
        if (err) TRIQS_RUNTIME_ERROR << "Error : det_manip deviation above critical threshold !! ";
      }

      // since we have the proper inverse, replace the matrix and the det
      mat_inv(RN, RN) = res;
      n_opts          = 0;

      // find the sign (there must be a better way...)
      double s = 1.0;
      nda::matrix<double> m(N, N);
      m() = 0.0;
      for (int i = 0; i < N; i++) m(i, row_num[i]) = 1;
      s *= nda::linalg::det(m);
      m() = 0.0;
      for (int i = 0; i < N; i++) m(i, col_num[i]) = 1;
      s *= nda::linalg::det(m);
      sign = (s > 0 ? 1 : -1);
    }

    // Regenerate and check the consistency of the stored inverse matrix, determinant and sign.
    void check_mat_inv() { _regenerate_with_check(true, precision_warning, precision_error); }

    // Check whether the determinant is considered singular: (singular_threshold < 0 ? not
    // std::isnormal(std::abs(det)) : (std::abs(det) < singular_threshold)). See set_singular_threshold().
    [[nodiscard]] bool is_singular() const {
      return (singular_threshold < 0 ? not std::isnormal(std::abs(det)) : (std::abs(det) < singular_threshold));
    }

    //------------------------------------------------------------------------------------------
    public:
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
      switch (last_try) {
        case (Insert): complete_insert(); break;
        case (Remove): complete_remove(); break;
        case (ChangeCol): complete_change_col(); break;
        case (ChangeRow): complete_change_row(); break;
        case (ChangeRowCol): complete_change_col_row(); break;
        case (InsertK): complete_insert_k(); break;
        case (RemoveK): complete_remove_k(); break;
        case (Refill): complete_refill(); break;
        case (NoTry): return; break;
        default: TRIQS_RUNTIME_ERROR << "Misuing det_manip"; // Never used?
      }

      det  = newdet;
      sign = newsign;
      ++n_opts;
      if (n_opts > n_opts_max_before_check) check_mat_inv();
      last_try = NoTry;
    }

    /**
     * @brief Reject the last try-operation.
     * @details All `try_*` operations have to be either accepted (via complete_operation()) or rejected.
     */
    void reject_last_try() { last_try = NoTry; }

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
    value_type insert_at_end(x_type const &x, y_type const &y) { return insert(N, N, x, y); }

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
      return insert2(N, N + 1, N, N + 1, x0, x1, y0, y1);
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
    value_type remove_at_end() { return remove(N - 1, N - 1); }

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
    value_type remove2_at_end() { return remove2(N - 1, N - 2, N - 1, N - 2); }

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
      const long NN = N;
      switch (roll) {
        case (None): return 1;
        case (Down):
          tmp = row_num[N - 1];
          for (long i = NN - 2; i >= 0; i--) row_num[i + 1] = row_num[i];
          row_num[0] = tmp;
          break;
        case (Up):
          tmp = row_num[0];
          for (long i = 0; i < N - 1; i++) row_num[i] = row_num[i + 1];
          row_num[N - 1] = tmp;
          break;
        case (Right):
          tmp = col_num[N - 1];
          for (long i = NN - 2; i >= 0; i--) col_num[i + 1] = col_num[i];
          col_num[0] = tmp;
          break;
        case (Left):
          tmp = col_num[0];
          for (long i = 0; i < N - 1; i++) col_num[i] = col_num[i + 1];
          col_num[N - 1] = tmp;
          break;
        default: assert(0);
      }
      // signature of the cycle of order N : (-1)^(N-1)
      if ((N - 1) % 2 == 1) {
        sign *= -1;
        return -1;
      }
      return 1;
    }
  };
} // namespace triqs::det_manip
