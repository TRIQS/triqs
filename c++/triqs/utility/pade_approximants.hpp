// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
// Copyright (c) 2017 Igor Krivenko
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
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Hiroshi Shinaoka, Nils Wentzell

/**
 * @file
 * @brief Padé approximants for analytic continuation, computed with arbitrary-precision GMP arithmetic.
 */

#pragma once

#include "./exceptions.hpp"

#include <gmpxx.h>
#include <triqs/arrays.hpp>

#include <complex>
#include <ostream>

namespace triqs::utility {

  // Alias for `std::complex<double>`.
  using dcomplex = std::complex<double>;

  /**
   * @addtogroup triqs-utility-math
   * @{
   */

  /**
   * @brief Lightweight complex number backed by GMP `mpf_class` floats, used during Padé coefficient computation.
   */
  struct gmp_complex {
    /// Real part.
    mpf_class re;

    /// Imaginary part.
    mpf_class im;

    /**
     * @brief Multiply two complex numbers.
     * 
     * @param z Right-hand operand.
     * @return Complex product.
     */
    gmp_complex operator*(const gmp_complex &z) const { return {.re = z.re * re - z.im * im, .im = z.re * im + z.im * re}; }

    /**
     * @brief Compute the multiplicative inverse of a complex number.
     * 
     * @details It throws a triqs::runtime_error on division by zero.
     * 
     * @param z Complex number \f$ z \f$ to invert.
     * @return Multiplicative inverse \f$ 1 / z \f$.
     */
    friend gmp_complex inverse(const gmp_complex &z) {
      mpf_class d = z.re * z.re + z.im * z.im;
      if (d == 0) TRIQS_RUNTIME_ERROR << "pade_approximant: GMP division by zero";
      return {.re = z.re / d, .im = -z.im / d};
    }

    /**
     * @brief Divide two complex numbers.
     * 
     * @details It throws a triqs::runtime_error on division by zero.
     * 
     * @param z Right-hand operand (divisor).
     * @return Complex quotient.
     */
    gmp_complex operator/(const gmp_complex &z) const { return (*this) * inverse(z); }

    /**
     * @brief Add two complex numbers.
     * 
     * @param z Right-hand operand.
     * @return Complex sum.
     */
    gmp_complex operator+(const gmp_complex &z) const { return {.re = z.re + re, .im = z.im + im}; }

    /**
     * @brief Subtract two complex numbers.
     * 
     * @param z Right-hand operand (subtrahend).
     * @return Complex difference.
     */
    gmp_complex operator-(const gmp_complex &z) const { return {.re = re - z.re, .im = im - z.im}; }

    /**
     * @brief Extract the real part of a complex number.
     * 
     * @param z Number \f$ z \f$ to inspect.
     * @return Real part \f$ \Re(z) \f$.
     */
    friend mpf_class real(const gmp_complex &z) { return z.re; }

    /**
     * @brief Extract the imaginary part of a complex number.
     * 
     * @param z Number \f$ z \f$ to inspect.
     * @return Imaginary part \f$ \Im(z) \f$.
     */
    friend mpf_class imag(const gmp_complex &z) { return z.im; }

    /**
     * @brief Squared magnitude of the complex number \f$ z \f$.
     * @return \f$ |z|^2 = \Re(z)^2 + \Im(z)^2 \f$.
     */
    [[nodiscard]] mpf_class norm() const { return real(*this) * real(*this) + imag(*this) * imag(*this); }

    /**
     * @brief Assign from a regular `std::complex<double>`.
     * 
     * @param z Value to assign from.
     * @return Reference to `*this`.
     */
    gmp_complex &operator=(const std::complex<double> &z) {
      re = real(z);
      im = imag(z);
      return *this;
    }

    /**
     * @brief Write a @ref gmp_complex to an output stream.
     * 
     * @param out Output stream to write to.
     * @param z Complex number to print.
     * @return Reference to the output stream.
     */
    friend std::ostream &operator<<(std::ostream &out, gmp_complex const &z) {
      return out << " gmp_complex(" << z.re << "," << z.im << ")" << std::endl;
    }
  };

