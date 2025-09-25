// Copyright (c) 2019-2021 Simons Foundation
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

/**
 * @file
 * @brief Provides functions to calculate the arithmetic mean and standard error of a range of values.
 */

#pragma once

#include "./concepts.hpp"
#include "./utils.hpp"

#include <itertools/itertools.hpp>
#include <mpi/mpi.hpp>
#include <nda/mpi.hpp>
#include <nda/nda.hpp>

#include <numeric>
#include <optional>
#include <ranges>
#include <utility>

namespace triqs::stat {

  /**
   * @addtogroup triqs-stat-meanerr
   * @{
   */

  /**
   * @brief Tag to indicate what to calculate when computing the mean of a range of values.
   *
   * @details This tag is mostly used internally. Given a range of values \f$ \{x_i : i = 1, \ldots, N\} \f$, the tag
   * determines if we either calculate the
   * - a simple sum (`sum`): \f$ R_x = \sum_{i=1}^N x_i \f$ or the
   * - arithmetic mean (`mean`): \f$ \overline{x} = \frac{1}{N} \sum_{i=1}^N x_i = \frac{1}{N} R_x \f$.
   */
  enum class mean_tag { sum, mean };

  /**
   * @brief Tag to indicate what to calculate when computing the error of a range of values.
   *
   * @details This tag is mostly used internally. Given a range of values \f$ \{x_i : i = 1, \ldots, N\} \f$, the tag
   * determines if we either calculate the
   * - sum of squared deviations from the mean (`sum`): \f$ S^2 = \sum_{i=1}^N \left| x_i - \overline{x} \right|^2 \f$,
   * - variance of the data itself (`var_data`): \f$ s_{x}^2 = \frac{1}{N - 1} S^2 \f$,
   * - variance of the mean (`var_mean`): \f$ s_{\overline{x}}^2 = \frac{s_x^2}{N} \f$,
   * - standard error of the data (`err_data`): \f$ s_x = \sqrt{s_x^2} \f$,
   * - standard error of the mean (`err_mean`): \f$ s_{\overline{x}} = \sqrt{s_{\overline{x}}^2} \f$ or
   * - jackknife error estimate (`jk_err`): \f$ s_x = \sqrt{\frac{N - 1}{N} S^2} \f$.
   */
  enum class error_tag { sum, var_data, var_mean, err_data, err_mean, jk_err };

  /**
   * @brief Given the mean \f$ \overline{x} \f$ and the number of samples \f$ N \f$, apply a transformation to get the
   * result specified by the given mean tag.
   *
   * @details See also triqs::stat::mean_tag.
   *
   * @tparam T triqs::stat::AccCompatible type.
   * @tparam mtag triqs::stat::mean_tag to indicate the transformation to apply.
   * @param m Mean \f$ \overline{x} \f$.
   * @param nsamples Number of samples \f$ n \f$.
   */
  template <mean_tag mtag, AccCompatible T> void apply_mean_tag(T &m, [[maybe_unused]] long nsamples) {
    if constexpr (mtag == mean_tag::sum) m *= nsamples;
  }

  /**
   * @brief Given the sum of squared deviations from the mean, \f$ S^2 = \sum_{i=1}^N \left| x_i - \overline{x}
   * \right|^2 \f$, and the number of samples \f$ N \f$, apply a transformation to get the result specified by the given
   * error tag.
   *
   * @details See also triqs::stat::error_tag.
   *
   * @tparam T triqs::stat::AccCompatible type.
   * @tparam etag triqs::stat::error_tag to indicate the transformation to apply.
   * @param sum_sq_devs Sum of squared deviations from the mean \f$ S^2 \f$.
   * @param nsamples Number of samples \f$ N \f$.
   */
  template <error_tag etag, AccCompatible T> void apply_error_tag(T &sum_sq_devs, [[maybe_unused]] long nsamples) {
    if constexpr (etag == error_tag::sum) return;
    auto const nd = static_cast<double>(nsamples);
    if constexpr (etag == error_tag::err_data || etag == error_tag::var_data)
      sum_sq_devs /= (nd - 1);
    else if constexpr (etag == error_tag::err_mean || etag == error_tag::var_mean)
      sum_sq_devs /= (nd * (nd - 1));
    else if constexpr (etag == error_tag::jk_err)
      sum_sq_devs *= (nd - 1) / nd;
    if constexpr (etag == error_tag::err_data || etag == error_tag::err_mean || etag == error_tag::jk_err) sum_sq_devs = nda::sqrt(sum_sq_devs);
  }

