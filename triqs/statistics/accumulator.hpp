/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, H. Strand, P. Dumitrescu
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
#include <type_traits>
#include <vector>
#include <triqs/h5/h5_serialize.hpp>
#include <triqs/utility/itertools.hpp>
#include <triqs/mpi/vector.hpp>

namespace triqs {
  template <typename T> struct has_element_wise_multiply : std::true_type {};

} // namespace triqs

namespace triqs::stat {

  /**
   * mc_value<T> contains various analysis for T, with a dynamical choice.
   *
   * T : any regular type
   *
   */
  template <typename T> struct mc_value {

    using V = std::vector<T>;

    T value;
    std::optional<T> variance;
    std::optional<V> auto_correlation;

    template <typename F> void h5_serialize(F &&f) { f("value", value), f("variance", variance), f("auto_correlation", auto_correlation); }

    /**
     *     @return HDF5 scheme name
     */
    static std::string hdf5_scheme() { return "mc_value"; }
  }; // namespace triqs::stat

  namespace details {
    // ************************************************************************************************
    template <typename T> struct lin_binning {

      T acc;               // accumulator
      long bin_size   = 1; // size of all the completed bins
      long max_n_bins = 0; // maximum number of bins before compression. 0 : no bins, -1 : unlimited.
      std::vector<T> bins; // completed bins
      long count = 0;      // counter associated to the accumulator

      template <typename F> void h5_serialize(F &&f) {
        f("bins", bins), f("acc", acc);
        f("count", count, h5::as_attribute), f("bin_size", bin_size, h5::as_attribute), f("max_n_bins", max_n_bins, h5::as_attribute);
      }
      static std::string hdf5_scheme() { return "lin_binning"; }

      lin_binning() = default;

      // Precondition : n_bins >= 0
      lin_binning(T model, long bin_size, long max_n_bins) : acc(std::move(model)), bin_size(bin_size), max_n_bins(max_n_bins) { acc = 0; }

      void advance() {
        ++count;
        if (max_n_bins == 0) return;
        if (count == bin_size) {
          if (long(bins.size()) == max_n_bins) compress();
          bins.push_back(acc / count);
          acc   = 0;
          count = 0;
        }
      }

      T sum_all() const {
        T r = acc;
        r   = 0;
        for (auto const &x : bins) { r += x; }
        r = bin_size * r + acc;
        return r;
      }

      private:
      // simple compression.
      void compress() {
        const int L = bins.size() / 2;
        bins[0] /= 2;
        bins[0] += bins[1] / 2;
        for (int i = 1; i < L; ++i) { bins[i] = (bins[2 * i] + bins[2 * i + 1]) / 2; }
        bins.resize(L); // drop empty bins but capacity is intact
        bin_size *= 2;
      }
    };

    // ************************************************************************************************

    // Formula
    // FIXME : add the ref.
    // Mk \equiv \frac{1}{k} \sum_{i=1}^k x_i
    // Qk \equiv \sum_{i=1}^k (x_i - M_k)^2
    // Update formula
    // M_{k} = M_{k-1} + \frac{x- M_{k-1}}{k}
    // Q_{k} = Q_{k-1} + \frac{(k-1)(x- M_{k-1})^2}{k}

    template <typename T> struct log_binning {

      std::vector<T> Qk, Mk;      // Cf comments
      std::vector<T> acc;         // partial accumulators at size 2^(n+1). WARNING : acc[n] correspond to sum[n+1] to save acc[0] which is unused.
      std::vector<int> acc_count; // number of elements for partial accumulators at size 2^(n+1)
      long count = 0;             // Number of elements summed in is Qk[0]

      template <typename F> void h5_serialize(F &&f) {
        f("Qk", Qk), f("Mk", Mk), f("acc", acc), f("count", count, h5::as_attribute), f("acc_count", acc_count);
      }
      static std::string hdf5_scheme() { return "log_binning"; }

      log_binning() = default;

      // Precondition : n_bins >= 0
      log_binning(T model, int n_bins) {
        model = 0;
        Qk.resize(n_bins, T{model * model});
        Mk.resize(n_bins, model);
        acc.resize(n_bins, model);
        acc_count.resize(n_bins, 0);
      }

      long n_bins() const { return Qk.size(); }

      void print() const {
        std::cout << "count = " << count << std::endl;
        for (int n = 0; n < Qk.size(); ++n) {
          std::cout << " n = " << n << " sq " << Qk[n];
          std::cout << " acc " << acc[n] << "  acc_count " << acc_count[n] << std::endl;
        }
      }

      void advance() {
        if (acc.size() == 0) return;
        ++count;
        // go up in n as long as the acc_count becomes full and add the acc in the
        // then go down, and store the acc
        int n = 1;
        for (; n < acc.size(); ++n) {
          acc[n] += acc[n - 1];
          if (++(acc_count[n]) < 2) break;
        }
        --n;
        for (; n >= 0; n--) {
          auto bin_size = (1ul << n);                  // 2^(n)
          T x_m         = (acc[n] / bin_size - Mk[n]); // Force T if expression template.
          auto k        = count / bin_size;
          Qk[n] += ((k - 1) / double(k)) * conj_r(x_m) * x_m; 
          Mk[n] += x_m / k;
          acc_count[n] = 0;
          acc[n]       = 0;
        }
      }
    };

  } // namespace details

