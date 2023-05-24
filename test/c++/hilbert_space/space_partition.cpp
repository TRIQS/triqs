// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2016 Igor Krivenko
// Copyright (c) 2018-2019 Simons Foundation
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

#include <triqs/test_tools/arrays.hpp>

#include <set>
#include <vector>
#include <algorithm>
#include <sstream>

#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/space_partition.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/hilbert_space/hilbert_space.hpp>
#include <triqs/hilbert_space/imperative_operator.hpp>
#include <triqs/hilbert_space/state.hpp>
#include <triqs/utility/numeric_ops.hpp>

using namespace triqs::hilbert_space;
using triqs::operators::c;
using triqs::operators::c_dag;
using triqs::operators::many_body_operator;
using triqs::operators::n;
using triqs::utility::is_zero;

// 3 bands Kanamori

// Parameters
double mu = 0.7;
double U  = 3.0;
double J  = 0.3;

// basis of operators to use
fundamental_operator_set fops;

// Hamiltonian
many_body_operator H;

int main(int argc, char **argv) {
  mpi::environment env(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);

  for (int o = 0; o < 3; ++o) fops.insert("dn", o);
  for (int o = 0; o < 3; ++o) fops.insert("up", o);

  for (int o = 0; o < 3; ++o) { H += -mu * (n("up", o) + n("dn", o)); }
  for (int o = 0; o < 3; ++o) { H += U * n("up", o) * n("dn", o); }
  for (int o1 = 0; o1 < 3; ++o1)
    for (int o2 = 0; o2 < 3; ++o2) {
      if (o1 == o2) continue;
      H += (U - 2 * J) * n("up", o1) * n("dn", o2);
    }
  for (int o1 = 0; o1 < 3; ++o1)
    for (int o2 = 0; o2 < 3; ++o2) {
      if (o2 >= o1) continue;
      H += (U - 3 * J) * n("up", o1) * n("up", o2);
      H += (U - 3 * J) * n("dn", o1) * n("dn", o2);
    }

  for (int o1 = 0; o1 < 3; ++o1)
    for (int o2 = 0; o2 < 3; ++o2) {
      if (o1 == o2) continue;
      H += -J * c_dag("up", o1) * c_dag("dn", o1) * c("up", o2) * c("dn", o2);
      H += -J * c_dag("up", o1) * c_dag("dn", o2) * c("up", o2) * c("dn", o1);
    }

  return RUN_ALL_TESTS();
}

using state_t  = state<hilbert_space, double, true>;
using imp_op_t = imperative_operator<hilbert_space, double, false>;

