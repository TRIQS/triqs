// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Henri Menke, Olivier Parcollet, Nils Wentzell

#pragma once

#include <nda/nda.hpp>

#include <cmath>
#include <complex>
#include <numbers>
#include <random>

// Matrix builder #1.
template <typename T> struct builder1 {
  static constexpr double beta = 10.0;
  static constexpr double epsi = 0.1;
  T operator()(double x, double y) const {
    using std::numbers::pi;
    using namespace std::complex_literals;
    double tau = x - y;
    bool s     = (tau > 0);
    tau        = (s ? tau : beta + tau);
    double r   = epsi + tau / beta * (1 - 2 * epsi);
    if constexpr (std::same_as<T, double>) {
      return -2 * (pi / beta) / std::sin(pi * r);
    } else {
      return -2i * (pi / beta) / std::sin(pi * r);
    }
  }
};

// Matrix builder #2.
struct builder2 {
  double operator()(int x, int y) const {
    if ((x < 0) and (y < 0))
      return 0;
    else
      return 2 / double(x + y);
  }
};

// Matrix builder #3.
template <typename T> struct builder3 {
  nda::matrix<T> M;
  builder3() : M(20, 20) {
    std::mt19937 gen{12345};
    std::uniform_real_distribution<double> dis(-2, 2);
    if constexpr (std::is_same_v<T, double>) {
      nda::for_each(M.shape(), [&](int i, int j) { M(i, j) = dis(gen); });
    } else {
      nda::for_each(M.shape(), [&](int i, int j) { M(i, j) = {dis(gen), dis(gen)}; });
    }
  }
  T operator()(int i, int j) const { return M(i, j); }
};

// Matrix builder #4.
struct builder4 {
  double operator()(int x, int y) const {
    if ((x < 0) and (y < 0))
      return 1;
    else
      return x + y;
  }
};
