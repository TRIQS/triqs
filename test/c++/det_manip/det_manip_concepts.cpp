// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Henri Menke, Olivier Parcollet, Nils Wentzell

#include <triqs/det_manip/concepts.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <complex>
#include <string>
#include <vector>

struct builder {
  int size{0};
  double operator()(int i, int j) const { return i * size + j; }
};

struct not_a_builder {
  int size{0};
  int operator()(int i, int j) const { return i * size + j; }
};

TEST(TRIQSDetManip, MatrixBuilderConcept) {
  using namespace triqs::det_manip;
  auto lambda_builder = [](std::string, std::complex<float>) { return std::complex<double>{1.0, 1.0}; };
  static_assert(MatrixBuilder<builder>);
  static_assert(not MatrixBuilder<not_a_builder>);
  static_assert(MatrixBuilder<decltype(lambda_builder)>);
}

TEST(TRIQSDetManip, MatrixBuilderRangeConcepts) {
  using namespace triqs::det_manip;
  static_assert(MatrixBuilderXRange<std::vector<int>, builder>);
  static_assert(MatrixBuilderYRange<std::vector<int>, builder>);
  static_assert(not MatrixBuilderXRange<std::vector<std::string>, builder>);
  static_assert(not MatrixBuilderYRange<std::vector<std::complex<double>>, builder>);
}

MAKE_MAIN