  // ************************************************************************************************

  /**
   *
   * T : any regular type with additional constraint 
   *   T x; x=0;  
   *
   */
  template <typename T> class accumulator {
    static_assert(has_element_wise_multiply<T>::value,
                  "accumulator only works with type which are multiplied elementwise : no matrix, no matrix_valued gf here.");
    details::log_binning<T> log_bins;
    details::lin_binning<T> lin_bins;

    template <typename F> void h5_serialize(F &&f) {
      f("log_bins", log_bins);
      f("lin_bins", lin_bins);
    }
    friend class triqs::h5::access;

    public:
    accumulator() = default;

    accumulator(T model, int n_log_bins = 0, int n_lin_bins_max = 0, int lin_bin_size = 1)
       : log_bins{model, n_log_bins}, //
         lin_bins{std::move(model), lin_bin_size, n_lin_bins_max} {
      if ((n_lin_bins_max != -1) && (n_lin_bins_max % 2 != 0)) throw std::invalid_argument("max_n_bins must be even");
    }

    void advance() {
      log_bins.advance();
      lin_bins.advance();
    }

    template <typename U> void operator<<(U const &x) {
      (*this) += x;
      advance();
    }

    int n_log_bins() const { return log_bins.n_bins(); }

    template <typename U> FORCEINLINE void operator+=(U const &u) {
      lin_bins.acc += u;
      if (n_log_bins() > 0) log_bins.acc[0] += u;
    }

    private:
    // ---------------- lazy expressions. delays [] and () ----------------------
    // As a result, for e.g. accumulator<array<T, 2>>, one can write :  a(i,j) += x;
    // for accumulator<gf<imfreq>> :   a[tau](i,j) +=x;
    // A mini expression template technique with 2 nodes, and an evaluation
    // the context (0,1) corresponds the arrays to be modified.
    template <char B, typename LHS, typename... RHS> struct lazy_expr {
      LHS lhs;
      std::tuple<RHS...> rhs;

      template <typename... U> FORCEINLINE lazy_expr<'P', lazy_expr, U...> operator()(U &&... u) { return {*this, {std::forward<U>(u)...}}; }
      template <typename U> FORCEINLINE lazy_expr<'B', lazy_expr, U> operator[](U &&u) { return {*this, {std::forward<U>(u)}}; }

      template <int Context> FORCEINLINE decltype(auto) eval() {
        if constexpr (B == 'P') { return std::apply(lhs.template eval<Context>(), rhs); }
        if constexpr (B == 'B') { return lhs.template eval<Context>()[std::get<0>(rhs)]; }
      }

      template <typename U> FORCEINLINE void operator+=(U const &u) {
        eval<0>() += u;
        if (n_log_bins() > 0) eval<1>() += u;
      }

      FORCEINLINE int n_log_bins() { return lhs.n_log_bins(); }

    }; //--- end  struct lazy_expr

    template <int Context> FORCEINLINE auto &eval() {
      if constexpr (Context == 0) { return lin_bins.acc; }
      if constexpr (Context == 1) { return log_bins.acc[0]; }
    }

    public:
    template <typename... U> FORCEINLINE lazy_expr<'P', accumulator &, U...> operator()(U &&... u) { return {*this, {std::forward<U>(u)...}}; }
    template <typename U> FORCEINLINE lazy_expr<'B', accumulator &, U> operator[](U &&u) { return {*this, {std::forward<U>(u)}}; }

    // -------- END lazy expressions. delays [] and () ---------

    /// @return HDF5 scheme name
    static std::string hdf5_scheme() { return "accumulator"; }

    //
    std::vector<T> auto_corr_variances(mpi::communicator c) const {
      std::vector<T> res1 = log_bins.Qk;
      for (int n = 0; n < res1.size(); ++n) {
        long count_n = (log_bins.count >> n); // /2^n
        if (count_n <= 1)
          res1[n] = 0;
        else
          res1[n] /= count_n * (count_n - 1);
      }
      std::vector<T> res = mpi_reduce(res1, c);
      for (auto &x : res) x /= c.size();
      return res;
    }

    ///Empirical average
    T empirical_average(mpi::communicator c) const {
      auto total_sum1 = lin_bins.sum_all();
      auto total_sum  = T{mpi_reduce(total_sum1, c)}; // FIXME : defect of mpi_reduce ... take
      return total_sum / lin_bins.count;
    }

    /// The linear bins on the node
    std::vector<T> const &linear_bins() const { return lin_bins.bins; }
  }; // namespace triqs::stat

  // Given any vector, compute the estimate
  template <typename V> auto tau_estimates(V const &v, long n) { return 0.5 * (v[n] / v[0] - 1); }

} // namespace triqs::stat
