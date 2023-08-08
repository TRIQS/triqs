// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
// Authors: Philipp D, Igor Krivenko, Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/arrays.hpp>
#include <triqs/stat/histograms.hpp>
#include <triqs/arrays.hpp>
#include <triqs/stat/histograms.hpp>
#include <triqs/test_tools/arrays.hpp>
using namespace triqs::stat;
namespace arrays = triqs::arrays;

histogram make_hi1() {
  histogram hi1{0, 10};
  std::vector<double> data{-1, 0, 0, 0, 1, 2, 2, 2, 3, 5, 9, 32};
  for (auto i : data) hi1 << i;
  return hi1;
}

histogram make_hd1() {
  histogram hd1{0, 10, 21};
  std::vector<double> data{-10, -0.05, 1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.5, 12.1, 32.2};
  for (auto i : data) hd1 << i;
  return hd1;
}

histogram make_hi2() {
  histogram hi2{0, 10};
  std::vector<double> data{1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.1, 12.1, 32.2};
  for (auto i : data) hi2 << i;
  return hi2;
}

histogram make_hd2() {
  histogram hd2{0, 10, 11};
  std::vector<double> data{1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.1, 12.1, 32.2};
  for (auto i : data) hd2 << i;
  return hd2;
}

// ----- TESTS ------------------

TEST(histogram, basic) {

  auto hi1 = make_hi1();
  auto hd1 = make_hd1();
  auto hi2 = make_hi2();
  auto hd2 = make_hd2();

  nda::vector<double> true_hi1 = {3, 1, 3, 1, 0, 1, 0, 0, 0, 1, 0};
  nda::vector<double> true_hd1 = {0, 0, 1, 0, 2, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1};
  nda::vector<double> true_h2  = {0, 1, 2, 2, 0, 1, 0, 0, 0, 1, 1};

  // hi1

  EXPECT_EQ(11, hi1.size());
  EXPECT_EQ(0, hi1.mesh_point(0));
  EXPECT_EQ(1, hi1.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), hi1.limits());
  EXPECT_ARRAY_NEAR(true_hi1, hi1.data());
  EXPECT_EQ(10, hi1.n_data_pts());
  EXPECT_EQ(2, hi1.n_lost_pts());

  // hd1

  EXPECT_EQ(21, hd1.size());
  EXPECT_EQ(0, hd1.mesh_point(0));
  EXPECT_EQ(0.5, hd1.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), hd1.limits());
  EXPECT_ARRAY_NEAR(true_hd1, hd1.data());
  EXPECT_EQ(8, hd1.n_data_pts());
  EXPECT_EQ(5, hd1.n_lost_pts());

  // hi2

  EXPECT_EQ(11, hi2.size());
  EXPECT_EQ(0, hi2.mesh_point(0));
  EXPECT_EQ(1, hi2.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), hi2.limits());
  EXPECT_ARRAY_NEAR(true_h2, hi2.data());
  EXPECT_EQ(8, hi2.n_data_pts());
  EXPECT_EQ(3, hi2.n_lost_pts());
  EXPECT_ARRAY_NEAR(hd2.data(), hi2.data());
}

TEST(histogram, clear) {
  auto hi1 = make_hi1();
  hi1.clear();
  EXPECT_ARRAY_EQ(nda::vector<double>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}), hi1.data());
  EXPECT_EQ(0, hi1.n_data_pts());
  EXPECT_EQ(0, hi1.n_lost_pts());
}

TEST(histogram, sum) {

  auto hi1 = make_hi1();
  auto hd1 = make_hd1();
  auto hd2 = make_hd2();

  nda::vector<double> true_hsum = {3, 2, 5, 3, 0, 2, 0, 0, 0, 2, 1};

  EXPECT_THROW(hi1 + hd1, triqs::runtime_error);

  auto hsum = hi1 + hd2;

  EXPECT_EQ(11, hsum.size());
  EXPECT_EQ(0, hsum.mesh_point(0));
  EXPECT_EQ(1, hsum.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), hsum.limits());
  EXPECT_ARRAY_NEAR(true_hsum, hsum.data());
  EXPECT_EQ(18, hsum.n_data_pts());
  EXPECT_EQ(5, hsum.n_lost_pts());
}

TEST(histogram, hdf5) {

  auto hd1 = make_hd1();

  auto hd1_r = rw_h5(hd1, "ess_histograms2", "hd1");
  EXPECT_EQ(hd1.limits(), hd1_r.limits());
  EXPECT_EQ(hd1.n_data_pts(), hd1_r.n_data_pts());
  EXPECT_EQ(hd1.n_lost_pts(), hd1_r.n_lost_pts());
  EXPECT_ARRAY_NEAR(hd1.data(), hd1_r.data());
}

TEST(histogram, pdf) {

  auto hi1 = make_hi1();

  nda::vector<double> true_pdf_hi1 = {.3, .1, .3, .1, .0, .1, .0, .0, .0, .1, .0};
  auto pdf_hi1                     = pdf(hi1);

  EXPECT_ARRAY_NEAR(true_pdf_hi1, pdf_hi1.data());
}

TEST(histogram, cdf) {

  auto hi1 = make_hi1();

  nda::vector<double> true_cdf_hi1 = {.3, .4, .7, .8, .8, .9, .9, .9, .9, 1.0, 1.0};
  auto cdf_hi1                     = cdf(hi1);

  EXPECT_ARRAY_NEAR(true_cdf_hi1, cdf_hi1.data());
}

// ------------------------

MAKE_MAIN;
