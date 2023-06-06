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
// Authors: Nils Wentzell

#include "legendre.hpp"

#include <boost/math/special_functions/bessel.hpp>

using namespace std::complex_literals;

std::complex<double> triqs::utility::legendre_T(int n, int l) {

  bool neg_n = false;
  if (n < 0) {
    neg_n = true;
    n     = std::abs(n + 1);
  }

  // note: cyl_bessel_j(l,x) give the Bessel functions of the first kind J_l
  // (x) one then gets the spherical Bessel with j_l (x) = \sqrt{\pi / (2x)}
  // J_{l+0.5} (x)
  std::complex<double> res =
     (sqrt(2 * l + 1) / sqrt(2 * n + 1)) * exp(1i * (n + 0.5) * M_PI) * std::pow(1i, l) * boost::math::cyl_bessel_j(l + 0.5, (n + 0.5) * M_PI);

  // T_{-nl} = T_{nl}^*
  return neg_n ? std::conj(res) : res;
}

double triqs::utility::legendre_t(int l, int p) {

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

double triqs::utility::mod_cyl_bessel_i(int n, double x) {
  if (x == 0) return (n == 0 ? 1.0 : 0);
  return std::sqrt(M_PI / (2 * x)) * boost::math::cyl_bessel_i(n + 0.5, x);
}
