// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <triqs/mc_tools/mc_move.hpp>
#include <triqs/mc_tools/mc_move_set.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <fmt/core.h>
#include <h5/h5.hpp>
#include <mpi/mpi.hpp>

#include <functional>
#include <map>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

// Counters for dummy move.
struct counter {
  int proposed{0};
  int accepted{0};
  int rejected{0};
};

// Dummy MCMove.
template <typename T> struct dummy_move {
  std::reference_wrapper<counter> ct;
  double ratio;
  T attempt() {
    ++ct.get().proposed;
    return ratio;
  }
  T accept() {
    ++ct.get().accepted;
    return 1.0;
  }
  void reject() { ++ct.get().rejected; }
  [[nodiscard]] static std::string hdf5_format() { return "dummy_move"; }
  friend void h5_write(h5::group g, std::string const &name, dummy_move const &m) {
    auto gr = g.create_group(name);
    h5::write_hdf5_format(gr, m);
    h5::write(gr, "ct.proposed", m.ct.get().proposed);
    h5::write(gr, "ct.accepted", m.ct.get().accepted);
    h5::write(gr, "ct.rejected", m.ct.get().rejected);
    h5::write(gr, "ratio", m.ratio);
  }
  friend void h5_read(h5::group g, std::string const &name, dummy_move &m) {
    auto gr = g.open_group(name);
    h5::assert_hdf5_format(gr, m);
    h5::read(gr, "ct.proposed", m.ct.get().proposed);
    h5::read(gr, "ct.accepted", m.ct.get().accepted);
    h5::read(gr, "ct.rejected", m.ct.get().rejected);
    h5::read(gr, "ratio", m.ratio);
  }
};

TEST(TRIQSMCTools, MoveBasics) {
  mpi::communicator comm{};

  // create move
  counter ct1;
  auto m1 = triqs::mc_tools::move<double>(dummy_move<double>{.ct = ct1, .ratio = 1.0});
  EXPECT_EQ(m1.n_accepted_config(), 0);
  EXPECT_EQ(m1.n_proposed_config(), 0);
  EXPECT_DOUBLE_EQ(m1.acceptance_rate(), -1);
  EXPECT_EQ((std::map<std::string, double>{}), m1.get_acceptance_rates());

  // perform some moves
  int nsteps = 100;
  for (int i = 0; i < nsteps; ++i) {
    m1.attempt();
    if (i % 2 == 0)
      m1.accept();
    else
      m1.reject();
  }

  // check statistics
  EXPECT_EQ(ct1.proposed, nsteps);
  EXPECT_EQ(ct1.accepted, nsteps / 2);
  EXPECT_EQ(ct1.rejected, nsteps / 2);
  m1.collect_statistics(comm);
  EXPECT_EQ(m1.n_accepted_config(), nsteps / 2);
  EXPECT_EQ(m1.n_proposed_config(), nsteps);
  EXPECT_DOUBLE_EQ(m1.acceptance_rate(), 0.5);

  // print statistics
  fmt::print("{}", m1.get_statistics("dummy move"));

  // HDF5 write/read
  if (comm.size() == 1) {
    counter ct2;
    auto m2 = triqs::mc_tools::move<double>(dummy_move<double>{.ct = ct2, .ratio = 1.0});
    h5::file file("mctools_move_set.h5", 'w');
    h5::write(file, "move", m1);
    h5::read(file, "move", m2);
    EXPECT_EQ(m2.n_accepted_config(), 0);
    EXPECT_EQ(m2.n_proposed_config(), 0);
    EXPECT_DOUBLE_EQ(m2.acceptance_rate(), -1);
    EXPECT_EQ(ct2.proposed, ct1.proposed);
    EXPECT_EQ(ct2.accepted, ct1.accepted);
    EXPECT_EQ(ct2.rejected, ct1.rejected);
  }

  // clear statistics
  m1.clear_statistics();
  EXPECT_EQ(m1.n_accepted_config(), 0);
  EXPECT_EQ(m1.n_proposed_config(), 0);
  EXPECT_DOUBLE_EQ(m1.acceptance_rate(), -1);
}

TEST(TRIQSMCTools, MoveSetBasics) {
  using namespace triqs::mc_tools;
  mpi::communicator comm{};
  auto rng = random_generator{};

  // lambda to create a move set
  auto create_move_set = [&](std::vector<counter> &ct_vec) {
    auto ms = move_set<double>{rng};
    ms.add(dummy_move<double>{.ct = ct_vec[0], .ratio = 1.0}, fmt::format("{}", 0), 2.0);
    auto ms_sub = move_set<double>{rng};
    ms_sub.add(dummy_move<double>{.ct = ct_vec[1], .ratio = 1.0}, fmt::format("{}.{}", 1, 1), 3.0);
    ms_sub.add(dummy_move<double>{.ct = ct_vec[2], .ratio = 1.0}, fmt::format("{}.{}", 1, 2), 2.0);
    ms.add(std::move(ms_sub), fmt::format("{}", 1), 5.0);
    ms.add(dummy_move<double>{.ct = ct_vec[3], .ratio = 1.0}, fmt::format("{}", 2), 3.0);
    return ms;
  };

  // create a move set
  std::vector<counter> ct_vec1(4);
  auto ms1 = create_move_set(ct_vec1);
  EXPECT_EQ(ms1.probabilities(), (std::vector<double>{0.2, 0.5, 0.3}));

  // check empty moves
  auto acc_rates = ms1.get_acceptance_rates();
  EXPECT_EQ(acc_rates.size(), 5);
  for (auto const &[name, acc_rate] : ms1.get_acceptance_rates()) EXPECT_DOUBLE_EQ(acc_rate, -1);

  // perform some moves
  int nsteps = 1000000;
  for (int i = 0; i < nsteps; ++i) {
    ms1.attempt();
    if (i % 2 == 0)
      ms1.accept();
    else
      ms1.reject();
  }

  // check statistics
  ms1.collect_statistics(comm);
  for (auto const &[name, acc_rate] : ms1.get_acceptance_rates()) EXPECT_NEAR(acc_rate, 0.5, 1e-2);
  std::vector<double> probs{0.2, 0.3, 0.2, 0.3};
  for (auto const &[ct, p] : std::views::zip(ct_vec1, probs)) {
    EXPECT_NEAR(static_cast<double>(ct.proposed) / nsteps, p, 1e-2);
    EXPECT_NEAR(static_cast<double>(ct.accepted) / ct.proposed, 0.5, 1e-2);
    EXPECT_NEAR(static_cast<double>(ct.rejected) / ct.proposed, 0.5, 1e-2);
  }

  // print statistics
  fmt::print("{}", ms1.get_statistics());

  // HDF5 write/read
  if (comm.size() == 1) {
    std::vector<counter> ct_vec2(4);
    auto ms2 = create_move_set(ct_vec2);
    h5::file file("mctools_move_set.h5", 'w');
    h5::write(file, "move_set", ms1);
    h5::read(file, "move_set", ms2);
    for (auto const &[ct1, ct2] : std::views::zip(ct_vec1, ct_vec2)) {
      EXPECT_EQ(ct2.proposed, ct1.proposed);
      EXPECT_EQ(ct2.accepted, ct1.accepted);
      EXPECT_EQ(ct2.rejected, ct1.rejected);
    }
  }

  // clear statistics
  ms1.clear_statistics();
  for (auto const &[name, acc_rate] : ms1.get_acceptance_rates()) EXPECT_DOUBLE_EQ(acc_rate, -1);
}

MAKE_MAIN;
