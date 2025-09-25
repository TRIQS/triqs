// Copyright (c) 2019-2023 Simons Foundation
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
// Authors: Philipp D, Olivier Parcollet, Nils Wentzell

#include <triqs/stat/concepts.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <array>
#include <complex>
#include <list>
#include <vector>

TEST(TRIQSStat, StatCompatible) {
  using namespace triqs::stat;
  static_assert(StatCompatible<double>);
  static_assert(StatCompatible<nda::array<double, 2>>);
  static_assert(StatCompatible<nda::array<std::complex<double>, 1>>);
  static_assert(!StatCompatible<nda::vector<std::complex<double>>>);
  static_assert(!StatCompatible<nda::matrix<std::complex<double>>>);
  static_assert(!StatCompatible<int>);
  static_assert(!StatCompatible<float>);
  static_assert(!StatCompatible<nda::array<float, 1>>);
}

TEST(TRIQSStat, AccCompatible) {
  using namespace triqs::stat;
  static_assert(AccCompatible<double>);
  static_assert(AccCompatible<nda::array<double, 2>>);
  static_assert(AccCompatible<nda::array<std::complex<double>, 1>>);
  static_assert(!AccCompatible<nda::vector<std::complex<double>>>);
  static_assert(!AccCompatible<nda::matrix<std::complex<double>>>);
  static_assert(!AccCompatible<int>);
  static_assert(!AccCompatible<float>);
  static_assert(!AccCompatible<nda::array<float, 1>>);
  static_assert(!AccCompatible<nda::array_view<double, 2>>);
  static_assert(!AccCompatible<nda::array_view<std::complex<double>, 1>>);
}

TEST(TRIQSStat, StatCompatibleRange) {
  using namespace triqs::stat;
  static_assert(StatCompatibleRange<std::vector<double>>);
  static_assert(StatCompatibleRange<std::list<nda::array<double, 2>>>);
  static_assert(StatCompatibleRange<std::array<nda::array<std::complex<double>, 1>, 5>>);
  static_assert(StatCompatibleRange<nda::vector<nda::array<std::complex<double>, 1>>>);
  static_assert(!StatCompatibleRange<std::vector<nda::vector<std::complex<double>>>>);
  static_assert(!StatCompatibleRange<nda::vector<nda::matrix<std::complex<double>>>>);
  static_assert(!StatCompatibleRange<std::array<int, 3>>);
  static_assert(!StatCompatibleRange<std::vector<float>>);
  static_assert(!StatCompatibleRange<std::list<nda::array<float, 1>>>);
}

MAKE_MAIN;
