// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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

#include "./MersenneRNG.hpp"
#include "./random_generator.hpp"

#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/ranlux.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <fmt/format.h>
#include <nda/macros.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

// List of all supported Boost random number generators.
#define RNG_LIST                                                                                                                                     \
  (mt19937)(mt11213b)(                                                                                                                               \
     lagged_fibonacci607)(lagged_fibonacci1279)(lagged_fibonacci2281)(lagged_fibonacci3217)(lagged_fibonacci4423)(lagged_fibonacci9689)(lagged_fibonacci19937)(lagged_fibonacci23209)(lagged_fibonacci44497)(ranlux3)

namespace triqs::mc_tools {

  random_generator::random_generator(std::string name, std::uint32_t seed, std::size_t buffer_size) : buffer_(buffer_size), name_(std::move(name)) {
    initialize_rng(name_, seed);
    refill();
  }

  void random_generator::initialize_rng(std::string const &name, std::uint32_t seed) {
    // empty string corresponds to RandMT
    if (name.empty()) {
      using rng_t = RandomGenerators::RandMT;
      ptr_        = std::make_unique<rng_model<rng_t>>(seed);
      return;
    }

    // now boost random number generators
#define DRNG(r, data, XX)                                                                                                                            \
  if (name == AS_STRING(XX)) {                                                                                                                       \
    using rng_t = boost::variate_generator<boost::XX, boost::uniform_real<double>>;                                                                  \
    ptr_        = std::make_unique<rng_model<rng_t>>(rng_t{boost::XX{seed}, boost::uniform_real<>{}});                                               \
    return;                                                                                                                                          \
  }
    BOOST_PP_SEQ_FOR_EACH(DRNG, ~, RNG_LIST)

    // throw an exception if the given name is not recognized
    throw std::runtime_error(fmt::format("Error in random_generator::initialize_rng: RNG with name {} is not supported", name));
  }

  std::string random_generator_names(std::string const &sep) {
#define PR(r, sep, p, XX) BOOST_PP_IF(p, +(sep) +, ) std::string(AS_STRING(XX))
    return BOOST_PP_SEQ_FOR_EACH_I(PR, sep, RNG_LIST);
  }

  std::vector<std::string> random_generator_names_list() {
    std::vector<std::string> res;
#define PR2(r, sep, p, XX) res.push_back(AS_STRING(XX));
    BOOST_PP_SEQ_FOR_EACH_I(PR2, sep, RNG_LIST);
    return res;
  }

} // namespace triqs::mc_tools
