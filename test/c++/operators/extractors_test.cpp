// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
// Copyright (c) 2015 Igor Krivenko
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
// Authors: Thomas Ayral, Igor Krivenko, Olivier Parcollet, Nils Wentzell

#ifndef NDA_DEBUG
#define NDA_DEBUG
#endif

#include <triqs/test_tools/arrays.hpp>
#include <vector>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/operators/util/extractors.hpp>

using namespace triqs::operators;
using namespace triqs::operators::utils;
using namespace triqs::arrays;
using namespace triqs::hilbert_space;

using dcomplex = std::complex<double>;

double U0 = 2.0, U1 = 2.2, U2 = 2.4;
double eps0 = 1.0, eps1 = 1.1, eps2 = 1.2;
double t01 = 0.4, t12 = 0.5, t20 = 0.6;
double V01 = 1.8, V12 = 1.7, V20 = 1.6;

auto gf_struct             = gf_struct_t({{"up", 3}, {"dn", 3}});
std::vector<indices_t> ind = {{"dn", 0}, {"dn", 1}, {"dn", 2}, {"up", 0}, {"up", 1}, {"up", 2}};

// Test extract_h_dict
TEST(Extractors, extract_h_dict) {
  auto h0  = eps0 * (n("up", 0) + n("dn", 0)) + eps1 * (n("up", 1) + n("dn", 1)) + eps2 * (n("up", 2) + n("dn", 2));
  auto hop = t01 * (c_dag("up", 0) * c("up", 1) + c_dag("dn", 0) * c("dn", 1)) + t12 * (c_dag("up", 1) * c("up", 2) + c_dag("dn", 1) * c("dn", 2))
     + t20 * (c_dag("up", 2) * c("up", 0) + c_dag("dn", 2) * c("dn", 0));
  h0 += hop + dagger(hop);

  array<double, 2> h_matrix_ref{{eps0, t01, t20, .0, .0, .0}, {t01, eps1, t12, .0, .0, .0}, {t20, t12, eps2, .0, .0, .0},
                                {.0, .0, .0, eps0, t01, t20}, {.0, .0, .0, t01, eps1, t12}, {.0, .0, .0, t20, t12, eps2}};

  dict2_t<real_or_complex> h_dict_ref;
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < 6; ++j) {
      auto val = h_matrix_ref(i, j);
      if (val != .0) h_dict_ref[std::make_tuple(ind[i], ind[j])] = val;
    }

  auto h_dict = extract_h_dict(h0);
  EXPECT_EQ(h_dict_ref, h_dict);
  EXPECT_ARRAY_NEAR(h_matrix_ref, dict_to_matrix(h_dict, gf_struct));
  EXPECT_ARRAY_NEAR(h_matrix_ref, dict_to_matrix<double>(h_dict, gf_struct));
  EXPECT_ARRAY_NEAR((array<dcomplex, 2>)h_matrix_ref, dict_to_matrix<dcomplex>(h_dict, gf_struct));
  EXPECT_ARRAY_NEAR(h_matrix_ref, (std::get<array<double, 2>>(dict_to_variant_matrix(h_dict, gf_struct))));

  auto nn = n("up", 0) * n("dn", 0);
  EXPECT_THROW(extract_h_dict(nn), triqs::exception);
  EXPECT_NO_THROW(extract_h_dict(nn, true));

  auto cc = c("up", 0) * c("dn", 1);
  EXPECT_THROW(extract_h_dict(cc), triqs::exception);
  EXPECT_NO_THROW(extract_h_dict(cc, true));

  auto wrong_index = n("up", 0) + n("dn", 5);
  EXPECT_THROW(dict_to_matrix(extract_h_dict(wrong_index), gf_struct), triqs::exception);
}

TEST(Extractors, extract_U_dict2) {
  auto h_int = U0 * n("up", 0) * n("dn", 0) + U1 * n("up", 1) * n("dn", 1) + U2 * n("up", 2) * n("dn", 2);
  auto N0    = n("up", 0) + n("dn", 0);
  auto N1    = n("up", 1) + n("dn", 1);
  auto N2    = n("up", 2) + n("dn", 2);
  h_int += V01 * N0 * N1 + V12 * N1 * N2 + V20 * N0 * N2;

  array<double, 2> U_matrix2_ref{{.0, V01, V20, U0, V01, V20}, {V01, .0, V12, V01, U1, V12}, {V20, V12, .0, V20, V12, U2},
                                 {U0, V01, V20, .0, V01, V20}, {V01, U1, V12, V01, .0, V12}, {V20, V12, U2, V20, V12, .0}};

  dict2_t<real_or_complex> U_dict2_ref;
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < 6; ++j) {
      auto val = U_matrix2_ref(i, j);
      if (val != .0) U_dict2_ref[std::make_tuple(ind[i], ind[j])] = val;
    }

  auto U_dict2 = extract_U_dict2(h_int);
  EXPECT_EQ(U_dict2_ref, U_dict2);
  EXPECT_ARRAY_NEAR(U_matrix2_ref, dict_to_matrix(U_dict2, gf_struct));
  EXPECT_ARRAY_NEAR(U_matrix2_ref, dict_to_matrix<double>(U_dict2, gf_struct));
  EXPECT_ARRAY_NEAR((array<dcomplex, 2>)U_matrix2_ref, dict_to_matrix<dcomplex>(U_dict2, gf_struct));
  EXPECT_ARRAY_NEAR(U_matrix2_ref, (std::get<array<double, 2>>(dict_to_variant_matrix(U_dict2, gf_struct))));

  auto quadratic = n("up", 0) + n("dn", 0);
  EXPECT_THROW(extract_U_dict2(quadratic), triqs::exception);
  EXPECT_NO_THROW(extract_U_dict2(quadratic, true));

  auto non_nn = c_dag("up", 0) * c_dag("dn", 0) * c("up", 2) * c("dn", 1);
  EXPECT_THROW(extract_U_dict2(non_nn), triqs::exception);
  EXPECT_NO_THROW(extract_U_dict2(non_nn, true));

  auto wrong_index = n("up", 4) * n("dn", 1);
  EXPECT_THROW(dict_to_matrix(extract_U_dict2(wrong_index), gf_struct), triqs::exception);
}

