// Copyright (c) 2023 Simons Foundation
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
// Authors: Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

TEST(GfLegendre, Mesh) {

  double const beta = 2.0;
  auto const n_l    = 10;
  auto mesh_l       = mesh::legendre(beta, Fermion, n_l);

  std::cout << mesh_l << "\n";
  std::cout << "Rank " << mesh_l.size() << "\n";

  for (const auto &l : mesh_l) { std::cout << mesh::legendre::domain_t::point_t{l} << "\n"; }

  auto const n_tau = 5;
  auto tmesh       = mesh::imtime(beta, Fermion, n_tau);

  triqs::clef::placeholder<0> l_;

  auto g = gf<legendre, scalar_valued>{mesh_l};
  g()    = 0.0;
  g[l_] << (l_ == 0);

  auto gmat = gf<legendre, matrix_valued>{mesh_l, {1, 1}};
  gmat()    = 0.0;
  gmat[l_] << (l_ == 0);

  for (auto t : tmesh) {
    std::cout << "tau: " << t << std::endl;
    std::cout << "g(tau): " << g(t) << std::endl;
    std::cout << "gmat(tau): " << gmat(t) << std::endl << std::endl;
    EXPECT_EQ(g(t), 1 / beta);
    EXPECT_EQ(gmat(t), nda::diag(nda::vector<double>{1 / beta}));
  }

  g() = 0.0;
  g[l_] << (l_ == 2);

  gmat() = 0.0;
  gmat[l_] << (l_ == 2);

  for (auto t : tmesh) {
    std::cout << "tau: " << t << std::endl;
    std::cout << "g(tau): " << g(t) << std::endl;
    std::cout << "gmat(tau): " << gmat(t) << std::endl << std::endl;
    auto leg2 = [&](double tau) {
      auto x    = 2 * tau / beta - 1;
      auto fact = std::sqrt(2 * 2 + 1) / beta;
      return fact * (3 * x * x - 1) / 2;
    };
    std::cout << leg2(t) << std::endl;

    EXPECT_EQ(g(t), leg2(t));
    EXPECT_EQ(gmat(t), nda::diag(nda::vector<double>{leg2(t)}));
  }
}

MAKE_MAIN;
