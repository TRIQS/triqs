// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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

#include "random_generator.hpp"
#include "./MersenneRNG.hpp"
#include "./../utility/macros.hpp"
//#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/ranlux.hpp>
#include <boost/random/variate_generator.hpp>
#include <sstream>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/control/if.hpp>

// List of All available Boost random number generator
#define RNG_LIST                                                                                                                                     \
  (mt19937)(mt11213b)(                                                                                                                               \
     lagged_fibonacci607)(lagged_fibonacci1279)(lagged_fibonacci2281)(lagged_fibonacci3217)(lagged_fibonacci4423)(lagged_fibonacci9689)(lagged_fibonacci19937)(lagged_fibonacci23209)(lagged_fibonacci44497)(ranlux3)

namespace triqs {
  namespace mc_tools {

    random_generator::random_generator(std::string const &RandomGeneratorName, uint32_t seed_) {
      _name = RandomGeneratorName;

      if (RandomGeneratorName == "") {
        gen = utility::buffered_function<double>(mc_tools::RandomGenerators::RandMT(seed_));
        return;
      }

      boost::uniform_real<> dis;

// now boost random number generators
#define DRNG(r, data, XX)                                                                                                                            \
  if (RandomGeneratorName == AS_STRING(XX)) {                                                                                                        \
    gen = utility::buffered_function<double>(boost::variate_generator<boost::XX, boost::uniform_real<>>(boost::XX(seed_), dis));                     \
    return;                                                                                                                                          \
  }

      BOOST_PP_SEQ_FOR_EACH(DRNG, ~, RNG_LIST)

      TRIQS_RUNTIME_ERROR << "The random generator " << RandomGeneratorName << " is not recognized";
    }

    //---------------------------------------------

    std::string random_generator_names(std::string const &sep) {
#define PR(r, sep, p, XX) BOOST_PP_IF(p, +sep +, ) std::string(AS_STRING(XX))
      return BOOST_PP_SEQ_FOR_EACH_I(PR, sep, RNG_LIST);
    }

    std::vector<std::string> random_generator_names_list() {
      std::vector<std::string> res;
#define PR2(r, sep, p, XX) res.push_back(AS_STRING(XX));
      BOOST_PP_SEQ_FOR_EACH_I(PR2, sep, RNG_LIST);
      return res;
    }
  } // namespace mc_tools
} // namespace triqs
