// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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

#include <triqs/mesh.hpp>
#include <triqs/test_tools/arrays.hpp>

using namespace triqs::mesh;

double beta = 10;

TEST(Mesh, imfreq) {
  auto m = imfreq{beta, Fermion, 5};
  for (auto x : m) {
    auto f = matsubara_freq{x};
    auto c = dcomplex{f};
    EXPECT_CLOSE(c.real(), 0);
    auto n = f.n;
    EXPECT_CLOSE(c.imag(), (2 * n + 1) * M_PI / beta);
  }
}

// ---------------------------

TEST(Mesh, imtime) {

  auto m = imtime{beta, Fermion, 20};
  for (auto x : m) std::cout << x << std::endl;

  // test that the 2 iterators are independant.
  auto it  = m.begin();
  auto it2 = m.begin();
  for (int i = 0; i < 10; ++i) ASSERT_EQ(it++, it2++);
}

// ---------------------------

TEST(Mesh, prod) {

  auto m1 = imtime{beta, Fermion, 5};
  auto m2 = imfreq{beta, Fermion};
  auto mm = prod<imtime, imfreq>{m1, m2};

  auto mm2 = prod{m1, m2};
  auto mm3 = m1 * m2;

  ASSERT_EQ(mm, mm2);
  ASSERT_EQ(mm, mm3);

  std::vector<std::pair<double, double>> res;
  for (auto t1 : m1)
    for (auto t2 : m1) res.push_back({double(t1), double(t2)});

  {
    std::vector<std::pair<double, double>> res2;
    for (auto [t1, t2] : itertools::product(m1, m1)) res2.push_back({double(t1), double(t2)});

    EXPECT_EQ(res, res2);
  }
  {

    std::vector<std::pair<double, double>> res2;
    long c = 0;
    for (auto x : prod{m1, m1}) {
      auto [t1, t2] = x;
      EXPECT_EQ(x.data_index(), std::make_tuple(c / m1.size(), c % m1.size()));
      c++;
      res2.push_back({double(t1), double(t2)});
    }
    EXPECT_EQ(res, res2);
  }

  {
    std::vector<std::pair<double, double>> res2;
    for (auto [t1, t2] : m1 *m1) res2.push_back({double(t1), double(t2)});

    EXPECT_EQ(res, res2);
  }
}

// ---------------------------

MAKE_MAIN;
