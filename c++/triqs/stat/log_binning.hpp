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

#include "./utils.hpp"

#include <h5/h5.hpp>
#include <mpi/mpi.hpp>
#include <nda/h5.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>

#include <functional>
#include <iterator>
#include <string>
#include <tuple>
#include <vector>

namespace triqs::stat {

  /**
   * @brief Logarithmic binning accumulator.
   *
   * @details Estimate the autocorrelation time of the samples by calculating the standard error of the data binned with
   * different bin sizes. For correlated samples, the error should increase as the bin size increases up to the point
   * where the bin size is large enough such that the binned samples become statistically independent.
   *
   * Logarithmic binning uses bin sizes that are powers of two, i.e. \f$ 1, 2, 4, 8, 16, \dots \f$, up to an optional
   * user-defined maximum.
   *
   * Depending on the given parameter `max_n_bins`, which is given during construction, logarithmic binning is done in
   * the following way:
   * - `max_n_bins == 0`: Logarithmic binning is turned off, i.e. no data is accumulated, only the count is increased.
   * - `max_n_bins > 1`: The maximum number of bins \f$ N \f$ is restricted. The bin size of the last bin is \f$
   * 2^{N - 1} \f$.
   * - `max_n_bins < 0`: The maximum number of bins is unbounded. A new bin is created as soon as the total number of
   * accumulated samples is equal to a power of 2. The bin size of the new bin is \f$ 2^{N - 1} \f$, where \f$ N \f$
   * is the number of bins after the new one has been added.
   *
   * See log_binning::operator<< for details on how the data is accumulated.
   *
   * @note Underestimating the autocorrelation time and a lack of ergodicity are common sources of systematic errors in
   * Monte Carlo simulations. Provided the computational and memory costs are acceptable, it is advisable to always turn
   * on logarithmic binning to check that \f$ \tau \f$ was estimated correctly and acts as expected.
   *
   * @tparam T triqs::stats::AccCompatible type.
   */
  template <AccCompatible T> class log_binning {
    public:
    /// Type of the accumulated data.
    using value_t = T;

    /// Real type corresponding to the accumulated data.
    using real_t = get_real_t<T>;

    /// Default constructor creates a turned off accumulator.
    log_binning() = default;

    /**
     * @brief Construct a logarithmic binning accumulator with a dummy sample and the maximum number of bins.
     *
     * @param sample Dummy sample to determine which type is stored and accumulated in the bins.
     * @param max_n_bins Maximum number of bins.
     */
    log_binning(T const &sample, int max_n_bins) : max_n_bins_(max_n_bins) {
      // turn off logarithmic binning
      if (max_n_bins == 0) return;

      // reserve space for max. number of bins
      if (max_n_bins > 0) {
        mean_bins_.reserve(max_n_bins);
        var_bins_.reserve(max_n_bins);
        // we accumulate directly into the bin with bin size 1, i.e. there is no need to accumulate any bare samples
        bare_bins_.reserve(max_n_bins - 1);
        bare_counts_.reserve(max_n_bins - 1);
      }

      // initialize the mean and variance bin with bin size 1
      mean_bins_.emplace_back(zeroed_sample(sample));
      var_bins_.emplace_back(make_real(zeroed_sample(sample)));

      // initialize the bare bin and count for the bin with bin size 2
      if (max_n_bins != 1) {
        bare_bins_.emplace_back(zeroed_sample(sample));
        bare_counts_.push_back(0);
      }
    }

    /// Get the maximum number of bins.
    [[nodiscard]] long max_n_bins() const { return max_n_bins_; }

    // Get the current number of bins.
    [[nodiscard]] long n_bins() const { return var_bins_.size(); }

    /// Get the total number of accumulated samples.
    [[nodiscard]] long count() const { return count_; }

    /// Get the number of effective samples accumulated in each bin.
    [[nodiscard]] auto effective_counts() const {
      std::vector<long> res(n_bins());
      for (int i = 0; i < n_bins(); ++i) res[i] = count_ >> i;
      return res;
    }

    /// Get the bins that store the mean data.
    [[nodiscard]] auto const &mean_bins() const { return mean_bins_; }

    /// Get the bins that store the sum of the squared deviations from the mean.
    [[nodiscard]] auto const &var_bins() const { return var_bins_; }

    /// Get the bins that are used for accumulating the bare samples.
    [[nodiscard]] auto const &bare_bins() const { return bare_bins_; }

    /// Get the number of samples currently in the bare bins.
    [[nodiscard]] auto const &bare_counts() const { return bare_counts_; }

    /**
     * @brief Accumulate a new sample.
     *
     * @details Let \f$ x_i \f$ be the <sup>i</sup>th sample that is currently being added to the accumulator.
     *
     * For each bin \f$ n = 0, 1, \dots, N - 1 \f$, let us define the following quantities:
     * - the bin size \f$ b_n = 2^n \f$,
     * - the current number of effective samples \f$ k_n(i) = \lfloor i / b_n \rfloor \f$,
     * - the current number of bare samples \f$ l_n(i) = i - k_n(i) b_n \f$,
     * - the current sum of bare samples \f$ p_n(i) = \sum_{j=1}^{l_n(i)} x_{j + k_n(i) b_n} \f$,
     * - the \f$ k_n(i) \f$ effective samples \f$ y_n[k_n(i)] = \frac{1}{b_n} \sum_{j=1}^b_n x_{j + k_n(i) b_n} \f$,
     * - the current mean of the effective samples \f$ m_n[k_n(i)] = \frac{1}{k_n(i)} \sum_{j=1}^{k_n(i)} y_j \f$ and
     * - the current sum of the squared deviations from the mean \f$ q_n[k_n(i)] = \sum_{j=1}^{k_n(i)} \left\{ y_n(j) -
     * m_n[k_n(i)] \right\}^2 \f$.
     *
     * The new data point is first added to the bare samples of each bin:
     * \f[
     *   p_n(i) =
     *   \begin{cases}
     *   p_n(i-1) + x_i & \text{if } 1 < l_n(i) <= b_n \\
     *   x_i & \text{if } l_n(i) = 1
     *   \end{cases}
     *   \; .
     * \f]
     * If the number of bare samples is equal to the bin size, i.e. \f$ l_n(i) = b_n \f$, we take the mean of the
     * accumulated bare samples to get the effective sample \f$ y_n[k_n(i)] = p_n(i) / b_n \f$. The effective sample is
     * then added to the mean
     * \f[
     *   m_n[k_n(i)] = m_n[k_n(i) - 1] + \frac{ y_n[k_n(i)] - m_n[k_n(i) - 1]}{k}
     * \f]
     * and to the sum of squared deviations from the mean
     * \f[
     *   q_n[k_n(i)] = q_n[k_n(i) - 1] + \frac{k - 1}{k} \left\{ y_n[k_n(i)] - m_n[k_n(i) - 1] \right\}^2 \; .
     * \f]
     *
     * In case, \f$ i = 2^{N - 1} \f$ and the maximum number of bins is not reached yet, a new bin with bin size
     * \f$ 2^{N} \f$ will be added.
     *
     * For more information, see e.g. <a href="https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance">
     * Wikipedia</a> or Chan, Golub, LeVeque. Am. Stat. 37, 242 (1983).
     *
     * @tparam U Type of the data point to be added to the accumulator.
     * @param x Data point to be added to the accumulator.
     * @return Reference to `this` object.
     */
    template <typename U> log_binning<T> &operator<<(U const &x) {
      ++count_;

      // early return if logarithmic binning is turned off
      if (max_n_bins_ == 0) return *this;

      // always accumulate into the mean and variance bin with block size 1
      T x_m = (x - mean_bins_[0]);
      var_bins_[0] += abs_square(x_m) * (static_cast<double>(count_ - 1) / count_);
      mean_bins_[0] += x_m / count_;

      // accumulate to bins with larger block sizes
      if (max_n_bins_ != 1) {
        // add a new bin if the total count is a power of 2 and we have not reached the maximum number of bins
        if (count_ == (1 << bare_bins_.size()) && (max_n_bins_ < 0 || n_bins() < max_n_bins_)) {
          // add new mean and variance bins with bin size 2^n_bins()
          mean_bins_.emplace_back(zeroed_sample(mean_bins_[0]));
          var_bins_.emplace_back(zeroed_sample(var_bins_[0]));

          // add new bare bin and count for the bin with bin size 2^{n_bins() + 1}
          if (max_n_bins_ < 0 || n_bins() < max_n_bins_) {
            bare_bins_.emplace_back(zeroed_sample(bare_bins_[0]));
            bare_counts_.push_back(0);
          }
        }

        // always accumulate into the bare bin with block size 2
        bare_bins_[0] += x;
        ++bare_counts_[0];

        // propagate full bare bins and add them to the mean and variance bins
        for (int i = 0; i < bare_bins_.size() && bare_counts_[i] == 2; ++i) {
          // propagate bare bin
          if (i + 1 < bare_bins_.size()) {
            bare_bins_[i + 1] += bare_bins_[i];
            ++bare_counts_[i + 1];
          }

          // accumulate into the mean and variance bin
          auto const bc = static_cast<double>(1ul << (i + 1));
          auto const k  = count_ / bc;
          x_m           = (bare_bins_[i] / bc - mean_bins_[i + 1]);
          var_bins_[i + 1] += abs_square(x_m) * ((k - 1) / k);
          mean_bins_[i + 1] += x_m / k;

          // reset bare bin and count
          bare_counts_[i] = 0;
          bare_bins_[i]   = 0;
        }
      }

      return *this;
    }

    /**
     * @brief Get the overall mean as well as the estimated standard error, integrated autocorrelation time and
     * effective number of samples for each bin.
     *
     * @details The standard error, \f$ s_n \f$, is defined as the square root of the variance
     * \f[
     *   s_n^2 = \frac{1}{k_n (k_n - 1)} \sum_{i=1}^{k_n} (y_i - m_n)^2 = \frac{1}{k (k - 1)} q_n \; ,
     * \f]
     * where \f$ m_n \f$ and \f$ q_n \f$ is the mean and the sum of the squared deviations from the mean currently
     * stored in the accumulator for each bin \f$ n \f$. \f$ k_n \f$ is the number of effective samples in a bin and
     * \f$ y_i \f$ is an effective sample.
     *
     * The integrated autocorrelation time, \f$ \tau_n \f$, can then be estimated as
     * \f[
     *   \tau_n = \frac{1}{2} \left( \frac{s_n^2}{s_0^2} - 1 \right) \; .
     * \f]
     * \f$ \tau_n \f$ and \f$ s_n \f$ are expected to increase with \f$ n \f$ and saturate for large enough bin sizes.
     * At this point, the effective samples in the bins become statistically independent.
     *
     * Only bins with at least `min_samples` effective samples are considered for error and autcorrelation time
     * esimation.
     *
     * @param min_samples Minimum number of effective samples required.
     * @return `std::tuple` containing the overall mean, the standard errors, the integrated autocorrelation times and
     * the effective number of samples for each bin.
     */
    [[nodiscard]] auto mean_errors_and_taus(int min_samples = 0) const {
      return calculate_mean_errors_and_taus(mean_bins_, var_bins_, effective_counts(), min_samples);
    }

    /**
     * @brief Get the overall mean as well as the estimated standard error, integrated autocorrelation time and
     * effective number of samples for each bin on multiple MPI processes.
     *
     * @details The data in each bin is first reduced across all MPI processes using log_binning::mpi_all_reduce, before
     * the overall mean, standard errors and integrated autocorrelation times are calculated for the reduced bins.
     *
     * Only bins with at least `min_samples` effective samples after the reduction are considered for error and
     * autcorrelation time esimation.
     *
     * The results will be available on all MPI processes.
     *
     * See also log_binning::mean_errors_and_taus.
     *
     * @param c MPI communicator.
     * @param min_samples Minimum number of effective samples required.
     * @return `std::tuple` containing the overall mean, the standard errors, the integrated autocorrelation times and
     * the effective number of samples for each reduced bin.
     */
    [[nodiscard]] auto mean_errors_and_taus(mpi::communicator c, int min_samples = 0) const {
      auto [mean_red, var_red, nsamples_red] = mpi_all_reduce(c);
      return calculate_mean_errors_and_taus(mean_red, var_red, nsamples_red, min_samples);
    }

    /**
     * @brief Allreduce the accumulated data over multiple MPI processes.
     *
     * @details First the bins are resized to the maximum number of bins across all processes. Then the number of
     * effective samples, the mean and sum of squared deviations from the mean are reduced for each bin separately.
     *
     * @note Any data in the bare bins is ignored.
     *
     * @param c MPI communicator.
     * @return `std::tuple` containing the reduced mean bins, variance bins and number of effective samples for each
     * bin.
     */
    [[nodiscard]] auto mpi_all_reduce(mpi::communicator c) const {
      auto mean_red     = mean_bins_;
      auto var_red      = var_bins_;
      auto nsamples_red = effective_counts();

      // make sure all ranks have the same number of bins
      auto const nbins = mpi::all_reduce(n_bins(), c, MPI_MAX);
      mean_red.resize(nbins, zeroed_sample(mean_bins_[0]));
      var_red.resize(nbins, zeroed_sample(var_bins_[0]));
      nsamples_red.resize(nbins, 0);

      // reduce each bin separately
      for (int i = 0; i < nbins; ++i) {
        // reduce the number of effective samples
        auto const ns   = nsamples_red[i];
        nsamples_red[i] = mpi::all_reduce(ns, c);

        // reduce the mean bins
        mean_red[i] *= static_cast<double>(ns) / nsamples_red[i];
        mean_red[i] = mpi::all_reduce(mean_red[i], c);

        // reduce the variance bins
        if (i < mean_bins_.size()) var_red[i] += ns * abs_square(mean_bins_[i] - mean_red[i]);
        var_red[i] = mpi::all_reduce(var_red[i], c);
      }

      return std::make_tuple(mean_red, var_red, nsamples_red);
    }

    /**
     * @brief Write a triqs::stat::log_binning accumulator to HDF5.
     *
     * @param g h5::group in which the subgroup is created.
     * @param name Name of the subgroup to which the accumulator will be written.
     * @param acc Accumulator to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, log_binning const &l) {
      auto gr = g.create_group(name);
      h5::write(gr, "max_n_bins", l.max_n_bins_);
      h5::write(gr, "count", l.count_);
      h5::write(gr, "mean_bins", l.mean_bins_);
      h5::write(gr, "var_bins", l.var_bins_);
      h5::write(gr, "bare_bins", l.bare_bins_);
      h5::write(gr, "bare_counts", l.bare_counts_);
    }

    /**
     * @brief Read a triqs::stat::log_binning accumulator from HDF5.
     *
     * @param g h5::group containing the subgroup.
     * @param name Name of the subgroup from which the accumulator will be read.
     * @param acc Accumulator to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, log_binning &l) {
      auto gr = g.open_group(name);
      h5::read(gr, "max_n_bins", l.max_n_bins_);
      h5::read(gr, "count", l.count_);
      h5::read(gr, "mean_bins", l.mean_bins_);
      h5::read(gr, "var_bins", l.var_bins_);
      h5::read(gr, "bare_bins", l.bare_bins_);
      h5::read(gr, "bare_counts", l.bare_counts_);
    }

    /// Get the HDF5 format string.
    [[nodiscard]] static std::string hdf5_format() { return "log_binning"; }

    private:
    // Get the overall mean as well as the standard error, integrated autocorrelation time and effective number of
    // samples for each bin with at least a given minimum number of samples.
    [[nodiscard]] auto calculate_mean_errors_and_taus(std::vector<value_t> const &mk, std::vector<real_t> const &qk,
                                                      std::vector<long> const &nsamples, int min_samples) const {
      // early return for empty vectors
      if (mk.empty()) return std::make_tuple(value_t{}, std::vector<real_t>{}, std::vector<real_t>{}, std::vector<long>{});

      // only consider bins with at least min_samples effective samples
      auto const size = std::distance(nsamples.begin(), std::ranges::upper_bound(nsamples, min_samples, std::greater{}));
      auto effs       = std::vector(nsamples.begin(), nsamples.begin() + size);
      auto errs       = std::vector(qk.begin(), qk.begin() + size);
      auto taus       = std::vector<real_t>(size);

      // calculate errors and taus
      real_t var0 = qk[0] / (nsamples[0] * (nsamples[0] - 1));
      for (int i = 0; i < size; ++i) {
        real_t var = errs[i] / (effs[i] * (effs[i] - 1));
        errs[i]    = nda::sqrt(var);
        taus[i]    = 0.5 * (var / var0 - 1.0);
      }
      return std::make_tuple(mk[0], errs, taus, effs);
    }

    private:
    long max_n_bins_{0};
    long count_{0};
    std::vector<value_t> mean_bins_{};
    std::vector<real_t> var_bins_{};
    std::vector<value_t> bare_bins_{};
    std::vector<int> bare_counts_{};
  };

} // namespace triqs::stat
