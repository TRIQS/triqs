// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Philipp Dumitrescu

#include <triqs/mesh/domains/real_complex.hpp>

#include <triqs/test_tools/arrays.hpp>
#include <h5/h5.hpp>
#include <iterator>

#include <string_view>

TEST(DomainRealComplex, RealDomain) {

  triqs::mesh::real_domain rd1{};

  EXPECT_TRUE(rd1.contains(0.0));
  EXPECT_TRUE(rd1.contains(17.0));
  EXPECT_TRUE(rd1.contains(-17.0));
  EXPECT_TRUE(rd1.contains(-std::numeric_limits<double>::infinity()));
  EXPECT_TRUE(rd1.contains(+std::numeric_limits<double>::infinity()));
  EXPECT_FALSE(rd1.contains(+std::numeric_limits<double>::quiet_NaN()));
  EXPECT_FALSE(rd1.contains(+std::numeric_limits<double>::signaling_NaN()));

  // TODO: Q: Should hdf5 schema label be tested?

  triqs::mesh::real_domain rd2{};

  EXPECT_TRUE(rd1 == rd2);
}

TEST(DomainRealComplex, ComplexDomain) {

  triqs::mesh::complex_domain cd1{};

  EXPECT_TRUE(cd1.contains({0.0, 0.0}));
  EXPECT_TRUE(cd1.contains({0.0, 17.0}));
  EXPECT_TRUE(cd1.contains({-17.0, 0.0}));
  EXPECT_TRUE(cd1.contains({-std::numeric_limits<double>::infinity(), 0}));
  EXPECT_TRUE(cd1.contains({0, +std::numeric_limits<double>::infinity()}));
  EXPECT_TRUE(cd1.contains({+std::numeric_limits<double>::infinity(), +std::numeric_limits<double>::infinity()}));
  EXPECT_TRUE(cd1.contains({-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()}));

  EXPECT_FALSE(cd1.contains({0.0, +std::numeric_limits<double>::quiet_NaN()}));
  EXPECT_FALSE(cd1.contains({0.0, +std::numeric_limits<double>::signaling_NaN()}));
  EXPECT_FALSE(cd1.contains({+std::numeric_limits<double>::quiet_NaN(), 0.0}));
  EXPECT_FALSE(cd1.contains({+std::numeric_limits<double>::signaling_NaN(), 0.0}));
  EXPECT_FALSE(cd1.contains({+std::numeric_limits<double>::quiet_NaN(), +std::numeric_limits<double>::quiet_NaN()}));
  EXPECT_FALSE(cd1.contains({+std::numeric_limits<double>::signaling_NaN(), +std::numeric_limits<double>::quiet_NaN()}));

  // TODO: Q: Should hdf5 schema label be tested?

  triqs::mesh::complex_domain cd2{};

  EXPECT_TRUE(cd1 == cd2);
}

TEST(DomainRealComplex, Real2Domain) {

  triqs::mesh::real_N_domain<2> rd1{};

  EXPECT_TRUE(rd1.contains({0.0, 1.0}));
  EXPECT_TRUE(rd1.contains({-5.0, 1.0}));
  EXPECT_TRUE(rd1.contains({-std::numeric_limits<double>::infinity(), 0}));
  EXPECT_TRUE(rd1.contains({0, +std::numeric_limits<double>::infinity()}));
  EXPECT_TRUE(rd1.contains({+std::numeric_limits<double>::infinity(), +std::numeric_limits<double>::infinity()}));
  EXPECT_TRUE(rd1.contains({-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()}));

  EXPECT_FALSE(rd1.contains({0.0, +std::numeric_limits<double>::quiet_NaN()}));
  EXPECT_FALSE(rd1.contains({0.0, +std::numeric_limits<double>::signaling_NaN()}));
  EXPECT_FALSE(rd1.contains({+std::numeric_limits<double>::quiet_NaN(), 0.0}));
  EXPECT_FALSE(rd1.contains({+std::numeric_limits<double>::signaling_NaN(), 0.0}));
  EXPECT_FALSE(rd1.contains({+std::numeric_limits<double>::quiet_NaN(), +std::numeric_limits<double>::quiet_NaN()}));
  EXPECT_FALSE(rd1.contains({+std::numeric_limits<double>::signaling_NaN(), +std::numeric_limits<double>::quiet_NaN()}));

  // TODO: Q: Should hdf5 schema label be tested?

  triqs::mesh::real_N_domain<2> rd2{};

  EXPECT_TRUE(rd1 == rd2);
}

MAKE_MAIN;
