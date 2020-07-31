// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2016 Igor Krivenko
// Copyright (c) 2018 Simons Foundation
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
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>
#include <sstream>
#include <map>

#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/hilbert_space.hpp>
#include <triqs/hilbert_space/imperative_operator.hpp>
#include <triqs/hilbert_space/state.hpp>

using namespace triqs::hilbert_space;

template <typename StateType> void check_state(StateType const &st, std::map<int, double> const &ref) {
  foreach (st, [&st, &ref](int i, double a) {
    auto it = ref.find(st.get_hilbert().get_fock_state(i));
    if (it != ref.end())
      EXPECT_EQ(it->second, a);
    else
      EXPECT_EQ(0, a);
  })
    ;
}

TEST(hilbert_space, fundamental_operator_set) {
  fundamental_operator_set fop1(std::vector<int>(2, 4));
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 4; ++j) { EXPECT_EQ(4 * i + j, (fop1[{i, j}])); }
  EXPECT_EQ(8, fop1.size());

  fundamental_operator_set fop2;
  fop2 = fop1;
  EXPECT_EQ(5, (fop2[{1, 1}]));

  fundamental_operator_set fop3;
  for (int i = 0; i < 4; ++i) fop3.insert(i);
  EXPECT_EQ(2, (fop3[{2}]));
  EXPECT_EQ(4, fop3.size());

  fundamental_operator_set fop4;
  for (int i = 0; i < 2; ++i) fop4.insert("up", i);
  for (int i = 0; i < 2; ++i) fop4.insert("down", i);
  EXPECT_EQ(2, (fop4[{"down", 0}]));
  EXPECT_EQ(4, fop4.size());
}

TEST(hilbert_space, hilbert_space) {
  fundamental_operator_set fop(std::vector<int>(2, 4));

  using triqs::hilbert_space::hilbert_space;
  hilbert_space hs1(fop);
  EXPECT_EQ(256, hs1.size());

  EXPECT_EQ(1, hs1.has_state(130));                             // Fock state 130 is here
  EXPECT_EQ(0, hs1.has_state(256));                             // Fock state 256 is here
  EXPECT_EQ(120, hs1.get_fock_state(120));                      // Fock state for index 120
  EXPECT_EQ(120, hs1.get_state_index(hs1.get_fock_state(120))); // index of fock state 120
  // Fock state for vacuum
  EXPECT_EQ(0, hs1.get_fock_state(fop, std::set<fundamental_operator_set::indices_t>{}));
  // Fock state for c^+(0,1)c^+(1,3)|vac>
  EXPECT_EQ(130, hs1.get_fock_state(fop, std::set<fundamental_operator_set::indices_t>{{0, 1}, {1, 3}})); //

  hilbert_space hs2;
  hs2 = hs1;
  EXPECT_EQ(256, hs2.size());

  // HDF5
  auto hs_h5 = rw_h5(hs1, "hilbert_space");
  EXPECT_EQ(hs1, hs_h5);
}

TEST(hilbert_space, fock_state) {
  fundamental_operator_set fop;
  for (int i = 0; i < 4; ++i) fop.insert(i);

  using triqs::hilbert_space::hilbert_space;
  hilbert_space hs(fop);

  fock_state_t fs1 = hs.get_fock_state(10);
  EXPECT_EQ(10, fs1);
  fock_state_t fs2 = fs1;
  EXPECT_EQ(10, fs2);
}

TEST(hilbert_space, state) {
  fundamental_operator_set fop;
  for (int i = 0; i < 5; ++i) fop.insert("up", i);

  using triqs::hilbert_space::hilbert_space;
  hilbert_space h_full(fop);

  state<hilbert_space, double, true> st(h_full);
  st(0) = 3.0;
  st(3) = 5.0;

  check_state(st, {{3, 5.0}, {0, 3.0}});
}

TEST(hilbert_space, imperative_operator) {
  fundamental_operator_set fop;
  for (int i = 0; i < 5; ++i) fop.insert("up", i);

  using triqs::hilbert_space::hilbert_space;
  hilbert_space h_full(fop);

  using triqs::operators::c;
  using triqs::operators::c_dag;

  auto H = 3 * c_dag("up", 1) * c("up", 1) + 2 * c_dag("up", 2) * c("up", 2) + c("up", 1) * c("up", 2);
  EXPECT_PRINT("3*c_dag('up',1)*c('up',1) + 2*c_dag('up',2)*c('up',2) + -1*c('up',2)*c('up',1)", H);

  auto opH = imperative_operator<hilbert_space>(H, fop);

  state<hilbert_space, double, true> old_state(h_full);
  old_state(7) = 1.0;
  check_state(old_state, {{7, 1.0}});

  auto new_state = opH(old_state);
  check_state(new_state, {{1, -1.0}, {7, 5.0}});
}

