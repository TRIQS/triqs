/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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

#ifndef POLYMORPH_RANDOM_GENERATOR_H
#define POLYMORPH_RANDOM_GENERATOR_H
#include <triqs/utility/first_include.hpp>
#include <boost/scoped_ptr.hpp>
#include "../utility/exceptions.hpp"
#include "./generator.hpp"
#include "math.h"
#include <string>

namespace triqs { 
namespace mc_tools { 

 /**
  *  This is a random generator that take the name of the random generator at construct
  *  and serve random numbers, buffered with a version boost::generator ,
  *  to hide the cost of dynamical polymorphism.
  */
 class random_generator {
  boost::shared_ptr<void> rng_ptr;
  boost::scoped_ptr< boost::generator <double> > gen;
  std::string name;
  void operator = ( random_generator const & p); //forbid
  std::size_t seed;
  public: 

  /// Takes the boost name of the generator e.g. mt19937,...
  random_generator(std::string const & RandomGeneratorName, std::size_t seed_ );

  ///
  random_generator( random_generator const & p);

  /// Return a list of the names of available generators, with separator sep
  static std::string random_generator_names(std::string const & sep=" ");

  /// Returns a integer in [0,i-1] with flat distribution
#define INTEGER_OVERLOAD(T) T operator()(T i) { return (i==1 ? 0 : T(floor(i*((*gen)()))));}
  INTEGER_OVERLOAD(int)
  INTEGER_OVERLOAD(size_t)
#undef INTEGER_OVERLOAD

  /// Returns a double in [0,1[ with flat distribution
  double operator()() { return ((*gen)());} 

  /// Returns a double in [0,x[ with flat distribution
  double operator()(double x) { return x*((*gen)());} 

  /// Returns a double in [a,b[ with flat distribution
  double operator()(double a, double b) { assert (b>a); return a + (b-a)*((*gen)());} 

 };

}
}

#endif
