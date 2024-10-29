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

#include "./mean_error.hpp"
#include "./utils.hpp"

#include <h5/h5.hpp>
#include <nda/h5.hpp>
#include <nda/nda.hpp>

#include <functional>
#include <iterator>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace triqs::stat {

  /**
   * @brief Compress a given number of adjacent bins.
   *
   * @details It simply averages the data of adjacent bins and returns a vector containing those averages. The
   * compression factor determines the number of adjacent bins that are merged together. Left over bins are discarded.
   *
   * It returns a copy of the given bins if the compression factor is less than 2 and it returns an empty vector if the
   * number of bins is smaller than the compression factor.
   *
   * @tparam T triqs::stats::AccCompatible type.
   * @param bins `std::vector` containing the bins to be compressed.
   * @param fac Compression factor.
   * @return Compressed bins.
   */
  template <AccCompatible T> auto compress_bins(std::vector<T> const &bins, int fac) {
    // early return if the compression factor < 2
    if (fac < 2) return bins;

    // prepare the compressed bins
    int const nbins = bins.size() / fac;
    std::vector<T> res(nbins);

    // compress bins
    for (int i = 0; i < nbins; ++i) {
      res[i] = bins[fac * i];
      for (int j = 1; j < fac; ++j) { res[i] += bins[fac * i + j]; }
      res[i] /= fac;
    }
    return res;
  }

  /**
   * @brief Linear binning accumulator.
   *
   * @details Accumulate consecutive data points into either a fixed number of bins with a dynamic bin capacity or a
   * growing number of bins with a fixed bin capacity.
   *
   * The bins are stored and can be accessed at any time to perform further analysis, e.g. to estimate the error of the
   * mean and the integrated autocorrelation time or to resample via the Jackknife method. The data stored in the bins
   * is the mean of the accumulated samples in each bin.
   *
   * Depending on the parameter `max_n_bins`, which is given during construction, linear binning is done in the
   * following way:
   * - `max_n_bins == 0`: Linear binning is turned off, i.e. no data is accumulated, only the count is increased.
   * - `max_n_bins == 1`: There is only one bin, which simply accumulates the mean of the data. The `bin_capacity`
   * parameter is set to -1 and is ignored.
   * - `max_n_bins > 1`: The data is accumulated into a fixed number of bins with initial capacity `bin_capacity`. Once,
   * all bins are full, i.e. they have accumulated `bin_capacity` data points, the bins are compressed by a factor of
   * 2. This means that the data of two adjacent bins is averaged and stored in the first `max_n_bins / 2` bins and that
   * the bin capacity is increased by a factor of 2. Any left over data is put into the bin with index `max_n_bins / 2`.
   * Before the compression is done, an optional callback function is called (usually to report autocorrelation times).
   * - `max_n_bins < 0`: The data is accumulated into an unbounded number of bins with capacity `bin_capacity`. Once, a
   * bin is full, a new bin is created and appended.
   *
   * In addition to the linear bins, the accumulator further keeps track of the overall mean and the sum of the squared
   * deviations from the mean. This is used to provide an estimate for the integrated autocorrelation time.
   *
   * @tparam T triqs::stats::AccCompatible type.
   */
  template <AccCompatible T> class lin_binning {
    public:
    /// Type of the accumulated data.
    using value_t = T;

    /// Real type corresponding to the accumulated data.
    using real_t = get_real_t<T>;

    /// Type of the callback function that is called when bins are compressed.
    using callback_t = std::function<void(lin_binning const &)>;

    /// Default constructor creates a turned off accumulator.
    lin_binning() = default;

    /**
     * @brief Construct a linear binning accumulator with a dummy sample, the maximum number of bins, the bin capacity
     * and a callback function.
     *
     * @details The callback function is called just before the bins are compressed. See also lin_binning::compress.
     *
     * @param sample Dummy sample to determine which type is stored and accumulated in the bins.
     * @param max_n_bins Maximum number of bins.
     * @param bin_capacity Initial/Fixed capacity of each bin.
     * @param callback Callback function of type lin_binning::callback_t.
     */
    lin_binning(T const &sample, long max_n_bins, long bin_capacity, callback_t callback = {})
       : max_n_bins_(max_n_bins), bin_capacity_((max_n_bins_ == 0 || max_n_bins_ == 1) ? -max_n_bins : bin_capacity), callback_(std::move(callback)) {
      // turn off linear binning
      if (max_n_bins == 0) return;

      // check the given bin capacity
      if (max_n_bins != 1 && bin_capacity < 1) {
        throw std::runtime_error("Linear bin capacity must be greater than 0 when max. number of bins != 1.");
      }

      // reserve space for max. number of bins
      if (max_n_bins > 0) { mean_bins_.reserve(max_n_bins); }

      // create the first bin and zero it
      mean_bins_.emplace_back(zeroed_sample(sample));

      // initialize the overall mean and sum of squared deviations from the mean
      mean_ = zeroed_sample(sample);
      var_  = make_real(zeroed_sample(sample));
    }

    /// Get the maximum number of bins.
    [[nodiscard]] long max_n_bins() const { return max_n_bins_; }

    /// Get the number of bins containing data including the currently active bin.
    [[nodiscard]] long n_bins() const { return mean_bins_.size(); }

    /// Get the number of full bins, i.e. bins that are filled up to the bin capacity.
    [[nodiscard]] long n_full_bins() const { return (last_bin_count_ == bin_capacity_ ? n_bins() : n_bins() - 1); }

    /// Get all the bins containing data including the currently active bin.
    [[nodiscard]] auto const &bins() const { return mean_bins_; }

    /// Get only the full bins, i.e. bins that are filled up to the bin capacity.
    [[nodiscard]] std::vector<T> full_bins() const {
      return (last_bin_count_ == bin_capacity_ ? mean_bins_ : std::vector<T>(mean_bins_.begin(), std::prev(mean_bins_.end())));
    }

    /// Get the current bin capacity.
    [[nodiscard]] long bin_capacity() const { return bin_capacity_; }

    /// Get the number of samples in the currently active bin.
    [[nodiscard]] long last_bin_count() const { return last_bin_count_; }

    /// Get the total number of accumulated samples.
    [[nodiscard]] long count() const { return count_; }

    /// Get the overall mean.
    [[nodiscard]] auto const &mean() const { return mean_; }

    /// Get the overall sum of the squared deviations from the mean.
    [[nodiscard]] auto const &var_data() const { return var_; }

    /// Get the callback function.
    [[nodiscard]] auto const &callback() const { return callback_; }

    /**
     * @brief Accumulate a new sample.
     *
     * @details Let \f$ x_i \f$ be the <sup>i</sup>th sample that is currently being added to the accumulator.
     *
     * The data point is accumulated into the currently active bin. In case that the active bin is full and the maximum
     * number of bins is reached, the bins are compressed by merging two adjacent bins into one (see
     * lin_binning::compress). Otherwise, a new bin with the given data point is added.
     *
     * Let \$ m_n(k_n) \f$ be the stored mean of the data in the currently active bin \f$ n \f$ and let \f$ k_n \f$ be
     * the number of data points in that bin. The mean in the bin is then updated as
     * \f[
     *   m_n(k_n) = m_n(k_n - 1) + \frac{x_i - m_n(k_n - 1)}{k_n} \; .
     * \f]
     *
     * The overall mean \f$ m(N) \f$ is updated similiarly, whereas the overall sum of the squared deviations is updated
     * as
     * \f[
     *   q(N) = q(N - 1) + \frac{N - 1}{N} \left| x_i - m(N - 1) \right|^2 \; ,
     * \f]
     * where \f$ N \f$ is the total number of data points accumulated.
     *
     * @tparam U Type of the data point to be added to the accumulator.
     * @param x Data point to be added to the accumulator.
     * @return Reference to `this` object.
     */
    template <typename U> auto &operator<<(U &&x) {
      ++count_;

      // early return if linear binning is turned off
      if (max_n_bins_ == 0) return *this;

      // compress the bins if necessary and call the optional callback function
      if (n_full_bins() == max_n_bins_) {
        if (callback_) callback_(*this);
        compress(2);
      }

      // update the overall mean and variance data
      var_ += abs_square(x - mean_) * (static_cast<double>(count_ - 1) / count_);
      mean_ += (x - mean_) / count_;

      // accumulate the data point
      if (last_bin_count_ == bin_capacity_) {
        // add a new bin if the current bin is full
        mean_bins_.emplace_back(std::forward<U>(x));
        last_bin_count_ = 1;
      } else {
        // accumulate into the currently active bin
        ++last_bin_count_;
        mean_bins_.back() += (x - mean_bins_.back()) / last_bin_count_;
      }

      return *this;
    }

    /**
     * @brief Compress bins and increase bin capacity.
     *
     * @details The compression factor determines the number of adjacent bins that are merged together and the factor
     * the bin capacity is multiplied with.
     *
     * The data of the bins is averaged and stored in the first `n_full_bins() / fac` bins. If there are any left over
     * bins, the data is put into a new bin, which becomes the currently active bin.
     *
     * It does nothing, if linear binning is turned off, if there is only one bin or if the compression factor is less
     * than 2.
     *
     * @param fac Compression factor.
     */
    void compress(int fac) {
      // early return if linear binning is turned off, if there is only 1 bin or if the compression factor < 2
      if (max_n_bins_ == 0 || n_bins() == 1 || fac < 2) return;

      // compress full bins into new full bins
      int nbins = n_full_bins() / fac;
      for (int i = 0; i < nbins; ++i) {
        mean_bins_[i] = mean_bins_[fac * i];
        for (int j = 1; j < fac; ++j) { mean_bins_[i] += mean_bins_[fac * i + j]; }
        mean_bins_[i] /= fac;
      }

      // handle any left over bins
      int const left_over = n_bins() - fac * nbins;
      if (left_over != 0) {
        // currently active bin might not be full
        auto const bc   = last_bin_count_ + (left_over - 1) * bin_capacity_;
        value_t tmp_bin = mean_bins_.back() * (static_cast<double>(last_bin_count_) / bc);
        for (int j = 0; j < left_over - 1; ++j) { tmp_bin += mean_bins_[fac * nbins + j] * (static_cast<double>(bin_capacity_) / bc); }
        mean_bins_[nbins] = tmp_bin;
        last_bin_count_   = bc;
        ++nbins;
      } else {
        // no left overs, currently active bin is full w.r.t. the new bin capacity
        last_bin_count_ = bin_capacity_ * fac;
      }

      // resize bin vector and update bin capacity
      mean_bins_.resize(nbins);
      bin_capacity_ *= fac;
    }

    /**
     * @brief Get the mean, its standard error and an estimate for the integrated autocorrelation time.
     *
     * @details For definitions of the standard error and the integrated autocorrelation time, see
     * log_binning::mean_errors_and_taus.
     *
     * @return `std::tuple` containing the mean, its standard error and an estimate for the integrated autocorrelation
     * time.
     */
    [[nodiscard]] auto mean_error_and_tau() const {
      auto [err, tau] = calculate_error_and_tau(mean_bins_, var_, count_);
      return std::make_tuple(mean_, err, tau);
    }

    /**
     * @brief Get the mean, its standard error and an estimate for the integrated autocorrelation time from accumulators
     * on multiple MPI processes.
     *
     * @details The overall mean and the sum of squared deviations from the mean are reduced and (full) bins are
     * gathered from all MPI processes.
     *
     * The results will be available on all MPI processes.
     *
     * For definitions of the standard error and the integrated autocorrelation time, see
     * log_binning::mean_errors_and_taus.
     *
     * @param c MPI communicator.
     * @return `std::tuple` containing the reduced mean, its standard error and an estimate for the integrated
     * autocorrelation time.
     */
    [[nodiscard]] auto mean_error_and_tau(mpi::communicator c) const {
      auto [mean_red, var_red, count_red] = mpi_all_reduce(c);
      auto bins_gathered                  = mpi_all_gather(c, true);
      auto [err, tau]                     = calculate_error_and_tau(bins_gathered, var_red, count_red);
      return std::make_tuple(mean_red, err, tau);
    }

    /**
     * @brief Allreduce the overall mean and sum of squared deviations from the mean over multiple MPI processes.
     *
     * @details The results will be available on all MPI processes.
     *
     * @param c MPI communicator.
     * @return `std::tuple` containing the reduced mean, sum of squared deviations from the mean and the total number of
     * accumulated samples.
     */
    [[nodiscard]] auto mpi_all_reduce(mpi::communicator c) const {
      // reduce count
      auto count_red = mpi::all_reduce(count_, c);

      // reduce overall mean data
      value_t mean_red = mean_ * (static_cast<double>(count_) / static_cast<double>(count_red));
      mean_red         = mpi::all_reduce(mean_red, c);

      // reduce overall variance data
      real_t var_red = var_ + count_ * abs_square(mean_ - mean_red);
      var_red        = mpi::all_reduce(var_red, c);

      return std::make_tuple(mean_red, var_red, count_red);
    }

    /**
     * @brief Allgather full bins from multiple MPI processes.
     *
     * @details Only full bins are considered. Furthermore, if `same_capacity` is set to `true`, the bins are compressed
     * to have the same capacity on all processes and only those bins are then gathered.
     *
     * @param c MPI communicator.
     * @return `std::vector' containing the full (and compressed) bins from all processes.
     */
    [[nodiscard]] auto mpi_all_gather(mpi::communicator c, bool same_capacity = true) const {
      // only consider full bins
      auto fbins = full_bins();

      // should all bins have the same capacity?
      if (same_capacity) {
        // get maximum bin capacity
        auto bc_max = mpi::all_reduce(bin_capacity_, c, MPI_MAX);

        // if current bin capacity is not the maximum, compress bins to the maximum capacity if possible
        if (bin_capacity_ != bc_max) {
          auto const fac = bc_max / bin_capacity_;
          if (fac <= fbins.size() && bc_max % bin_capacity_ == 0) {
            // compress bins
            fbins = compress_bins(fbins, fac);
          } else {
            // cannot compress to the maximum capacity --> process doesn't contribute any bins
            fbins.clear();
          }
        }
      }

      // gather bins
      auto bins_gathered = mpi::all_gather(fbins, c);
      return bins_gathered;
    }

    /**
     * @brief Write a triqs::stat::lin_binning accumulator to HDF5.
     *
     * @param g h5::group in which the subgroup is created.
     * @param name Name of the subgroup to which the accumulator will be written.
     * @param acc Accumulator to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, lin_binning const &acc) {
      auto gr = g.create_group(name);
      h5::write(gr, "max_n_bins", acc.max_n_bins_);
      h5::write(gr, "bin_capacity", acc.bin_capacity_);
      h5::write(gr, "last_bin_count", acc.last_bin_count_);
      h5::write(gr, "count", acc.count_);
      h5::write(gr, "mean_bins", acc.mean_bins_);
      h5::write(gr, "mean", acc.mean_);
      h5::write(gr, "var", acc.var_);
    }

    /**
     * @brief Read a triqs::stat::lin_binning accumulator from HDF5.
     *
     * @param g h5::group containing the subgroup.
     * @param name Name of the subgroup from which the accumulator will be read.
     * @param acc Accumulator to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, lin_binning &acc) {
      auto gr = g.open_group(name);
      h5::read(gr, "max_n_bins", acc.max_n_bins_);
      h5::read(gr, "bin_capacity", acc.bin_capacity_);
      h5::read(gr, "last_bin_count", acc.last_bin_count_);
      h5::read(gr, "count", acc.count_);
      h5::read(gr, "mean_bins", acc.mean_bins_);
      h5::read(gr, "mean", acc.mean_);
      h5::read(gr, "var", acc.var_);
    }

    /// Get the HDF5 format string.
    [[nodiscard]] static std::string hdf5_format() { return "lin_binning"; }

    private:
    // Get the mean, its standard error and an estimate for the integrated autocorrelation time.
    [[nodiscard]] auto calculate_error_and_tau(std::vector<value_t> const &bins, real_t const &q0, long ct) const {
      // unbinned estimate of the error of the mean
      real_t var0 = q0 / static_cast<double>(ct * (ct - 1));

      // binned estimate of the error of the mean
      auto [m, err] = mean_and_err(bins);

      // estimate of the integrated autocorrelation time
      real_t tau = 0.5 * (abs_square(err) / var0 - 1.0);

      return std::make_pair(err, tau);
    }

    private:
    long max_n_bins_{0};
    long bin_capacity_{-1};
    long last_bin_count_{0};
    long count_{0};
    std::vector<value_t> mean_bins_{};
    value_t mean_{};
    real_t var_{};
    callback_t callback_{};
  };

} // namespace triqs::stat
