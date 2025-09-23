// Copyright (c) 2022-2023 Simons Foundation
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
// Authors: GitHub Copilot

#include <triqs/mesh/evaluate.hpp>
#include <triqs/mesh/discrete.hpp>
#include <triqs/mesh/imfreq.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

// simple domain to exercise tuple-based currying evaluate
namespace triqs::mesh {
  struct test_domain final {};
  inline auto evaluate(test_domain, auto const &f, double x) { return f(x); }
} // namespace triqs::mesh

using namespace triqs::mesh;

TEST(TRIQSMesh, EvaluateIndex) {
  auto const m = discrete{7};
  auto const f = [](long n) { return n + 10; };
  EXPECT_EQ(evaluate(m, f, 3l), 13);
}

TEST(TRIQSMesh, EvaluateClosestMeshPoint) {
  auto const m   = discrete{5};
  auto const f   = [](auto const &cmp) { return cmp.value + 1.5; };
  auto const cmp = closest_mesh_pt(2.5);
  EXPECT_EQ(evaluate(m, f, cmp), 4.0);
}

TEST(TRIQSMesh, EvaluateAllRange) {
  auto const m = discrete{3};
  auto const f = [](nda::range::all_t) { return 123; };
  EXPECT_EQ(evaluate(m, f, nda::range::all), 123);
}

TEST(TRIQSMesh, EvaluateMeshPointDiscrete) {
  auto const m  = discrete{6};
  auto const mp = m(4);
  auto const f  = [](discrete::mesh_point_t const &p) { return p.index() * 2; };
  EXPECT_EQ(evaluate(m, f, mp), 8);
}

TEST(TRIQSMesh, EvaluateMeshPointImfreq) {
  auto const m_iw = imfreq{10.0, Fermion, 2};
  auto const mp   = m_iw(0);
  auto const f    = [](long n) { return n + 100; };
  EXPECT_EQ(evaluate(m_iw, f, mp), 100);
}

TEST(TRIQSMesh, EvaluateImfreqMatsubara) {
  auto const m_iw = imfreq{10.0, Boson, 2};
  auto const iw0  = m_iw.to_value(0);
  auto const f    = [](long n) { return 2 * n + 1; };
  EXPECT_EQ(evaluate(m_iw, f, iw0), 1);
}

TEST(TRIQSMesh, EvaluateTupleCurrying) {
  auto const tup = std::make_tuple(test_domain{}, test_domain{}, test_domain{});
  auto const f   = [](double x, double y, double z) { return x + y + z; };
  EXPECT_EQ(evaluate(tup, f, 1.0, 2.0, 3.0), 6.0);
}

TEST(TRIQSMesh, EvaluateProdMesh) {
  auto const m_iw = imfreq{10.0, Fermion, 2};
  auto const m_d  = discrete{3};
  auto const m_p  = m_iw * m_d;

  auto const f = [](long n, long i) { return n * 10 + i; };

  auto const iw1 = m_iw.to_value(1); // use a non-zero Matsubara frequency
  EXPECT_EQ(evaluate(m_p, f, iw1, 2l), 12);
}

MAKE_MAIN;
