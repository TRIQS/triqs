// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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

#include "./MersenneRNG.hpp"

#ifdef DEBUG
#include <iostream>
#endif // DEBUG

namespace triqs::mc_tools::RandomGenerators {

  double RandMT::eval() {
#ifdef DEBUG
    double r = (double)(randomMT()) / 0xFFFFFFFFU;
    std::cout << "RANDOM " << r << std::endl;
    return r;
#else
    return ((double)(randomMT()) / 0xFFFFFFFFU);
#endif
  }

} // namespace triqs::mc_tools::RandomGenerators
