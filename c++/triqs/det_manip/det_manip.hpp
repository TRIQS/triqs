/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2015 by M. Ferrero, O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

#include <triqs/utility/first_include.hpp>
#include <vector>
#include <iterator>
#include <numeric>
#include <cmath>
#include <triqs/arrays.hpp>
#include <triqs/arrays/algorithms.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/blas_lapack/dot.hpp>
#include <triqs/arrays/blas_lapack/ger.hpp>
#include <triqs/arrays/blas_lapack/gemm.hpp>
#include <triqs/arrays/blas_lapack/gemv.hpp>
#include <triqs/utility/function_arg_ret_type.hpp>

namespace triqs {
  namespace det_manip {

    namespace blas = arrays::blas;

    /**
  * @brief Standard matrix/det manipulations used in several QMC.
  */
    template <typename FunctionType> class det_manip {
      private:
      using f_tr = utility::function_arg_ret_type<FunctionType>;
      static_assert(f_tr::arity == 2, "det_manip : the function must take two arguments !");

      public:
      using x_type     = typename f_tr::template decay_arg<0>::type;
      using y_type     = typename f_tr::template decay_arg<1>::type;
      using value_type = typename f_tr::result_type;
      // options the det could be kept in a long double to minimize overflow
      //using det_type = std::conditional_t<std::is_same<value_type, double>::value, long double, std::complex<long double>>;
      using det_type = value_type;
      static_assert(std::is_floating_point<value_type>::value || triqs::is_complex<value_type>::value,
                    "det_manip : the function must return a floating number or a complex number");

      using vector_type            = arrays::vector<value_type>;
      using matrix_type            = arrays::matrix<value_type>;
      using matrix_view_type       = arrays::matrix_view<value_type>;
      using matrix_const_view_type = arrays::matrix_const_view<value_type>;

      protected: // the data
      using int_type = std::ptrdiff_t;
      using range    = itertools::range;

      FunctionType f;

      // serialized data. There are all VALUES.
      det_type det;
      size_t Nmax, N;
      enum {
        NoTry,
        Insert,
        Remove,
        ChangeCol,
        ChangeRow,
        ChangeRowCol,
        Insert2 = 10,
        Remove2 = 11,
        Refill  = 20
      } last_try = NoTry; // keep in memory the last operation not completed
      std::vector<size_t> row_num, col_num;
      std::vector<x_type> x_values;
      std::vector<y_type> y_values;
      int sign = 1;
      matrix_type mat_inv;
      uint64_t n_opts                  = 0;   // count the number of operation
      uint64_t n_opts_max_before_check = 100; // max number of ops before the test of deviation of the det, M^-1 is performed.
      double singular_threshold = -1; // the test to see if the matrix is singular is abs(det) > singular_threshold. If <0, it is !isnormal(abs(det))
      double precision_warning  = 1.e-8; // bound for warning message in check for singular matrix
      double precision_error    = 1.e-5; // bound for throwing error in check for singular matrix

      private:
      //  ------------     BOOST Serialization ------------
      //  What about f ? Not serialized at the moment.
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &Nmax;
        ar &N;
        ar &n_opts;
        ar &n_opts_max_before_check;
        ar &singular_threshold;
        ar &det;
        ar &sign;
        ar &mat_inv;
        ar &row_num;
        ar &col_num;
        ar &x_values;
        ar &y_values;
      }

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
      // temporary work data, not saved, serialized, etc....
      struct work_data_type1 {
        x_type x;
        y_type y;
        // MB = A^(-1)*B,
        // MC = C*A^(-1)
        vector_type MB, MC, B, C;
        // ksi = newdet/det
        value_type ksi;
        size_t i, j, ireal, jreal;
        void reserve(size_t s) {
          B.resize(s);
          C.resize(s);
          MB.resize(s);
          MC.resize(s);
          MB() = 0;
          MC() = 0;
        }
      };

      struct work_data_type2 {
        x_type x[2];
        y_type y[2];
        // MB = A^(-1)*B,
        // MC = C*A^(-1)
        matrix_type MB, MC, B, C, ksi;
        size_t i[2], j[2], ireal[2], jreal[2];
        void reserve(size_t s) {
          MB.resize(s, 2);
          MC.resize(2, s);
          B.resize(s, 2), C.resize(2, s);
          ksi.resize(2, 2);
          MB() = 0;
          MC() = 0;
        }
        value_type det_ksi() const { return ksi(0, 0) * ksi(1, 1) - ksi(1, 0) * ksi(0, 1); }
      };

