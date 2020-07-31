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

#include "./common.hpp"
#include <triqs/clef/adapters/vector.hpp>

using namespace triqs::clef;

triqs::clef::placeholder<10> i_;
triqs::clef::placeholder<11> j_;

int main() {

  std::vector<int> V(3), W(3);
  V[0] = 14;
  V[1] = 2;
  V[2] = 3;
  std::cout << "V = " << V[0] << " " << V[1] << " " << V[2] << std::endl;

  TEST(tql::eval(make_expr(V)[i_], i_ = 0));

  make_expr(V)[i_] << i_ + 2;

  std::cout << "V = " << V[0] << " " << V[1] << " " << V[2] << std::endl;

  make_expr(W)[i_] << i_ + make_expr(V)[i_];

  std::cout << "W = " << W[0] << " " << W[1] << " " << W[2] << std::endl;

  std::vector<std::vector<int>> v2(3, std::vector<int>(2));

  make_expr(v2)[i_][j_] << (i_ + j_ + 1);

  for (size_t u = 0; u < v2.size(); ++u)
    for (size_t up = 0; up < v2[0].size(); ++up) std::cout << v2[u][up] << u + up + 1 << std::endl;
}
