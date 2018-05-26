/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013-2016 by I. Krivenko
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
#include <triqs/utility/first_include.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/operators/many_body_operator_serialization.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <string>
#include <vector>

using namespace triqs::operators;
using namespace triqs;
using triqs::hilbert_space::fundamental_operator_set;

template <typename O> void test_comm_anticomm(std::vector<O> const &C, std::vector<O> const &Cd) {
  using T           = typename O::scalar_t;
  auto ref_anticomm = [](int i, int j) {
    std::stringstream s;
    if (i == j)
      s << T(1);
    else
      s << 0;
    return s.str();
  };
  auto ref_comm = [](int i, int j) {
    std::stringstream s;
    if (i == j) s << -T(1) << " + ";
    s << T(2) << "*c_dag(" << i << ")*c(" << j << ")";
    return s.str();
  };
  for (int i = 0; i < C.size(); ++i)
    for (int j = 0; j < Cd.size(); ++j) {
      EXPECT_PRINT(ref_anticomm(i, j), Cd[i] * C[j] + C[j] * Cd[i]);
      EXPECT_PRINT(ref_comm(i, j), Cd[i] * C[j] - C[j] * Cd[i]);
    }
}

TEST(Operator, Real) {
  // Operators without indices
  auto op_with_no_indices = c<double>() + c_dag<double>() - n<double>();
  EXPECT_PRINT("1*c_dag() + 1*c() + -1*c_dag()*c()", op_with_no_indices);

  // Operators with many indices
  auto op_with_many_indices = c<double>(1, 2, "a", true, -2) + c_dag<double>(3, 15, "b", false, -5);
  EXPECT_PRINT("1*c_dag(3,15,'b',0,-5) + 1*c(1,2,'a',1,-2)", op_with_many_indices);

  // Constant operator
  many_body_operator_real const_op(3.14);
  EXPECT_PRINT("3.14", const_op);

  // Test anticommutators & commutators
  std::vector<many_body_operator_real> C  = {c<double>(0), c<double>(1), c<double>(2)};
  std::vector<many_body_operator_real> Cd = {c_dag<double>(0), c_dag<double>(1), c_dag<double>(2)};
  test_comm_anticomm(C, Cd);

  // Algebra
  auto x = c<double>(0), y = c_dag<double>(1);
  EXPECT_PRINT("1*c(0)", x);
  EXPECT_PRINT("1*c_dag(1)", y);
  EXPECT_PRINT("-1*c(0)", -x);
  EXPECT_PRINT("2 + 1*c(0)", x + 2.0);
  EXPECT_PRINT("2 + 1*c(0)", 2.0 + x);
  EXPECT_PRINT("-2 + 1*c(0)", x - 2.0);
  EXPECT_PRINT("2 + -1*c(0)", 2.0 - x);
  EXPECT_PRINT("3*c_dag(1)", 3.0 * y);
  EXPECT_PRINT("3*c_dag(1)", y * 3.0);
  EXPECT_PRINT("1*c_dag(1) + 1*c(0)", x + y);
  EXPECT_PRINT("-1*c_dag(1) + 1*c(0)", x - y);
  EXPECT_PRINT("2*c_dag(1)*c(0)", (x + y) * (x - y));

  // N^3
  auto N  = n<double>("up") + n<double>("dn");
  auto N3 = N * N * N;
  EXPECT_PRINT("1*c_dag('dn')*c('dn') + 1*c_dag('up')*c('up')", N);
  EXPECT_PRINT("1*c_dag('dn')*c('dn') + 1*c_dag('up')*c('up') + 6*c_dag('dn')*c_dag('up')*c('up')*c('dn')", N3);

  // Dagger
  auto X = c_dag<double>(1) * c_dag<double>(2) * c<double>(3) * c<double>(4);
  EXPECT_PRINT("-1*c_dag(1)*c_dag(2)*c(4)*c(3)", X);
  EXPECT_PRINT("-1*c_dag(3)*c_dag(4)*c(2)*c(1)", dagger(X));

  // Real & Imaginary
  EXPECT_PRINT("-1*c_dag(1)*c_dag(2)*c(4)*c(3)", real(X));
  EXPECT_PRINT("0", imag(X));
}

