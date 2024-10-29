// Copyright (c) 2021 Simons Foundation
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
// Authors: Philipp Dumitrescu, Nils Wentzell

#pragma once

#include <nda/nda.hpp>

#include <complex>
#include <ranges>
#include <utility>

namespace triqs::stat {

  /**
   * @brief Concept to check if a type can be used with various triqs::stat tools.
   *
   * @details Allowed types include
   * - `double` or `std::complex<double>` scalars and
   * - `nda::Array` types with `double` or `std::complex<double>` as their value type and with algebra 'A'.
   *
   * @tparam T Type to check.
   */
  template <typename T>
  concept StatCompatible =
     (nda::Scalar<T> || (nda::Array<T> && nda::get_algebra<T> == 'A')) && nda::is_any_of<nda::get_value_t<T>, double, std::complex<double>>;

  /**
   * @brief Concept to check if a type can be used with triqs::stat::lin_binning or triqs::stat::log_binning.
   *
   * @details Allowed types include triqs::stat::StatCompatible types that are either scalar or regular array types.
   *
   * @tparam T Type to check.
   */
  template <typename T>
  concept AccCompatible = StatCompatible<T> && (nda::Scalar<T> || nda::is_regular_v<T>);

  /**
   * @brief Concept to check if a range can be used with various triqs::stat tools.
   *
   * @details Allowed ranges are required to
   * - have a triqs::stat::StatCompatible value type,
   * - to be a `std::ranges::sized_range`, and
   * - to be a `std::ranges::forward_range`.
   *
   * @tparam R Range to check.
   */
  template <typename R>
  concept StatCompatibleRange = std::ranges::sized_range<R> && std::ranges::forward_range<R> && StatCompatible<std::ranges::range_value_t<R>>;

  /**
   * @brief Make a given object real and regular.
   *
   * @details It simply calls `nda::make_regular` and `nda::real` on the input object.
   *
   * The return type of this function depends on the input type `T`.
   *
   * @tparam T Input type.
   * @param t Input object to make real and regular.
   * @return Real, regular copy of the input object.
   */
  template <typename T> [[nodiscard]] auto make_real(T &&t) { return nda::make_regular(nda::real(std::forward<T>(t))); }

  /// Type trait to get the type that would be returned by triqs::stat::make_real.
  template <typename T> using get_real_t = std::remove_cvref_t<decltype(make_real(std::declval<T>()))>;

  /// Type trait to get the type that would be returned by nda::make_regular.
  template <typename T> using get_regular_t = std::remove_cvref_t<decltype(nda::make_regular(std::declval<T>()))>;

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
   * - standard error of the mean (`err_mean`): \f$ s_{\overline{x}} = \sqrt{s_{\overline{x}}^2} \f$,
   * - jackknife error estimate (`jk_err`): \f$ s_x = \sqrt{\frac{N - 1}{N} S^2} \f$, or
   */
  enum class error_tag { sum, var_data, var_mean, err_data, err_mean, jk_err };

  /**
   * @brief Given the mean and the number of samples, apply a transformation to get the result specified by the given
   * mean tag.
   *
   * @details See also triqs::stat::mean_tag.
   *
   * @tparam T triqs::stat::AccCompatible type.
   * @tparam mtag triqs::stat::mean_tag to indicate the transformation to apply.
   * @param m Mean to be transformed.
   * @param nsamples Number of samples.
   */
  template <mean_tag mtag, AccCompatible T> void apply_mean_tag(T &m, [[maybe_unused]] long nsamples) {
    if constexpr (mtag == mean_tag::sum) m *= nsamples;
  }

  /**
   * @brief Given the sum of squared deviations from the mean and the number of samples, apply a transformation to get
   * the result specified by the given error tag.
   *
   * @details See also triqs::stat::error_tag.
   *
   * @tparam T triqs::stat::AccCompatible type.
   * @tparam etag triqs::stat::error_tag to indicate the transformation to apply.
   * @param sum_sq_devs Sum of squared deviations from the mean to be transformed.
   * @param nsamples Number of samples.
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
   * @brief Get a sample with all elements set to zero.
   *
   * @tparam T triqs::stat::StatCompatible type.
   * @param sample Dummy sample to determine its type and shape in case of an `nda::Array` type.
   * @return Zeroed sample.
   */
  template <StatCompatible T> [[nodiscard]] auto zeroed_sample([[maybe_unused]] T const &sample) {
    if constexpr (nda::Scalar<T>) {
      return T{0};
    } else {
      return get_regular_t<T>::zeros(sample.shape());
    }
  }

  /**
   * @brief Calculate the (elementwise) absolute square of an array/view/scalar.
   *
   * @details The return type depends on the input type.
   *
   * @param x Input array/view/scalar.
   * @return (Elementwise) Absolute square of the input.
   */
  [[nodiscard]] auto abs_square(auto const &x) { return make_real(nda::hadamard(nda::conj(x), x)); }

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
   * @tparam T triqs::stats::StatCompatible type.
   * @param s_n Standard error of the mean with binning.
   * @param s_0 Standard error of the mean without binning.
   * @return Estimate of the integrated auto-correlation time.
   */
  template <StatCompatible T> auto tau_estimate_from_errors(T const &s_n, T const &s_0) {
    return nda::make_regular(0.5 * (abs_square(s_n) / abs_square(s_0) - 1.0));
  }

} // namespace triqs::stat