// Check subspaces
TEST(space_partition, Phase1) {

  // Hilbert space
  hilbert_space hs(fops);

  // Sample state
  state_t st(hs);

  // Imperative operator for H
  imp_op_t Hop(H, fops);

  // Space partition
  space_partition<state_t, imp_op_t> SP(st, Hop);

  // Calculated classification of states
  // sets are used to neglect order of subspaces and of states within a subspace
  std::vector<std::set<int>> v_cl(SP.n_subspaces());
  foreach (SP, [&v_cl](int st, int spn) { v_cl[spn].insert(st); })
    ;
  std::set<std::set<int>> cl{v_cl.cbegin(), v_cl.cend()};

  int u0 = 1 << fops[{"up", 0}];
  int u1 = 1 << fops[{"up", 1}];
  int u2 = 1 << fops[{"up", 2}];
  int d0 = 1 << fops[{"dn", 0}];
  int d1 = 1 << fops[{"dn", 1}];
  int d2 = 1 << fops[{"dn", 2}];

  // Expected classification of states
  std::set<std::set<int>> ref_cl{// N=0
                                 {0},
                                 // N=1
                                 {d0},
                                 {d1},
                                 {d2},
                                 {u0},
                                 {u1},
                                 {u2},
                                 // N=2, same spin
                                 {d0 + d1},
                                 {d0 + d2},
                                 {d1 + d2},
                                 {u0 + u1},
                                 {u0 + u2},
                                 {u1 + u2},
                                 // N=2, pair hopping
                                 {d0 + u0, d1 + u1, d2 + u2},
                                 // N=2, spin flip
                                 {d0 + u1, d1 + u0},
                                 {d0 + u2, d2 + u0},
                                 {d1 + u2, d2 + u1},
                                 // N=3
                                 {d0 + d1 + d2},
                                 {u0 + u1 + u2},
                                 {d0 + d1 + u0, d1 + d2 + u2},
                                 {d0 + d2 + u0, d1 + d2 + u1},
                                 {d0 + d1 + u1, d0 + d2 + u2},
                                 {d0 + u0 + u1, d2 + u1 + u2},
                                 {d1 + u0 + u1, d2 + u0 + u2},
                                 {d0 + u0 + u2, d1 + u1 + u2},
                                 {d1 + d2 + u0, d0 + d2 + u1, d0 + d1 + u2},
                                 {d2 + u0 + u1, d0 + u1 + u2, d1 + u0 + u2},
                                 // N=4, 2 holes with the same spin
                                 {d2 + u0 + u1 + u2},
                                 {d1 + u0 + u1 + u2},
                                 {d0 + u0 + u1 + u2},
                                 {d0 + d1 + d2 + u2},
                                 {d0 + d1 + d2 + u1},
                                 {d0 + d1 + d2 + u0},
                                 // N=4, pair hopping
                                 {d1 + d2 + u1 + u2, d0 + d2 + u0 + u2, d0 + d1 + u0 + u1},
                                 // N=4, spin flip
                                 {d1 + d2 + u0 + u2, d0 + d2 + u1 + u2},
                                 {d1 + d2 + u0 + u1, d0 + d1 + u1 + u2},
                                 {d0 + d2 + u0 + u1, d0 + d1 + u0 + u2},
                                 // N=5
                                 {d1 + d2 + u0 + u1 + u2},
                                 {d0 + d2 + u0 + u1 + u2},
                                 {d0 + d1 + u0 + u1 + u2},
                                 {d0 + d1 + d2 + u1 + u2},
                                 {d0 + d1 + d2 + u0 + u2},
                                 {d0 + d1 + d2 + u0 + u1},
                                 // N=6
                                 {d0 + d1 + d2 + u0 + u1 + u2}};

  EXPECT_EQ(ref_cl, cl);
}

