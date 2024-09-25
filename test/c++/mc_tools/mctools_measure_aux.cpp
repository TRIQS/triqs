// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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

#include <triqs/mc_tools/mc_measure_aux_set.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <memory>

// Dummy MCMove.
struct dummy_measure_aux {
  int counter{0};
  void operator()() { ++counter; }
};

TEST(TRIQSMCTools, MeasureAuxBasics) {
  // create auxiliary measure
  auto m1_ptr = std::make_shared<dummy_measure_aux>();
  auto m1     = triqs::mc_tools::measure_aux(m1_ptr);

  // perform some measurements
  int const nsteps = 10;
  for (int i = 0; i < nsteps; ++i) m1();

  // check the counter
  EXPECT_EQ(m1_ptr->counter, nsteps);
}

MAKE_MAIN;
