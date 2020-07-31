// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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

#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>
#include <iostream>

using namespace triqs::gfs;
using namespace triqs::arrays;
using namespace triqs::utility;
using namespace triqs::clef;

namespace std {} // namespace std

TEST(Gf, Fourier) { // NOLINT

  double beta = 10;
  int n_iw    = 100;

  auto gw = gf<imfreq, scalar_valued>{{beta, Fermion, n_iw}};

  placeholder<0> w_;
  gw(w_) << 1. / (w_ - 1);

  std::cout << fit_tail(gw) << "\n";

  auto gt  = make_gf_from_fourier(gw, 100 * n_iw);
  auto gw2 = make_gf_from_fourier(gt);

  std::cout << fit_tail(gw2) << "\n";
}

TEST(Gf, Tail) { // NOLINT

  double beta = 500;
  int n_iw    = 3000;

  auto g = gf<imfreq, scalar_valued>{{beta, Fermion, n_iw}};

  placeholder<0> w_;
  g(w_) << (1. / (w_ - 5) + 1. / (w_ + 10)) / 2.0;

  std::cout << fit_tail(g) << "\n\n";

  auto known_moments = array<dcomplex, 1>(2);

  known_moments(0) = 0.0;
  known_moments(1) = 1.0;
  std::cout << fit_tail(g, known_moments) << "\n\n";
}
MAKE_MAIN;