// Check matrix elements
TEST(space_partition, MatrixElements) {

  // Hilbert space
  hilbert_space hs(fops);

  // Sample state
  state_t st(hs);

  // Imperative operator for H
  imp_op_t Hop(H, fops);

  // Space partition
  space_partition<state_t, imp_op_t> SP(st, Hop);

  using idx_t = typename space_partition<state_t, imp_op_t>::idx_t;
  struct melem_t {
    idx_t from;
    idx_t to;
    double val;
    bool operator<(melem_t const &me) const { return ((from != me.from) ? from < me.from : to < me.to); }
    bool operator==(melem_t const &me) const { return from == me.from && to == me.to && is_zero(val - me.val, 1e-10); }
  };
  using melem_set_t = std::set<std::set<melem_t>>;

  std::vector<std::set<melem_t>> v_melem(SP.n_subspaces());
  auto melem_map = SP.get_matrix_elements();
  for (auto x : melem_map) { v_melem[SP.lookup_basis_state(x.first.first)].insert({x.first.first, x.first.second, x.second}); }
  melem_set_t melem(v_melem.cbegin(), v_melem.cend());

  idx_t u0 = 1 << fops[{"up", 0}];
  idx_t u1 = 1 << fops[{"up", 1}];
  idx_t u2 = 1 << fops[{"up", 2}];
  idx_t d0 = 1 << fops[{"dn", 0}];
  idx_t d1 = 1 << fops[{"dn", 1}];
  idx_t d2 = 1 << fops[{"dn", 2}];

  // Expected matrix elements
  melem_set_t ref_melem{
     // N=0
     std::set<melem_t>{},
     // N=1
     {{d0, d0, -mu}},
     {{d1, d1, -mu}},
     {{d2, d2, -mu}},
     {{u0, u0, -mu}},
     {{u1, u1, -mu}},
     {{u2, u2, -mu}},
     // N=2, same spin
     {{d0 + d1, d0 + d1, -2 * mu + U - 3 * J}},
     {{d0 + d2, d0 + d2, -2 * mu + U - 3 * J}},
     {{d1 + d2, d1 + d2, -2 * mu + U - 3 * J}},
     {{u0 + u1, u0 + u1, -2 * mu + U - 3 * J}},
     {{u0 + u2, u0 + u2, -2 * mu + U - 3 * J}},
     {{u1 + u2, u1 + u2, -2 * mu + U - 3 * J}},
     // N=2, pair hopping
     {{d0 + u0, d0 + u0, -2 * mu + U},
      {d1 + u1, d1 + u1, -2 * mu + U},
      {d2 + u2, d2 + u2, -2 * mu + U},
      {d0 + u0, d1 + u1, J},
      {d0 + u0, d2 + u2, J},
      {d1 + u1, d2 + u2, J},
      {d1 + u1, d0 + u0, J},
      {d2 + u2, d0 + u0, J},
      {d2 + u2, d1 + u1, J}},
     // N=2, spin flip
     {{d0 + u1, d0 + u1, -2 * mu + U - 2 * J}, {d1 + u0, d1 + u0, -2 * mu + U - 2 * J}, {d0 + u1, d1 + u0, J}, {d1 + u0, d0 + u1, J}},
     {{d0 + u2, d0 + u2, -2 * mu + U - 2 * J}, {d2 + u0, d2 + u0, -2 * mu + U - 2 * J}, {d0 + u2, d2 + u0, J}, {d2 + u0, d0 + u2, J}},
     {{d1 + u2, d1 + u2, -2 * mu + U - 2 * J}, {d2 + u1, d2 + u1, -2 * mu + U - 2 * J}, {d1 + u2, d2 + u1, J}, {d2 + u1, d1 + u2, J}},
     // N=3
     {{d0 + d1 + d2, d0 + d1 + d2, -3 * mu + 3 * U - 9 * J}},
     {{u0 + u1 + u2, u0 + u1 + u2, -3 * mu + 3 * U - 9 * J}},
     {{d0 + d1 + u0, d0 + d1 + u0, -3 * mu + 3 * U - 5 * J},
      {d1 + d2 + u2, d1 + d2 + u2, -3 * mu + 3 * U - 5 * J},
      {d0 + d1 + u0, d1 + d2 + u2, -J},
      {d1 + d2 + u2, d0 + d1 + u0, -J}},
     {{d0 + d2 + u0, d0 + d2 + u0, -3 * mu + 3 * U - 5 * J},
      {d1 + d2 + u1, d1 + d2 + u1, -3 * mu + 3 * U - 5 * J},
      {d0 + d2 + u0, d1 + d2 + u1, J},
      {d1 + d2 + u1, d0 + d2 + u0, J}},
     {{d0 + d1 + u1, d0 + d1 + u1, -3 * mu + 3 * U - 5 * J},
      {d0 + d2 + u2, d0 + d2 + u2, -3 * mu + 3 * U - 5 * J},
      {d0 + d1 + u1, d0 + d2 + u2, J},
      {d0 + d2 + u2, d0 + d1 + u1, J}},
     {{d0 + u0 + u1, d0 + u0 + u1, -3 * mu + 3 * U - 5 * J},
      {d2 + u1 + u2, d2 + u1 + u2, -3 * mu + 3 * U - 5 * J},
      {d0 + u0 + u1, d2 + u1 + u2, -J},
      {d2 + u1 + u2, d0 + u0 + u1, -J}},
     {{d1 + u0 + u1, d1 + u0 + u1, -3 * mu + 3 * U - 5 * J},
      {d2 + u0 + u2, d2 + u0 + u2, -3 * mu + 3 * U - 5 * J},
      {d1 + u0 + u1, d2 + u0 + u2, J},
      {d2 + u0 + u2, d1 + u0 + u1, J}},
     {{d0 + u0 + u2, d0 + u0 + u2, -3 * mu + 3 * U - 5 * J},
      {d1 + u1 + u2, d1 + u1 + u2, -3 * mu + 3 * U - 5 * J},
      {d0 + u0 + u2, d1 + u1 + u2, J},
      {d1 + u1 + u2, d0 + u0 + u2, J}},
     {{d1 + d2 + u0, d1 + d2 + u0, -3 * mu + 3 * U - 7 * J},
      {d0 + d2 + u1, d0 + d2 + u1, -3 * mu + 3 * U - 7 * J},
      {d0 + d1 + u2, d0 + d1 + u2, -3 * mu + 3 * U - 7 * J},
      {d1 + d2 + u0, d0 + d2 + u1, J},
      {d0 + d2 + u1, d1 + d2 + u0, J},
      {d1 + d2 + u0, d0 + d1 + u2, -J},
      {d0 + d1 + u2, d1 + d2 + u0, -J},
      {d0 + d2 + u1, d0 + d1 + u2, J},
      {d0 + d1 + u2, d0 + d2 + u1, J}},
     {{d2 + u0 + u1, d2 + u0 + u1, -3 * mu + 3 * U - 7 * J},
      {d0 + u1 + u2, d0 + u1 + u2, -3 * mu + 3 * U - 7 * J},
      {d1 + u0 + u2, d1 + u0 + u2, -3 * mu + 3 * U - 7 * J},
      {d2 + u0 + u1, d0 + u1 + u2, -J},
      {d0 + u1 + u2, d2 + u0 + u1, -J},
      {d2 + u0 + u1, d1 + u0 + u2, J},
      {d1 + u0 + u2, d2 + u0 + u1, J},
      {d0 + u1 + u2, d1 + u0 + u2, J},
      {d1 + u0 + u2, d0 + u1 + u2, J}},
     // N=4, 2 holes with the same spin
     {{d2 + u0 + u1 + u2, d2 + u0 + u1 + u2, -4 * mu + 6 * U - 13 * J}},
     {{d1 + u0 + u1 + u2, d1 + u0 + u1 + u2, -4 * mu + 6 * U - 13 * J}},
     {{d0 + u0 + u1 + u2, d0 + u0 + u1 + u2, -4 * mu + 6 * U - 13 * J}},
     {{d0 + d1 + d2 + u0, d0 + d1 + d2 + u0, -4 * mu + 6 * U - 13 * J}},
     {{d0 + d1 + d2 + u1, d0 + d1 + d2 + u1, -4 * mu + 6 * U - 13 * J}},
     {{d0 + d1 + d2 + u2, d0 + d1 + d2 + u2, -4 * mu + 6 * U - 13 * J}},
     // N=4, pair hopping
     {{d1 + d2 + u1 + u2, d1 + d2 + u1 + u2, -4 * mu + 6 * U - 10 * J},
      {d0 + d2 + u0 + u2, d0 + d2 + u0 + u2, -4 * mu + 6 * U - 10 * J},
      {d0 + d1 + u0 + u1, d0 + d1 + u0 + u1, -4 * mu + 6 * U - 10 * J},
      {d1 + d2 + u1 + u2, d0 + d2 + u0 + u2, J},
      {d0 + d2 + u0 + u2, d1 + d2 + u1 + u2, J},
      {d1 + d2 + u1 + u2, d0 + d1 + u0 + u1, J},
      {d0 + d1 + u0 + u1, d1 + d2 + u1 + u2, J},
      {d0 + d2 + u0 + u2, d0 + d1 + u0 + u1, J},
      {d0 + d1 + u0 + u1, d0 + d2 + u0 + u2, J}},
     // N=4, spin flip
     {{d1 + d2 + u0 + u2, d1 + d2 + u0 + u2, -4 * mu + 6 * U - 12 * J},
      {d0 + d2 + u1 + u2, d0 + d2 + u1 + u2, -4 * mu + 6 * U - 12 * J},
      {d1 + d2 + u0 + u2, d0 + d2 + u1 + u2, J},
      {d0 + d2 + u1 + u2, d1 + d2 + u0 + u2, J}},
     {{d1 + d2 + u0 + u1, d1 + d2 + u0 + u1, -4 * mu + 6 * U - 12 * J},
      {d0 + d1 + u1 + u2, d0 + d1 + u1 + u2, -4 * mu + 6 * U - 12 * J},
      {d1 + d2 + u0 + u1, d0 + d1 + u1 + u2, J},
      {d0 + d1 + u1 + u2, d1 + d2 + u0 + u1, J}},
     {{d0 + d2 + u0 + u1, d0 + d2 + u0 + u1, -4 * mu + 6 * U - 12 * J},
      {d0 + d1 + u0 + u2, d0 + d1 + u0 + u2, -4 * mu + 6 * U - 12 * J},
      {d0 + d2 + u0 + u1, d0 + d1 + u0 + u2, J},
      {d0 + d1 + u0 + u2, d0 + d2 + u0 + u1, J}},
     // N=5
     {{d1 + d2 + u0 + u1 + u2, d1 + d2 + u0 + u1 + u2, -5 * mu + 10 * U - 20 * J}},
     {{d0 + d2 + u0 + u1 + u2, d0 + d2 + u0 + u1 + u2, -5 * mu + 10 * U - 20 * J}},
     {{d0 + d1 + u0 + u1 + u2, d0 + d1 + u0 + u1 + u2, -5 * mu + 10 * U - 20 * J}},
     {{d0 + d1 + d2 + u1 + u2, d0 + d1 + d2 + u1 + u2, -5 * mu + 10 * U - 20 * J}},
     {{d0 + d1 + d2 + u0 + u2, d0 + d1 + d2 + u0 + u2, -5 * mu + 10 * U - 20 * J}},
     {{d0 + d1 + d2 + u0 + u1, d0 + d1 + d2 + u0 + u1, -5 * mu + 10 * U - 20 * J}},
     // N=6
     {{d0 + d1 + d2 + u0 + u1 + u2, d0 + d1 + d2 + u0 + u1 + u2, -6 * mu + 15 * U - 30 * J}}};

  EXPECT_EQ(ref_melem, melem);
}

