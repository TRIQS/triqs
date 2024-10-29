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

#include <triqs/stat/histograms.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <vector>
#include <utility>

// Create histograms.
auto make_histogram1() {
  triqs::stat::histogram h{0, 10};
  std::vector<double> data{-1, 0, 0, 0, 1, 2, 2, 2, 3, 5, 9, 32};
  for (auto i : data) h << i;
  return h;
}

auto make_histogram2() {
  triqs::stat::histogram h{0, 10, 21};
  std::vector<double> data{-10, -0.05, 1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.5, 12.1, 32.2};
  for (auto i : data) h << i;
  return h;
}

auto make_histogram3() {
  triqs::stat::histogram h{0, 10};
  std::vector<double> data{1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.1, 12.1, 32.2};
  for (auto i : data) h << i;
  return h;
}

auto make_histogram4() {
  triqs::stat::histogram h{0, 10, 11};
  std::vector<double> data{1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.1, 12.1, 32.2};
  for (auto i : data) h << i;
  return h;
}

TEST(TRIQSStat, HistogramBasics) {
  // expected bins
  nda::vector<double> exp_h1 = {3, 1, 3, 1, 0, 1, 0, 0, 0, 1, 0};
  nda::vector<double> exp_h2 = {0, 0, 1, 0, 2, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1};
  nda::vector<double> exp_h3 = {0, 1, 2, 2, 0, 1, 0, 0, 0, 1, 1};

  // create histograms
  auto h1 = make_histogram1();
  auto h2 = make_histogram2();
  auto h3 = make_histogram3();
  auto h4 = make_histogram4();

  // check histograms
  EXPECT_EQ(11, h1.size());
  EXPECT_EQ(0, h1.mesh_point(0));
  EXPECT_EQ(1, h1.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), h1.limits());
  EXPECT_ARRAY_NEAR(exp_h1, h1.data());
  EXPECT_EQ(10, h1.n_data_pts());
  EXPECT_EQ(2, h1.n_lost_pts());

  EXPECT_EQ(21, h2.size());
  EXPECT_EQ(0, h2.mesh_point(0));
  EXPECT_EQ(0.5, h2.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), h2.limits());
  EXPECT_ARRAY_NEAR(exp_h2, h2.data());
  EXPECT_EQ(8, h2.n_data_pts());
  EXPECT_EQ(5, h2.n_lost_pts());

  EXPECT_EQ(11, h3.size());
  EXPECT_EQ(0, h3.mesh_point(0));
  EXPECT_EQ(1, h3.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), h3.limits());
  EXPECT_ARRAY_NEAR(exp_h3, h3.data());
  EXPECT_EQ(8, h3.n_data_pts());
  EXPECT_EQ(3, h3.n_lost_pts());

  EXPECT_EQ(11, h4.size());
  EXPECT_EQ(0, h4.mesh_point(0));
  EXPECT_EQ(1, h4.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), h4.limits());
  EXPECT_ARRAY_NEAR(exp_h3, h4.data());
  EXPECT_EQ(8, h4.n_data_pts());
  EXPECT_EQ(3, h4.n_lost_pts());
}

TEST(TRIQSStat, HistogramClear) {
  auto h1 = make_histogram1();
  h1.clear();
  EXPECT_ARRAY_EQ(nda::vector<double>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}), h1.data());
  EXPECT_EQ(0, h1.n_data_pts());
  EXPECT_EQ(0, h1.n_lost_pts());
}

TEST(TRIQSStat, HistogramSum) {
  // expected sum
  nda::vector<double> exp_hsum = {3, 2, 5, 3, 0, 2, 0, 0, 0, 2, 1};

  // create histograms
  auto h1 = make_histogram1();
  auto h2 = make_histogram2();
  auto h3 = make_histogram3();

  // invalid addition
  EXPECT_THROW(h1 + h2, triqs::runtime_error);

  // check sum
  auto hsum = h1 + h3;
  EXPECT_EQ(11, hsum.size());
  EXPECT_EQ(0, hsum.mesh_point(0));
  EXPECT_EQ(1, hsum.mesh_point(1));
  EXPECT_EQ(std::make_pair(.0, 10.), hsum.limits());
  EXPECT_ARRAY_NEAR(exp_hsum, hsum.data());
  EXPECT_EQ(18, hsum.n_data_pts());
  EXPECT_EQ(5, hsum.n_lost_pts());
}

TEST(TRIQSStat, HistogramHDF5) {
  // create histogram
  auto h1 = make_histogram1();

  // write and read to HDF5
  auto h1_r = rw_h5(h1, "triqs_stat_histograms", "h1");
  EXPECT_EQ(h1.limits(), h1_r.limits());
  EXPECT_EQ(h1.n_data_pts(), h1_r.n_data_pts());
  EXPECT_EQ(h1.n_lost_pts(), h1_r.n_lost_pts());
  EXPECT_ARRAY_NEAR(h1.data(), h1_r.data());
}

TEST(TRIQSStat, HistogramPDF) {
  // expected pdf
  nda::vector<double> exp_pdf = {.3, .1, .3, .1, .0, .1, .0, .0, .0, .1, .0};

  // check pdf of histogram
  auto h1     = make_histogram1();
  auto pdf_h1 = pdf(h1);
  EXPECT_ARRAY_NEAR(exp_pdf, pdf_h1.data());
}

TEST(TRIQSStat, HistogramCDF) {
  // expected cdf
  nda::vector<double> exp_cdf = {.3, .4, .7, .8, .8, .9, .9, .9, .9, 1.0, 1.0};

  // check cdf of histogram
  auto h1     = make_histogram1();
  auto cdf_h1 = cdf(h1);
  EXPECT_ARRAY_NEAR(exp_cdf, cdf_h1.data());
}

MAKE_MAIN;
