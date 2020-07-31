// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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

#include "triqs/clef.hpp"
#include "triqs/clef/adapters/vector.hpp"
#include "triqs/clef/adapters/math.hpp"
#include <iostream>
namespace tql = triqs::clef;

int main() {

  int N     = 10;
  double pi = std::acos(-1);
  std::vector<double> V(N);

  // automatic assignment of vector and use of make_expr math function
  tql::placeholder<0> k_;
  tql::make_expr(V)[k_] << cos((2 * pi * k_) / N);

  // check result...
  for (size_t u = 0; u < V.size(); ++u) std::cout << u << " " << V[u] << "  " << cos((2 * pi * u) / N) << std::endl;
}