  /**
   * @brief Calculate the arithmetic mean or the simple sum of some range of values.
   *
   * @details The return type depends on the value type of the data.
   *
   * See triqs::stat::mean_tag for more information.
   *
   * @tparam mtag triqs::stat::mean_tag.
   * @tparam R triqs::stat::StatCompatibleRange type.
   * @param rg Range object containing the data.
   * @return Arithmetic mean/Sum of the data.
   */
  template <mean_tag mtag = mean_tag::mean, StatCompatibleRange R> auto mean(R &&rg) { // NOLINT (ranges should not be forwarded)
    if constexpr (mtag == mean_tag::mean) {
      // calculate the arithmetic mean
      auto res = zeroed_sample(*std::ranges::begin(rg));
      for (auto const &[n, x] : itertools::enumerate(rg)) res += (x - res) / (n + 1);
      return res;
    } else {
      // calculate the simple sum
      return std::accumulate(std::ranges::begin(rg), std::ranges::end(rg), zeroed_sample(*std::ranges::begin(rg)));
    }
  }

  /**
   * @brief Calculate the arithmetic mean or the simple sum of some range of values spread across multiple MPI
   * processes.
   *
   * @details If the optional MPI communicator is provided, we first use triqs::stat::mean to get the mean/sum of the
   * data on each process and then reduce it with
   * `mpi::all_reduce`:
   * - `mtag == mean_tag::sum`: Reduce the sum of the data over all MPI processes.
   * - `mtag == mean_tag::mean`: Let \f$ \overline{x}_i \f$ be the mean of the data on process \f$ i \f$. Then the mean
   * of the combined data can be written as
   * \f[
   *   \overline{x} = \frac{1}{N} \sum_{i=1}^P N_i \overline{x}_i \; ,
   * \f]
   * where \f$ N_i \f$ is the number of samples on process \f$ i \f$, \f$ N \f$ is the total number of samples and \f$
   * P \f$ is the number of MPI processes.
   *
   * The result will be available on all MPI processes and its type depends on the value type of the range.
   *
   * If the optional MPI communicator is not provided, it simply calls triqs::stat::mean.
   *
   * See triqs::stat::mean_tag for more information.
   *
   * @tparam mtag triqs::stat::mean_tag.
   * @tparam R triqs::stat::StatCompatibleRange type.
   * @param c `std::optional` MPI communicator.
   * @param rg Range object containing the data.
   * @return Arithmetic mean/Sum of the combined data from all MPI processes.
   */
  template <mean_tag mtag = mean_tag::mean, StatCompatibleRange R>
  auto mean_mpi(std::optional<mpi::communicator> c, R &&rg) { // NOLINT (ranges should not be forwarded)
    using value_t = get_regular_t<std::ranges::range_value_t<R>>;

    // early return if no communicator is provided
    if (!c) return mean<mtag>(rg);

    auto tmp = mean<mtag>(rg);
    if constexpr (mtag == mean_tag::mean) {
      // for mtag == mean_tag::mean, we need to take care of different sample sizes
      auto const n_i = std::ranges::size(rg);
      auto const n   = mpi::all_reduce(n_i, *c);
      tmp *= static_cast<double>(n_i) / static_cast<double>(n);
    }
    value_t res = mpi::all_reduce(tmp, *c);
    return res;
  }

  /**
   * @brief Calculate the arithmetic mean or the simple sum as well as a corresponding error estimate of some range of
   * values.
   *
   * @details In addition to the mean/sum (see triqs::stat::mean), we also calculate an error estimate which depends on
   * the given error tag.
   *
   * The return type depends on the value type of the data.
   *
   * See triqs::stat::mean_tag and triqs::stat::error_tag for more information.
   *
   * @tparam etag triqs::stat::error_tag.
   * @tparam mtag triqs::stat::mean_tag.
   * @tparam R triqs::stat::StatCompatibleRange type.
   * @param rg Range object containing the data.
   * @return `std::pair` containing the mean/sum and an error estimate of the data.
   */
  template <error_tag etag = error_tag::err_mean, mean_tag mtag = mean_tag::mean, StatCompatibleRange R>
  auto mean_and_err(R &&rg) { // NOLINT (ranges should not be forwarded)
    // calculate the arithmetic mean and the sum of squared deviations from the mean
    auto res_m = zeroed_sample(*std::ranges::begin(rg));
    auto res_s = make_real(res_m);
    for (auto const &[n, x] : itertools::enumerate(rg)) {
      auto const nd = static_cast<double>(n);
      res_s += abs_square(x - res_m) * nd / (nd + 1);
      res_m += (x - res_m) / (nd + 1);
    }
    // apply the mean and error tags
    apply_mean_tag<mtag>(res_m, std::ranges::size(rg));
    apply_error_tag<etag>(res_s, std::ranges::size(rg));
    return std::make_pair(res_m, res_s);
  }

