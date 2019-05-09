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
#include <triqs/arrays.hpp>
#include <triqs/clef/clef.hpp>
#include <triqs/h5/h5_serialize.hpp>
#include <type_traits>
#include <vector>

namespace triqs::stat {

  template <typename V> auto mean(V const &data) {
    // ENSURE(data.size() > 0);
    auto mean_calc = data[0];
    mean_calc      = 0;
    for (auto const &[n, x] : itertools::enumerate(data)) { mean_calc += (x - mean_calc) / (n + 1); }
    return mean_calc;
  }

  template <typename V> auto mean_mpi(mpi::communicator c, V const &data) {
    // ENSURE(data.size() > 0);
    auto M = make_regular(mean(data));
    long N = mpi::all_reduce(data.size(), c);
    M *= double(data.size()) / N;
    mpi::mpi_reduce_in_place(M, c, 0, true); // FIXME == mpi_all_reduce_in_place
    return M;
  }

  template <typename V> auto mean_and_err(V const &data) {
    // ENSURE(data.size() > 0);
    using std::real;
    using std::sqrt;
    using triqs::arrays::conj_r;
    long length    = data.size();
    auto mean_calc = mean(data);
    auto err_calc  = make_regular(real(mean_calc));
    err_calc       = 0;
    for (auto const &x : data) { err_calc += conj_r(x - mean_calc) * (x - mean_calc) / (length * (length - 1)); }
    err_calc = sqrt(err_calc);
    return std::make_pair(mean_calc, err_calc);
  }

  template <typename V> auto mean_and_err_mpi(mpi::communicator c, V const &data) {
    // ENSURE(data.size() > 0);
    using std::real;
    using std::sqrt;
    using triqs::arrays::conj_r;
    long length    = mpi::all_reduce(data.size(), c);
    auto mean_calc = mean_mpi(data);
    auto err_calc  = make_regular(real(mean_calc));
    err_calc       = 0;
    for (auto const &x : data) { err_calc += conj_r(x - mean_calc) * (x - mean_calc) / (length * (length - 1)); }
    mpi::mpi_reduce_in_place(err_calc, c);
    err_calc = sqrt(err_calc);
    return std::make_pair(mean_calc, err_calc);
  }

