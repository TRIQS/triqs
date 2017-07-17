// Copyright (c) 2015-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2016 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2020 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell, Hugo Strand

#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;

TEST(Gf, AutoAssignMatrixGf2) {

  double beta = 2.3;
  auto g2     = gf<prod<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 10}, {beta, Fermion, 10}}, {2, 2}};

  placeholder<0> i_;
  placeholder<1> j_;
  placeholder<3> om_;
  placeholder<4> nu_;

  g2(om_, nu_)(i_, j_) << i_ + j_ + 2 * om_ - 0.3 * nu_;

  // CHECK
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j)
      for (int om = 0; om < 10; ++om)
        for (int nu = 0; nu < 10; ++nu) {
          auto xom = ((2 * om + 1) * M_PI * 1i / beta);
          auto xnu = ((2 * nu + 1) * M_PI * 1i / beta);
          EXPECT_CLOSE(g2.data()(10 + om, 10 + nu, i, j), i + j + 2 * xom - 0.3 * xnu);
        }
}

TEST(Gf, AutoAssignImagTime) {

  int ntau    = 10;
  double beta = 2.3;
  auto g      = gf<prod<imtime, imtime, imtime>, matrix_valued>{{{beta, Fermion, ntau}, {beta, Fermion, ntau}, {beta, Fermion, ntau}}, {1, 1}};

  {
    placeholder<0> t1;
    placeholder<1> t2;
    placeholder<3> t3;
    placeholder<4> a;
    placeholder<5> b;

    g(t1, t2, t3)(a, b) << a + b + 2 * t1 - 0.3 * t2 - 10. * t3;
  }

  // CHECK
  for (auto const &t1 : std::get<0>(g.mesh())) {
    for (auto const &t2 : std::get<1>(g.mesh())) {
      for (auto const &t3 : std::get<2>(g.mesh())) {
        double ref_val = 2 * t1 - 0.3 * t2 - 10. * t3;
        double val     = g[closest_mesh_pt(t1, t2, t3)](0, 0).real();
        //std::cout << t1 << ", " << t2 << ", " << t3 << ", " << val << " -- " << ref_val << "\n";
        EXPECT_CLOSE(val, ref_val);
      }
    }
  }
}

TEST(Gf, AutoAssignAccumulateImagTime) {

  int ntau    = 10;
  double beta = 2.3;
  auto g      = gf<prod<imtime, imtime, imtime>, matrix_valued>{{{beta, Fermion, ntau}, {beta, Fermion, ntau}, {beta, Fermion, ntau}}, {1, 1}};

  {
    placeholder<0> t1;
    placeholder<1> t2;
    placeholder<3> t3;
    placeholder<4> a;
    placeholder<5> b;

    g(t1, t2, t3)(a, b) << g(t1, t2, t3)(a, b) + a + b + 2 * t1 - 0.3 * t2 - 10. * t3;
  }

  // CHECK
  for (auto const &t1 : std::get<0>(g.mesh())) {
    for (auto const &t2 : std::get<1>(g.mesh())) {
      for (auto const &t3 : std::get<2>(g.mesh())) {
        double ref_val = 2 * t1 - 0.3 * t2 - 10. * t3;
        double val     = g[closest_mesh_pt(t1, t2, t3)](0, 0).real();
        //std::cout << t1 << ", " << t2 << ", " << t3 << ", " << val << " -- " << ref_val << "\n";
        EXPECT_CLOSE(val, ref_val);
      }
    }
  }
}

TEST(Gf, MixedIndexClefImFreq) {

  int nw      = 10;
  double beta = 2.3;
  auto g      = gf<prod<imfreq, imfreq, imfreq>, matrix_valued>{{{beta, Boson, nw}, {beta, Fermion, nw}, {beta, Fermion, nw}}, {1, 1}};

  {
    placeholder<2> Omega;
    placeholder<3> n;
    placeholder<4> np;
    placeholder<0> a;
    placeholder<1> b;

    g(Omega, n, np)(a, b) << kronecker(n + np, Omega) * (a + b + 2 * Omega - 0.2 * n);
  }

  // CHECK
  for (auto const &Omega : std::get<0>(g.mesh())) {
    for (auto const &n : std::get<1>(g.mesh())) {
      for (auto const &np : std::get<2>(g.mesh())) {
        dcomplex ref_val = 0.0;
        if (std::abs(dcomplex(n + np - Omega)) < 1e-6) ref_val = 2 * Omega - 0.2 * n;
        dcomplex val = g(Omega, n, np)(0, 0);
        //std::cout << Omega << ", " << n << ", " << np << ", " << val << " -- " << ref_val << "\n";
        EXPECT_CLOSE(val, ref_val);
      }
    }
  }
}

TEST(Gf, MixedIndexSliceClefImFreq) {

  int nw      = 10;
  double beta = 2.3;
  auto g      = gf<prod<imfreq, imfreq, imfreq>, matrix_valued>{{{beta, Boson, nw}, {beta, Fermion, nw}, {beta, Fermion, nw}}, {1, 1}};
  g() = 0.0;

  auto Omega = std::get<0>(g.mesh())[4];
  auto n     = std::get<1>(g.mesh())[5];

  placeholder<0> a;
  placeholder<1> b;

  g[{Omega, n, Omega - n}](a, b) << a + b + 2 * Omega - 0.2 * n;

  dcomplex ref_val = 2 * Omega - 0.2 * n;
  dcomplex val     = g[{Omega, n, Omega - n}](0, 0);

  EXPECT_CLOSE(val, ref_val);
}

MAKE_MAIN;
