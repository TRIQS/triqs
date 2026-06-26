// Copyright (c) 2025 Simons Foundation
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
// Authors: Nils Wentzell

/**
 * @file
 * @brief Provides utilities for Chebyshev polynomial computations.
 */

#pragma once

#include <nda/nda.hpp>

#include <cmath>
#include <numbers>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-math
   * @{
   */

  /**
   * @brief Compute Chebyshev points of the first kind on the interval \f$ [-1, 1] \f$.
   *
   * @details Points are computed as:
   * \f[
   *   x_i = \cos\left(\frac{(2i + 1) \pi}{2N}\right) \quad \text{for } i = 0, \ldots, N-1
   * \f]
   * These are the roots of the Chebyshev polynomial \f$ T_N(x) \f$.
   * Points are returned in ascending order.
   *
   * @param N Number of Chebyshev points.
   * @return Vector of \f$ N \f$ Chebyshev points in ascending order on \f$ [-1, 1] \f$.
   */
  inline nda::vector<double> chebyshev_points(long N) {
    nda::vector<double> points(N);
    const double pi = std::numbers::pi;
    for (long i = 0; i < N; ++i) {
      // Store in ascending order (cos decreases as argument increases)
      points[N - 1 - i] = std::cos((2.0 * static_cast<double>(i) + 1.0) * pi / (2.0 * static_cast<double>(N)));
    }
    return points;
  }

  /**
   * @brief Compute barycentric weights for Chebyshev points of the first kind.
   *
   * @details Weights are computed as:
   * \f[
   *   w_i = (-1)^i \sin\left(\frac{(2i + 1) \pi}{2N}\right)
   * \f]
   * These weights are used in the barycentric interpolation formula.
   * Weights are returned in the same order as chebyshev_points().
   *
   * @param N Number of Chebyshev points.
   * @return Vector of \f$ N \f$  barycentric weights.
   */
  inline nda::vector<double> chebyshev_barycentric_weights(long N) {
    nda::vector<double> weights(N);
    const double pi = std::numbers::pi;
    for (long i = 0; i < N; ++i) {
      // Match point ordering (ascending)
      double sign        = (i % 2 == 0) ? 1.0 : -1.0;
      weights[N - 1 - i] = sign * std::sin((2.0 * static_cast<double>(i) + 1.0) * pi / (2.0 * static_cast<double>(N)));
    }
    return weights;
  }

  /**
   * @brief Scale a value from interval \f$ [a, b] \f$ to the standard interval \f$ [-1, 1] \f$.
   *
   * @param x Value in \f$ [a, b] \f$.
   * @param a Lower bound of the original interval.
   * @param b Upper bound of the original interval.
   * @return Scaled value in \f$ [-1, 1] \f$.
   */
  inline double to_standard_interval(double x, double a, double b) { return 2.0 * (x - 0.5 * (b + a)) / (b - a); }

  /**
   * @brief Scale a value from the standard interval \f$ [-1, 1] \f$ to interval \f$ [a, b] \f$.
   *
   * @param x Value in \f$ [-1, 1] \f$.
   * @param a Lower bound of the target interval.
   * @param b Upper bound of the target interval.
   * @return Scaled value in \f$ [a, b] \f$.
   */
  inline double from_standard_interval(double x, double a, double b) { return 0.5 * (b - a) * x + 0.5 * (b + a); }

  /** @} */

} // namespace triqs::utility