  namespace details {
    // ************************************************************************************************
    template <typename T> struct lin_binning {

      long max_n_bins     = 0; // Maximum number of bins before compression
      long bin_capacity   = 1; // Target size of a bin (must be >= 1)
      long last_bin_count = 0; // Number of data points the last active bin [bins.back()]
      std::vector<T> bins;     // Bins storing accumulated data (stores means)

      template <typename F> void h5_serialize(F &&f) {
        f("bins", bins), f("last_bin_count", last_bin_count, h5::as_attribute), f("bin_capacity", bin_capacity, h5::as_attribute),
           f("max_n_bins", max_n_bins, h5::as_attribute);
      }
      static std::string hdf5_scheme() { return "lin_binning"; }

      lin_binning() = default;

      lin_binning(T const &data_instance, long max_n_bins, long bin_capacity) : bin_capacity(bin_capacity), max_n_bins(max_n_bins) {
        if (max_n_bins == 0) return;
        T data_instance_local = data_instance;
        data_instance_local   = 0;
        if (max_n_bins > 0) { bins.reserve(max_n_bins); }
        bins.emplace_back(std::move(data_instance_local));
      }

      long n_bins() const { return bins.size(); }

      template <typename U> void operator<<(U &&x) {
        if (max_n_bins == 0) return;
        // Check if all bins full
        if (max_n_bins > 1 && n_bins() == max_n_bins && last_bin_count == bin_capacity) {
          compress(2); // Adjusts last_bin_count internally
        }
        // Check if current bin full: push new vs add to current
        if (last_bin_count == bin_capacity && max_n_bins != 1) {
          bins.emplace_back(std::forward<U>(x));
          last_bin_count = 1;
        } else {
          last_bin_count++;
          bins.back() += (x - bins.back()) / last_bin_count;
        }
      }

      // Compresses bins by scaling up bin_capacity by compression_factor (>= 2).
      void compress(int compression_factor) {
        if (max_n_bins == 0 || compression_factor < 2) return;
        const int bins_left    = n_bins() % compression_factor;
        int n_bins_new         = n_bins() / compression_factor;
        int n_bins_last_chunck = compression_factor;
        if (bins_left != 0) {
          n_bins_new++;
          n_bins_last_chunck = bins_left;
        }
        // Compress all except last bin
        for (int i = 0; i < n_bins_new - 1; ++i) {
          bins[i] = std::move(bins[compression_factor * i]); // TODO: i = 0 case
          for (int j = 1; j < compression_factor; j++) { bins[i] += bins[compression_factor * i + j]; }
          bins[i] /= compression_factor;
        }
        // Weight of new final bin: could be partially full
        int new_last_bin_count = last_bin_count + (n_bins_last_chunck - 1) * bin_capacity;
        auto &new_last_bin     = bins[n_bins_new - 1];
        new_last_bin           = std::move(bins[compression_factor * (n_bins_new - 1)]);
        // If n_bins_last_chunck == 1, we have already copied its value
        if (n_bins_last_chunck > 1) {
          for (int j = 1; j < n_bins_last_chunck - 1; j++) { new_last_bin += bins[compression_factor * (n_bins_new - 1) + j]; }
          new_last_bin *= bin_capacity; // full bins in last chunk
          new_last_bin += bins[compression_factor * (n_bins_new - 1) + (n_bins_last_chunck - 1)] * last_bin_count;
          new_last_bin /= new_last_bin_count;
        }
        // Adjust final Parameters
        bins.resize(n_bins_new);
        last_bin_count = new_last_bin_count;
        bin_capacity *= compression_factor;
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
      std::vector<T> Mk; // Cf comments
      using Q_t = decltype(make_regular(triqs::arrays::real(Mk[0])));
      std::vector<Q_t> Qk; // Cf comments
      int max_n_bins = 0;
      std::vector<T> acc;         // partial accumulators at size 2^(n+1). WARNING : acc[n] correspond to sum[n+1] to save acc[0] which is unused.
      std::vector<int> acc_count; // number of elements for partial accumulators at size 2^(n+1)
      long count = 0;             // Number of elements summed in is Qk[0]

      template <typename F> void h5_serialize(F &&f) {
        f("Qk", Qk), f("Mk", Mk), f("acc", acc), f("count", count, h5::as_attribute), f("acc_count", acc_count);
      }
      static std::string hdf5_scheme() { return "log_binning"; }

      log_binning() = default;

      log_binning(T const &data_instance, int max_n_bins) : max_n_bins(max_n_bins) {
        if (max_n_bins == 0) return;
        T data_instance_local = data_instance;
        data_instance_local   = 0;
        if (max_n_bins > 0) {
          Qk.reserve(max_n_bins);
          Mk.reserve(max_n_bins);
          acc.reserve(max_n_bins);
          acc_count.reserve(max_n_bins);
        }
        Qk.emplace_back(T{data_instance_local * data_instance_local});
        Mk.emplace_back(data_instance_local);
        acc.emplace_back(std::move(data_instance_local));
        acc_count.push_back(0);
      }

      long n_bins() const { return Qk.size(); }

      template <typename U> void operator<<(U const &x) {
        if (max_n_bins == 0) return;
        acc[0] += x;
        acc_count[0]++;
        advance();
      }

      void advance() {
        using triqs::arrays::conj_r;
        ++count;

        // Add space if last element full; stop if max_n_bins reached
        if (count == (1 << (acc.size() - 1)) && acc.size() < max_n_bins) {
          T data_instance = acc[0];
          data_instance   = 0;
          Qk.push_back(T{data_instance * data_instance});
          Mk.push_back(data_instance);
          acc.emplace_back(data_instance);
          acc_count.push_back(0);
        }

        // go up in n as long as the acc_count becomes full and add the acc in the
        // then go down, and store the acc
        int n = 1;
        for (; n < acc.size(); ++n) {
          acc[n] += acc[n - 1];
          acc_count[n]++;
          if (acc_count[n] < 2) break;
        }

        --n;
        for (; n >= 0; n--) {
          auto bin_capacity = (1ul << n);                      // 2^(n)
          T x_m             = (acc[n] / bin_capacity - Mk[n]); // Force T if expression template.
          auto k            = count / bin_capacity;
          Qk[n] += ((k - 1) / double(k)) * conj_r(x_m) * x_m;
          Mk[n] += x_m / k;
          acc_count[n] = 0;
          acc[n]       = 0;
        }
      }
    };

  } // namespace details

