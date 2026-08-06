// Copyright (c) 2025--present, The Simons Foundation
// This file is part of TRIQS/modest and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once

#include "../../utility/report_stream.hpp"

#include <fmt/format.h>

#include <cmath>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace triqs::experimental::utility {

  using std::abs;
  using triqs::utility::report_stream;

  /**
   * @addtogroup triqs-experimental-utility
   * @{
   */

  //------------------------------------------------------
  /**
   * @brief Find a lower and an upper bound that bracket the solution of \f$ f(x) = y \f$.
   *
   * @details Starting from an initial guess, this function steps along \f$ x \f$ in increments of `delta_x` until the
   * target value \f$ y \f$ is bracketed by the function values at the two most recent points (or until the maximum
   * number of steps is reached). The returned bounds are ordered such that the first is smaller than the second.
   *
   * @param f Function \f$ f(x) : \mathbb{R} \to \mathbb{R} \f$ whose root is sought.
   * @param x_init Initial guess for \f$ x \f$.
   * @param y_value Target value \f$ y = f(x) \f$.
   * @param delta_x Step size by which \f$ x \f$ is incremented.
   * @param precision Absolute precision \f$ |f(x) - y| \f$ used to stop the search early.
   * @param max_loops Maximum number of steps.
   * @return Pair \f$ (x_1, x_2) \f$ with \f$ x_1 \leq x_2 \f$ whose function values bracket \f$ y \f$.
   */
  inline std::pair<double, double> find_bounds(std::function<double(double)> f, double x_init, double y_value, double delta_x, double precision,
                                               long max_loops = 1000) {

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
    }

    if (x1 > x2) {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }
    return {x1, x2};
  }
  //------------------------------------------------------

  /**
   * @brief Solve \f$ f(x) = y \f$ on a bracketing interval using the false-position (dichotomy) method.
   *
   * @details Given an interval \f$ [x_\mathrm{low}, x_\mathrm{high}] \f$ that brackets the target value, this method
   * iteratively replaces one of the bounds with the linear (secant) estimate of the root until the residual drops below
   * the requested precision. 
   * 
   * It raises an error if convergence is not reached within `max_loops` iterations.
   *
   * @param f Function \f$ f(x) : \mathbb{R} \to \mathbb{R} \f$ whose root is sought.
   * @param x_low Lower bound of the bracketing interval.
   * @param x_high Upper bound of the bracketing interval.
   * @param y_target Target value \f$ y \f$ for \f$ f(x) \f$.
   * @param precision Absolute precision \f$ |f(x) - y| \f$ at which the iteration stops.
   * @param max_loops Maximum number of iterations.
   * @param x_name Name of the \f$ x \f$ variable, used in the progress log.
   * @param y_name Name of the \f$ y \f$ variable, used in the progress log.
   * @param verbosity Whether to print the convergence progress.
   * @return Pair \f$ (x, f(x)) \f$ with \f$ f(x) = y \f$ within the requested precision.
   */
  inline std::pair<double, double> dichotomy(std::function<double(double)> f, double x_low, double x_high, double y_target, double precision,
                                             long max_loops, std::string x_name, std::string y_name, bool verbosity) {

    auto out = report_stream(std::cout, verbosity);

    auto x1 = x_low;
    auto x2 = x_high;

    auto y1 = f(x_low);
    auto y2 = f(x_high);

    out << fmt::format(" {:>4} | {:^24} | residual\n", "iter", fmt::format("{} interval", x_name));
    out << fmt::format("------+--------------------------+----------\n");

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
      out << fmt::format(" {:4d} | [{:10g}, {:10g}]   | {:8.2e}\n", nbre_loop + 1, x1, x2, abs(yfound - y_target));
    }

    if (abs(yfound - y_target) < precision) {
      out << fmt::format("Converged ({} iters): {} = {:g}, {} = {:g}\n", nbre_loop, x_name, x, y_name, yfound);
      return {x, yfound};
    } else {
      out << fmt::format("Failed: {} did not converge to {:g} after {} iters\n", x_name, y_target, nbre_loop);
      throw std::runtime_error{fmt::format("Dichotomy adjustment for {} failed after {} iterations", x_name, nbre_loop)};
    }
  }
  //------------------------------------------------------

  /**
   * @brief Solve \f$ f(x) = y \f$ on a bracketing interval using the bisection method.
   *
   * @details Given an interval \f$ [x_\mathrm{low}, x_\mathrm{high}] \f$ that brackets the target value, this method
   * repeatedly halves the interval, keeping the half in which the target value remains bracketed, until the residual
   * drops below the requested precision. 
   * 
   * It raises an error if convergence is not reached within `max_loops` iterations.
   *
   * @param f Function \f$ f(x) : \mathbb{R} \to \mathbb{R} \f$ whose root is sought.
   * @param x_low Lower bound of the bracketing interval.
   * @param x_high Upper bound of the bracketing interval.
   * @param y_target Target value \f$ y \f$ for \f$ f(x) \f$.
   * @param precision Absolute precision \f$ |f(x) - y| \f$ at which the iteration stops.
   * @param max_loops Maximum number of iterations.
   * @param x_name Name of the \f$ x \f$ variable, used in the progress log.
   * @param y_name Name of the \f$ y \f$ variable, used in the progress log.
   * @param verbosity Whether to print the convergence progress.
   * @return Pair \f$ (x, f(x)) \f$ with \f$ f(x) = y \f$ within the requested precision.
   */
  inline std::pair<double, double> bisection(std::function<double(double)> f, double x_low, double x_high, double y_target, double precision,
                                             long max_loops, std::string x_name, std::string y_name, bool verbosity) {

    auto out = report_stream(std::cout, verbosity);

    out << fmt::format(" {:>4} | {:^24} | residual\n", "iter", fmt::format("{} interval", x_name));
    out << fmt::format("------+--------------------------+----------\n");

    for (auto it = 0; it < max_loops; it++) {
      auto x_mid = (x_high + x_low) / 2.0;
      auto y_mid = f(x_mid);

      if (abs(y_mid - y_target) <= precision) {
        out << fmt::format("Converged ({} iters): {} = {:g}, {} = {:g}\n", it + 1, x_name, x_mid, y_name, y_mid);
        return {x_mid, y_mid};
      }

      (y_mid - y_target >= 0 ? x_high : x_low) = x_mid;

      out << fmt::format(" {:4d} | [{:10g}, {:10g}]   | {:8.2e}\n", it + 1, x_low, x_high, abs(y_mid - y_target));
    }

    out << fmt::format("Failed: {} did not converge to {:g} after {} iters\n", x_name, y_target, max_loops);

    throw std::runtime_error{fmt::format("Bisection adjustment for {} failed after {} iterations", x_name, max_loops)};
  }
  //------------------------------------------------------

  /**
   * @brief Find the value \f$ x \f$ that solves \f$ f(x) = y \f$ using the requested root-finding method.
   *
   * @details This is the main entry point of the root finder. It first brackets the solution and then refines it with
   * either a dichotomy or a bisection step, depending on the chosen method.
   *
   * It raises an error if the method name is not recognized.
   *
   * @param method Root-finding method, either `"dichotomy"` or `"bisection"`.
   * @param f Function \f$ f(x) : \mathbb{R} \to \mathbb{R} \f$ whose root is sought.
   * @param x_init Initial guess for \f$ x \f$.
   * @param y_value Target value \f$ y \f$ for \f$ f(x) \f$.
   * @param precision Absolute precision \f$ |f(x) - y| \f$ at which the iteration stops.
   * @param delta_x Step size used when bracketing the solution.
   * @param max_loops Maximum number of iterations.
   * @param x_name Name of the \f$ x \f$ variable, used in the progress log.
   * @param y_name Name of the \f$ y = f(x) \f$ variable, used in the progress log.
   * @param verbosity Whether to print the convergence progress.
   * @return Pair \f$ (x, f(x)) \f$ with \f$ f(x) = y \f$ within the requested precision.
   */
  inline std::pair<double, double> root_finder(std::string method, std::function<double(double)> f, double x_init, double y_value, double precision,
                                               double delta_x, long max_loops = 1000, std::string x_name = "", std::string y_name = "",
                                               bool verbosity = false) {

    if (verbosity) fmt::print("Root finder: seeking {} s.t. {} = {:g} \u00b1 {:g}\n", x_name, y_name, y_value, precision);

    auto [x1, x2] = find_bounds(f, x_init, y_value, delta_x, precision, max_loops);

    if (method == "dichotomy") {
      return dichotomy(f, x1, x2, y_value, precision, max_loops, x_name, y_name, verbosity);
    } else if (method == "bisection") {
      return bisection(f, x1, x2, y_value, precision, max_loops, x_name, y_name, verbosity);
    } else {
      throw std::runtime_error("Not a valid choice of root finder method!");
    }
  }

  /** @} */

} // namespace triqs::experimental::utility