  /**
   * @brief Calculate the arithmetic mean or the simple sum as well as a corresponding error estimate of some range of
   * values spread across multiple MPI processes.
   *
   * @details If the optional MPI communicator is provided, we first use triqs::stat::mean_and_err to calculate the
   * mean and the sum of squared deviations from the mean on each process. Then the mean is reduced following the same
   * procedure as in triqs::stat::mean_mpi. With the reduced mean, we calculate
   * \f[
   *   r_i^2 = S_i^2 + N_i \left| \overline{x}_i - \overline{x} \right|^2 \; ,
   * \f]
   * on every process \f$ i \f$, and reduce it to get the sum of squared deviations from the mean for the combined data.
   * The result will be available on all MPI processes and its type depends on the value type of the range.
   *
   * If the optional MPI communicator is not provided, it simply calls triqs::stat::mean_and_err.
   *
   * See triqs::stat::mean_tag and triqs::stat::error_tag for more information.
   *
   * @tparam etag triqs::stat::error_tag.
   * @tparam mtag triqs::stat::mean_tag.
   * @tparam R triqs::stat::StatCompatibleRange type.
   * @param c `std::optional` MPI communicator.
   * @param rg Range object containing the data.
   * @return `std::pair` containing the mean/sum and an error estimate of the data from all MPI processes.
   */
  template <error_tag etag = error_tag::err_mean, mean_tag mtag = mean_tag::mean, StatCompatibleRange R>
  auto mean_and_err_mpi(std::optional<mpi::communicator> c, R &&rg) { // NOLINT (ranges should not be forwarded)
    using value_t = get_regular_t<std::ranges::range_value_t<R>>;
    using real_t  = get_real_t<value_t>;

    // early return if no communicator is provided
    if (!c) return mean_and_err<etag, mtag>(rg);

    // local mean and sum of squared deviations from the mean
    auto [m, ssqdev] = mean_and_err<error_tag::sum>(rg);

    // reduce the sample size
    auto const n     = std::ranges::size(rg);
    auto const n_red = mpi::all_reduce(n, *c);

    // reduce the mean
    value_t res_m = m * (static_cast<double>(n) / static_cast<double>(n_red));
    res_m         = mpi::all_reduce(res_m, *c);

    // reduce the sum of squared deviations from the mean
    ssqdev += n * abs_square(m - res_m);
    real_t res_s = mpi::all_reduce(ssqdev, *c);

    // apply the mean and error tags
    apply_mean_tag<mtag>(res_m, n_red);
    apply_error_tag<etag>(res_s, n_red);
    return std::make_pair(res_m, res_s);
  }

  /**
   * @brief Compute an estimate for the integrated auto-correlation time.
   *
   * @details The integrated autocorrelation time is estimated as
   * \f[
   *   \tau = \frac{1}{2} \left( \frac{s^2_n}{s^2_0} - 1 \right)^2 \; ,
   * \f]
   * where \f$ s^2_n \f$ is the variance of the mean with binning and \f$ s_0 \f$ is the variance of the mean without
   * binning.
   *
   * @tparam T triqs::stat::StatCompatible type.
   * @param s_n Standard error of the mean with binning.
   * @param s_0 Standard error of the mean without binning.
   * @return Estimate of the integrated auto-correlation time.
   */
  template <StatCompatible T> auto tau_estimate_from_errors(T const &s_n, T const &s_0) {
    return nda::make_regular(0.5 * (abs_square(s_n) / abs_square(s_0) - 1.0));
  }

  /** @} */

} // namespace triqs::stat
