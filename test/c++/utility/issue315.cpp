// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Nils Wentzell

#include <iostream>

#include <triqs/utility/time_pt.hpp>

using triqs::utility::time_pt;
using triqs::utility::time_segment;

int main(int, char **) {

  time_segment seg(10);

  time_pt zero_pt = seg.get_lower_pt();
  time_pt beta_pt = seg.get_upper_pt();
  time_pt pt      = seg.make_time_pt(3);

  std::cout << beta_pt << std::endl;

  std::cout << pt + zero_pt << std::endl;
  std::cout << pt + beta_pt << std::endl;
  std::cout << pt - zero_pt << std::endl;
  std::cout << pt - beta_pt << std::endl;

  std::cout << zero_pt + zero_pt << std::endl;
  std::cout << zero_pt + beta_pt << std::endl;
  std::cout << beta_pt + zero_pt << std::endl;
  std::cout << beta_pt + beta_pt << std::endl;

  std::cout << zero_pt - zero_pt << std::endl;
  std::cout << zero_pt - beta_pt << std::endl;
  std::cout << beta_pt - zero_pt << std::endl;
  std::cout << beta_pt - beta_pt << std::endl;

  return 0;
}
