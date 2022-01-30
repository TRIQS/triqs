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

#include "triqs/mesh/details/mesh_tools.hpp"
#include <triqs/mesh/domains/matsubara.hpp>

#include <triqs/test_tools/arrays.hpp>
#include <h5/h5.hpp>
#include <iterator>

#include <numbers>
#include <string_view>

TEST(DomainMatsubara, TimeDomain) {
  triqs::mesh::matsubara_time_domain m1(75.0, triqs::mesh::Boson);

  EXPECT_TRUE(m1.contains(0.0));
  EXPECT_TRUE(m1.contains(17.0));
  EXPECT_TRUE(m1.contains(75.0));

  EXPECT_FALSE(m1.contains(-17.0));
  EXPECT_FALSE(m1.contains(75.0 + 1e-8));
  EXPECT_FALSE(m1.contains(+std::numeric_limits<double>::quiet_NaN()));

  triqs::mesh::matsubara_time_domain m2(std::numeric_limits<double>::infinity(), triqs::mesh::Boson);

  EXPECT_TRUE(m2.contains(0.0));
  EXPECT_TRUE(m2.contains(std::numeric_limits<double>::infinity()));

  EXPECT_TRUE(m2 != m1);

  triqs::mesh::matsubara_time_domain m3(75.0, triqs::mesh::Fermion);
  EXPECT_TRUE(m3 != m2);
  EXPECT_TRUE(m3 != m1);

  std::cout << m2 << std::endl;
}

TEST(DomainMatsubara, FreqDomain) {
  double beta1 = 1000.0;
  triqs::mesh::matsubara_freq_domain m1(beta1, triqs::mesh::Boson);

  EXPECT_TRUE(m1.contains(triqs::mesh::matsubara_freq{0, beta1, triqs::mesh::Boson}));
  EXPECT_TRUE(m1.contains(triqs::mesh::matsubara_freq{5, beta1, triqs::mesh::Boson}));
  EXPECT_TRUE(m1.contains(triqs::mesh::matsubara_freq{250, beta1, triqs::mesh::Boson}));
  EXPECT_TRUE(m1.contains(triqs::mesh::matsubara_freq{-3000, beta1, triqs::mesh::Boson}));

  EXPECT_FALSE(m1.contains(triqs::mesh::matsubara_freq{0, 10.0, triqs::mesh::Boson}));
  EXPECT_FALSE(m1.contains(triqs::mesh::matsubara_freq{0, beta1, triqs::mesh::Fermion}));
  EXPECT_FALSE(m1.contains(triqs::mesh::matsubara_freq{2, beta1, triqs::mesh::Fermion}));

  std::cout << m1 << std::endl;
}

MAKE_MAIN;
