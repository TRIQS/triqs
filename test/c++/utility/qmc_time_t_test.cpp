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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <iostream>

#include <triqs/utility/time_pt.hpp>

using triqs::utility::time_pt;
using triqs::utility::time_segment;

void print_me(double x) { std::cout << " print " << x << std::endl; }

int main(int, char **) {
  time_segment ts(40);
  time_pt zero = ts.get_lower_pt();
  time_pt beta = ts.get_upper_pt();

  auto t1 = div_by_int(beta, 5); // 8
  auto t2 = mult_by_int(t1, 4);  // 32

  std::cout << "zero: " << zero << std::endl;
  std::cout << "beta: " << beta << std::endl;
  std::cout << "t1: " << t1 << std::endl;
  std::cout << "t2: " << t2 << std::endl;

  std::cout << "t1 + t2: " << t1 + t2 << std::endl;
  std::cout << "t1 + t2 - (as double) : " << t1 + t2 - (double(t1) + double(t2)) << "" << std::endl;

  std::cout << "t1 - t2: " << t1 - t2 << std::endl;
  std::cout << "t1 - t2 - (as double) : " << t1 - t2 - (double(t1) - double(t2)) << "" << std::endl;

  std::cout << "t1 * t2: " << t1 * t2 << std::endl;
  std::cout << "t1 * t2 - (as double) : " << t1 * t2 - (double(t1) * double(t2)) << "" << std::endl;

  std::cout << "t1 / t2: " << t1 / t2 << std::endl;
  std::cout << "t1 / t2 - (as double) : " << t1 / t2 - (double(t1) / double(t2)) << "" << std::endl;

  std::cout << "t1 + beta: " << t1 + beta << std::endl;
  std::cout << "t1 - beta: " << t1 - beta << std::endl;

  print_me(double(t1)); // to check explicit cast

  std::cout << " -t1: " << -t1 << std::endl;

  std::cout << "epsilon: " << ts.get_epsilon() << std::endl;
  std::cout << "- epsilon: " << -ts.get_epsilon() << std::endl;
  std::cout << "t1 - epsilon :" << t1 - ts.get_epsilon() << std::endl;
  std::cout << "beta - epsilon :" << beta - ts.get_epsilon() << std::endl;
  std::cout << "beta - epsilon + epsilon:" << (beta - ts.get_epsilon()) + ts.get_epsilon() << std::endl;
  std::cout << "beta + 0:" << beta + zero << std::endl;

  std::cout << " num limit max " << std::numeric_limits<uint64_t>::max() << std::endl;
  uint64_t n = std::numeric_limits<uint64_t>::max() - 2;
  n++;
  std::cout << " num limit-1  " << n << std::endl;
  n++;
  std::cout << " num limit+0  " << n << std::endl;
  n++;
  std::cout << " num limit+1  " << n << std::endl;
  n++;
  std::cout << " num limit+2  " << n << std::endl;

  return 0;
}
