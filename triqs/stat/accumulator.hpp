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
  namespace details {

    template <typename T> struct lin_binning {
      long max_n_bins     = 0; // Maximum number of bins
      long bin_capacity   = 1; // Current capacity of each bin (must be >= 1)
      long last_bin_count = 0; // Number of data points the currently active bin [bins.back()]
      std::vector<T> bins;     // Bins with accumulated data (stores means)

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
        // Check if all bins are full and compress if needed
        if (max_n_bins > 1 && n_bins() == max_n_bins && last_bin_count == bin_capacity) {
          compress(2); // Adjusts bin_capacity & last_bin_count
        }
        // Check if current bin full: push new bin or add data to current bin
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
        // Compress data into new bins, except the last new bin
        for (int i = 0; i < n_bins_new - 1; ++i) {
          bins[i] = std::move(bins[compression_factor * i]); // TODO: i = 0 case
          for (int j = 1; j < compression_factor; j++) { bins[i] += bins[compression_factor * i + j]; }
          bins[i] /= compression_factor;
        }
        // Last new bin is special: last old bin could be filled below cpacity
        int new_last_bin_count = last_bin_count + (n_bins_last_chunck - 1) * bin_capacity;
        auto &new_last_bin     = bins[n_bins_new - 1];
        new_last_bin           = std::move(bins[compression_factor * (n_bins_new - 1)]);
        // If n_bins_last_chunck == 1, we have already copied its value above
        if (n_bins_last_chunck > 1) {
          for (int j = 1; j < n_bins_last_chunck - 1; j++) { new_last_bin += bins[compression_factor * (n_bins_new - 1) + j]; }
          new_last_bin *= bin_capacity; // full bins in last chunk
          new_last_bin += bins[compression_factor * (n_bins_new - 1) + (n_bins_last_chunck - 1)] * last_bin_count;
          new_last_bin /= new_last_bin_count;
        }
        // Adjust final parameters
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

  /// The class takes in measurements during a Monte Carlo simulation and serves a dual purpose:
  ///
  /// (a) It can estimate the auto-correlation time of the data (so-called "logarithmic binning").
  /// (b) It can averge groups of consequtive measurements and store the final data (so-called "linear binning").
  ///
  /// The accumulators can be configured to perform either or both of these purposes, as needed. The design is purposefully flexible -- it is useful both where the auto-correlation time $\tau$ of the data is known as well as when it has to be estimated during the simulation.
  ///
  /// Logarithmic (Log) Binning
  /// -------------------------
  ///
  /// This part of the accumulator is used to estimate the autocorrelation time of the data, by calculating the standard error of the data binned with different bin sizes. For correlated data, the error should grow as the bin size increases up to the autocorrelation time, where it saturates [LINK].
  ///
  /// The log binning uses bin sizes that are powers of two $2, 4, 8, 16, \ldots$ up to a user-defined maximum. Note that binning is performed only once there is at one full bin of data at a given size -- any partial accumulation is not considered. In the the end, one can obtain the list of standard errors for the different levels of binning; this should be analyzed to see if saturation with size has occurred.
  ///
  /// .. note:: Underestimating the auto-correlation time and a lack of ergodicity are common sources of systematic errors in Monte Carlo simulations. Provided the computational and memory costs are acceptable, it is advisable to always turn on logarithmic binning to check that $\tau$ was estimated correctly and acts as expected.
  ///
  /// Linear (Lin) Binning
  /// --------------------
  ///
  /// This part of the accumulator is responsible for partially averaging (binning) and storing the data. It takes in data and accumulates it in a bin up to a user-defined bin-size. At every point, a bin stores the mean value of the data within it bin. If the bin size is larger then the auto-correlation time $\tau$ of the measurement, the bins are independent statistical samples, which can be used for further analysis.
  ///
  ///
  /// TODO: EXAMPLE!
  /// TODO: Tutorial
  ///
  /// @brief Bins and analyzes correlated data
  /// @include triqs/stat/accumulator.hpp
  template <typename T> class accumulator {
    private:
    details::log_binning<T> log_bins;
    details::lin_binning<T> lin_bins;

    friend class triqs::h5::access;

    template <typename F> void h5_serialize(F &&f) {
      f("log_bins", log_bins);
      f("lin_bins", lin_bins);
    }

    public:
    static std::string hdf5_scheme() { return "accumulator"; }

    accumulator() = default;

    /**
     * @tparam T 
     *
     *   Type of object to be accumulated. Pre-requisites:
     *    * T is a regular type
     *    * T can be set to zero with: T x=0
     *
     *    * T has a multiplication operator (x * x) defined in an element-wise manner
     *    * T can be made real using: triqs::arrays::real(T)
     * 
     * @param data_instance 
     *
     *   An instance of the data type T that will be accumulated.
     *   This will be copied and zeroed to initialize the linear and logarithmic parts. Should be set-up so that further data can be correctly added to it (e.g. have the right dimensions and size for an array).
     * 
     * @param n_log_bins_max The maximum number of bins to be kept in the logarithmic binning. Possible values are:
     *
     *   * n_log_bins_max == 0: turns off logarithmic binning.
     *   * n_log_bins_max > 0: finite number of bins; the capacity of the largest bin is $2^{\texttt{n_log_bins_max}}$.
     *   * n_log_bins_max < 0: unbounded number of bins. A new bin of capacity $2^m$ get created as soon as there are $2^m$ measurements available.
     * 
     * @param n_lin_bins_max The maximum number of data points to be kept by the linear bin. Possible values are:
     *
     *   * n_lin_bins_max == 0: turns off linear binning.
     *   * n_lin_bins_max == 1: when there is only a single linear bin, the accumulator ignores lin_bin_capacity. This is so that all no data which is passed to the accumulator is ignored.
     *   * n_lin_bins_max > 1: imposes a finite maximum bin number, causes automatic compression[REF] of the data when all bins are filled and additional data is being passed to the accumulator
     *   * n_lin_bins_max < 0: unbounded number of bins. A new bin is created when all current bins have reached capacity.
     * 
     * @param lin_bin_capacity The number of measurements the linear part will average together in a single bin, before starting a new bin.
     */
    accumulator(T const &data_instance, int n_log_bins_max = 0, int n_lin_bins_max = 0, int lin_bin_capacity = 1)
       : log_bins{data_instance, n_log_bins_max}, //
         lin_bins{data_instance, n_lin_bins_max, lin_bin_capacity} {}

    /// Returns the maximum number of bins the logarithmic part of the accumulator can hold.
    /// @brief Max. number of bins in the logarithmic accumulator
    /// @return Maximum number of bins
    int n_log_bins_max() const { return log_bins.max_n_bins; }

    /// Returns the number of bins currently in the logarithmic part of the accumulator
    /// When the accumulator is active (n_log_bins_max != 0), there is always at least one zeroed bin even if data has been passed to the accumulator.
    /// @brief Number of bins in the logarithmic accumulator
    /// @return Number of bins
    /// @example triqs/stat/acc_nlogbin.cpp
    int n_log_bins() const { return log_bins.n_bins(); }

    /// Returns the maximum number of bins the linear part of the accumulator can hold.
    /// @brief Max. number of bins in the linear accumulator
    /// @return Maximum number of bins
    int n_lin_bins_max() const { return lin_bins.max_n_bins; }

    /// Returns the number of bins currently in the linear part of the accumulator.
    /// When the accumulator is active (n_lin_bins_max != 0), there is always at least one zeroed bin even if data has been passed to the accumulator.
    /// @brief Number of bins in the linear accumulator
    /// @return Number of bins
    /// @example triqs/stat/acc_nlinbin.cpp
    int n_lin_bins() const { return lin_bins.n_bins(); }

    /// Returns the current cpacaity of a linear bin. This is number of measurements that will be averaged in a single linear bin, until the next bin is started. The capacity increases when the linear bins are compressed, either :ref:`manually <accumulator_compress_linear_bins>` or automatically when reaching the maximum number of bins [REF?].
    ///
    /// When there is only a single bin [:ref:`n_lin_bins() <accumulator_n_lin_bins>` == 1], this parameter is ignored in order to avoid data loss.
    ///
    /// @brief Capacity of a linear bin
    /// @return Bin capacity
    int lin_bin_capacity() const { return lin_bins.bin_capacity; }

    /// Input a measurement into the accumulator. This measurement is then added to the linear and logarithmic binning parts, unless a part as been turned off (lin_bin_size = 0 or log_bin_size = 0).
    ///
    /// @brief Input a measurement into the accumulator
    /// @tparam U type of the object to be added to the the accumulator.
    ///           This is often the same as type **T** as was used to define the accumulator, but might be more general. The user should ensure that the object passed can be added to the accumulator, else an error will occur.
    /// @param x object to be added to the accumulator
    /// @example triqs/stat/acc_data_entry.cpp
    template <typename U> accumulator & operator<<(U const &x) {
      log_bins << x;
      lin_bins << x;
      return *this;
    }

    /// Returns the standard errors for data with different power-of-two capacity.
    /// @return std::vector, where element v[n] contains the standard error of data bined with a bin capacity of $2^n$. The return type is deduced from triqs::arrays::real(T), where T is the type defining the accumulator.
    /// @brief Get standard errors of log binned data
    auto log_bin_errors() const {
      auto res1 = log_bins.Qk;
      if (res1.size() == 0) return res1;
      for (int n = 0; n < res1.size(); ++n) {
        long count_n = (log_bins.count >> n); // /2^n
        if (count_n <= 1) {
          res1[n] = 0;
        } else {
          using std::sqrt;
          res1[n] = sqrt(res1[n] / (count_n * (count_n - 1)));
        }
      }
      return res1;
    }

    /// Returns the standard errors for data with different power-of-two capacity, reduced from data over all MPI threads. The final answer is reduced only to the zero MPI thread (not all reduce).
    /// @param c TRIQS MPI communicator
    /// @return std::vector, where element v[n] contains the standard error of data bined with a bin capacity of $2^n$. The return type is deduced from triqs::arrays::real(T), where T is the type defining the accumulator. Reduced only to zero MPI thread.
    /// @brief Get standard errors of log binned data (MPI Version)
    auto log_bin_errors_mpi(mpi::communicator c) const {
      // FIXME WITH NEW MACHINARY
      // FIXME: MQ can be different lenghts onb different accumualtors...
      // FIXME: Don't create new bins from data
      if (n_log_bins() == 0) { return std::vector<T>(); } // FIXME: Stops MPI Crashing on reducing empty
      auto res1 = log_bins.Qk;
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

    /// Returns vector with data stored from linear binning
    /// @brief Returns data stored from linear binning
    /// @return Vector with the data of type T, which defines the accumulator
    /// @example triqs/stat/acc_linear_bins.cpp
    std::vector<T> const &linear_bins() const { return lin_bins.bins; }

    /// Increases the capacity of each linear bin by a integer scaling factor and compresses all the data into the smallest number of bins with the new capacity.
    /// @brief Increases linear bin capacity and compresses data within
    /// @param compression_factor Scaling factor by which to increase capacity; if < 2 nothing is done.
    /// @example triqs/stat/acc_compress_manual.cpp
    void compress_linear_bins(int compression_factor) { lin_bins.compress(compression_factor); }
  };

  /// Compute estimates for the auto-correlation times $\tau$ log-binned standard errors.
  /// @tparam T regular type which devines element-wise multiplication and division.
  /// @param error_with_binning standard error of measurement with binning $\Delta_n$
  /// @param error_no_binning standard error of measurement with without binning $\Delta_0$
  /// @return Estimate of the auto-correlation time $\tau$, using
  /// $$\tau = \frac{1}{2}\left[\left(\frac{\Delta_n}{\Delta_0}\right)^2 - 1\right]$$
  /// @brief Convert log bin errors in auto-correlation times
  template <typename T> auto tau_estimate_from_errors(T const &error_with_binning, T const &error_no_binning) {
    // Last part is simply 1.0, but could be an array
    return 0.5 * ((error_with_binning * error_with_binning) / (error_no_binning * error_no_binning) - error_no_binning / error_no_binning);
  }

} // namespace triqs::stat