TEST(Operator, Complex) {
  // Operators without indices
  auto op_with_no_indices = c<dcomplex>() + c_dag<dcomplex>() - n<dcomplex>();
  EXPECT_PRINT("(1,0)*c_dag() + (1,0)*c() + (-1,-0)*c_dag()*c()", op_with_no_indices);

  // Operators with many indices
  auto op_with_many_indices = c<dcomplex>(1, 2, "a", true, -2) + c_dag<dcomplex>(3, 15, "b", false, -5);
  EXPECT_PRINT("(1,0)*c_dag(3,15,'b',0,-5) + (1,0)*c(1,2,'a',1,-2)", op_with_many_indices);

  // Constant operator
  many_body_operator_complex const_op(3.14 + 2_j);
  EXPECT_PRINT("(3.14,2)", const_op);

  // Test anticommutators & commutators
  std::vector<many_body_operator_complex> C  = {c<dcomplex>(0), c<dcomplex>(1), c<dcomplex>(2)};
  std::vector<many_body_operator_complex> Cd = {c_dag<dcomplex>(0), c_dag<dcomplex>(1), c_dag<dcomplex>(2)};
  test_comm_anticomm(C, Cd);

  // Algebra
  auto x = c<dcomplex>(0), y = c_dag<dcomplex>(1);
  EXPECT_PRINT("(1,0)*c(0)", x);
  EXPECT_PRINT("(1,0)*c_dag(1)", y);
  EXPECT_PRINT("(-1,-0)*c(0)", -x);
  EXPECT_PRINT("(2,0) + (1,0)*c(0)", x + 2.0);
  EXPECT_PRINT("(2,0) + (1,0)*c(0)", 2.0 + x);
  EXPECT_PRINT("(0,2) + (1,0)*c(0)", x + 2.0_j);
  EXPECT_PRINT("(0,2) + (1,0)*c(0)", 2.0_j + x);
  EXPECT_PRINT("(-2,-0) + (1,0)*c(0)", x - 2.0);
  EXPECT_PRINT("(2,0) + (-1,-0)*c(0)", 2.0 - x);
  EXPECT_PRINT("(-0,-2) + (1,0)*c(0)", x - 2.0_j);
  EXPECT_PRINT("(0,2) + (-1,-0)*c(0)", 2.0_j - x);
  EXPECT_PRINT("(3,0)*c_dag(1)", 3.0 * y);
  EXPECT_PRINT("(3,0)*c_dag(1)", y * 3.0);
  EXPECT_PRINT("(0,3)*c_dag(1)", 3.0_j * y);
  EXPECT_PRINT("(0,3)*c_dag(1)", y * 3.0_j);
  EXPECT_PRINT("(1,0)*c_dag(1) + (1,0)*c(0)", x + y);
  EXPECT_PRINT("(-1,-0)*c_dag(1) + (1,0)*c(0)", x - y);
  EXPECT_PRINT("(2,0)*c_dag(1)*c(0)", (x + y) * (x - y));

  // N^3
  auto N  = n<dcomplex>("up") + n<dcomplex>("dn");
  auto N3 = N * N * N;
  EXPECT_PRINT("(1,0)*c_dag('dn')*c('dn') + (1,0)*c_dag('up')*c('up')", N);
  EXPECT_PRINT("(1,0)*c_dag('dn')*c('dn') + (1,0)*c_dag('up')*c('up') + (6,0)*c_dag('dn')*c_dag('up')*c('up')*c('dn')", N3);

  // Dagger
  auto X = (1 + 2_j) * c_dag<dcomplex>(1) * c_dag<dcomplex>(2) * c<dcomplex>(3) * c<dcomplex>(4);
  EXPECT_PRINT("(-1,-2)*c_dag(1)*c_dag(2)*c(4)*c(3)", X);
  EXPECT_PRINT("(-1,2)*c_dag(3)*c_dag(4)*c(2)*c(1)", dagger(X));

  // Real & Imaginary
  EXPECT_PRINT("(-1,0)*c_dag(1)*c_dag(2)*c(4)*c(3)", real(X));
  EXPECT_PRINT("(-2,0)*c_dag(1)*c_dag(2)*c(4)*c(3)", imag(X));
}

