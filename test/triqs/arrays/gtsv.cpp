/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016 by I. Krivenko
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include <triqs/test_tools/arrays.hpp>

#include <triqs/arrays.hpp>
#include "triqs/arrays/blas_lapack/gtsv.hpp"

using namespace triqs::arrays;

TEST(gtsv, dgtsv) {

  vector<double> DL = {4, 3, 2, 1};    // sub-diagonal elements
  vector<double> D  = {1, 2, 3, 4, 5}; // diagonal elements
  vector<double> DU = {1, 2, 3, 4};    // super-diagonal elements

  vector<double> B1 = {6, 2, 7, 4, 5};  // RHS column 1
  vector<double> B2 = {1, 3, 8, 9, 10}; // RHS column 2
  matrix<double> B(5, 2, FORTRAN_LAYOUT);
  B(range(), 0) = B1;
  B(range(), 1) = B2;

  // reference solutions
  vector<double> ref_sol_1 = {43.0 / 33.0, 155.0 / 33.0, -208.0 / 33.0, 130.0 / 33.0, 7.0 / 33.0};
  vector<double> ref_sol_2 = {-28.0 / 33.0, 61.0 / 33.0, 89.0 / 66.0, -35.0 / 66.0, 139.0 / 66.0};
  matrix<double> ref_sol(5, 2, FORTRAN_LAYOUT);
  ref_sol(range(), 0) = ref_sol_1;
  ref_sol(range(), 1) = ref_sol_2;

  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B1);
    EXPECT_ARRAY_NEAR(B1, ref_sol_1);
  }
  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B2);
    EXPECT_ARRAY_NEAR(B2, ref_sol_2);
  }
  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B);
    EXPECT_ARRAY_NEAR(B, ref_sol);
  }
}

TEST(gtsv, cgtsv) {
  using dcomplex = std::complex<double>;

  vector<dcomplex> DL = {-4_j, -3_j, -2_j, -1_j}; // sub-diagonal elements
  vector<dcomplex> D  = {1, 2, 3, 4, 5};          // diagonal elements
  vector<dcomplex> DU = {1_j, 2_j, 3_j, 4_j};     // super-diagonal elements

  vector<dcomplex> B1 = {6 + 0_j, 2_j, 7 + 0_j, 4_j, 5 + 0_j}; // RHS column 1
  vector<dcomplex> B2 = {1_j, 3 + 0_j, 8_j, 9 + 0_j, 10_j};    // RHS column 2
  matrix<dcomplex> B(5, 2, FORTRAN_LAYOUT);
  B(range(), 0) = B1;
  B(range(), 1) = B2;

  // reference solutions
  vector<dcomplex> ref_sol_1 = {137.0 / 33.0 + 0_j, -61_j / 33.0, 368.0 / 33.0 + 0_j, 230_j / 33.0, -13.0 / 33.0 + 0_j};
  vector<dcomplex> ref_sol_2 = {-35_j / 33.0, 68.0 / 33.0 + 0_j, -103_j / 66.0, 415.0 / 66.0 + 0_j, 215_j / 66.0};
  matrix<dcomplex> ref_sol(5, 2, FORTRAN_LAYOUT);
  ref_sol(range(), 0) = ref_sol_1;
  ref_sol(range(), 1) = ref_sol_2;

  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B1);
    EXPECT_ARRAY_NEAR(B1, ref_sol_1);
  }
  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B2);
    EXPECT_ARRAY_NEAR(B2, ref_sol_2);
  }
  {
    auto dl(DL);
    auto d(D);
    auto du(DU);
    lapack::gtsv(dl, d, du, B);
    EXPECT_ARRAY_NEAR(B, ref_sol);
  }
}

MAKE_MAIN;
