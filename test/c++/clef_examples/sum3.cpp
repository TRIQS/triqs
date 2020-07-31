// Copyright (c) 2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014 Centre national de la recherche scientifique (CNRS)
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

#include <triqs/arrays.hpp>
/// ---- test ------------

using itertools::range;

int main() {
  triqs::clef::placeholder<0> a_;
  triqs::clef::placeholder<1> i_;
  triqs::clef::placeholder<2> j_;

  auto expr = i_ + 2 * j_;

  auto xa = sum(a_ * i_, i_ = range(0, 5));
  auto x  = eval(xa, a_ = 2);

  std::cout << x << std::endl;

  auto y = sum(2 * i_ + j_, i_ = range(0, 3), j_ = range(0, 2));
  std::cout << y << std::endl;
}
