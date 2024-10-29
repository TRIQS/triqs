// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

#pragma once

#include "./lin_binning.hpp"
#include "./log_binning.hpp"
#include "./utils.hpp"
#include "mean_error.hpp"
#include <mpi/mpi.hpp>
#include <mpi/vector.hpp>
#include <optional>
#include <triqs/arrays.hpp>
#include <nda/clef/clef.hpp>
#include <h5/h5.hpp>
#include <tuple>
#include <type_traits>
#include <vector>
#include <algorithm>

namespace triqs::stat {

  namespace details {

    template <typename T> auto mpi_reduce_MQ(const T &Mi, const get_real_t<T> &Qi, const long &count_i, mpi::communicator c, int root = 0) {
      using nda::conj;
      using nda::real;

      long count_total  = mpi::all_reduce(count_i, c);
      double count_frac = double(count_i) / count_total;

      T M = Mi * count_frac;
      mpi::all_reduce_in_place(M, c);

      T diff          = Mi - M;
      get_real_t<T> Q = Qi + count_i * make_real(conj(diff) * diff);

      mpi::reduce_in_place(Q, c, root);

      return std::make_tuple(M, Q, count_total);
    }

  } // namespace details

  /// The class takes in measurements during a Monte Carlo simulation and serves a dual purpose:
  ///
  /// (a) It can estimate the auto-correlation time of the data (so-called "logarithmic binning").
  /// (b) It can average groups of consecutive measurements and store the final data (so-called "linear binning").
  ///
  /// The accumulators can be configured to perform either or both of these purposes, as needed. The design is
  /// purposefully flexible -- it is useful both where the auto-correlation time \f$\tau\f$ of the data is known as well as
  /// when it has to be estimated during the simulation.
  ///
  /// Logarithmic (Log) Binning
  /// -------------------------
  ///
  /// This part of the accumulator is used to estimate the autocorrelation time of the data, by calculating the standard
  /// error of the data binned with different bin sizes. For correlated data, the error should grow as the bin size
  /// increases up to the autocorrelation time, where it saturates.
  ///
  /// The log binning uses bin sizes that are powers of two \f$2, 4, 8, 16, \ldots\f$ up to a user-defined maximum. Note
  /// that binning is performed only once there is at one full bin of data at a given size -- any partial accumulation
  /// is not considered. In the the end, one can obtain the list of standard errors for the different levels of binning;
  /// this should be analyzed to see if saturation with size has occurred.
  ///
  /// .. note:: Underestimating the auto-correlation time and a lack of ergodicity are common sources of systematic
  /// errors in Monte Carlo simulations. Provided the computational and memory costs are acceptable, it is advisable to
  /// always turn on logarithmic binning to check that \f$\tau\f$ was estimated correctly and acts as expected.
  ///
  /// Linear (Lin) Binning
  /// --------------------
  ///
  /// This part of the accumulator is responsible for partially averaging (binning) and storing the data. It takes in
  /// data and accumulates it in a bin up to a user-defined bin-size. At every point, a bin stores the mean value of the
  /// data within it bin. If the bin size is larger then the auto-correlation time \f$\tau\f$ of the measurement, the bins
  /// are independent statistical samples, which can be used for further analysis.
  ///
  ///
  /// @brief Bins and analyzes correlated data
  template <typename T> class accumulator {
    private:
    long count = 0;
    log_binning<T> log_bins;
    lin_binning<T> lin_bins;
    std::vector<T> cb_means;
    std::vector<get_real_t<T>> cb_errors;
    std::vector<get_real_t<T>> cb_taus; // will be filled by the default callback if n_log_bins > 0

    // HDF5
    friend void h5_write(h5::group g, std::string const &name, accumulator<T> const &l) {
      auto gr = g.create_group(name);
      h5_write(gr, "log_bins", l.log_bins);
      h5_write(gr, "lin_bins", l.lin_bins);
      h5_write(gr, "count", l.count);
    }

    friend void h5_read(h5::group g, std::string const &name, accumulator<T> &l) {
      auto gr = g.open_group(name);
      h5_read(gr, "log_bins", l.log_bins);
      h5_read(gr, "lin_bins", l.lin_bins);
      h5_read(gr, "count", l.count);
    }

    public:
    [[nodiscard]] static std::string hdf5_format() { return "accumulator"; }

    accumulator() = default;
    auto const &auto_correlation_times() const { return cb_taus; }

    ///
    /// @tparam T
    ///
    ///   Type of object to be accumulated. Pre-requisites:
    ///    * T is a regular type
    ///    * T can be set to zero with: T x=0
    ///
    ///    * T has a multiplication operator (x * x) defined in an element-wise manner
    ///    * T can be made real using: nda::real(T)
    ///
    /// @param data_instance
    ///
    ///   An instance of the data type T that will be accumulated. This will be copied and zeroed to initialize the
    ///   linear and logarithmic parts. Should be set-up so that further data can be correctly added to it (e.g. have the
    ///   right dimensions and size for an array).
    ///
    /// @param n_log_bins_max The maximum number of bins to be kept in the logarithmic binning. Possible values are:
    ///
    ///   * n_log_bins_max == 0: turns off logarithmic binning.
    ///   * n_log_bins_max > 0: finite number of bins; the capacity of the largest bin is \f$2^{\texttt{n_log_bins_max}}\f$.
    ///   * n_log_bins_max < 0: unbounded number of bins. A new bin of capacity \f$2^m\f$ get created as soon as there are \f$2^m\f$ measurements available.
    ///
    /// @param n_lin_bins_max The maximum number of data points to be kept by the linear bin. Possible values are:
    ///
    ///   * n_lin_bins_max == 0: turns off linear binning.
    ///   * n_lin_bins_max == 1: when there is only a single linear bin, the accumulator ignores lin_bin_capacity. This is so that all no data which is passed to the accumulator is ignored.
    ///   * n_lin_bins_max > 1: imposes a finite maximum bin number, causes automatic compression[REF] of the data when all bins are filled and additional data is being passed to the accumulator
    ///   * n_lin_bins_max < 0: unbounded number of bins. A new bin is created when all current bins have reached capacity.
    ///
    /// @param lin_bin_capacity The number of measurements the linear part will average together in a single bin, before starting a new bin.
    ///
    accumulator(T const &data_instance, int n_log_bins_max = 0, int n_lin_bins_max = 0, int lin_bin_capacity = 1,
                std::function<void(lin_binning<T> const & /* lin_bins */)> callback = {})
       : log_bins{data_instance, n_log_bins_max}, //
         lin_bins{data_instance, n_lin_bins_max, lin_bin_capacity, (callback ? callback : [this](lin_binning<T> const &lb) {
                    auto [m, e, t] = lb.mean_error_and_tau();
                    cb_means.push_back(m);
                    cb_errors.push_back(e);
                    cb_taus.push_back(t);
                  })} {}

    /// Returns the maximum number of bins the logarithmic part of the accumulator can hold.
    /// @brief Max. number of bins in the logarithmic accumulator
    /// @return Maximum number of bins
    [[nodiscard]] int n_log_bins_max() const { return log_bins.max_n_bins(); }

    /// Returns the number of bins currently in the logarithmic part of the accumulator
    /// When the accumulator is active (n_log_bins_max != 0), there is always at least one zeroed bin even if no data has been passed to the accumulator.
    /// @brief Number of bins in the logarithmic accumulator
    /// @return Number of bins
    /// @example triqs/stat/acc_nlogbin.cpp
    [[nodiscard]] int n_log_bins() const { return log_bins.n_bins(); }

    /// Returns the maximum number of bins the linear part of the accumulator can hold.
    /// @brief Max. number of bins in the linear accumulator
    /// @return Maximum number of bins
    [[nodiscard]] int n_lin_bins_max() const { return lin_bins.max_n_bins(); }

    /// Returns the number of bins currently in the linear part of the accumulator.
    /// When the accumulator is active (n_lin_bins_max != 0), there is always at least one zeroed bin even if no data has been passed to the accumulator.
    /// @brief Number of bins in the linear accumulator
    /// @return Number of bins
    /// @example triqs/stat/acc_nlinbin.cpp
    [[nodiscard]] int n_lin_bins() const { return lin_bins.n_bins(); }

    /// Returns the current capacity of a linear bin. This is number of measurements that will be averaged in a single linear bin, until the next bin is started.
    /// The capacity increases when the linear bins are compressed, either :ref:`manually <accumulator_compress_linear_bins>` or automatically when reaching the maximum number of bins [REF?].
    ///
    /// When there is only a single bin [:ref:`n_lin_bins() <accumulator_n_lin_bins>` == 1], this parameter is ignored in order to avoid data loss.
    ///
    /// @brief Capacity of a linear bin
    /// @return Bin capacity
    [[nodiscard]] int lin_bin_capacity() const { return lin_bins.bin_capacity(); }

    /// Input a measurement into the accumulator. This measurement is then added to the linear and logarithmic binning parts, unless a part as been turned off (lin_bin_size = 0 or log_bin_size = 0).
    ///
    /// @brief Input a measurement into the accumulator
    /// @tparam U type of the object to be added to the the accumulator.
    ///           This is often the same as type **T** as was used to define the accumulator, but might be more general. The user should ensure that the object passed can be added to the accumulator, else an error will occur.
    /// @param x object to be added to the accumulator
    /// @returns Returns the current accumulator so that :code:`<<` operations can be chained together
    /// @example triqs/stat/acc_data_entry.cpp
    template <typename U> accumulator<T> &operator<<(U const &x) {
      ++count;
      log_bins << x;
      lin_bins << x;
      return *this;
    }

    /// Returns the standard errors for data with different power-of-two capacity.
    /// @return std::vector, where element v[n] contains the standard error of data binned with a bin capacity of \f$2^n\f$. The return type is deduced from nda::real(T), where T is the type defining the accumulator.
    /// @brief Get standard errors of log binned data
    [[nodiscard]] auto log_bin_errors() const {
      auto [m, e, t, ct] = log_bins.mean_errors_and_taus();
      if (e.size() > 0 && ct.back() < 2) e.back() = 0.0;
      return std::make_pair(e, ct);
    }

    /// Returns the standard errors for data with different power-of-two capacity, reduced from data over all MPI threads. The final answer is reduced only to the zero MPI thread (not all reduce).
    /// @param c TRIQS MPI communicator
    /// @return std::vector, where element v[n] contains the standard error of data binned with a bin capacity of \f$2^n\f$. The return type is deduced from nda::real(T), where T is the type defining the accumulator. Reduced only to zero MPI thread.
    /// @brief Get standard errors of log binned data (MPI Version)
    ///
    [[nodiscard]] auto log_bin_errors_all_reduce(mpi::communicator c) const {
      auto [m, e, t, ct] = log_bins.mean_errors_and_taus(c);
      if (e.size() > 0 && ct.back() < 2) e.back() = 0.0;
      return std::make_pair(e, ct);
    }

    /// Returns the total number of data points that were put into the accumulator
    /// @brief Number of data points put into the accumulator
    /// @return Number of data points
    [[nodiscard]] auto data_input_count() const { return count; }

    /// Returns vector with data stored from linear binning
    /// @brief Returns data stored from linear binning
    /// @return Vector with the data of type T, which defines the accumulator
    /// @example triqs/stat/acc_linear_bins.cpp
    [[nodiscard]] std::vector<T> const &linear_bins() const { return lin_bins.bins(); }

    /// Increases the capacity of each linear bin by a integer scaling factor and compresses all the data into the smallest number of bins with the new capacity.
    /// @brief Increases linear bin capacity and compresses data within
    /// @param compression_factor Scaling factor by which to increase capacity; if < 2 nothing is done.
    /// @example triqs/stat/acc_compress_manual.cpp
    void compress_linear_bins(int compression_factor) { lin_bins.compress(compression_factor); }
  };

} // namespace triqs::stat