  // ************************************************************************************************

  /// accumulator class with bins and stores data from Monte Carlo measurements
  /**
   * This class takes in measurements during the simulation and serves a dual 
   * purpose of (a) estimating the auto-correlation time of the data and 
   * (b) binning the indvidual measurements and holding the final data.
   * 
   * @include triqs/stat/accumulator.hpp
   */
  template <typename T> class accumulator {
    details::log_binning<T> log_bins;
    details::lin_binning<T> lin_bins;

    template <typename F> void h5_serialize(F &&f) {
      f("log_bins", log_bins);
      f("lin_bins", lin_bins);
    }
    friend class triqs::h5::access;

    public:
    accumulator() = default;

    /**
     @tparam T Type of object to be accumulated. This can be any regular type
     provided that it can be set to zero (T x; x=0) and has multiplication defined in an element-wise manner
     @param data_instance An instance of the data type T that will be accumulated.
     @param n_log_bins_max 
     @param n_lin_bins_max 
     @param lin_bin_capacity 
   */
    accumulator(T const &data_instance, int n_log_bins_max = 0, int n_lin_bins_max = 0, int lin_bin_capacity = 1)
       : log_bins{data_instance, n_log_bins_max}, //
         lin_bins{data_instance, n_lin_bins_max, lin_bin_capacity} {}

    // @return Maximum number of bins in the logarithmic accumulator
    int n_log_bins_max() const { return log_bins.max_n_bins; }

    // @return Current number of bins in the linear accumulator
    int n_log_bins() const { return log_bins.n_bins(); }

    // @return Maximum number of bins in the linear accumulator
    int n_lin_bins_max() const { return lin_bins.max_n_bins; }

    // @return Current number of bins in the linear accumulator
    int n_lin_bins() const { return lin_bins.n_bins(); }

    /// Input a measurement into the accumulator
    template <typename U> void operator<<(U const &x) {
      log_bins << x;
      lin_bins << x;
    }

    // FIXME: Where to put this?
    // private:
    /// @return HDF5 scheme name
    static std::string hdf5_scheme() { return "accumulator"; }

    // Returns the standard errors for different power-of-two bin sizes
    auto auto_corr_variances() const {
      auto res1 = log_bins.Qk;
      if (res1.size() == 0) return res1;
      for (int n = 0; n < res1.size(); ++n) {
        long count_n = (log_bins.count >> n); // /2^n
        if (count_n <= 1) {
          res1[n] = 0;
        } else {
          res1[n] = sqrt(res1[n] / (count_n * (count_n - 1)));
        }
      }
      return res1;
    }

    // Returns the standard errors for different power-of-two bin sizes
    std::vector<T> auto_corr_variances_mpi(mpi::communicator c) const {
      // FIXME WITH NEW MACHINARY
      if (n_log_bins() == 0) { return std::vector<T>(); } // FIXME: Stops MPI Crashing on reducing empty
      std::vector<T> res1 = log_bins.Qk;
      for (int n = 0; n < res1.size(); ++n) {
        long count_n = (log_bins.count >> n); // /2^n
        if (count_n <= 1) {
          res1[n] = 0;
        } else {
          using std::sqrt;
          res1[n] = sqrt(res1[n] / (count_n * (count_n - 1)));
        }
      }
      std::vector<T> res = mpi_reduce(res1, c);
      using std::sqrt;
      for (auto &x : res) {
        x /= c.size();
        x = sqrt(x);
      }
      return res;
    }

    // @return Vector with the data from linear binning
    std::vector<T> const &linear_bins() const { return lin_bins.bins; }

    /// Manually increase the size of each linear bin and compress the stored data
    /**
     @param compression_factor Factor by which to scale lin_bin_capacity; if < 2 nothing is done
   */
    void compress_linear_bins(int compression_factor) { lin_bins.compress(compression_factor); }
  }; // namespace triqs::stat

  //! Helper function to computse auto-correlation time estimate from vector of
  //! standard errors.
  template <typename V> auto tau_estimates(V const &v, long n) { return 0.5 * ((v[n] * v[n]) / (v[0] * v[0]) - 1); }

} // namespace triqs::stat
