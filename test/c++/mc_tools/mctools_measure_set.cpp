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

#include <triqs/mc_tools/mc_measure.hpp>
#include <triqs/mc_tools/mc_measure_set.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <fmt/ranges.h>
#include <h5/h5.hpp>
#include <mpi/mpi.hpp>

#include <complex>
#include <functional>
#include <ranges>
#include <string>
#include <thread>
#include <vector>

// Counters for dummy move.
struct counter {
  std::complex<double> accumulated{0};
};

// Dummy MCMove.
struct dummy_measure {
  std::reference_wrapper<counter> ct;
  void accumulate(std::complex<double> sign) {
    ct.get().accumulated += sign;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  void collect_results(mpi::communicator const &c) { ct.get().accumulated = mpi::all_reduce(ct.get().accumulated, c); }
  [[nodiscard]] auto report() const {
    return fmt::format("accumulated = ({},{})\n", std::real(ct.get().accumulated), std::imag(ct.get().accumulated));
  }
  [[nodiscard]] static std::string hdf5_format() { return "dummy_measure"; }
  friend void h5_write(h5::group g, std::string const &name, dummy_measure const &m) {
    auto gr = g.create_group(name);
    h5::write_hdf5_format(gr, m); // NOLINT (downcast to base class)
    h5::write(gr, "ct.accumulated", m.ct.get().accumulated);
  }
  friend void h5_read(h5::group g, std::string const &name, dummy_measure &m) {
    auto gr = g.open_group(name);
    h5::assert_hdf5_format(gr, m);
    h5::read(gr, "ct.accumulated", m.ct.get().accumulated);
  }
};

TEST(TRIQSMCTools, MeasureBasics) {
  mpi::communicator comm{};

  // create measure
  counter ct1;
  auto m1 = triqs::mc_tools::measure<std::complex<double>>(dummy_measure{.ct = ct1}, true, true);
  EXPECT_EQ(m1.count(), 0);
  EXPECT_EQ((std::vector<std::string>{}), m1.names());
  EXPECT_FALSE(m1.is_set());

  // perform some measurements
  int nsteps      = 100;
  auto const sign = std::complex<double>{1.0, -0.5};
  for (int i = 0; i < nsteps; ++i) m1.accumulate(sign);

  // check statistics
  EXPECT_EQ(ct1.accumulated, nsteps * sign);
  EXPECT_EQ(m1.count(), nsteps);
  m1.collect_results(comm);
  EXPECT_EQ(ct1.accumulated, nsteps * sign * comm.size());
  EXPECT_EQ(m1.count(), nsteps);

  // print timings
  fmt::print("{}", m1.get_timings("1"));

  // print report
  fmt::print("{}", m1.report());

  // HDF5 write/read
  if (comm.size() == 1) {
    counter ct2;
    auto m2 = triqs::mc_tools::measure<std::complex<double>>(dummy_measure{.ct = ct2}, true, true);
    h5::file file("mctools_measure_set.h5", 'w');
    h5::write(file, "measure", m1);
    h5::read(file, "measure", m2);
    EXPECT_EQ(ct2.accumulated, ct1.accumulated);
  }
}

TEST(TRIQSMCTools, MeasureSetBasics) {
  using namespace triqs::mc_tools;
  mpi::communicator comm{};

  // lambda to create a measure set
  auto create_measure_set = [&](std::vector<counter> &ct_vec) {
    auto ms = measure_set<std::complex<double>>{};
    ms.insert(dummy_measure{.ct = ct_vec[0]}, "0", true, true);
    auto ms_sub = measure_set<std::complex<double>>{};
    ms_sub.insert(dummy_measure{.ct = ct_vec[1]}, "1.1", true, false);
    ms_sub.insert(dummy_measure{.ct = ct_vec[2]}, "1.2", false, true);
    ms.insert(std::move(ms_sub), "1", true, true);
    ms.insert(dummy_measure{.ct = ct_vec[3]}, "2", false, false);
    return ms;
  };

  // create a move set
  std::vector<counter> ct_vec1(4);
  auto ms1 = create_measure_set(ct_vec1);
  EXPECT_TRUE(ms1.has("0"));
  EXPECT_TRUE(ms1.has("1"));
  EXPECT_TRUE(ms1.has("2"));

  // print and check names
  EXPECT_EQ(ms1.names(), (std::vector<std::string>{"0", "1", "1.1", "1.2", "2"}));
  if (comm.rank() == 0) fmt::print("{}\n", ms1.names());

  // perform some measurements
  int nsteps      = 100;
  auto const sign = std::complex<double>{1.0, -0.5};
  for (int i = 0; i < nsteps; ++i) ms1.accumulate(sign);

  // check statistics
  for (auto const &ct : ct_vec1) EXPECT_EQ(ct.accumulated, nsteps * sign);
  ms1.collect_results(comm);
  for (auto const &ct : ct_vec1) EXPECT_EQ(ct.accumulated, nsteps * sign * comm.size());

  // print timings
  if (comm.rank() == 0) {
    fmt::print("{}", ms1.get_timings());
    fmt::print("{}", ms1.report());
  }

  // HDF5 write/read
  if (comm.size() == 1) {
    std::vector<counter> ct_vec2(4);
    auto ms2 = create_measure_set(ct_vec2);
    h5::file file("mctools_measure_set.h5", 'w');
    h5::write(file, "measure_set", ms1);
    h5::read(file, "measure_set", ms2);
    for (auto const &[ct1, ct2] : std::views::zip(ct_vec1, ct_vec2)) EXPECT_EQ(ct2.accumulated, ct1.accumulated);
  }

  // clear measure set
  ms1.clear();
  EXPECT_FALSE(ms1.has("0"));
  EXPECT_FALSE(ms1.has("1"));
  EXPECT_FALSE(ms1.has("2"));
}

MAKE_MAIN;