      struct work_data_type_refill {
        std::vector<x_type> x_values;
        std::vector<y_type> y_values;
        matrix_type M;
        void reserve(size_t s) {
          x_values.reserve(s);
          y_values.reserve(s);
          if (s > first_dim(M)) M.resize(s, s);
        }
      };

      work_data_type1 w1;
      work_data_type2 w2;
      work_data_type_refill w_refill;
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
        SW(w2);
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
     * @param new_size The new size of the reserved memory
     */
      void reserve(size_t new_size) {
        if (new_size <= Nmax) return;
        matrix_type Mcopy(mat_inv);
        size_t N0 = Nmax;
        Nmax      = new_size;
        mat_inv.resize(Nmax, Nmax);
        mat_inv(range(0, N0), range(0, N0)) = Mcopy; // keep the content of mat_inv ---> into the lib ?
        row_num.reserve(Nmax);
        col_num.reserve(Nmax);
        x_values.reserve(Nmax);
        y_values.reserve(Nmax);
        w1.reserve(Nmax);
        w2.reserve(Nmax);
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
      det_manip(FunctionType F, size_t init_size) : f(std::move(F)), Nmax(0), N(0) {
        reserve(init_size);
        mat_inv() = 0;
        det       = 1;
      }

      /** @brief Constructor.
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
        if (N > Nmax) reserve(2 * N); // put some margin..
        std::copy(X.begin(), X.end(), std::back_inserter(x_values));
        std::copy(Y.begin(), Y.end(), std::back_inserter(y_values));
        mat_inv() = 0;
        for (size_t i = 0; i < N; ++i) {
          row_num.push_back(i);
          col_num.push_back(i);
          for (size_t j = 0; j < N; ++j) mat_inv(i, j) = f(x_values[i], y_values[j]);
        }
        range R(0, N);
        det           = arrays::determinant(mat_inv(R, R));
        mat_inv(R, R) = inverse(mat_inv(R, R));
      }

      det_manip(det_manip const &) = default;
      det_manip(det_manip &&rhs) noexcept : f(std::move(rhs.f)) {
        this->swap_but_f(rhs);
      } // f need not have a default constructor and we dont swap the temp data...
      //det_manip& operator=(const det_manip&) = default;
      det_manip &operator=(const det_manip &) = delete;
      det_manip &operator                     =(det_manip &&rhs) noexcept {
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
      size_t size() const { return N; }

      /// Returns the i-th values of x
      x_type const &get_x(size_t i) const { return x_values[row_num[i]]; }

      /// Returns the j-th values of y
      y_type const &get_y(size_t j) const { return y_values[col_num[j]]; }

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
        for (size_t i = 0; i < N; i++)
          for (size_t j = 0; j < N; j++) res(i, j) = inverse_matrix(i, j);
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
      matrix_const_view_type inverse_matrix_internal_order() const { return mat_inv(range(N), range(N)); }

      /// Rebuild the matrix. Warning : this is slow, since it create a new matrix and re-evaluate the function.
      matrix_type matrix() const {
        matrix_type res(N, N);
        for (size_t i = 0; i < N; i++)
          for (size_t j = 0; j < N; j++) res(i, j) = f(get_x(i), get_y(j));
        return res;
      }

      // Given a lambda f : x,y,M, it calls f(x_i,y_j,M_ji) for all i,j
      // Order of iteration is NOT fixed, it is optimised (for memory traversal)
      template <typename LambdaType> friend void foreach (det_manip const &d, LambdaType const &f) {
        //for (size_t i=0; i<d.N;i++)
        //for (size_t j=0; j<d.N;j++)
        // f(d.x_values[i], d.y_values[j], d.mat_inv(j,i));
        range R(0, d.N);
        foreach (d.mat_inv(R, R), [&f, &d](int i, int j) { return f(d.x_values[i], d.y_values[j], d.mat_inv(j, i)); })
          ;
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
      value_type try_insert(size_t i, size_t j, x_type const &x, y_type const &y) {

        // check input and store it for complete_operation
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i <= N);
        TRIQS_ASSERT(j <= N);
        TRIQS_ASSERT(i >= 0);
        TRIQS_ASSERT(j >= 0);
        if (N == Nmax) reserve(2 * Nmax);
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
        for (size_t k = 0; k < N; k++) {
          w1.B(k) = f(x_values[k], y);
          w1.C(k) = f(x, y_values[k]);
        }
        range R(0, N);
        //w1.MB(R) = mat_inv(R,R) * w1.B(R);// OPTIMIZE BELOW
        blas::gemv(1.0, mat_inv(R, R), w1.B(R), 0.0, w1.MB(R));
        w1.ksi  = f(x, y) - arrays::dot(w1.C(R), w1.MB(R));
        newdet  = det * w1.ksi;
        newsign = ((i + j) % 2 == 0 ? sign : -sign); // since N-i0 + N-j0  = i0+j0 [2]
        return w1.ksi * (newsign * sign);            // sign is unity, hence 1/sign == sign
      }

      //fx gives the new line coefficients, fy gives the new column coefficients and ksi is the last coeff (at the intersection of the line and the column).
      template <typename Fx, typename Fy> value_type try_insert_from_function(size_t i, size_t j, Fx fx, Fy fy, value_type const ksi) {

        // check input and store it for complete_operation
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i <= N);
        TRIQS_ASSERT(j <= N);
        TRIQS_ASSERT(i >= 0);
        TRIQS_ASSERT(j >= 0);
        if (N == Nmax) reserve(2 * Nmax);
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
        for (size_t k = 0; k < N; k++) {
          w1.B(k) = fx(x_values[k]);
          w1.C(k) = fy(y_values[k]);
        }
        range R(0, N);
        //w1.MB(R) = mat_inv(R,R) * w1.B(R);// OPTIMIZE BELOW
        blas::gemv(1.0, mat_inv(R, R), w1.B(R), 0.0, w1.MB(R));
        w1.ksi  = ksi - arrays::dot(w1.C(R), w1.MB(R));
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

        range R1(0, N);
        //w1.MC(R1) = mat_inv(R1,R1).transpose() * w1.C(R1); //OPTIMIZE BELOW
        blas::gemv(1.0, mat_inv(R1, R1).transpose(), w1.C(R1), 0.0, w1.MC(R1));
        w1.MC(N) = -1;
        w1.MB(N) = -1;

        N++;

        // keep the real position of the row/col
        // since we insert a col/row, we have first to push the col at the right
        // and then say that col w1.i is stored in N, the last col.
        // same for rows
        for (int_type i = N - 2; i >= int_type(w1.i); i--) row_num[i + 1] = row_num[i];
        row_num[w1.i] = N - 1;
        for (int_type i = N - 2; i >= int_type(w1.j); i--) col_num[i + 1] = col_num[i];
        col_num[w1.j] = N - 1;

        // Minv is ok, we need to complete
        w1.ksi = 1 / w1.ksi;

        // compute the change to the inverse
        // M += w1.ksi w1.MB w1.MC with BLAS. first put the 0
        range R(0, N);
        mat_inv(R, N - 1) = 0;
        mat_inv(N - 1, R) = 0;
        //mat_inv(R,R) += w1.ksi* w1.MB(R) * w1.MC(R)// OPTIMIZE BELOW
        blas::ger(w1.ksi, w1.MB(R), w1.MC(R), mat_inv(R, R));
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

      value_type try_insert2(size_t i0, size_t i1, size_t j0, size_t j1, x_type const &x0_, x_type const &x1_, y_type const &y0_, y_type const &y1_) {

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

        // w1.ksi = Delta(x_values,y_values) - Cw.MB using BLAS
        w2.ksi(0, 0) = f(x0, y0);
        w2.ksi(0, 1) = f(x0, y1);
        w2.ksi(1, 0) = f(x1, y0);
        w2.ksi(1, 1) = f(x1, y1);

        // treat empty matrix separately
        if (N == 0) {
          newdet  = w2.det_ksi();
          newsign = 1;
          return value_type(newdet);
        }

        // I add the rows and cols and the end. If the move is rejected,
        // no effect since N will not be changed : inv_mat(i,j) for i,j>=N has no meaning.
        for (size_t k = 0; k < N; k++) {
          w2.B(k, 0) = f(x_values[k], y0);
          w2.B(k, 1) = f(x_values[k], y1);
          w2.C(0, k) = f(x0, y_values[k]);
          w2.C(1, k) = f(x1, y_values[k]);
        }
        range R(0, N), R2(0, 2);
        //w2.MB(R,R2) = mat_inv(R,R) * w2.B(R,R2); // OPTIMIZE BELOW
        blas::gemm(1.0, mat_inv(R, R), w2.B(R, R2), 0.0, w2.MB(R, R2));
        //w2.ksi -= w2.C (R2, R) * w2.MB(R, R2); // OPTIMIZE BELOW
        blas::gemm(-1.0, w2.C(R2, R), w2.MB(R, R2), 1.0, w2.ksi);
        auto ksi = w2.det_ksi();
        newdet   = det * ksi;
        newsign  = ((i0 + j0 + i1 + j1) % 2 == 0 ? sign : -sign); // since N-i0 + N-j0 + N + 1 -i1 + N+1 -j1 = i0+j0 [2]
        return ksi * (newsign * sign);                            // sign is unity, hence 1/sign == sign
      }

      //------------------------------------------------------------------------------------------
      private:
      void complete_insert2() {
        // store the new value of x,y. They are seen through the same permutations as rows and cols resp.
        for (int k = 0; k < 2; ++k) {
          x_values.push_back(w2.x[k]);
          y_values.push_back(w2.y[k]);
          row_num.push_back(0);
          col_num.push_back(0);
        }

        range R2(0, 2);
        // treat empty matrix separately
        if (N == 0) {
          N                = 2;
          mat_inv(R2, R2)  = inverse(w2.ksi);
          row_num[w2.i[1]] = 1;
          col_num[w2.j[1]] = 1;
          return;
        }

        range Ri(0, N);
        //w2.MC(R2,Ri) = w2.C(R2,Ri) * mat_inv(Ri,Ri);// OPTIMIZE BELOW
        blas::gemm(1.0, w2.C(R2, Ri), mat_inv(Ri, Ri), 0.0, w2.MC(R2, Ri));
        w2.MC(R2, range(N, N + 2)) = -1; // -identity matrix
        w2.MB(range(N, N + 2), R2) = -1; // -identity matrix !

        // keep the real position of the row/col
        // since we insert a col/row, we have first to push the col at the right
        // and then say that col w2.i[0] is stored in N, the last col.
        // same for rows
        for (int k = 0; k < 2; ++k) {
          N++;
          for (int_type i = N - 2; i >= int_type(w2.i[k]); i--) row_num[i + 1] = row_num[i];
          row_num[w2.i[k]] = N - 1;
          for (int_type i = N - 2; i >= int_type(w2.j[k]); i--) col_num[i + 1] = col_num[i];
          col_num[w2.j[k]] = N - 1;
        }
        w2.ksi = inverse(w2.ksi);
        range R(0, N);
        mat_inv(R, range(N - 2, N)) = 0;
        mat_inv(range(N - 2, N), R) = 0;
        //mat_inv(R,R) += w2.MB(R,R2) * (w2.ksi * w2.MC(R2,R)); // OPTIMIZE BELOW
        blas::gemm(1.0, w2.MB(R, R2), (w2.ksi * w2.MC(R2, R)), 1.0, mat_inv(R, R));
      }

      public:
      //------------------------------------------------------------------------------------------

      /**
     * Consider the removal the colj0 and row i0 from the matrix.
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_remove(size_t i, size_t j) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i < N);
        TRIQS_ASSERT(j < N);
        TRIQS_ASSERT(i >= 0);
        TRIQS_ASSERT(j >= 0);
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

        // repack the matrix inv_mat
        // swap the rows w1.ireal and N, w1.jreal and N in inv_mat
        // Remember that for M row/col is interchanged by inversion, transposition.
        {
          range R(0, N);
          if (w1.jreal != N - 1) {
            arrays::deep_swap(mat_inv(w1.jreal, R), mat_inv(N - 1, R));
            y_values[w1.jreal] = y_values[N - 1];
          }

          if (w1.ireal != N - 1) {
            arrays::deep_swap(mat_inv(R, w1.ireal), mat_inv(R, N - 1));
            x_values[w1.ireal] = x_values[N - 1];
          }
        }

        N--;

        // M <- a - d^-1 b c with BLAS
        w1.ksi = -1 / mat_inv(N, N);
        ASSERT(std::isfinite(std::abs(w1.ksi)));
        range R(0, N);

        //mat_inv(R,R) += w1.ksi, * mat_inv(R,N) * mat_inv(N,R);
        blas::ger(w1.ksi, mat_inv(R, N), mat_inv(N, R), mat_inv(R, R));

        // modify the permutations
        for (size_t k = w1.i; k < N; k++) { row_num[k] = row_num[k + 1]; }
        for (size_t k = w1.j; k < N; k++) { col_num[k] = col_num[k + 1]; }
        for (size_t k = 0; k < N; k++) {
          if (col_num[k] == N) col_num[k] = w1.jreal;
          if (row_num[k] == N) row_num[k] = w1.ireal;
        }
        row_num.pop_back();
        col_num.pop_back();
        x_values.pop_back();
        y_values.pop_back();
      }

      public:
      //------------------------------------------------------------------------------------------

      /**
     * Double Removal operation of cols j0,j1 and rows i0,i1
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_remove2(size_t i0, size_t i1, size_t j0, size_t j1) {

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

        w2.i[0]     = std::min(i0, i1);
        w2.i[1]     = std::max(i0, i1);
        w2.j[0]     = std::min(j0, j1);
        w2.j[1]     = std::max(j0, j1);
        w2.ireal[0] = row_num[w2.i[0]];
        w2.ireal[1] = row_num[w2.i[1]];
        w2.jreal[0] = col_num[w2.j[0]];
        w2.jreal[1] = col_num[w2.j[1]];

        // compute the newdet
        w2.ksi(0, 0) = mat_inv(w2.jreal[0], w2.ireal[0]);
        w2.ksi(1, 0) = mat_inv(w2.jreal[1], w2.ireal[0]);
        w2.ksi(0, 1) = mat_inv(w2.jreal[0], w2.ireal[1]);
        w2.ksi(1, 1) = mat_inv(w2.jreal[1], w2.ireal[1]);
        auto ksi     = w2.det_ksi();
        newdet       = det * ksi;
        newsign      = ((i0 + j0 + i1 + j1) % 2 == 0 ? sign : -sign);

        return ksi * (newsign * sign); // sign is unity, hence 1/sign == sign
      }
      //------------------------------------------------------------------------------------------
      private:
      void complete_remove2() {
        if (N == 2) {
          clear();
          return;
        } // put the sign to 1 also .... Change complete_remove...

        size_t i_real_max = std::max(w2.ireal[0], w2.ireal[1]);
        size_t i_real_min = std::min(w2.ireal[0], w2.ireal[1]);
        size_t j_real_max = std::max(w2.jreal[0], w2.jreal[1]);
        size_t j_real_min = std::min(w2.jreal[0], w2.jreal[1]);

        range R(0, N);

        if (j_real_max != N - 1) {
          arrays::deep_swap(mat_inv(j_real_max, R), mat_inv(N - 1, R));
          y_values[j_real_max] = y_values[N - 1];
        }
        if (j_real_min != N - 2) {
          arrays::deep_swap(mat_inv(j_real_min, R), mat_inv(N - 2, R));
          y_values[j_real_min] = y_values[N - 2];
        }
        if (i_real_max != N - 1) {
          arrays::deep_swap(mat_inv(R, i_real_max), mat_inv(R, N - 1));
          x_values[i_real_max] = x_values[N - 1];
        }
        if (i_real_min != N - 2) {
          arrays::deep_swap(mat_inv(R, i_real_min), mat_inv(R, N - 2));
          x_values[i_real_min] = x_values[N - 2];
        }

        N -= 2;

        // M <- a - d^-1 b c with BLAS
        range Rn(0, N), Rl(N, N + 2);
        //w2.ksi = mat_inv(Rl,Rl);
        //w2.ksi = inverse( w2.ksi);
        w2.ksi = inverse(mat_inv(Rl, Rl));

        // write explicitely the second product on ksi for speed ?
        //mat_inv(Rn,Rn) -= mat_inv(Rn,Rl) * (w2.ksi * mat_inv(Rl,Rn)); // OPTIMIZE BELOW
        blas::gemm(-1.0, mat_inv(Rn, Rl), w2.ksi * mat_inv(Rl, Rn), 1.0, mat_inv(Rn, Rn));

        // modify the permutations
        for (size_t k = w2.i[0]; k < w2.i[1] - 1; k++) row_num[k] = row_num[k + 1];
        for (size_t k = w2.i[1] - 1; k < N; k++) row_num[k] = row_num[k + 2];
        for (size_t k = w2.j[0]; k < w2.j[1] - 1; k++) col_num[k] = col_num[k + 1];
        for (size_t k = w2.j[1] - 1; k < N; k++) col_num[k] = col_num[k + 2];
        for (size_t k = 0; k < N; k++) {
          if (col_num[k] == N + 1) col_num[k] = j_real_max;
          if (col_num[k] == N) col_num[k] = j_real_min;
          if (row_num[k] == N + 1) row_num[k] = i_real_max;
          if (row_num[k] == N) row_num[k] = i_real_min;
        }

        for (int u = 0; u < 2; ++u) {
          row_num.pop_back();
          col_num.pop_back();
          x_values.pop_back();
          y_values.pop_back();
        }
      }
      //------------------------------------------------------------------------------------------
      public:
      /**
     * Consider the change the column j and the corresponding y.
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_change_col(size_t j, y_type const &y) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(j < N);
        TRIQS_ASSERT(j >= 0);
        w1.j     = j;
        last_try = ChangeCol;
        w1.jreal = col_num[j];
        w1.y     = y;

        // Compute the col B.
        for (size_t i = 0; i < N; i++) w1.MC(i) = f(x_values[i], w1.y) - f(x_values[i], y_values[w1.jreal]);
        range R(0, N);
        //w1.MB(R) = mat_inv(R,R) * w1.MC(R);// OPTIMIZE BELOW
        blas::gemv(1.0, mat_inv(R, R), w1.MC(R), 0.0, w1.MB(R));

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
        range R(0, N);
        y_values[w1.jreal] = w1.y;

        // modifying M : Mij += w1.ksi Bi Mnj
        // using Shermann Morrison formula.
        // implemented in 2 times : first Bn=0 so that Mnj is not modified ! and then change Mnj
        // Cf notes : simply multiply by -w1.ksi
        w1.ksi          = -1 / w1.ksi;
        w1.MB(w1.jreal) = 0;
        //mat_inv(R,R) += w1.ksi * w1.MB(R) * mat_inv(w1.jreal,R)); // OPTIMIZE BELOW
        blas::ger(w1.ksi, w1.MB(R), mat_inv(w1.jreal, R), mat_inv(R, R));
        mat_inv(w1.jreal, R) *= -w1.ksi;
      }

      //------------------------------------------------------------------------------------------
      public:
      /**
     * Consider the change the row i and the corresponding x.
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_change_row(size_t i, x_type const &x) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(i < N);
        TRIQS_ASSERT(i >= 0);
        w1.i     = i;
        last_try = ChangeRow;
        w1.ireal = row_num[i];
        w1.x     = x;

        // Compute the col B.
        for (size_t i = 0; i < N; i++) w1.MB(i) = f(w1.x, y_values[i]) - f(x_values[w1.ireal], y_values[i]);
        range R(0, N);
        //w1.MC(R) = mat_inv(R,R).transpose() * w1.MB(R); // OPTIMIZE BELOW
        blas::gemv(1.0, mat_inv(R, R).transpose(), w1.MB(R), 0.0, w1.MC(R));

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
        range R(0, N);
        x_values[w1.ireal] = w1.x;

        // modifying M : M ij += w1.ksi Min Cj
        // using Shermann Morrison formula.
        // impl. Cf case 3
        w1.ksi          = -1 / w1.ksi;
        w1.MC(w1.ireal) = 0;
        //mat_inv(R,R) += w1.ksi * mat_inv(R,w1.ireal) * w1.MC(R);
        blas::ger(w1.ksi, mat_inv(R, w1.ireal), w1.MC(R), mat_inv(R, R));
        mat_inv(R, w1.ireal) *= -w1.ksi;
      }

      //------------------------------------------------------------------------------------------
      public:
      /**
     * Consider the change the row i and column j and the corresponding x and y
     *
     * Returns the ratio of det Minv_new / det Minv.
     * This routine does NOT make any modification. It has to be completed with complete_operation().
     */
      value_type try_change_col_row(size_t i, size_t j, x_type const &x, y_type const &y) {
        TRIQS_ASSERT(last_try == NoTry);
        TRIQS_ASSERT(j < N);
        TRIQS_ASSERT(j >= 0);
        TRIQS_ASSERT(i < N);
        TRIQS_ASSERT(i >= 0);

        last_try = ChangeRowCol;
        w1.i     = i;
        w1.j     = j;
        w1.ireal = row_num[i];
        w1.jreal = col_num[j];
        w1.x     = x;
        w1.y     = y;

        // Compute the col B.
        for (size_t i = 0; i < N; i++) { // MC :  delta_x, MB : delta_y
          w1.MC(i) = f(x_values[i], y) - f(x_values[i], y_values[w1.jreal]);
          w1.MB(i) = f(x, y_values[i]) - f(x_values[w1.ireal], y_values[i]);
        }
        w1.MC(w1.ireal) = f(x, y) - f(x_values[w1.ireal], y_values[w1.jreal]);
        w1.MB(w1.jreal) = 0;

        range R(0, N);
        // C : X, B : Y
        //w1.C(R) = mat_inv(R,R) * w1.MC(R);// OPTIMIZE BELOW
        blas::gemv(1.0, mat_inv(R, R), w1.MC(R), 0.0, w1.C(R));
        //w1.B(R) = mat_inv(R,R).transpose() * w1.MB(R); // OPTIMIZE BELOW
        blas::gemv(1.0, mat_inv(R, R).transpose(), w1.MB(R), 0.0, w1.B(R));

        // compute the det_ratio
        auto Xn        = w1.C(w1.jreal);
        auto Yn        = w1.B(w1.ireal);
        auto Z         = arrays::dot(w1.MB(R), w1.C(R));
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
        range R(0, N);
        x_values[w1.ireal] = w1.x;
        y_values[w1.jreal] = w1.y;

        // FIXME : Use blas for this ? Is it better
        auto Xn  = w1.C(w1.jreal);
        auto Yn  = w1.B(w1.ireal);
        auto Mnn = mat_inv(w1.jreal, w1.ireal);

        auto D   = w1.ksi;        // get back
        auto a   = -(1 + Yn) / D; // D in the notes
        auto b   = -(1 + Xn) / D;
        auto Z   = arrays::dot(w1.MB(R), w1.C(R)); // FIXME : store this ?
        Z        = Z / D;
        Mnn      = Mnn / D;
        w1.MB(R) = mat_inv(w1.jreal, R); // Mnj
        w1.MC(R) = mat_inv(R, w1.ireal); // Min

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

        size_t s = X.size();
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

        for (size_t i = 0; i < s; ++i)
          for (size_t j = 0; j < s; ++j) w_refill.M(i, j) = f(w_refill.x_values[i], w_refill.y_values[j]);
        range R(0, s);
        newdet  = arrays::determinant(w_refill.M(R, R));
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

        if (N > Nmax) reserve(2 * N);
        std::swap(x_values, w_refill.x_values);
        std::swap(y_values, w_refill.y_values);

        row_num.resize(N);
        col_num.resize(N);
        std::iota(row_num.begin(), row_num.end(), 0);
        std::iota(col_num.begin(), col_num.end(), 0);

        range R(0, N);
        mat_inv(R, R) = inverse(w_refill.M(R, R));
      }

      //------------------------------------------------------------------------------------------
      private:
      void _regenerate_with_check(bool do_check, double precision_warning, double precision_error) {
        if (N == 0) {
          det  = 1;
          sign = 1;
          return;
        }

        range R(0, N);
        matrix_type res(N, N);
        for (int i = 0; i < N; i++)
          for (int j = 0; j < N; j++) res(i, j) = f(x_values[i], y_values[j]);
        det = arrays::determinant(res);

        if (is_singular()) TRIQS_RUNTIME_ERROR << "ERROR in det_manip regenerate: Determinant is singular";
        res = inverse(res);

        if (do_check) { // check that mat_inv is close to res
          const bool relative = true;
          double r            = max_element(abs(res - mat_inv(R, R)));
          double r2           = max_element(abs(res + mat_inv(R, R)));
          bool err            = !(r < (relative ? precision_error * r2 : precision_error));
          bool war            = !(r < (relative ? precision_warning * r2 : precision_warning));
          if (err || war) {
            std::cerr << "matrix  = " << matrix() << std::endl;
            std::cerr << "inverse_matrix = " << inverse_matrix() << std::endl;
          }
          if (war)
            std::cerr << "Warning : det_manip deviation above warning threshold "
                      << "check "
                      << "N = " << N << "  "
                      << "\n   max(abs(M^-1 - M^-1_true)) = " << r
                      << "\n   precision*max(abs(M^-1 + M^-1_true)) = " << (relative ? precision_warning * r2 : precision_warning) << " "
                      << std::endl;
          if (err) TRIQS_RUNTIME_ERROR << "Error : det_manip deviation above critical threshold !! ";
        }

        // since we have the proper inverse, replace the matrix and the det
        mat_inv(R, R) = res;
        n_opts        = 0;

        // find the sign (there must be a better way...)
        double s = 1.0;
        arrays::matrix<double> m(N, N);
        m() = 0.0;
        for (int i = 0; i < N; i++) m(i, row_num[i]) = 1;
        s *= arrays::determinant(m);
        m() = 0.0;
        for (int i = 0; i < N; i++) m(i, col_num[i]) = 1;
        s *= arrays::determinant(m);
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
          case (Insert2): complete_insert2(); break;
          case (Remove2): complete_remove2(); break;
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
      value_type insert(size_t i, size_t j, x_type const &x, y_type const &y) {
        auto r = try_insert(i, j, x, y);
        complete_operation();
        return r;
      }

      /// Insert_at_end (try_insert + complete)
      value_type insert_at_end(x_type const &x, y_type const &y) { return insert(N, N, x, y); }

      /// Insert2 (try_insert2 + complete)
      value_type insert2(size_t i0, size_t i1, size_t j0, size_t j1, x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
        auto r = try_insert2(i0, i1, j0, j1, x0, x1, y0, y1);
        complete_operation();
        return r;
      }

      /// Insert2_at_end (try_insert2 + complete)
      value_type insert2_at_end(x_type const &x0, x_type const &x1, y_type const &y0, y_type const &y1) {
        return insert2(N, N + 1, N, N + 1, x0, x1, y0, y1);
      }

      /// Remove (try_remove + complete)
      value_type remove(size_t i, size_t j) {
        auto r = try_remove(i, j);
        complete_operation();
        return r;
      }

      /// Remove_at_end (try_remove + complete)
      value_type remove_at_end() { return remove(N - 1, N - 1); }

      /// Remove2 (try_remove2 + complete)
      value_type remove2(size_t i0, size_t i1, size_t j0, size_t j1) {
        auto r = try_remove2(i0, i1, j0, j1);
        complete_operation();
        return r;
      }

      /// Remove2_at_end (try_remove2 + complete)
      value_type remove2_at_end() { return remove2(N - 1, N - 2, N - 1, N - 2); }

      /// change_col (try_change_col + complete)
      value_type change_col(size_t j, y_type const &y) {
        auto r = try_change_col(j, y);
        complete_operation();
        return r;
      }

      /// change_row (try_change_row + complete)
      value_type change_row(size_t i, x_type const &x) {
        auto r = try_change_row(i, x);
        complete_operation();
        return r;
      }

      value_type change_one_row_and_one_col(size_t i, size_t j, x_type const &x, y_type const &y) {
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
        size_t tmp;
        const int_type NN = N;
        switch (roll) {
          case (None): return 1;
          case (Down):
            tmp = row_num[N - 1];
            for (int_type i = NN - 2; i >= 0; i--) row_num[i + 1] = row_num[i];
            row_num[0] = tmp;
            break;
          case (Up):
            tmp = row_num[0];
            for (int_type i = 0; i < N - 1; i++) row_num[i] = row_num[i + 1];
            row_num[N - 1] = tmp;
            break;
          case (Right):
            tmp = col_num[N - 1];
            for (int_type i = NN - 2; i >= 0; i--) col_num[i + 1] = col_num[i];
            col_num[0] = tmp;
            break;
          case (Left):
            tmp = col_num[0];
            for (int_type i = 0; i < N - 1; i++) col_num[i] = col_num[i + 1];
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
  } // namespace det_manip
} // namespace triqs
