// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
//#include "utils/defs.hpp"
#include <fmt/core.h>
#include <triqs/utility/report_stream.hpp>

namespace triqs::utility {

  using std::abs;

  //------------------------------------------------------
  /**
 * @ingroup root
 * @brief find upper and lower bounds of f(x)
 *
 * @param f function of f(x)
 * @param x_init initial guess x
 * @param y_value target value y = f(x)
 * @param delta_x x increment
 * @param precision f(x) - y_value < precision
 * @param max_loops maximum number of loops
 * @param verbosity verbosity of the algorithm
 * @return x1, x2 where f(x1) and f(x2) bound y_value
 */
  inline std::pair<double, double> find_bounds(std::function<double(double)> f, double x_init, double y_value, double delta_x, double precision,
                                               long max_loops = 1000, bool verbosity = false) {

    auto out = ostream_with_verbosity(std::cout, verbosity);
    double x = x_init;
    delta_x  = abs(delta_x);

    double y1  = f(x);
    double eps = (y1 - y_value >= 0) ? 1.0 : -1.0;

    double x1 = x;
    double y2 = y1;
    double x2 = x1;

    long nbre_loop = 0;
    for (; nbre_loop <= max_loops && (y2 - y_value) * eps > 0 && abs(y2 - y_value) > precision; ++nbre_loop) {
      x2 -= eps * delta_x;
      y2 = f(x2);
      out << fmt::format("x={}, f(x)= {}\n", x2, y2);
    }

    if (x1 > x2) {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }
    return {x1, x2};
  }
  //------------------------------------------------------

  /**
 * @ingroup root
 * @brief dichotomy algorithm
 *
 * @param f function f(x)
 * @param x_low lower bound on x
 * @param x_high upper bound on x
 * @param y_target target value for f(x)
 * @param precision f(x) - y_target < precision
 * @param max_loops maxmimum number of iterations
 * @param x_name Name of x variable
 * @param y_name Name of y variable
 * @param verbosity Print progress
 * @return x, f(x) where f(x) = y_target
 */
  inline std::pair<double, double> dichotomy(std::function<double(double)> f, double x_low, double x_high, double y_target, double precision,
                                             long max_loops, std::string x_name, std::string y_name, bool verbosity) {

    auto out = ostream_with_verbosity(std::cout, verbosity);

    auto x1 = x_low;
    auto x2 = x_high;

    auto y1 = f(x_low);
    auto y2 = f(x_high);

    out << fmt::format("{} < {} < {}\n", x1, x_name, x2);
    out << fmt::format("{} < {} < {}\n", y1, y_name, y2);

    double yfound = (abs(y1 - y_target) < abs(y2 - y_target)) ? y1 : y2;
    double x      = (abs(y1 - y_target) < abs(y2 - y_target)) ? x1 : x2;

    long nbre_loop = 0;
    for (; nbre_loop <= max_loops && abs(yfound - y_target) > precision; ++nbre_loop) {
      x      = x1 + (x2 - x1) * (y_target - y1) / (y2 - y1);
      yfound = f(x);
      if ((y1 - y_target) * (yfound - y_target) > 0) {
        x1 = x;
        y1 = yfound;
      } else {
        x2 = x;
        y2 = yfound;
      }
      out << fmt::format("{} < {} < {}\n", x1, x_name, x2);
      out << fmt::format("{} < {} < {}\n", y1, y_name, y2);
    }

    if (std::abs(yfound - y_target) < precision) {
      out << fmt::format("{} found in {} iterations:\n", x_name, nbre_loop);
      out << fmt::format("{} = {}; {} = {}\n", y_name, yfound, x_name, x);
      return {x, yfound};
    } else {
      out << fmt::format("FAILURE to adjust {} to the value {} after {} iterations.\n", x_name, y_target, nbre_loop);
      throw std::runtime_error{fmt::format("Dichotomy adjustment for {} failed after {} iterations", x_name, nbre_loop)};
    }
  }
  //------------------------------------------------------

  /**
 * @ingroup root
 * @brief bisection algorithm
 *
 * @param f function f(x)
 * @param x_low lower bound on f(x)
 * @param x_high upper bound on f(x)
 * @param y_target target value for f(x) = y_target
 * @param precision f(x) - y_target < precision
 * @param max_loops maximum number of attempts
 * @param x_name Name of x variable
 * @param y_name Name of y variable
 * @param verbosity Print progress
 * @return x, f(x) where f(x) = y_target
 */
  inline std::pair<double, double> bisection(std::function<double(double)> f, double x_low, double x_high, double y_target, double precision,
                                             long max_loops, std::string x_name, std::string y_name, bool verbosity) {

    auto out = ostream_with_verbosity(std::cout, verbosity);

    auto y_low  = f(x_low);
    auto y_high = f(x_high);

    out << fmt::format("{} < {} < {}\n", x_low, x_name, x_high);
    out << fmt::format("{} < {} < {}\n", y_low, y_name, y_high);

    for (auto it = 0; it < max_loops; it++) {
      auto x_mid = (x_high + x_low) / 2.0;
      auto y_mid = f(x_mid);

      if (abs(y_mid - y_target) <= precision) return {x_mid, y_mid};

      if (y_mid - y_target >= 0) {
        x_high = x_mid;
        y_high = y_mid;
      } else {
        x_low = x_mid;
        y_low = y_mid;
      }

      out << fmt::format("{} < {} < {}\n", x_low, x_name, x_high);
      out << fmt::format("{} < {} < {}\n", y_low, y_name, y_high);
    }

    out << fmt::format("FAILURE to adjust {} to the value {} after {} iterations.\n", x_name, y_target, max_loops);
    throw std::runtime_error{fmt::format("Dichotomy adjustment for {} failed after {} iterations", x_name, max_loops)};
  }
  //------------------------------------------------------

  /**
   * @brief Root finder \f$ f(x) = 0 \f$.
   *
   * @param method Root finding method (`dichtomy` or `bisection`).
   * @param f \f$ f(x) : \mathbb{R} \to \mathbb{R} \f$.
   * @param x_init Initial value for \f$ x \f$.
   * @param y_value Target value for \f$ y \f$.
   * @param precision Precision for algorithm.
   * @param delta_x Increment of \f$ x \f$.
   * @param max_loops Max number of iterations.
   * @param x_name Name of \f$ x \f$ variable.
   * @param y_name Name of \f$ y = f(x) \f$ variable.
   * @param verbosity Turn on/off logging.
   * @return \f$ x, f(x) \f$ where \f$ f(x) = y \f$.
   */
  inline std::pair<double, double> root_finder(std::string method, std::function<double(double)> f, double x_init, double y_value, double precision,
                                               double delta_x, long max_loops = 1000, std::string x_name = "", std::string y_name = "",
                                               bool verbosity = false) {

    auto out = ostream_with_verbosity(std::cout, verbosity);

    out << fmt::format("Root finder search of {} to obtain {} = {} +/- {}\n", x_name, y_name, y_value, precision);

    auto [x1, x2] = find_bounds(f, x_init, y_value, delta_x, precision, max_loops, verbosity);

    if (method == "dichotomy") {
      return dichotomy(f, x1, x2, y_value, precision, max_loops, x_name, y_name, verbosity);
    } else if (method == "bisection") {
      return bisection(f, x1, x2, y_value, precision, max_loops, x_name, y_name, verbosity);
    } else {
      throw std::runtime_error("Not a valid choice of root finder method!");
    }
  }

} // namespace triqs::utility