TEST(Operator, RealOrComplex) {
  // Operators without indices
  auto op_with_no_indices = c() + 1_j * c_dag() - n();
  EXPECT_PRINT("(0+1j)*c_dag() + 1*c() + -1*c_dag()*c()", op_with_no_indices);

  // Operators with many indices
  auto op_with_many_indices = c(1, 2, "a", true, -2) + 1_j * c_dag(3, 15, "b", false, -5);
  EXPECT_PRINT("(0+1j)*c_dag(3,15,'b',0,-5) + 1*c(1,2,'a',1,-2)", op_with_many_indices);

  // Constant operator
  many_body_operator_complex const_op(3.14 + 2_j);
  EXPECT_PRINT("(3.14,2)", const_op);

  // Test anticommutators & commutators
  std::vector<many_body_operator> C  = {c(0), c(1), c(2)};
  std::vector<many_body_operator> Cd = {c_dag(0), c_dag(1), c_dag(2)};
  test_comm_anticomm(C, Cd);

  // Algebra
  auto x = c(0), y = c_dag(1);
  EXPECT_PRINT("1*c(0)", x);
  EXPECT_PRINT("1*c_dag(1)", y);
  EXPECT_PRINT("-1*c(0)", -x);
  EXPECT_PRINT("2 + 1*c(0)", x + 2.0);
  EXPECT_PRINT("2 + 1*c(0)", 2.0 + x);
  EXPECT_PRINT("(0+2j) + 1*c(0)", x + 2.0_j);
  EXPECT_PRINT("(0+2j) + 1*c(0)", 2.0_j + x);
  EXPECT_PRINT("-2 + 1*c(0)", x - 2.0);
  EXPECT_PRINT("2 + -1*c(0)", 2.0 - x);
  EXPECT_PRINT("(-0-2j) + 1*c(0)", x - 2.0_j);
  EXPECT_PRINT("(0+2j) + -1*c(0)", 2.0_j - x);
  EXPECT_PRINT("3*c_dag(1)", 3.0 * y);
  EXPECT_PRINT("3*c_dag(1)", y * 3.0);
  EXPECT_PRINT("(0+3j)*c_dag(1)", 3.0_j * y);
  EXPECT_PRINT("(0+3j)*c_dag(1)", y * 3.0_j);
  EXPECT_PRINT("1*c_dag(1) + 1*c(0)", x + y);
  EXPECT_PRINT("-1*c_dag(1) + 1*c(0)", x - y);
  EXPECT_PRINT("2*c_dag(1)*c(0)", (x + y) * (x - y));

  // N^3
  auto N  = n("up") + n("dn");
  auto N3 = N * N * N;
  EXPECT_PRINT("1*c_dag('dn')*c('dn') + 1*c_dag('up')*c('up')", N);
  EXPECT_PRINT("1*c_dag('dn')*c('dn') + 1*c_dag('up')*c('up') + 6*c_dag('dn')*c_dag('up')*c('up')*c('dn')", N3);

  // Dagger
  auto X = (1 + 2_j) * c_dag(1) * c_dag(2) * c(3) * c(4);
  EXPECT_PRINT("(-1-2j)*c_dag(1)*c_dag(2)*c(4)*c(3)", X);
  EXPECT_PRINT("(-1+2j)*c_dag(3)*c_dag(4)*c(2)*c(1)", dagger(X));

  // Real & Imaginary
  EXPECT_PRINT("-1*c_dag(1)*c_dag(2)*c(4)*c(3)", real(X));
  EXPECT_PRINT("-2*c_dag(1)*c_dag(2)*c(4)*c(3)", imag(X));

  // HDF5
  auto f = h5::file("ess.h5", 'w');

  auto op1 = 2 * c_dag(1) * c_dag(2) * c(3) * c(4) + 3.4 * c_dag(0, "a") * c(0, "a");

  h5_write(f, "OP", op1);
  auto op2 = h5::h5_read<many_body_operator>(f, "OP");
  EXPECT_TRUE((op1 - op2).is_zero());

  // Write operator op1 with imposed fundamental operator set
  fundamental_operator_set fs;
  fs.insert(6);
  for (int i = 1; i < 5; i++) fs.insert(i);
  fs.insert(0, "a");
  fs.insert(1, "a");
  h5_write(f, "OP_fs", op1, fs);

  // Read both operator and fundamental operator set
  fundamental_operator_set fs2;
  many_body_operator op3;
  h5_read(f, "OP_fs", op3, fs2);
  EXPECT_TRUE((op1 - op3).is_zero());
  EXPECT_EQ(fundamental_operator_set::reduction_t(fs), fundamental_operator_set::reduction_t(fs2));
}

MAKE_MAIN;
