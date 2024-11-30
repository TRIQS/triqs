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

#include <triqs/utility/first_include.hpp>
#include <vector>
#include <iterator>
#include <numeric>
#include <cmath>
#include <triqs/arrays.hpp>
#include <triqs/utility/callable_traits.hpp>
#include <nda/linalg/det_and_inverse.hpp>

namespace triqs::det_manip {

  namespace blas = nda::blas;

  // ================ Work Data Types =====================

  // For single-row/column operations
  template <typename x_type, typename y_type, typename value_type> struct work_data_type1 {
    x_type x;
    y_type y;
    long i, j, ireal, jreal;
    // MB = A^(-1)*B,
    // MC = C*A^(-1)
    nda::vector<value_type> MB, MC, B, C;
    // ksi = newdet/det
    value_type ksi;
    void resize(long N) {
      MB.resize(N);
      MC.resize(N);
      B.resize(N);
      C.resize(N);
    }
  };

  // For multiple-row/column operations
  template <typename x_type, typename y_type, typename value_type> struct work_data_typek {
    std::vector<x_type> x;
    std::vector<y_type> y;
    std::vector<long> i, j, ireal, jreal;
    // MB = A^(-1)*B,
    // MC = C*A^(-1)
    nda::matrix<value_type> MB, MC, B, C, ksi;
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
        return nda::determinant(ksi(Rk, Rk));
      };
    }
  };

  // For refill operations
  template <typename x_type, typename y_type, typename value_type> struct work_data_type_refill {
    std::vector<x_type> x_values;
    std::vector<y_type> y_values;
    nda::matrix<value_type> M;
    void reserve(long N) {
      x_values.reserve(N);
      y_values.reserve(N);
      M.resize(N, N);
    }
  };

  // ================ det_manip implementation =====================

  /**
     * @brief Standard matrix/det manipulations used in several QMC.
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
    static_assert(std::is_floating_point<value_type>::value || nda::is_complex_v<value_type>,
                  "det_manip : the function must return a floating number or a complex number");

    using matrix_type = nda::matrix<value_type>;

    protected: // the data
    FunctionType f;

    det_type det;
    long Nmax{0}, N;
    long kmax{1}, k;
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

    /// Write into HDF5
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

    /// Read from HDF5
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
    det_type newdet;
    int newsign;

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
       * Like for std::vector, reserve memory for a bigger size.
       * Preserves only the matrix, not the temporary working vectors/matrices, so do NOT use it
       * between a try_XXX and a complete_operation
       *
       * @param new_N The new size of the reserved memory
       */
    void reserve(long new_N, long new_k = 1) {
      if (new_k > kmax) {
        kmax = new_k;
        if (new_N <= Nmax) wk.resize(Nmax, kmax);
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
        wk.resize(Nmax, kmax);
      }
    }

    /// Get the number below which abs(det) is considered 0. If <0, the test will be isnormal(abs(det))
    double get_singular_threshold() const { return singular_threshold; }

    /// Sets the number below which abs(det) is considered 0. Cf get_is_singular_threshold
    void set_singular_threshold(double threshold) { singular_threshold = threshold; }

    /// Gets the number of operations done before a check in the dets.
    double get_n_operations_before_check() const { return n_opts_max_before_check; }

    /// Sets the number of operations done before a check in the dets.
    void set_n_operations_before_check(uint64_t n) { n_opts_max_before_check = n; }

    /// Get the bound for warning messages in the singular tests
    double get_precision_warning() const { return precision_warning; }

    /// Set the bound for warning messages in the singular tests
    void set_precision_warning(double threshold) { precision_warning = threshold; }

    /// Get the bound for throwing error in the singular tests
    double get_precision_error() const { return precision_error; }

    /// Set the bound for throwing error in the singular tests
    void set_precision_error(double threshold) { precision_error = threshold; }

    /**
       * @brief Constructor.
       *
       * @param F         The function (NB : a copy is made of the F object in this class).
       * @param init_size The maximum size of the matrix before a resize (like reserve in std::vector).
       *                  Like std::vector, resize is automatic (by a factor 2) but can yield a performance penalty
       *                  if it happens too often.
       */
    det_manip(FunctionType F, long init_size) : f(std::move(F)), Nmax(0), N(0) {
      reserve(init_size);
      mat_inv() = 0;
      det       = 1;
    }

    /**
       * @brief Constructor.
       *
       * @param F         The function (NB : a copy is made of the F object in this class).
       * @tparam ArgumentContainer
       * @param X, Y : container for X,Y.
       */
    template <typename ArgumentContainer1, typename ArgumentContainer2>
    det_manip(FunctionType F, ArgumentContainer1 const &X, ArgumentContainer2 const &Y) : f(std::move(F)), Nmax(0) {
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
      det             = nda::determinant(mat_inv(RN, RN));
      mat_inv(RN, RN) = inverse(mat_inv(RN, RN));
    }

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

    /// Put to size 0 : like a vector
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

    /// Current size of the matrix
    long size() const { return N; }

    /// Returns the i-th values of x
    x_type const &get_x(long i) const { return x_values[row_num[i]]; }

    /// Returns the j-th values of y
    y_type const &get_y(long j) const { return y_values[col_num[j]]; }

    /// Returns a vector with all x_values. Warning : this is slow, since it creates a new copy, and reorders the lines
    std::vector<x_type> get_x() const {
      std::vector<x_type> res;
      res.reserve(N);
      for (int i : range(N)) res.emplace_back(x_values[row_num[i]]);
      return res;
    }

    /// Returns a vector with all y_values. Warning : this is slow, since it creates a new copy, and reorders the cols
    std::vector<y_type> get_y() const {
      std::vector<y_type> res;
      res.reserve(N);
      for (int i : range(N)) res.emplace_back(y_values[col_num[i]]);
      return res;
    }

    /**
       * Advanced: Returns the vector of x_values using the INTERNAL STORAGE ORDER,
       * which differs by some permutation from the one given by the user.
       * Useful for some performance-critical loops.
       * To be used together with other *_internal_order functions.
       */
    std::vector<x_type> const &get_x_internal_order() const { return x_values; }

    /**
       * Advanced: Returns the vector of y_values using the INTERNAL STORAGE ORDER.
       * See doc of get_x_internal_order.
       */
    std::vector<y_type> const &get_y_internal_order() const { return y_values; }

    /// Returns the function f
    FunctionType const &get_function() const { return f; }

    /** det M of the current state of the matrix.  */
    det_type determinant() {
      if (is_singular()) regenerate();
      return sign * det;
    }

    /** Returns M^{-1}(i,j) */
    // warning : need to invert the 2 permutations: (AP)^-1= P^-1 A^-1.
    value_type inverse_matrix(int i, int j) const { return mat_inv(col_num[i], row_num[j]); }

    /// Returns the inverse matrix. Warning : this is slow, since it create a new copy, and reorder the lines/cols
    matrix_type inverse_matrix() const {
      matrix_type res(N, N);
      for (long i = 0; i < N; i++)
        for (long j = 0; j < N; j++) res(i, j) = inverse_matrix(i, j);
      return res;
    }

    /**
       * Advanced: Returns the inverse matrix using the INTERNAL STORAGE ORDER.
       * See doc of get_x_internal_order.
       */
    value_type inverse_matrix_internal_order(int i, int j) const { return mat_inv(i, j); }

    /**
       * Advanced: Returns the inverse matrix using the INTERNAL STORAGE ORDER.
       * See doc of get_x_internal_order.
       */
    nda::matrix_const_view<value_type> inverse_matrix_internal_order() const { return mat_inv(range(N), range(N)); }

    /// Rebuild the matrix. Warning : this is slow, since it create a new matrix and re-evaluate the function.
    matrix_type matrix() const {
      matrix_type res(N, N);
      for (long i = 0; i < N; i++)
        for (long j = 0; j < N; j++) res(i, j) = f(get_x(i), get_y(j));
      return res;
    }

    // Given a lambda fn : x,y,M, it calls fn(x_i,y_j,M_ji) for all i,j
    // Order of iteration is NOT fixed, it is optimised (for memory traversal)
    template <typename LambdaType> friend void foreach (det_manip const &d, LambdaType const &fn) {
      nda::for_each(std::array{d.N, d.N}, [&fn, &d](int i, int j) { return fn(d.x_values[i], d.y_values[j], d.mat_inv(j, i)); });
    }

    // ------------------------- OPERATIONS -----------------------------------------------

    /** Simply swap two lines
         NB very quick, we just change the permutation table internally
	 This operation is so simple that it has no try, complete.
       */
    void swap_row(long i, long j) {
      if (i == j) return;
      std::swap(row_num[i], row_num[j]);
      sign = -sign;
      // we do not need to change the det, or the matrix, just the permutation
    }

    /** Simply swap two lines and cols.
         NB very quick, we just change the permutation table internally
	 This operation is so simple that it has no try, complete.
       */
    void swap_col(long i, long j) {
      if (i == j) return;
      std::swap(col_num[i], col_num[j]);
      sign = -sign;
    }

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

    //fx gives the new line coefficients, fy gives the new column coefficients and ksi is the last coeff (at the intersection of the line and the column).
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
    value_type try_insert_k(std::vector<long> i, std::vector<long> j, std::vector<x_type> x, std::vector<y_type> y) {
      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(i.size() == j.size());
      TRIQS_ASSERT(j.size() == x.size());
      TRIQS_ASSERT(x.size() == y.size());

      k = i.size();
      reserve(N + k, k);
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
      for (long l = 0; l < k; ++l) {
        wk.i[l] = i[idx[l]];
        wk.x[l] = x[idx[l]];
        wk.j[l] = j[idy[l]];
        wk.y[l] = y[idy[l]];
      };

      // check consistency
      for (int l = 0; l < k - 1; ++l) {
        TRIQS_ASSERT(wk.i[l] != wk.i[l + 1] and 0 <= wk.i[l] and wk.i[l] < N + k);
        TRIQS_ASSERT(wk.j[l] != wk.j[l + 1] and 0 <= wk.j[l] and wk.j[l] < N + k);
      }

      // w1.ksi = Delta(x_values,y_values) - Cw.MB using BLAS
      for (long m = 0; m < k; ++m) {
        for (long n = 0; n < k; ++n) { wk.ksi(m, n) = f(wk.x[m], wk.y[n]); }
      }

      // treat empty matrix separately
      if (N == 0) {
        newdet  = wk.det_ksi(k);
        newsign = 1;
        return value_type(newdet);
      }

      // I add the rows and cols and the end. If the move is rejected,
      // no effect since N will not be changed : inv_mat(i,j) for i,j>=N has no meaning.
      for (long n = 0; n < N; n++) {
        for (long l = 0; l < k; ++l) {
          wk.B(n, l) = f(x_values[n], wk.y[l]);
          wk.C(l, n) = f(wk.x[l], y_values[n]);
        }
      }
      range RN(N), Rk(k);
      //wk.MB(RN,Rk) = mat_inv(RN,N) * wk.B(RN,Rk); // OPTIMIZE BELOW
      blas::gemm(1.0, mat_inv(RN, RN), wk.B(RN, Rk), 0.0, wk.MB(RN, Rk));
      //ksi -= wk.C (Rk, RN) * wk.MB(RN, Rk); // OPTIMIZE BELOW
      blas::gemm(-1.0, wk.C(Rk, RN), wk.MB(RN, Rk), 1.0, wk.ksi(Rk, Rk));
      auto ksi     = wk.det_ksi(k);
      newdet       = det * ksi;
      long idx_sum = 0;
      for (long l = 0; l < k; ++l) { idx_sum += wk.i[l] + wk.j[l]; }
      newsign = (idx_sum % 2 == 0 ? sign : -sign); // since N-i0 + N-j0 + N + 1 -i1 + N+1 -j1 = i0+j0 [2]
      return ksi * (newsign * sign);               // sign is unity, hence 1/sign == sign
    }
    value_type try_insert2(long i0, long i1, long j0, long j1, x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
      return try_insert_k({i0, i1}, {j0, j1}, {x0, x1}, {y0, y1});
    }

    //------------------------------------------------------------------------------------------
    private:
    void complete_insert_k() {

      // store the new value of x,y. They are seen through the same permutations as rows and cols resp.
      for (int l = 0; l < k; ++l) {
        x_values.push_back(wk.x[l]);
        y_values.push_back(wk.y[l]);
        row_num.push_back(0);
        col_num.push_back(0);
      }

      range Rk(0, k);
      // treat empty matrix separately
      if (N == 0) {
        N               = k;
        mat_inv(Rk, Rk) = inverse(wk.ksi(Rk, Rk));
        for (long l = 0; l < k; ++l) {
          row_num[wk.i[l]] = l;
          col_num[wk.j[l]] = l;
        }
        return;
      }

      range RN(N);
      //wk.MC(Rk,RN) = wk.C(Rk,RN) * mat_inv(RN,RN);// OPTIMIZE BELOW
      blas::gemm(1.0, wk.C(Rk, RN), mat_inv(RN, RN), 0.0, wk.MC(Rk, RN));
      wk.MC(Rk, range(N, N + k)) = -1; // -identity matrix
      wk.MB(range(N, N + k), Rk) = -1; // -identity matrix !

      // keep the real position of the row/col
      // since we insert a col/row, we have first to push the col at the right
      // and then say that col wk.i[0] is stored in N, the last col.
      // same for rows
      for (int l = 0; l < k; ++l) {
        N++;
        for (long i = N - 2; i >= wk.i[l]; i--) row_num[i + 1] = row_num[i];
        row_num[wk.i[l]] = N - 1;
        for (long i = N - 2; i >= wk.j[l]; i--) col_num[i + 1] = col_num[i];
        col_num[wk.j[l]] = N - 1;
      }
      RN = range(N);

      wk.ksi(Rk, Rk)               = inverse(wk.ksi(Rk, Rk));
      mat_inv(RN, range(N - k, N)) = 0;
      mat_inv(range(N - k, N), RN) = 0;
      //mat_inv(RN,RN) += wk.MB(RN,Rk) * (wk.ksi(Rk, Rk) * wk.MC(Rk,RN)); // OPTIMIZE BELOW
      blas::gemm(1.0, wk.MB(RN, Rk), (wk.ksi(Rk, Rk) * wk.MC(Rk, RN)), 1.0, mat_inv(RN, RN));
    }
    void complete_insert2() { complete_insert_k(); }

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
        auto iitr          = std::find(row_num.begin(), row_num.end(), w1.ireal);
        auto titr          = std::find(row_num.begin(), row_num.end(), N - 1);
        std::swap(*iitr, *titr);
      }
      if (w1.jreal != N - 1) {
        deep_swap(mat_inv(w1.jreal, RN), mat_inv(N - 1, RN));
        y_values[w1.jreal] = y_values[N - 1];
        auto jitr          = std::find(col_num.begin(), col_num.end(), w1.jreal);
        auto titr          = std::find(col_num.begin(), col_num.end(), N - 1);
        std::swap(*jitr, *titr);
      }
      N--;
      RN = range(N);

      auto it1 [[maybe_unused]] = std::remove(row_num.begin(), row_num.end(), N);
      auto it2 [[maybe_unused]] = std::remove(col_num.begin(), col_num.end(), N);

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
       * Double Removal operation of cols j0,j1 and rows i0,i1
       *
       * Returns the ratio of det Minv_new / det Minv.
       * This routine does NOT make any modification. It has to be completed with complete_operation().
       */
    value_type try_remove_k(std::vector<long> i, std::vector<long> j) {

      std::sort(i.begin(), i.end());
      std::sort(j.begin(), j.end());

      TRIQS_ASSERT(last_try == NoTry);
      TRIQS_ASSERT(N >= 2);
      TRIQS_ASSERT(i.size() == j.size());

      k = i.size();
      reserve(N - k, k);
      last_try = RemoveK;

      // check inputs
      for (int l = 0; l < k - 1; ++l) {
        TRIQS_ASSERT(i[l] != i[l + 1] and 0 <= i[l] and i[l] < N);
        TRIQS_ASSERT(j[l] != j[l + 1] and 0 <= j[l] and j[l] < N);
      }

      for (long l = 0; l < k; ++l) {
        wk.i[l]     = i[l];
        wk.j[l]     = j[l];
        wk.ireal[l] = row_num[wk.i[l]];
        wk.jreal[l] = col_num[wk.j[l]];
      }

      // compute the newdet
      for (long l1 = 0; l1 < k; ++l1) {
        for (long l2 = 0; l2 < k; ++l2) { wk.ksi(l1, l2) = mat_inv(wk.jreal[l1], wk.ireal[l2]); }
      }
      auto det_ksi = wk.det_ksi(k);
      newdet       = det * det_ksi;
      long idx_sum = 0;
      for (long l = 0; l < k; ++l) { idx_sum += wk.i[l] + wk.j[l]; }
      newsign = (idx_sum % 2 == 0 ? sign : -sign);

      return det_ksi * (newsign * sign); // sign is unity, hence 1/sign == sign
    }
    value_type try_remove2(long i0, long i1, long j0, long j1) { return try_remove_k({i0, i1}, {j0, j1}); }
    //------------------------------------------------------------------------------------------
    private:
    void complete_remove_k() {
      if (N == k) {
        clear();
        return;
      } // put the sign to 1 also .... Change complete_remove...

      std::vector<long> ireal = wk.ireal;
      std::vector<long> jreal = wk.jreal;
      std::sort(ireal.begin(), ireal.begin() + k);
      std::sort(jreal.begin(), jreal.begin() + k);

      // Move rows and cols to be removed to the end, starting from the right.
      // Adjust the x_values and y_values vector accordingly and
      // swap the associated row_num and col_num elements
      // Remember that for M row/col is interchanged by inversion, transposition.
      range RN(N);
      for (long m = k - 1, target = N - 1; m >= 0; --m, --target) {
        if (ireal[m] != target) {
          deep_swap(mat_inv(RN, ireal[m]), mat_inv(RN, target));
          x_values[ireal[m]] = x_values[target];
          auto iitr          = std::find(row_num.begin(), row_num.end(), ireal[m]);
          auto titr          = std::find(row_num.begin(), row_num.end(), target);
          std::swap(*iitr, *titr);
        }
        if (jreal[m] != target) {
          deep_swap(mat_inv(jreal[m], RN), mat_inv(target, RN));
          y_values[jreal[m]] = y_values[target];
          auto jitr          = std::find(col_num.begin(), col_num.end(), jreal[m]);
          auto titr          = std::find(col_num.begin(), col_num.end(), target);
          std::swap(*jitr, *titr);
        }
      }
      N -= k;
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
      range Rl(N, N + k), Rk(k);
      wk.ksi(Rk, Rk) = inverse(mat_inv(Rl, Rl));

      // write explicitely the second product on ksi for speed ?
      //mat_inv(RN,RN) -= mat_inv(RN,Rl) * (wk.ksi * mat_inv(Rl,RN)); // OPTIMIZE BELOW
      blas::gemm(-1.0, mat_inv(RN, Rl), wk.ksi(Rk, Rk) * mat_inv(Rl, RN), 1.0, mat_inv(RN, RN));
    }
    void complete_remove2() { complete_remove_k(); }

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
       * Consider the change the row i and the corresponding x.
       *
       * Returns the ratio of det Minv_new / det Minv.
       * This routine does NOT make any modification. It has to be completed with complete_operation().
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
       * Consider the change the row i and column j and the corresponding x and y
       *
       * Returns the ratio of det Minv_new / det Minv.
       * This routine does NOT make any modification. It has to be completed with complete_operation().
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
      newdet  = nda::determinant(w_refill.M(R, R));
      newsign = 1;

      return newdet / (sign * det);
    }

    //------------------------------------------------------------------------------------------
    private:
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
      mat_inv(RN, RN) = inverse(w_refill.M(RN, RN));
    }

    //------------------------------------------------------------------------------------------
    private:
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
      det = nda::determinant(res);

      if (is_singular()) TRIQS_RUNTIME_ERROR << "ERROR in det_manip regenerate: Determinant is singular";
      res = inverse(res);

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
      s *= nda::determinant(m);
      m() = 0.0;
      for (int i = 0; i < N; i++) m(i, col_num[i]) = 1;
      s *= nda::determinant(m);
      sign = (s > 0 ? 1 : -1);
    }

    void check_mat_inv() { _regenerate_with_check(true, precision_warning, precision_error); }

    /// it the det 0 ? I.e. (singular_threshold <0 ? not std::isnormal(std::abs(det)) : (std::abs(det)<singular_threshold))
    bool is_singular() const { return (singular_threshold < 0 ? not std::isnormal(std::abs(det)) : (std::abs(det) < singular_threshold)); }

    //------------------------------------------------------------------------------------------
    public:
    void regenerate() { _regenerate_with_check(false, 0, 0); }

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
      long tmp;
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
