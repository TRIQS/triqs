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

/**
 * @file
 * @brief Provides a simple reference implementation for manipulating determinants.
 */

#pragma once

#include "../arrays.hpp"
#include "../utility/callable_traits.hpp"
#include "../utility/first_include.hpp"

#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <vector>

namespace triqs::det_manip {

  namespace blas = nda::blas;

  /**
   * @ingroup triqs-detmanip
   * @brief Simple reference implementation of determinant manipulation for CTQMC solvers.
   *
   * @details This class manipulates the determinant \f$ \det(F^{(n)}) \f$ of a matrix whose elements are given by a
   * callable \f$ F^{(n)}_{ij} = f(x_i, y_j) \f$ (see @ref triqs-detmanip for the general setup).
   *
   * It exposes the same API as triqs::det_manip::det_manip (the same `try`/`complete` operations, accessors and HDF5
   * support), but uses a straightforward implementation: instead of the fast rank-update algorithm, it rebuilds the
   * matrix and recomputes the determinant (and, on demand, the inverse) from scratch on every operation. It does not
   * track row/column permutations or a sign, so the matrix is stored in the user's order, i.e. the internal order
   * equals the user order.
   *
   * This makes it simpler and easier to verify, but asymptotically slower than triqs::det_manip::det_manip; it is
   * primarily useful as a reference/testing implementation.
   *
   * @tparam FunctionType Callable object \f$ f \f$ that takes two arguments and returns a real or complex value. It
   * determines the elements of the matrix via \f$ F^{(n)}_{ij} = f(x_i, y_j) \f$.
   */
  template <typename FunctionType> class det_manip_basic {
    // ---------------------------------------------------------------------------------------------------
    // Type Checks and Definitions
    private:
    using f_tr = utility::callable_traits<FunctionType>;
    static_assert(f_tr::arity == 2, "det_manip_basic : the function must take two arguments !");

    using int_type = std::ptrdiff_t;
    using range    = itertools::range;

    public:
    using x_type     = typename f_tr::template decay_arg_t<0>;
    using y_type     = typename f_tr::template decay_arg_t<1>;
    using value_type = typename f_tr::result_type;
    using det_type   = value_type;
    static_assert(std::is_floating_point_v<value_type> || nda::is_complex_v<value_type>,
                  "det_manip_basic : the function must return a floating number or a complex number");

    using matrix_type = nda::matrix<value_type>;

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
      Insert2 = 10,
      Remove2 = 11,
      InsertK = 12,
      RemoveK = 13,
      Refill  = 20
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

    // Temporary work data

    // Working data for single-row/column operations: arguments x, y and positions i, j of the new/changed row/column.
    struct work_data_type1 {
      x_type x;
      y_type y;
      long i;
      long j;
    } w1;

    // Working data for two-row/column operations: arguments and positions of the two new/changed rows/columns.
    struct work_data_type2 {
      std::array<x_type, 2> x;
      std::array<y_type, 2> y;
      std::array<long, 2> i, j;
    } w2;

    // Working data for k-row/column operations: arguments and positions of the k rows/columns (k = current count).
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

    // Working data for the refill operation: the new matrix builder arguments.
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
    /**
     * @brief Write a triqs::det_manip::det_manip_basic object to HDF5.
     *
     * @param fg `h5::group` containing the subgroup to be written to.
     * @param subgroup_name Name of the subgroup.
     * @param g Manipulator object to be written.
     */
    friend void h5_write(h5::group fg, std::string subgroup_name, det_manip_basic const &g) {
      auto gr = fg.create_group(subgroup_name);
      h5_write(gr, "n_opts", g.n_opts);
      h5_write(gr, "N", g.N);
      h5_write(gr, "mat", g.mat);
      h5_write(gr, "det", g.det);
      h5_write(gr, "x_values", g.x_values);
      h5_write(gr, "y_values", g.y_values);
    }

    /**
     * @brief Read a triqs::det_manip::det_manip_basic object from HDF5.
     *
     * @param fg `h5::group` containing the subgroup to be read from.
     * @param subgroup_name Name of the subgroup.
     * @param g Manipulator object to be read into.
     */
    friend void h5_read(h5::group fg, std::string subgroup_name, det_manip_basic &g) {
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
     * @brief Reserve memory to increase the capacity of the data storages.
     *
     * @details Like for `std::vector`, this reserves memory for a bigger matrix size. It only reserves/resizes if
     * the requested capacity is larger than the current one. It preserves the matrix but not the temporary working
     * data, so it must NOT be called between a `try_*` function and the corresponding complete_operation().
     *
     * @param new_size New capacity for the size of the matrix, i.e. the maximum number of rows and columns.
     * @param new_k Unused, kept for API compatibility with triqs::det_manip::det_manip.
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
     * @brief Construct a det_manip_basic object with a callable `FunctionType` and an initial capacity for the data
     * storages.
     *
     * @details Like for `std::vector`, the capacity grows automatically (by a factor of 2) when needed, but this can
     * yield a performance penalty if it happens too often.
     *
     * @param F Callable `FunctionType` object (a copy is stored in the class).
     * @param init_size Initial capacity for the size of the matrix, i.e. the maximum number of rows and columns.
     */
    det_manip_basic(FunctionType F, long init_size) : f(std::move(F)) {
      reserve(init_size);
      mat() = 0;
    }

    /**
     * @brief Construct a det_manip_basic object with a callable `FunctionType` and two containers holding the
     * arguments for the matrix builder.
     *
     * @tparam ArgumentContainer1 Container type holding the first arguments.
     * @tparam ArgumentContainer2 Container type holding the second arguments.
     * @param F Callable `FunctionType` object (a copy is stored in the class).
     * @param X Container holding the first arguments \f$ \mathbf{x} \f$.
     * @param Y Container holding the second arguments \f$ \mathbf{y} \f$.
     */
    template <typename ArgumentContainer1, typename ArgumentContainer2>
    det_manip_basic(FunctionType F, ArgumentContainer1 const &X, ArgumentContainer2 const &Y) : f(std::move(F)) {
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

    det_manip_basic(det_manip_basic const &)            = default;
    det_manip_basic(det_manip_basic &&rhs) noexcept     = default;
    det_manip_basic &operator=(const det_manip_basic &) = delete;
    // det_manip_basic &operator                           = default;

    /**
     * @brief Clear the data storages and reset the matrix to size zero.
     */
    void clear() {
      N        = 0;
      det      = 1;
      last_try = NoTry;
      x_values.clear();
      y_values.clear();
    }
    //----------------------- Computations ----------------------------------

    /**
     * @brief Build the matrix \f$ F^{(n)} \f$ from scratch from the stored arguments.
     * @details Evaluates \f$ f(x_i, y_j) \f$ for all \f$ i, j \f$ and returns a freshly allocated matrix.
     * @return The matrix \f$ F^{(n)} \f$.
     */
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
    // Rebuild the stored matrix mat from the stored arguments by evaluating f(x_i, y_j) for all i, j.
    void build_matrix() {
      TRIQS_ASSERT(x_values.size() == N)
      TRIQS_ASSERT(y_values.size() == N)
      if (N > Nmax) reserve(N); // TODO: no extra margin

      for (long i = 0; i < N; i++) {
        for (long j = 0; j < N; j++) { mat(i, j) = f(x_values[i], y_values[j]); }
      }
    }

    // Recompute the determinant of the stored matrix from scratch.
    void compute_determinant() {
      range R(0, N);
      det = nda::linalg::det(mat(R, R));
    }

    // Lazily recompute the inverse matrix from scratch (cached in mat_inverse, guarded by mat_inverse_is_valid).
    void compute_inverse() const {
      if (mat_inverse_is_valid) return;
      if (N == 0) {
        mat_inverse_is_valid = true;
        return;
      }
      range R(0, N);
      mat_inverse(R, R)    = nda::linalg::inv(mat(R, R));
      mat_inverse_is_valid = true;
    }

    //----------------------- READ ACCESS TO DATA ----------------------------------
    public:
    /**
     * @brief Get the current size of the matrix.
     * @return Number of rows/columns of the matrix.
     */
    auto size() const { return N; }

    /**
     * @brief Get a vector with all matrix builder arguments \f$ \mathbf{x} \f$.
     * @return `std::vector` containing the arguments \f$ x_i \f$.
     */
    auto get_x() const { return x_values; }

    /**
     * @brief Get a vector with all matrix builder arguments \f$ \mathbf{y} \f$.
     * @return `std::vector` containing the arguments \f$ y_j \f$.
     */
    auto get_y() const { return y_values; }

    /**
     * @brief Get the matrix builder argument \f$ x_i \f$ that determines the elements of the i<sup>th</sup> row.
     * @param i Argument index.
     * @return Argument value \f$ x_i \f$.
     */
    x_type const &get_x(long i) const { return x_values[i]; }

    /**
     * @brief Get the matrix builder argument \f$ y_j \f$ that determines the elements of the j<sup>th</sup> column.
     * @param j Argument index.
     * @return Argument value \f$ y_j \f$.
     */
    y_type const &get_y(long j) const { return y_values[j]; }

    /**
     * @brief Get the matrix builder arguments \f$ \mathbf{x} \f$ in internal storage order.
     * @details For this class the internal order equals the user order (there is no permutation), so this returns
     * the same as get_x().
     * @return `std::vector` containing the arguments \f$ x_i \f$.
     */
    auto const &get_x_internal_order() const { return x_values; }

    /**
     * @brief Get the matrix builder arguments \f$ \mathbf{y} \f$ in internal storage order.
     * @details For this class the internal order equals the user order (there is no permutation), so this returns
     * the same as get_y().
     * @return `std::vector` containing the arguments \f$ y_j \f$.
     */
    auto const &get_y_internal_order() const { return y_values; }

    /**
     * @brief Get the callable `FunctionType` object \f$ f \f$ used as the matrix builder.
     * @return Const reference to the stored callable.
     */
    FunctionType const &get_function() const { return f; }

    /**
     * @brief Get the determinant of the matrix \f$ F^{(n)} \f$.
     * @return Determinant \f$ \det(F^{(n)}) \f$.
     */
    auto determinant() { return det; }

    /**
     * @brief Get the full inverse matrix \f$ [F^{(n)}]^{-1} \f$.
     * @details The inverse is recomputed from scratch on demand and cached until the next operation.
     * @return The inverse matrix \f$ [F^{(n)}]^{-1} \f$.
     */
    nda::matrix_const_view<value_type> inverse_matrix() const {
      compute_inverse();
      range R(0, N);
      return mat_inverse(R, R);
    }

    /**
     * @brief Get an element of the inverse matrix \f$ [F^{(n)}]^{-1} \f$.
     * @param i Row index.
     * @param j Column index.
     * @return The matrix element \f$ [F^{(n)}]^{-1}_{ij} \f$.
     */
    value_type inverse_matrix(int i, int j) const {
      compute_inverse();
      return mat_inverse(i, j);
    }

    /**
     * @brief Get the full inverse matrix in internal storage order.
     * @details For this class the internal order equals the user order, so this returns the same as
     * inverse_matrix().
     * @return The inverse matrix \f$ [F^{(n)}]^{-1} \f$.
     */
    nda::matrix_const_view<value_type> inverse_matrix_internal_order() const { return inverse_matrix(); }

    /**
     * @brief Get an element of the inverse matrix in internal storage order.
     * @details For this class the internal order equals the user order, so this returns the same as
     * inverse_matrix(int, int).
     * @param i Row index.
     * @param j Column index.
     * @return The matrix element \f$ [F^{(n)}]^{-1}_{ij} \f$.
     */
    value_type inverse_matrix_internal_order(int i, int j) const { return inverse_matrix(i, j); }

    /**
     * @brief Get the matrix \f$ F^{(n)} \f$.
     * @return The matrix \f$ F^{(n)} \f$.
     */
    matrix_type matrix() const {
      range R(0, N);
      return mat(R, R);
    }

    /**
     * @brief Get an element of the matrix \f$ F^{(n)} \f$.
     * @param i Row index.
     * @param j Column index.
     * @return The matrix element \f$ F^{(n)}_{ij} \f$.
     */
    value_type matrix(int i, int j) const { return mat(i, j); }

    /**
     * @brief For-each implementation for triqs::det_manip::det_manip_basic objects.
     *
     * @details It loops over all elements of the inverse matrix and calls the given callable object for each
     * element together with the corresponding arguments \f$ x_i \f$ and \f$ y_j \f$. The order of iteration is not
     * fixed; it is optimised for memory traversal.
     *
     * @tparam LambdaType Callable type.
     * @param d triqs::det_manip::det_manip_basic object.
     * @param func Callable object that takes three arguments: \f$ x_i \f$, \f$ y_j \f$, and \f$ [F^{(n)}]^{-1}_{ji} \f$.
     */
    template <typename LambdaType> friend void foreach (det_manip_basic const &d, LambdaType const &func) {
      d.compute_inverse();
      nda::for_each(std::array{long(d.N), long(d.N)}, [&func, &d](int i, int j) { return func(d.x_values[i], d.y_values[j], d.mat_inverse(j, i)); });
    }

    // ------------------------- OPERATIONS -----------------------------------------------

    /**
     * @brief Try to insert one row and column.
     *
     * @details The row is inserted at position \f$ i \f$ and the column at position \f$ j \f$ in the matrix
     * \f$ F^{(n)} \f$, with elements determined by the given matrix builder arguments \f$ x \f$ and \f$ y \f$. The
     * current column \f$ j \f$ (resp. row \f$ i \f$) becomes column \f$ j + 1 \f$ (resp. row \f$ i + 1 \f$);
     * inserting at \f$ n \f$ simply appends at the end (\f$ 0 \leq i, j \leq n \f$).
     *
     * It builds the candidate matrix and recomputes its determinant from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the elements of the new row.
     * @param y Argument to the matrix builder that determines the elements of the new column.
     * @return Determinant ratio \f$ \det(F^{(n+1)}) / \det(F^{(n)}) \f$.
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

      range Row_A(0, i);
      range Row_B_0(i, N);
      range Row_B_1 = Row_B_0 + std::ptrdiff_t{1};

      range Col_A(0, j);
      range Col_B_0(j, N);
      range Col_B_1 = Col_B_0 + std::ptrdiff_t{1};

      mat_new(Row_A, Col_A)     = mat(Row_A, Col_A);
      mat_new(Row_A, Col_B_1)   = mat(Row_A, Col_B_0);
      mat_new(Row_B_1, Col_A)   = mat(Row_B_0, Col_A);
      mat_new(Row_B_1, Col_B_1) = mat(Row_B_0, Col_B_0);

      for (auto k : Row_A) { mat_new(k, j) = f(x_values[k], y); }
      for (auto k : Row_B_0) { mat_new(k + 1, j) = f(x_values[k], y); }

      for (auto k : Col_A) { mat_new(i, k) = f(x, y_values[k]); }
      for (auto k : Col_B_0) { mat_new(i, k + 1) = f(x, y_values[k]); }

      mat_new(i, j) = f(x, y);

      range R(0, N + 1);
      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the insert operation.
    void complete_insert() {
      N++;
      x_values.insert(begin(x_values) + w1.i, w1.x);
      y_values.insert(begin(y_values) + w1.j, w1.y);
      std::swap(mat, mat_new);
    }

    public:
    //------------------------------------------------------------------------------------------

    /**
     * @brief Try to insert two rows and columns.
     *
     * @details The rows are inserted at positions \f$ i_0 \f$ and \f$ i_1 \f$ and the columns at positions
     * \f$ j_0 \f$ and \f$ j_1 \f$ in the matrix \f$ F^{(n)} \f$, with elements determined by the given matrix
     * builder arguments \f$ x_0, x_1, y_0, y_1 \f$ (\f$ 0 \leq i_0,i_1,j_0,j_1 \leq n+1 \f$).
     *
     * It builds the candidate matrix and recomputes its determinant from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i0 Position of the first row to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param i1 Position of the second row to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param j0 Position of the first column to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param j1 Position of the second column to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param x0_ Argument to the matrix builder that determines the elements of the first new row.
     * @param x1_ Argument to the matrix builder that determines the elements of the second new row.
     * @param y0_ Argument to the matrix builder that determines the elements of the first new column.
     * @param y1_ Argument to the matrix builder that determines the elements of the second new column.
     * @return Determinant ratio \f$ \det(F^{(n+2)}) / \det(F^{(n)}) \f$.
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

      range Row_A(0, i0);
      range Row_B_0(i0, i1);
      range Row_B_1 = Row_B_0 + std::ptrdiff_t{1};
      range Row_C_0(i1, N);
      range Row_C_1 = Row_C_0 + std::ptrdiff_t{2};

      range Col_A(0, j0);
      range Col_B_0(j0, j1);
      range Col_B_1 = Col_B_0 + std::ptrdiff_t{1};
      range Col_C_0(j1, N);
      range Col_C_1 = Col_C_0 + std::ptrdiff_t{2};

      mat_new(Row_A, Col_A)   = mat(Row_A, Col_A);
      mat_new(Row_A, Col_B_1) = mat(Row_A, Col_B_0);
      mat_new(Row_A, Col_C_1) = mat(Row_A, Col_C_0);

      mat_new(Row_B_1, Col_A)   = mat(Row_B_0, Col_A);
      mat_new(Row_B_1, Col_B_1) = mat(Row_B_0, Col_B_0);
      mat_new(Row_B_1, Col_C_1) = mat(Row_B_0, Col_C_0);

      mat_new(Row_C_1, Col_A)   = mat(Row_C_0, Col_A);
      mat_new(Row_C_1, Col_B_1) = mat(Row_C_0, Col_B_0);
      mat_new(Row_C_1, Col_C_1) = mat(Row_C_0, Col_C_0);

      // Note: need to shift i1/j1 below by +1 to adjust for first row/column

      for (auto k : Row_A) {
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

      for (auto k : Col_A) {
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
      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the insert2 operation.
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

    // Helper: flatten an nda::Array to a std::vector in C-order
    template <nda::Array A> static auto flatten_array(A const &a) {
      auto v    = std::vector<typename A::value_type>(a.size());
      long flat = 0;
      nda::for_each(a.shape(), [&](auto... idx) { v[flat++] = a(idx...); });
      return v;
    }

    /**
     * @brief Compute independent single-insertion determinant ratios at position \f$ (i, j) \f$ for paired elements
     * of `xs` and `ys`.
     *
     * @details For each pair `(xs[m], ys[m])` it computes the determinant ratio that a single try_insert() at
     * position \f$ (i, j) \f$ would produce, without modifying the internal state. The inputs must be `nda::Array`
     * objects with the same rank and shape, and the result has the same shape.
     *
     * @tparam X `nda::Array` type of the first arguments.
     * @tparam Y `nda::Array` type of the second arguments.
     * @param i Position of the row to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param xs Array of first arguments to the matrix builder.
     * @param ys Array of second arguments to the matrix builder.
     * @return Array of determinant ratios with the same shape as `xs` and `ys`.
     */
    template <nda::Array X, nda::Array Y>
      requires(nda::get_rank<X> == nda::get_rank<Y>)
    auto insert_ratios(long i, long j, X const &xs, Y const &ys) const -> nda::array<value_type, nda::get_rank<X>> {
      constexpr int Rk = nda::get_rank<X>;
      TRIQS_ASSERT(xs.shape() == ys.shape());
      TRIQS_ASSERT(0 <= i and i <= N);
      TRIQS_ASSERT(0 <= j and j <= N);

      long nbatch = xs.size();
      nda::array<value_type, Rk> result(xs.shape());
      auto xs_flat = flatten_array(xs);
      auto ys_flat = flatten_array(ys);

      for (long m = 0; m < nbatch; ++m) result.data()[m] = compute_insert_ratio(i, j, xs_flat[m], ys_flat[m]);
      return result;
    }

    /// Compute independent rank-2 insertion det-ratios at positions (i0, i1, j0, j1).
    /// Paired args must have equal rank and shape. Lower-rank pairs are broadcast.
    /// Read-only: does not modify internal state.
    template <nda::Array X0, nda::Array X1, nda::Array Y0, nda::Array Y1>
      requires(nda::get_rank<X0> == nda::get_rank<Y0>) && (nda::get_rank<X1> == nda::get_rank<Y1>)
    auto insert2_ratios(long i0, long i1, long j0, long j1, X0 const &x0s, X1 const &x1s, Y0 const &y0s, Y1 const &y1s) const
        -> nda::array<value_type, std::max(nda::get_rank<X0>, nda::get_rank<X1>)> {
      constexpr int R0   = nda::get_rank<X0>;
      constexpr int R1   = nda::get_rank<X1>;
      constexpr int Rmax = std::max(R0, R1);

      TRIQS_ASSERT(x0s.shape() == y0s.shape());
      TRIQS_ASSERT(x1s.shape() == y1s.shape());

      if constexpr (R0 == R1) {
        // Same rank: all arrays have the same size
        long nbatch = x0s.size();
        TRIQS_ASSERT(nbatch == x1s.size());
        nda::array<value_type, Rmax> result(x0s.shape());
        auto fx0 = flatten_array(x0s);
        auto fx1 = flatten_array(x1s);
        auto fy0 = flatten_array(y0s);
        auto fy1 = flatten_array(y1s);

        for (long m = 0; m < nbatch; ++m) {
          result.data()[m] = compute_insert2_ratio(i0, i1, j0, j1, fx0[m], fx1[m], fy0[m], fy1[m]);
        }
        return result;
      } else if constexpr (R0 > R1) {
        // Pair 0 is higher rank. Broadcast pair 1 along extra leading dims.
        auto shape0 = x0s.shape();
        // Empty batch: avoid the division by Nc below.
        if (x0s.size() == 0) return nda::array<value_type, R0>(shape0);
        long Nc = x1s.size();
        long M  = x0s.size() / Nc;
        TRIQS_ASSERT(x0s.size() % Nc == 0);

        nda::array<value_type, R0> result(shape0);
        auto fx0 = flatten_array(x0s);
        auto fx1 = flatten_array(x1s);
        auto fy0 = flatten_array(y0s);
        auto fy1 = flatten_array(y1s);

        for (long m = 0; m < M; ++m)
          for (long n = 0; n < Nc; ++n) {
            long mn = m * Nc + n;
            result.data()[mn] = compute_insert2_ratio(i0, i1, j0, j1, fx0[mn], fx1[n], fy0[mn], fy1[n]);
          }
        return result;
      } else {
        // R0 < R1: swap pairs and recurse
        return insert2_ratios(i1, i0, j1, j0, x1s, x0s, y1s, y0s);
      }
    }

    private:
    // Helper: compute a single rank-1 insertion det-ratio by building augmented matrix
    auto compute_insert_ratio(long i, long j, x_type const &x, y_type const &y) const -> value_type {
      matrix_type aug(N + 1, N + 1);
      for (long r = 0; r < N; ++r)
        for (long c = 0; c < N; ++c) aug(r < i ? r : r + 1, c < j ? c : c + 1) = mat(r, c);
      for (long c = 0; c < N; ++c) aug(i, c < j ? c : c + 1) = f(x, y_values[c]);
      for (long r = 0; r < N; ++r) aug(r < i ? r : r + 1, j) = f(x_values[r], y);
      aug(i, j) = f(x, y);
      return nda::linalg::det(aug) / det;
    }

    // Helper: compute a single rank-2 insertion det-ratio by building augmented matrix
    auto compute_insert2_ratio(long i0, long i1, long j0, long j1, x_type const &x0, x_type const &x1, y_type const &y0,
                               y_type const &y1) const -> value_type {
      long ii0 = std::min(i0, i1), ii1 = std::max(i0, i1);
      long jj0 = std::min(j0, j1), jj1 = std::max(j0, j1);
      auto const &xf = (i0 < i1) ? x0 : x1;
      auto const &xs = (i0 < i1) ? x1 : x0;
      auto const &yf = (j0 < j1) ? y0 : y1;
      auto const &ys = (j0 < j1) ? y1 : y0;

      long ii1_adj = ii1 - 1;
      long jj1_adj = jj1 - 1;

      matrix_type aug(N + 2, N + 2);

      auto row_map = [&](long r) -> long {
        if (r < ii0) return r;
        if (r < ii1_adj) return r + 1;
        return r + 2;
      };
      auto col_map = [&](long c) -> long {
        if (c < jj0) return c;
        if (c < jj1_adj) return c + 1;
        return c + 2;
      };

      for (long r = 0; r < N; ++r)
        for (long c = 0; c < N; ++c) aug(row_map(r), col_map(c)) = mat(r, c);
      for (long c = 0; c < N; ++c) {
        aug(ii0, col_map(c))         = f(xf, y_values[c]);
        aug(ii1_adj + 1, col_map(c)) = f(xs, y_values[c]);
      }
      for (long r = 0; r < N; ++r) {
        aug(row_map(r), jj0)         = f(x_values[r], yf);
        aug(row_map(r), jj1_adj + 1) = f(x_values[r], ys);
      }
      aug(ii0, jj0)                 = f(xf, yf);
      aug(ii0, jj1_adj + 1)         = f(xf, ys);
      aug(ii1_adj + 1, jj0)         = f(xs, yf);
      aug(ii1_adj + 1, jj1_adj + 1) = f(xs, ys);

      return nda::linalg::det(aug) / det;
    }

    public:
    /**
     * @brief Try to insert \f$ k \f$ rows and columns.
     *
     * @details The new rows and columns are inserted at the positions given by the tuples \f$ \mathbf{i} \f$ and
     * \f$ \mathbf{j} \f$, with elements determined by the matrix builder arguments \f$ \mathbf{x} \f$ and
     * \f$ \mathbf{y} \f$ (each position \f$ i_l, j_l \f$ valid up to \f$ n + l \f$).
     *
     * It builds the candidate matrix and recomputes its determinant from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Positions of the rows to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param j Positions of the columns to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param x Arguments to the matrix builder that determine the elements of the new rows.
     * @param y Arguments to the matrix builder that determine the elements of the new columns.
     * @return Determinant ratio \f$ \det(F^{(n+k)}) / \det(F^{(n)}) \f$.
     */
    value_type try_insert_k(std::vector<long> i, std::vector<long> j, std::vector<x_type> x, std::vector<y_type> y) {
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(i.size() == j.size());
      TRIQS_ASSERT(j.size() == x.size());
      TRIQS_ASSERT(x.size() == y.size());

      long k = static_cast<long>(i.size());
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
        long offset     = 0;
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
        long offset     = 0;
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
        for (long old_j = 0; old_j < N; ++old_j) { mat_new(row_map[old_i], col_map[old_j]) = mat(old_i, old_j); }
      }

      // Fill in new rows
      for (long l = 0; l < k; ++l) {
        long new_row = wk.i[l];
        // Elements from existing columns
        for (long old_j = 0; old_j < N; ++old_j) { mat_new(new_row, col_map[old_j]) = f(wk.x[l], y_values[old_j]); }
        // Elements from new columns
        for (long m = 0; m < k; ++m) { mat_new(new_row, wk.j[m]) = f(wk.x[l], wk.y[m]); }
      }

      // Fill in new columns (for existing rows)
      for (long l = 0; l < k; ++l) {
        long new_col = wk.j[l];
        for (long old_i = 0; old_i < N; ++old_i) { mat_new(row_map[old_i], new_col) = f(x_values[old_i], wk.y[l]); }
      }

      range R(0, N + k);
      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the insert_k operation.
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
     * @brief Try to remove one row and column.
     *
     * @details The row at position \f$ i \f$ and the column at position \f$ j \f$ are removed from the matrix
     * \f$ F^{(n)} \f$. It builds the candidate matrix and recomputes its determinant from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be removed in the matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be removed in the matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ \det(F^{(n-1)}) / \det(F^{(n)}) \f$.
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
      if (N == 1) {
        det_new = 1.0;
        return det_new / det;
      }

      range Row_A(0, i);
      range Row_B_0(i + 1, N);
      range Row_B_1 = Row_B_0 + std::ptrdiff_t{-1};

      range Col_A(0, j);
      range Col_B_0(j + 1, N);
      range Col_B_1 = Col_B_0 + std::ptrdiff_t{-1};

      mat_new(Row_A, Col_A)     = mat(Row_A, Col_A);
      mat_new(Row_A, Col_B_1)   = mat(Row_A, Col_B_0);
      mat_new(Row_B_1, Col_A)   = mat(Row_B_0, Col_A);
      mat_new(Row_B_1, Col_B_1) = mat(Row_B_0, Col_B_0);

      range R(0, N - 1);
      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the remove operation.
    void complete_remove() {
      N--;
      x_values.erase(begin(x_values) + w1.i);
      y_values.erase(begin(y_values) + w1.j);
      std::swap(mat, mat_new);
    }

    public:
    //------------------------------------------------------------------------------------------

    /**
     * @brief Try to remove two rows and two columns.
     *
     * @details The rows at positions \f$ i_0, i_1 \f$ and the columns at positions \f$ j_0, j_1 \f$ are removed from
     * the matrix \f$ F^{(n)} \f$. It builds the candidate matrix and recomputes its determinant from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i0 Position of the first row to be removed in the matrix \f$ F^{(n)} \f$.
     * @param i1 Position of the second row to be removed in the matrix \f$ F^{(n)} \f$.
     * @param j0 Position of the first column to be removed in the matrix \f$ F^{(n)} \f$.
     * @param j1 Position of the second column to be removed in the matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ \det(F^{(n-2)}) / \det(F^{(n)}) \f$.
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
      if (N == 2) {
        det_new = 1.0;
        return det_new / det;
      }

      range Row_A(0, i0);
      range Row_B_0(i0 + 1, i1);
      range Row_B_1 = Row_B_0 + std::ptrdiff_t{-1};
      range Row_C_0(i1 + 1, N);
      range Row_C_1 = Row_C_0 + std::ptrdiff_t{-2};

      range Col_A(0, j0);
      range Col_B_0(j0 + 1, j1);
      range Col_B_1 = Col_B_0 + std::ptrdiff_t{-1};
      range Col_C_0(j1 + 1, N);
      range Col_C_1 = Col_C_0 + std::ptrdiff_t{-2};

      mat_new(Row_A, Col_A)   = mat(Row_A, Col_A);
      mat_new(Row_A, Col_B_1) = mat(Row_A, Col_B_0);
      mat_new(Row_A, Col_C_1) = mat(Row_A, Col_C_0);

      mat_new(Row_B_1, Col_A)   = mat(Row_B_0, Col_A);
      mat_new(Row_B_1, Col_B_1) = mat(Row_B_0, Col_B_0);
      mat_new(Row_B_1, Col_C_1) = mat(Row_B_0, Col_C_0);

      mat_new(Row_C_1, Col_A)   = mat(Row_C_0, Col_A);
      mat_new(Row_C_1, Col_B_1) = mat(Row_C_0, Col_B_0);
      mat_new(Row_C_1, Col_C_1) = mat(Row_C_0, Col_C_0);

      range R(0, N - 2);
      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the remove2 operation.
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
     * @brief Try to remove \f$ k \f$ rows and columns.
     *
     * @details The rows and columns at the positions given by the tuples \f$ \mathbf{i} \f$ and \f$ \mathbf{j} \f$
     * are removed from the matrix \f$ F^{(n)} \f$. It builds the candidate matrix and recomputes its determinant
     * from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Positions of the rows to be removed in the matrix \f$ F^{(n)} \f$.
     * @param j Positions of the columns to be removed in the matrix \f$ F^{(n)} \f$.
     * @return Determinant ratio \f$ \det(F^{(n-k)}) / \det(F^{(n)}) \f$.
     */
    value_type try_remove_k(std::vector<long> i, std::vector<long> j) {
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(i.size() == j.size());
      TRIQS_ASSERT(N >= static_cast<long>(i.size()));

      long k = static_cast<long>(i.size());
      wk.resize(k);

      // Sort indices in descending order for proper removal
      std::ranges::sort(i);
      std::ranges::sort(j);

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
        for (long new_j = 0; new_j < static_cast<long>(col_keep.size()); ++new_j) { mat_new(new_i, new_j) = mat(row_keep[new_i], col_keep[new_j]); }
      }

      range R(0, N - k);
      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the remove_k operation.
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
     * @brief Try to change one column in the matrix \f$ F^{(n)} \f$.
     *
     * @details The column at position \f$ j \f$ is replaced; its new elements are determined by the given matrix
     * builder argument \f$ y \f$. It builds the candidate matrix and recomputes its determinant from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param j Position of the column to be changed in the matrix \f$ F^{(n)} \f$.
     * @param y Argument to the matrix builder that determines the new elements of the column.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
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

      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the change column operation.
    void complete_change_col() {
      y_values[w1.j] = w1.y;
      std::swap(mat, mat_new);
    }

    //------------------------------------------------------------------------------------------
    public:
    /**
     * @brief Try to change one row in the matrix \f$ F^{(n)} \f$.
     *
     * @details The row at position \f$ i \f$ is replaced; its new elements are determined by the given matrix
     * builder argument \f$ x \f$. It builds the candidate matrix and recomputes its determinant from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be changed in the matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the new elements of the row.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
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

      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }
    //------------------------------------------------------------------------------------------
    private:
    // Complete the change row operation.
    void complete_change_row() {
      x_values[w1.i] = w1.x;
      std::swap(mat, mat_new);
    }

    //------------------------------------------------------------------------------------------
    public:
    /**
     * @brief Try to change one column and one row in the matrix \f$ F^{(n)} \f$.
     *
     * @details The row at position \f$ i \f$ and the column at position \f$ j \f$ are replaced; their new elements
     * are determined by the given matrix builder arguments \f$ x \f$ and \f$ y \f$. It builds the candidate matrix
     * and recomputes its determinant from scratch.
     *
     * @warning This routine does not make any modification. It has to be completed with complete_operation().
     *
     * @param i Position of the row to be changed in the matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be changed in the matrix \f$ F^{(n)} \f$.
     * @param x Argument to the matrix builder that determines the new elements of the row.
     * @param y Argument to the matrix builder that determines the new elements of the column.
     * @return Determinant ratio \f$ \det(\widetilde{F}^{(n)}) / \det(F^{(n)}) \f$.
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

      det_new = nda::linalg::det(mat_new(R, R));

      return det_new / det;
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the change row and column operation.
    void complete_change_col_row() {
      x_values[w1.i] = w1.x;
      y_values[w1.j] = w1.y;
      std::swap(mat, mat_new);
    }

    //------------------------------------------------------------------------------------------
    public:
    /**
     * @brief Try to fill the matrix \f$ F^{(n)} \f$ with new elements.
     *
     * @details Builds a completely new matrix \f$ \widetilde{F}^{(\widetilde{n})} \f$ from the given matrix builder
     * arguments \f$ \mathbf{x} \f$ and \f$ \mathbf{y} \f$ (with \f$ \widetilde{F}_{ij} = f(x_i, y_j) \f$) and
     * recomputes its determinant from scratch.
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

      long s = X.size();
      if (s > Nmax) {
        w_refill.reserve(2 * s);
        reserve(2 * s);
      }
      last_try = Refill;

      w_refill.clear();
      if (s == 0) { // treat empty matrix separately
        det_new = 1.0;
      } else {
        std::copy(X.begin(), X.end(), std::back_inserter(w_refill.x_values));
        std::copy(Y.begin(), Y.end(), std::back_inserter(w_refill.y_values));
        for (long i = 0; i < s; ++i)
          for (long j = 0; j < s; ++j) mat_new(i, j) = f(w_refill.x_values[i], w_refill.y_values[j]);

        range R(0, s);
        det_new = nda::linalg::det(mat_new(R, R));
      }

      return det_new / det;
    }

    //------------------------------------------------------------------------------------------
    private:
    // Complete the refill operation.
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
     * @brief Complete the last try-operation.
     *
     * @details It completes the last try-operation by applying the candidate matrix and determinant that were
     * prepared by the corresponding `try_*` call, depending on the tag set in the last try function call.
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
        default: TRIQS_RUNTIME_ERROR << "Misuing det_manip_basic"; // Never used?
      }

      mat_inverse_is_valid = false;
      det                  = det_new;
      ++n_opts;

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
     * @param i Position of the row to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be inserted in the matrix \f$ F^{(n)} \f$.
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
     * @param i0,i1 Positions of the rows to be inserted in the matrix \f$ F^{(n)} \f$.
     * @param j0,j1 Positions of the columns to be inserted in the matrix \f$ F^{(n)} \f$.
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
     * @param i Position of the row to be removed in the matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be removed in the matrix \f$ F^{(n)} \f$.
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
     * @param i0,i1 Positions of the rows to be removed in the matrix \f$ F^{(n)} \f$.
     * @param j0,j1 Positions of the columns to be removed in the matrix \f$ F^{(n)} \f$.
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
     * @param j Position of the column to be changed in the matrix \f$ F^{(n)} \f$.
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
     * @param i Position of the row to be changed in the matrix \f$ F^{(n)} \f$.
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
     * @details Wrapper for try_change_col_row() followed by a complete_operation() call. See try_change_col_row()
     * for details.
     * @param i Position of the row to be changed in the matrix \f$ F^{(n)} \f$.
     * @param j Position of the column to be changed in the matrix \f$ F^{(n)} \f$.
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
     * @details It specifies the direction of the circular shift performed on either the rows or columns of the
     * matrix \f$ F^{(n)} \f$. The following directions are supported:
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
     * the modification (it rebuilds the matrix and updates the determinant directly) and does not need to be
     * completed.
     *
     * A circular shift permutation of a finite set is equivalent to \f$ N \f$ transpositions, where \f$ N \f$ is the
     * size of the set. The sign of the permutation is therefore given by \f$ (-1)^{N-1} \f$.
     *
     * @param roll Direction of the roll operation.
     * @return -1 if the roll changes the sign of the determinant, 1 otherwise.
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
