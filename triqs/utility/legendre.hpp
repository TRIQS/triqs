/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by L. Boehnke, M. Ferrero, O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/constants/constants.hpp>
#include <complex>
#include <ostream>

namespace triqs {
  namespace utility {

    const std::complex<double> i_c(0.0, 1.0);
    const double pi = boost::math::constants::pi<double>();

    // This is T_{nl} following Eq.(E2) of our paper
    inline std::complex<double> legendre_T(int n, int l) {

      bool neg_n = false;
      if (n < 0) {
        neg_n = true;
        n     = std::abs(n + 1);
      }

      // note: cyl_bessel_j(l,x) give the Bessel functions of the first kind J_l (x)
      // one then gets the spherical Bessel with j_l (x) = \sqrt{\pi / (2x)} J_{l+0.5} (x)
      std::complex<double> res =
         (sqrt(2 * l + 1) / sqrt(2 * n + 1)) * exp(i_c * (n + 0.5) * pi) * std::pow(i_c, l) * boost::math::cyl_bessel_j(l + 0.5, (n + 0.5) * pi);
      // T_{-nl} = T_{nl}^*
      return neg_n ? std::conj(res) : res;
    }

    // This is t_l^p following Eq.(E8) of our paper
    inline double legendre_t(int l, int p) {

      // p is the 1/omega power, it can't be negative
      assert(p > 0);

      // in these two cases we can directly give back 0
      if ((l + p) % 2 == 0 || p > l + 1) return 0.0;

      // the factorials are done here
      double f = 1;
      for (int i = l + p - 1; (i > l - p + 1) && (i > 1); i--) f *= i;
      for (int i = p - 1; i > 1; i--) f /= i;

      return std::pow(double(-1), double(p)) * 2 * sqrt(2 * l + 1) * f;
    }

    /*
   Generates the Legendre polynomials
     P_0(x) = 1.0
     P_1(x) = x
     n P_{n} = (2n-1) x P_{n-1}(x) - (n-1) P_{n-2}(x)
 */
    class legendre_generator {

      double _x;
      unsigned int n;
      double cyclicArray[2];

      public:
      double next() {
        if (n > 1) {
          unsigned int eo = (n) % 2;
          cyclicArray[eo] = ((2 * n - 1) * _x * cyclicArray[1 - eo] - (n - 1) * cyclicArray[eo]) / n;
          n++;
          return cyclicArray[eo];
        } else {
          n++;
          return cyclicArray[n - 1];
        }
      }

      void reset(double x) {
        _x             = x;
        n              = 0;
        cyclicArray[0] = 1.0;
        cyclicArray[1] = x;
      }
    };

    // Modified spherical Bessel function of the first kind i(n,x)
    inline double mod_cyl_bessel_i(int n, double x) {
      if (x == 0) return (n == 0 ? 1.0 : 0);
      return std::sqrt(M_PI / (2 * x)) * boost::math::cyl_bessel_i(n + 0.5, x);
    }

  } // namespace utility
} // namespace triqs