// Check merged subspaces
TEST(space_partition, Phase2) {

  // Hilbert space
  hilbert_space hs(fops);

  // Sample state
  state_t st(hs);

  // Imperative operator for H
  imp_op_t Hop(H, fops);

  // Space partition
  space_partition<state_t, imp_op_t> SP(st, Hop);

  std::vector<decltype(Hop)> Cd, C;
  for (int o = 0; o < 3; ++o)
    for (auto spin : {"up", "dn"}) {
      Cd.emplace_back(c_dag(spin, o), fops);
      C.emplace_back(c(spin, o), fops);

      SP.merge_subspaces(Cd.back(), C.back());
    }

  // Calculated classification of states
  std::vector<std::set<int>> v_cl(SP.n_subspaces());
  foreach (SP, [&v_cl](int st, int spn) { v_cl[spn].insert(st); })
    ;
  std::set<std::set<int>> cl{v_cl.cbegin(), v_cl.cend()};

  // Are all operators subspace-to-subspace mappings?
  decltype(Cd) all_ops;
  for (auto const &op : Cd) all_ops.push_back(op);
  for (auto const &op : C) all_ops.push_back(op);

  for (auto op : all_ops) {
    for (auto i_sp : cl) {
      std::set<int> f_sp;
      for (auto i : i_sp) {
        st(i)            = 1.0;
        auto final_state = op(st);
        foreach (final_state, [&f_sp](int f, double amplitude) {
          if (is_zero(amplitude, 1e-10)) return;
          f_sp.insert(f);
        })
          ;
        st(i) = 0;
      }

      // op maps i_sp onto zero
      if (f_sp.size() == 0) continue;

      // Check if op maps i_sp to only one subspace
      int n = 0;
      for (auto f_sp_ref : cl)
        if (std::includes(f_sp_ref.cbegin(), f_sp_ref.cend(), f_sp.cbegin(), f_sp.cend())) n++;
      EXPECT_EQ(1, n);
    }
  }
}
