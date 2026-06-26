// Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides Legendre polynomials and related functions.
 */

#pragma once

#include <array>
#include <complex>

#include <nda/macros.hpp>

namespace triqs::utility {

  /**
   * @addtogroup triqs-utility-math
   * @{
   */

  /**
   * @brief Get the quantity \f$ T_{nl} \f$ from Eq.(E2) in the paper
   * <a href="https://doi.org/10.1103/PhysRevB.84.075145">https://doi.org/10.1103/PhysRevB.84.075145</a>.
   *
   * @param n Matsubara index \f$ n \f$.
   * @param l Legendre polynomial order \f$ l \f$.
   * @return \f$ T_{nl} = (-1)^n i^{l+1} \sqrt{2l + 1} j_l(\bar{\nu_n} / 2) \f$.
   */
  std::complex<double> legendre_T(int n, int l);

  /**
   * @brief Get the quantity \f$ t_l^{(p)} \f$ from Eq.(E8) in the paper
   * <a href="https://doi.org/10.1103/PhysRevB.84.075145">https://doi.org/10.1103/PhysRevB.84.075145</a>.
   *
   * @param l Legendre polynomial order \f$ l \f$.
   * @param p Index \f$ p \f$.
   * @return \f$ t_l^{(p)} = (-1)^p 2 \sqrt{2l + 1} \frac{(l + p - 1)!}{(p - 1)! (l - p + 1)!} \delta_{p+l,\text{odd}}
   * \f$.
   */
  double legendre_t(int l, int p);

  /**
   * @brief Get the modified spherical bessel function \f$ i_n(x) \f$ of the first kind of order \f$ n \f$ evaluated at
   * \f$ x \f$.
   *
   * @param n Order \f$ n \f$ of the modified spherical Bessel function.
   * @param x Value \f$ x \f$ at which to evaluate the function.
   * @return \f$ i_n(x) = \sqrt{\frac{\pi}{2x}} I_{n+1/2}(x) \f$.
   */
  double mod_cyl_bessel_i(int n, double x);

  /**
   * @brief Recursive generation of Legendre polynomials \f$ P_l(x) \f$.
   *
   * @details Legendre polynomials are defined on the interval \f$ [-1, 1] \f$. They form an orthogonal basis with
   * respect to the inner product
   * \f[
   *   \langle P_k, P_l \rangle = \int_{-1}^1 P_k(x) P_l(x) dx = \frac{2}{2l + 1} \delta_{kl} \; .
   * \f]
   *
   * The recurrence relation is given by
   * \f[
   *   (l + 1) P_{l+1}(x) = (2l + 1) x P_l(x) - l P_{l-1}(x) \; ,
   * \f]
   * with \f$ P_0(x) = 1 \f$ and \f$ P_1(x) = x \f$.
   *
   * See [Wikipedia](https://en.wikipedia.org/wiki/Legendre_polynomials) for more information.
   */
  class legendre_generator {
    public:
    /**
     * @brief Construct a Legendre polynomial generator at a given value \f$ x \f$.
     * @param x Value \f$ x \f$ at which to evaluate the Legendre polynomials.
     */
    legendre_generator(double x = 0.0) : x_(x), arr_{1.0, x} { EXPECTS(x >= -1.0 and x <= 1.0); }

    /**
     * @brief Increase the degree of the polynomial from \f$ l \f$ to \f$ l + 1 \f$ using \f$ (l + 1) P_{l+1}(x) =
     * (2l + 1) x P_l(x) - l P_{l-1}(x) \f$.
     *
     * @return Value of the l<sup>th</sup> order Legendre polynomial evaluated at \f$ x \f$.
     */
    double next() {
      if (l_ > 1) {
        auto idx  = static_cast<unsigned int>(l_ % 2);
        arr_[idx] = ((2 * l_ - 1) * x_ * arr_[1 - idx] - (l_ - 1) * arr_[idx]) / l_;
        ++l_;
        return arr_[idx];
      } else {
        ++l_;
        return arr_[l_ - 1];
      }
    }

    /**
     * @brief Reset the generator to 0<sup>th</sup> order and with a new \f$ x \f$ value.
     * @param x Value \f$ x \f$ at which to evaluate the Legendre polynomials.
     */
    void reset(double x) {
      EXPECTS(x >= -1.0 and x <= 1.0);
      x_      = x;
      l_      = 0;
      arr_[0] = 1.0;
      arr_[1] = x;
    }

    private:
    double x_{0.0};
    unsigned int l_{0};
    std::array<double, 2> arr_{1.0, 0.0};
  };

  /** @} */

} // namespace triqs::utility