  /**
   * @brief Padé approximant of a complex function sampled at \f$ \{ z_i \} \f$ with values \f$ \{ u_i \} \f$.
   *
   * @details Given \f$ N \f$ sample points \f$ z_i \f$ and values \f$ u_i = u(z_i) \f$, the Padé approximant is
   * represented as the continued fraction
   * \f[
   *   C_N(z) = \cfrac{a_1}{1 + \cfrac{a_2 (z - z_1)}{1 + \cfrac{a_3 (z - z_2)}{1 + \cdots
   *            \cfrac{a_N (z - z_{N-1})}{1}}}} ,
   * \f]
   * which interpolates the data exactly, i.e. \f$ C_N(z_i) = u_i \f$.
   *
   * The coefficients are \f$ a_i = g_i(z_i) \f$, where the \f$ g_p \f$ are obtained from the Vidberg–Serene recurrence
   * \f[
   *   g_1(z_i) = u_i , \qquad
   *   g_p(z_i) = \frac{g_{p-1}(z_{p-1}) - g_{p-1}(z_i)}{(z_i - z_{p-1})\, g_{p-1}(z_i)} , \quad p \geq 2 .
   * \f]
   * These coefficients are computed with GMP arbitrary precision (`GMP_default_prec` mantissa bits), and the continued
   * fraction is then evaluated in standard `std::complex<double>` precision via operator()().
   *
   * See also <a href="https://doi.org/10.1007/BF00655090">J. Low Temp. Phys. 29, 179 (1977)</a>.
   */
  class pade_approximant {

    nda::vector<dcomplex> z_in;
    nda::vector<dcomplex> a;

    public:
    /// Default precision (in mantissa bits) used for the GMP floats during coefficient calculation.
    static const int GMP_default_prec = 256;

    /**
     * @brief Constructor computes the Padé coefficients from a set of complex sample points and values.
     *
     * @details Builds the lower-triangular table \f$ g_{p}(z_j) \f$ from the Vidberg–Serene recurrence
     * \f[
     *   g_1(z_j) = u_j , \qquad
     *   g_p(z_j) = \frac{g_{p-1}(z_{p-1}) - g_{p-1}(z_j)}{(z_j - z_{p-1})\, g_{p-1}(z_j)} , \quad p \geq 2 ,
     * \f]
     * and stores the continued-fraction coefficients \f$ a_j = g_j(z_j) \f$ as the diagonal of that table. The
     * recurrence is truncated once \f$ |g_{p-1}(z_{p-1})|^2 < 10^{-20} \f$.
     *
     * @param z_in Complex points where the function is known.
     * @param u_in Function values at the corresponding points.
     */
    pade_approximant(const nda::vector<dcomplex> &z_in, const nda::vector<dcomplex> &u_in) : z_in(z_in), a(z_in.size()) {

      long N = z_in.size();

      // temporarily switch GMP's default precision
      unsigned long old_prec = mpf_get_default_prec();
      mpf_set_default_prec(GMP_default_prec);

      nda::array<gmp_complex, 2> g(N, N);
      gmp_complex MP_0 = {.re = 0.0, .im = 0.0};
      g()              = MP_0;
      for (long f = 0; f < N; ++f) { g(0, f) = u_in(f); }

      gmp_complex MP_1 = {.re = 1.0, .im = 0.0};

      for (long p = 1; p < N; ++p) {

        // truncate the continued fraction when |g| becomes very small
        if (g(p - 1, p - 1).norm() < 1.0e-20) break;

        for (long j = p; j < N; ++j) {
          gmp_complex x = g(p - 1, p - 1) / g(p - 1, j) - MP_1;
          gmp_complex y;
          y       = z_in(j) - z_in(p - 1);
          g(p, j) = x / y;
        }
      }

      for (long j = 0; j < N; ++j) {
        gmp_complex gj = g(j, j);
        a(j)           = dcomplex(real(gj).get_d(), imag(gj).get_d());
      }

      // restore the precision.
      mpf_set_default_prec(old_prec);
    }

    /**
     * @brief Evaluate the Padé continued fraction at the complex point \f$ z \f$.
     *
     * @details The continued fraction \f$ C_N(z) = A_N / B_N \f$ is evaluated with the standard recurrence for the
     * numerators and denominators of the convergents,
     * \f[
     *   A_{n+1} = A_n + (z - z_n)\, a_{n+1}\, A_{n-1} , \qquad
     *   B_{n+1} = B_n + (z - z_n)\, a_{n+1}\, B_{n-1} ,
     * \f]
     * starting from \f$ A_0 = 0 \f$, \f$ A_1 = a_1 \f$ and \f$ B_0 = B_1 = 1 \f$. Each step is rescaled by \f$ B_{n+1} 
     * \f$ to avoid overflow.
     *
     * @param z Complex evaluation point.
     * @return The Padé approximant value at \f$ z \f$.
     */
    dcomplex operator()(dcomplex z) const {

      dcomplex A1(0);
      dcomplex A2 = a(0);
      dcomplex B1(1.0);

      long N = a.size();
      for (long i = 0; i <= N - 2; ++i) {
        dcomplex Anew = A2 + (z - z_in(i)) * a(i + 1) * A1;
        dcomplex Bnew = 1.0 + (z - z_in(i)) * a(i + 1) * B1;
        A1            = A2 / Bnew;
        A2            = Anew / Bnew;
        B1            = 1.0 / Bnew;
      }

      return A2;
    }
  };

  /** @} */

} // namespace triqs::utility