TEST(Extractors, extract_U_dict4) {
  auto h_int = U0 * n("up", 0) * n("dn", 0) + U1 * n("up", 1) * n("dn", 1) + U2 * n("up", 2) * n("dn", 2);
  auto N0    = n("up", 0) + n("dn", 0);
  auto N1    = n("up", 1) + n("dn", 1);
  auto N2    = n("up", 2) + n("dn", 2);
  h_int += V01 * N0 * N1 + V12 * N1 * N2 + V20 * N0 * N2;

  array<double, 2> U_matrix2_ref{{.0, V01, V20, U0, V01, V20}, {V01, .0, V12, V01, U1, V12}, {V20, V12, .0, V20, V12, U2},
                                 {U0, V01, V20, .0, V01, V20}, {V01, U1, V12, V01, .0, V12}, {V20, V12, U2, V20, V12, .0}};

  dict4_t<real_or_complex> U_dict4_ref;
  array<double, 4> U_matrix4_ref(6, 6, 6, 6);
  U_matrix4_ref() = 0;
  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < 6; ++j) {
      auto val = U_matrix2_ref(i, j);
      if (val != .0) {
        U_dict4_ref[std::make_tuple(ind[i], ind[j], ind[i], ind[j])] = 0.5 * val;
        U_dict4_ref[std::make_tuple(ind[j], ind[i], ind[j], ind[i])] = 0.5 * val;
        U_dict4_ref[std::make_tuple(ind[i], ind[j], ind[j], ind[i])] = -0.5 * val;
        U_dict4_ref[std::make_tuple(ind[j], ind[i], ind[i], ind[j])] = -0.5 * val;

        U_matrix4_ref(i, j, i, j) = U_matrix4_ref(j, i, j, i) = 0.5 * val;
        U_matrix4_ref(i, j, j, i) = U_matrix4_ref(j, i, i, j) = -0.5 * val;
      }
    }

  auto U_dict4 = extract_U_dict4(h_int);
  EXPECT_EQ(U_dict4_ref, U_dict4);
  EXPECT_ARRAY_NEAR(U_matrix4_ref, dict_to_matrix(U_dict4, gf_struct));
  EXPECT_ARRAY_NEAR(U_matrix4_ref, dict_to_matrix<double>(U_dict4, gf_struct));
  EXPECT_ARRAY_NEAR((array<dcomplex, 4>)U_matrix4_ref, dict_to_matrix<dcomplex>(U_dict4, gf_struct));
  EXPECT_ARRAY_NEAR(U_matrix4_ref, (std::get<array<double, 4>>(dict_to_variant_matrix(U_dict4, gf_struct))));

  auto quadratic = n("up", 0) + n("dn", 0);
  EXPECT_THROW(extract_U_dict4(quadratic), triqs::exception);
  EXPECT_NO_THROW(extract_U_dict4(quadratic, true));

  auto non_conserving = c_dag("up", 0) * c_dag("dn", 0) * c_dag("up", 2) * c("dn", 1);
  EXPECT_THROW(extract_U_dict4(non_conserving), triqs::exception);
  EXPECT_NO_THROW(extract_U_dict4(non_conserving, true));

  auto wrong_index = n("up", 4) * n("dn", 1);
  EXPECT_THROW(dict_to_matrix(extract_U_dict4(wrong_index), gf_struct), triqs::exception);
}

// Test filter_op
TEST(Extractors, filter_op) {
  auto h_int = U0 * c_dag("up", 0) * c("dn", 0) * c_dag("dn", 1) * c("up", 1);

  auto N0 = n("up", 0) + n("dn", 0);
  auto N1 = n("up", 1) + n("dn", 1);
  auto h0 = eps0 * N0 + eps1 * N1;

  auto h_imp = h_int + h0;

  EXPECT_EQ(quadratic_terms(h0), h0);
  EXPECT_EQ(quartic_terms(h_int), h_int);
  EXPECT_EQ(quadratic_terms(h_imp), h0);
  EXPECT_EQ(quartic_terms(h_imp), h_int);
}

// Test block_matrix_from_op and op_from_block_matrix
TEST(Extractors, block_matrix) {
  auto h0  = eps0 * (n("up", 0) + n("dn", 0)) + eps1 * (n("up", 1) + n("dn", 1)) + eps2 * (n("up", 2) + n("dn", 2));
  auto hop = t01 * (c_dag("up", 0) * c("up", 1) + c_dag("dn", 0) * c("dn", 1)) + t12 * (c_dag("up", 1) * c("up", 2) + c_dag("dn", 1) * c("dn", 2))
     + t20 * (c_dag("up", 2) * c("up", 0) + c_dag("dn", 2) * c("dn", 0));
  h0 += hop + dagger(hop);

  EXPECT_EQ(op_from_block_matrix(block_matrix_from_op(h0, gf_struct), gf_struct), h0);
}

MAKE_MAIN;
