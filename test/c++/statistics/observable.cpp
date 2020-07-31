// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Nils Wentzell, tayral

#include <triqs/arrays.hpp>
#include <triqs/statistics.hpp>
#include <iostream>

using triqs::statistics::observable;
using namespace triqs::arrays;

template <typename T> std::ostream &operator<<(std::ostream &out, std::vector<T> const &v) {
  for (auto const &x : v) out << x << " ";
  return out;
}

int main() {

  try {
    {
      auto A = observable<double>{};

      for (int i = 0; i < 1000; ++i) A << 6;
      std::cout << average_and_error(A) << std::endl;
    }

    {
      auto A = observable<array<double, 2>>{};

      for (int i = 0; i < 1000; ++i) A << array<double, 2>{{i, 2 * i}, {3 * i, 4 * i}};

      for (int i = 0; i < 1000; ++i) A << 2 * array<double, 2>{{i, 2 * i}, {3 * i, 4 * i}};

      std::cout << average(A) << std::endl;
      std::cout << average_and_error(A) << std::endl;
    }

    {
      observable<double> A, B;

      for (int i = 0; i < 1000; ++i) {
        A << i;
        B << 5;
      }

      // operations
      auto ab = A / B;
      //auto ab_j = make_jackknife(A) / make_jackknife(B);
      double r = eval(ab, 1);
      std::cout << "eval A/B in 1 " << r << std::endl;

      r = eval(cos(A), 1);
      std::cout << r << " == " << std::cos(1) << std::endl;

      std::cout << "<A/B> = " << average(A / B) << std::endl;
      std::cout << average_and_error(A / B) << std::endl;
      std::cout << average_and_error(cos(A)) << std::endl;
      std::cout << average_and_error(cos(A) / B) << std::endl;
    }
  }
  TRIQS_CATCH_AND_ABORT;
}
