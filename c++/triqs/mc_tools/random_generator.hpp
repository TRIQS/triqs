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

#pragma once
#include <triqs/utility/first_include.hpp>
#include "../utility/exceptions.hpp"
#include "../utility/buffered_function.hpp"
#include <cmath>
#include <string>
#include <assert.h>
#include <type_traits>
#include <cstdint>

namespace triqs {
  namespace mc_tools {

    /// Return a list of the names of available generators, with separator sep
    std::string random_generator_names(std::string const &sep = " ");
    std::vector<std::string> random_generator_names_list();

    /**
  * Random generator, adapting the boost random generator.
  *
  * The name of the generator is given at construction, and its type is erased in this class.
  * For performance, the call to the generator is bufferized, with chunks of 1000 numbers.
  */
    class random_generator {
      utility::buffered_function<double> gen;
      std::string _name;

      public:
      /** Constructor
   *  @param RandomGeneratorName : Name of a boost generator e.g. mt19937, or "" (another Mersenne Twister).
   *  @param seed : The seed of the random generator
   */
      random_generator(std::string const &RandomGeneratorName, uint32_t seed_);

      random_generator() : random_generator("mt19937", 198) {}

      ///
      random_generator(random_generator const &p);

      random_generator(random_generator &&) = default;

      //
      random_generator &operator=(random_generator &&) = default;

      /// Name of the random generator
      std::string name() const { return _name; }

      /// Returns a integer in [0,i-1] with flat distribution
      template <typename T> typename std::enable_if<std::is_integral<T>::value, T>::type operator()(T i) {
        return (i == 1 ? 0 : T(floor(i * (gen()))));
      }

      /// Returns a double in [0,1[ with flat distribution
      double preview() { return gen.preview(); }

      double operator()() { return gen(); }

      /// Returns a double in [0,x[ with flat distribution
      double operator()(double x) { return x * (gen()); }

      /// Returns a double in [a,b[ with flat distribution
      double operator()(double a, double b) {
        assert(b > a);
        return a + (b - a) * (gen());
      }
    };
  } // namespace mc_tools
} // namespace triqs
