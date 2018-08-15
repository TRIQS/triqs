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
#include <mpi/vector.hpp>
#include <optional>
#include <triqs/clef/clef.hpp>
#include <triqs/h5/h5_serialize.hpp>
#include <type_traits>
#include <vector>

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

      T acc;               //<! Accumulator: adds input data until flushing average to bin 
      long count = 0;      //<! Number of data points currently in accumulator
      long bin_size   = 1; //<! Size of a bin
      long max_n_bins = 0; //<! Max number of bins before compression. If < 0 : unlimited.
      std::vector<T> bins; //<! Completed bins, which stores the data

      template <typename F> void h5_serialize(F &&f) {
        f("bins", bins), f("acc", acc);
        f("count", count, h5::as_attribute), f("bin_size", bin_size, h5::as_attribute), f("max_n_bins", max_n_bins, h5::as_attribute);
      }
      static std::string hdf5_scheme() { return "lin_binning"; }

      lin_binning() = default;

      lin_binning(T data_instance, long bin_size, long max_n_bins, bool init_zero) :
            bin_size(bin_size), max_n_bins(max_n_bins) { 
             if(max_n_bins != 0){
                if ((max_n_bins > 1) && (max_n_bins % 2 != 0)) throw std::invalid_argument("max_n_bins must be even");
                acc = std::move(data_instance);
                if(init_zero == true){ acc = 0; }
             }
           }

      void advance() {
        if (max_n_bins == 0) return;
        ++count;
        if (static_cast<long>(bins.size()) == max_n_bins) compress();
        if (count == bin_size) {
          bins.push_back(acc / count);
          acc   = 0;
          count = 0;
        }
      }

      //! Halves the number of bins by pairwise averaging of data
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
    // Updating Formulae
    // Mk \equiv \frac{1}{k} \sum_{i=1}^k x_i
    // Qk \equiv \sum_{i=1}^k (x_i - M_k)^2
    // Update formula
    // M_{k} = M_{k-1} + \frac{x- M_{k-1}}{k}
    // Q_{k} = Q_{k-1} + \frac{(k-1)(x- M_{k-1})^2}{k}
    // Ref: see e.g. Chan, Golub, LeVeque. Am. Stat. 37, 242 (1983) and therein

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
      log_binning(T data_instance, int n_bins) {
        if(n_bins != 0){
          data_instance = 0;
          Qk.resize(n_bins, T{data_instance * data_instance});
          Mk.resize(n_bins, data_instance);
          acc.resize(n_bins, data_instance);
          acc_count.resize(n_bins, 0);
        }
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
        using triqs::arrays::conj_r;

        if (n_bins() == 0) return;
        ++count;
        // go up in n as long as the acc_count becomes full and add the acc in the
        // then go down, and store the acc
        int n = 1;
        for (; n < acc.size(); ++n) {
          acc[n] += acc[n - 1];
          ++(acc_count[n]);
          if (acc_count[n] < 2) break;
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

    accumulator(T data_instance, int n_log_bins = 0, int n_lin_bins_max = 0,
                int lin_bin_size = 1, bool init_zero = true)
       : log_bins{data_instance, n_log_bins}, //
         lin_bins{std::move(data_instance), lin_bin_size, n_lin_bins_max, init_zero} {
    }

    void advance() {
      log_bins.advance();
      lin_bins.advance();
    }

    int n_log_bins()     const { return log_bins.n_bins(); }

    int n_lin_bins_max() const { return lin_bins.max_n_bins; }

    template <typename U> void operator<<(U const &x) {
      (*this) += x;
      advance();
    }

    template <typename U> FORCEINLINE void operator+=(U const &u) {
      if (n_lin_bins_max() != 0) lin_bins.acc += u;
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
        if (n_lin_bins_max() != 0) eval<0>() += u;
        if (n_log_bins() > 0) eval<1>() += u;
      }

      FORCEINLINE int n_log_bins() { return lhs.n_log_bins(); }
      FORCEINLINE int n_lin_bins_max() const { return lhs.n_lin_bins_max(); }

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

    //! Returns the standard errors for different power-of-two bin sizes
    std::vector<T> auto_corr_variances(mpi::communicator c) const {
      if( n_log_bins() == 0 ){return std::vector<T>(); } // FIXME: Stops MPI Crashing on reducing empty
      std::vector<T> res1 = log_bins.Qk;
      for (int n = 0; n < res1.size(); ++n) {
        long count_n = (log_bins.count >> n); // /2^n
        if (count_n <= 1)
          res1[n] = 0;
        else
          res1[n] /= count_n * (count_n - 1);
      }
      std::vector<T> res = mpi_reduce(res1, c);
      using std::sqrt;
      for (auto &x : res) {
        x /= c.size();
        x = sqrt(x);
      }
      return res;
    }

    //! Returns the bins from the linear binning
    std::vector<T> const &linear_bins() const { 
      return lin_bins.bins;
    }
  }; // namespace triqs::stat

  //! Helper function to computse auto-correlation time estimate from vector of 
  //! standard errors.
  template <typename V> auto tau_estimates(V const &v, long n) { 
    return 0.5 * ((v[n] * v[n]) / (v[0] * v[0]) - 1); 
    }

} // namespace triqs::stat

