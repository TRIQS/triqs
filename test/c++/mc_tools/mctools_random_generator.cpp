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

#include <triqs/mc_tools/MersenneRNG.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/ranlux.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <fmt/ranges.h>

#include <sstream>
#include <string>

TEST(TRIQSMCTools, RandomGeneratorNames) {
  fmt::print("{}\n", triqs::mc_tools::random_generator_names());
  fmt::print("{}\n", triqs::mc_tools::random_generator_names_list());
}

TEST(TRIQSMCTools, RandomGeneratorUnderlyingRNG) {
  using namespace triqs::mc_tools;
  int const seed = 0x18a2b3c4;
  auto check_rng = [](auto &&rng, auto &&exp_rng) {
    for (int i = 0; i < 10; ++i) {
      double const val     = rng.preview();
      double const exp_val = exp_rng();
      EXPECT_DOUBLE_EQ(rng(), exp_val);
      EXPECT_DOUBLE_EQ(val, exp_val);
    }
  };

  check_rng(random_generator("mt19937", seed), boost::variate_generator(boost::mt19937{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("mt11213b", seed), boost::variate_generator(boost::mt11213b{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci607", seed), boost::variate_generator(boost::lagged_fibonacci607{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci1279", seed), boost::variate_generator(boost::lagged_fibonacci1279{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci2281", seed), boost::variate_generator(boost::lagged_fibonacci2281{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci3217", seed), boost::variate_generator(boost::lagged_fibonacci3217{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci4423", seed), boost::variate_generator(boost::lagged_fibonacci4423{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci9689", seed), boost::variate_generator(boost::lagged_fibonacci9689{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci19937", seed), boost::variate_generator(boost::lagged_fibonacci19937{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci23209", seed), boost::variate_generator(boost::lagged_fibonacci23209{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("lagged_fibonacci44497", seed), boost::variate_generator(boost::lagged_fibonacci44497{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("ranlux3", seed), boost::variate_generator(boost::ranlux3{seed}, boost::uniform_real<>{}));
  check_rng(random_generator("", seed), triqs::mc_tools::RandomGenerators::RandMT{seed});
}

TEST(TRIQSMCTools, RandomGeneratorRestoreRandMT) {
  using namespace triqs::mc_tools::RandomGenerators;
  auto rng  = RandMT();
  auto rng2 = RandMT();
  for (int i = 0; i < 10; ++i) rng();
  EXPECT_NE(rng(), rng2());
  std::stringstream ss;
  ss << rng;
  ss >> rng2;
  for (int i = 0; i < 10; ++i) EXPECT_EQ(rng(), rng2());
}

TEST(TRIQSMCTools, RandomGeneratorHDF5) {
  using namespace triqs::mc_tools;
  auto check_hdf5 = [](std::string const &name) {
    int const seed = 0x18a2b3c4;
    auto rng       = random_generator(name, seed);
    for (int i = 0; i < 10; ++i) rng();
    auto rng2 = rw_h5(rng, "mctools_random_generator_" + name, name);
    for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(rng(), rng2());
  };

  // boost RNGs
  for (auto const &name : random_generator_names_list()) check_hdf5(name);

  // RandMT
  using namespace std::string_literals;
  int const seed = 0x18a2b3c4;
  auto rng       = random_generator("", seed);
  for (int i = 0; i < 10; ++i) rng();
  auto rng2 = rw_h5(rng, "mctools_random_generator"s + "_RandMT"s, "RandMT");
  for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(rng(), rng2());
}

TEST(TRIQSMCTools, RandomGeneratorMoveOperation) {
  using namespace triqs::mc_tools;
  auto rng = random_generator();
  auto rng2 = random_generator();
  for (int i = 0; i < 10; ++i) {
    rng();
    rng2();
  }

  // move constructor
  auto rng3 = std::move(rng);
  for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(rng2(), rng3());

  // move assignment
  auto rng4 = random_generator();
  rng4 = std::move(rng2);
  for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(rng3(), rng4());
}

MAKE_MAIN;
