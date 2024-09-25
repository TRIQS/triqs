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

#include <triqs/mc_tools/concepts.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>
#include <mpi/mpi.hpp>

using namespace triqs::mc_tools;

struct foo {
  double x{0.0};
};

template <typename T> struct dummy_move {
  T attempt() { return 0.0; }
  T accept() { return 0.0; }
};

struct not_a_move {
  double attempt() { return 0.0; }
};

template <typename T> struct dummy_measure {
  void accumulate(T) {}
  void collect_results(mpi::communicator const &) {}
};

struct not_a_measure {
  void accumulate(double) {}
};

// Test the concept DoubleOrComplex.
static_assert(DoubleOrComplex<double>);
static_assert(DoubleOrComplex<std::complex<double>>);
static_assert(not DoubleOrComplex<float>);
static_assert(not DoubleOrComplex<std::complex<float>>);

// Test the concept MCMove.
static_assert(MCMove<dummy_move<double>, double>);
static_assert(MCMove<dummy_move<std::complex<double>>, std::complex<double>>);
static_assert(not MCMove<dummy_move<std::complex<double>>, double>);
static_assert(not MCMove<not_a_move, double>);

// Test the concept MCMeasure.
static_assert(MCMeasure<dummy_measure<double>, double>);
static_assert(MCMeasure<dummy_measure<std::complex<double>>, std::complex<double>>);
static_assert(not MCMeasure<dummy_measure<foo>, double>);
static_assert(not MCMeasure<not_a_measure, double>);

MAKE_MAIN;
