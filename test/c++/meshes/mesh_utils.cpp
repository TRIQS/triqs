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
// Authors: Nils Wentzell

#include <triqs/mesh/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

TEST(TRIQSMesh, UtilitiesPositiveModulo) {
  EXPECT_EQ(triqs::mesh::positive_modulo(0, 5), 0);
  EXPECT_EQ(triqs::mesh::positive_modulo(12, 5), 2);
  EXPECT_EQ(triqs::mesh::positive_modulo(-12, 5), 3);
}

TEST(TRIQSMesh, UtilitiesStatistics) {
  auto boson   = triqs::mesh::statistic_enum::Boson;
  auto fermion = triqs::mesh::statistic_enum::Fermion;
  EXPECT_EQ(triqs::mesh::sign(boson), 1);
  EXPECT_EQ(triqs::mesh::sign(fermion), -1);
  EXPECT_EQ(boson * boson, boson);
  EXPECT_EQ(boson * fermion, fermion);
  EXPECT_EQ(fermion * boson, fermion);
  EXPECT_EQ(fermion * fermion, boson);
}

TEST(TRIQSMesh, UtilitiesSumToRegular) {
  nda::array<int, 2> A{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
  auto res = triqs::mesh::detail::sum_to_regular(nda::range(3), [&](long i) { return A(i, nda::range::all); });
  EXPECT_ARRAY_EQ(res, (nda::array<int, 1>{12, 15, 18}));
}

MAKE_MAIN;
