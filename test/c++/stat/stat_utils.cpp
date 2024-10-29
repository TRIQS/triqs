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

#include <triqs/stat/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <complex>
#include <concepts>

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
}

TEST(TRIQSStat, ZeroedSample) {
  using namespace triqs::stat;
  using arr_d_type = nda::array<double, 3>;
  using arr_c_type = nda::array<std::complex<double>, 1>;
  EXPECT_EQ(zeroed_sample(1.0), 0);
  EXPECT_EQ(zeroed_sample(std::complex<double>{1.2, 2.3}), (std::complex<double>{0, 0}));
  EXPECT_ARRAY_EQ(zeroed_sample(arr_d_type::rand(3, 4, 5)), arr_d_type::zeros(3, 4, 5));
  EXPECT_ARRAY_EQ(zeroed_sample(arr_c_type::rand(3)), arr_c_type::zeros(3));
}

TEST(TRIQSStat, MakeReal) {
  using namespace triqs::stat;

  double r_d    = 1.2;
  auto r_d_real = make_real(r_d);
  EXPECT_DOUBLE_EQ(r_d_real, r_d);
  static_assert(std::same_as<decltype(r_d_real), double>);

  auto z_d      = std::complex<double>{1.2, 2.3};
  auto z_d_real = make_real(z_d);
  EXPECT_DOUBLE_EQ(z_d_real, std::real(z_d));
  static_assert(std::same_as<decltype(z_d_real), double>);

  auto arr_d      = nda::array<double, 1>{1.2, 3.4};
  auto arr_d_real = make_real(arr_d);
  EXPECT_ARRAY_EQ(arr_d_real, (nda::array<double, 1>{1.2, 3.4}));
  static_assert(std::same_as<decltype(arr_d_real), nda::array<double, 1>>);

  auto arr_c      = nda::array<std::complex<double>, 1>{{1.2, 2.3}, {3.4, 4.5}};
  auto arr_c_real = make_real(arr_c);
  EXPECT_ARRAY_EQ(arr_c_real, (nda::array<double, 1>{1.2, 3.4}));
  static_assert(std::same_as<decltype(arr_c_real), nda::array<double, 1>>);
}

TEST(TRIQSStat, GetRegularType) {
  using namespace triqs::stat;
  static_assert(std::same_as<get_regular_t<double>, double>);
  static_assert(std::same_as<get_regular_t<std::complex<double>>, std::complex<double>>);
  static_assert(std::same_as<get_regular_t<nda::array<double, 1>>, nda::array<double, 1>>);
  static_assert(std::same_as<get_regular_t<nda::matrix<std::complex<double>>>, nda::matrix<std::complex<double>>>);
  nda::vector<double> v{1, 2, 3};
  nda::matrix<std::complex<double>> M(10, 10);
  static_assert(std::same_as<get_regular_t<decltype(v + v)>, nda::vector<double>>);
  static_assert(std::same_as<get_regular_t<decltype(M + M)>, nda::matrix<std::complex<double>>>);
}

MAKE_MAIN;