TEST(hilbert_space, sub_hilbert_space) {
  using triqs::operators::c_dag;
  auto Cdag = c_dag("up", 2);
  EXPECT_PRINT("1*c_dag('up',2)", Cdag);

  fundamental_operator_set fop1;
  for (int i = 0; i < 5; ++i) fop1.insert("up", i);
  fundamental_operator_set fop2;
  for (int i = 0; i < 2; ++i) fop2.insert("up", i);
  for (int i = 0; i < 2; ++i) fop2.insert("down", i);

  using triqs::hilbert_space::hilbert_space;
  hilbert_space hs2(fop2);

  sub_hilbert_space phs0(0);
  phs0.add_fock_state(hs2.get_fock_state(0)); // 000
  phs0.add_fock_state(hs2.get_fock_state(1)); // 001
  phs0.add_fock_state(hs2.get_fock_state(2)); // 010
  phs0.add_fock_state(hs2.get_fock_state(3)); // 011

  EXPECT_TRUE(phs0.has_state(hs2.get_fock_state(2)));  // phs0 has state 2
  EXPECT_FALSE(phs0.has_state(hs2.get_fock_state(6))); // phs0 has state 6

  sub_hilbert_space phs1(1);
  phs1.add_fock_state(hs2.get_fock_state(4)); // 100
  phs1.add_fock_state(hs2.get_fock_state(5)); // 101
  phs1.add_fock_state(hs2.get_fock_state(6)); // 110
  phs1.add_fock_state(hs2.get_fock_state(7)); // 111

  EXPECT_FALSE(phs1.has_state(hs2.get_fock_state(2))); // phs1 has state 2
  EXPECT_TRUE(phs1.has_state(hs2.get_fock_state(6)));  // phs1 has state 6

  std::vector<int> Cdagmap(2, -1);
  Cdagmap[phs0.get_index()] = phs1.get_index();
  std::vector<sub_hilbert_space> sub1{phs0, phs1};
  auto opCdag = imperative_operator<sub_hilbert_space, double, true>(Cdag, fop1, Cdagmap, &sub1);

  state<sub_hilbert_space, double, false> start(phs0);
  start(0) = 1.0;
  start(1) = 2.0;
  start(2) = 3.0;
  start(3) = 4.0;

  check_state(start, {{0, 1.0}, {1, 2.0}, {2, 3.0}, {3, 4.0}});
  check_state(opCdag(start), {{4, 1.0}, {5, -2.0}, {6, -3.0}, {7, 4.0}});

  // HDF5
  auto hs_h5 = rw_h5(phs1, "sub_hilbert_space");
  EXPECT_EQ(phs1, hs_h5);
}

TEST(hilbert_space, QuarticOperators) {
  fundamental_operator_set fops;
  fops.insert("down", 0);
  fops.insert("down", 1);
  fops.insert("up", 0);
  fops.insert("up", 1);

  using triqs::hilbert_space::hilbert_space;
  hilbert_space hs(fops);
  EXPECT_EQ(16, hs.size());

  using triqs::operators::c;
  using triqs::operators::c_dag;
  auto quartic_op = -1.0 * c_dag("up", 0) * c_dag("down", 1) * c("up", 1) * c("down", 0);

  state<hilbert_space, double, false> st1(hs);
  st1(9) = 1.0;                                                                       // 0110
  check_state(st1, {{9, 1.0}});                                                       // old state
  EXPECT_PRINT("1*c_dag('down',1)*c_dag('up',0)*c('up',1)*c('down',0)", quartic_op);  // quartic operator
  check_state(imperative_operator<hilbert_space>(quartic_op, fops)(st1), {{6, 1.0}}); // new state
}

TEST(hilbert_space, StateProjection) {
  fundamental_operator_set fop;
  for (int i = 0; i < 3; ++i) fop.insert("s", i);

  using triqs::hilbert_space::hilbert_space;
  hilbert_space hs_full(fop);

  state<hilbert_space, double, true> st(hs_full);
  st(0) = 0.1;
  st(2) = 0.2;
  st(4) = 0.3;
  st(6) = 0.4;
  check_state(st, {{6, 0.4}, {4, 0.3}, {2, 0.2}, {0, 0.1}}); // original state

  sub_hilbert_space hs(0);
  hs.add_fock_state(hs_full.get_fock_state(4));
  hs.add_fock_state(hs_full.get_fock_state(5));
  hs.add_fock_state(hs_full.get_fock_state(6));
  hs.add_fock_state(hs_full.get_fock_state(7));

  auto proj_st = project<state<sub_hilbert_space, double, false>>(st, hs);
  check_state(proj_st, {{4, 0.3}, {6, 0.4}}); // projected state
}

MAKE_MAIN;
