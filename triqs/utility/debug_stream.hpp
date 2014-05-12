/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#pragma once
#include <ostream>
#include <sstream>

namespace triqs {
namespace utility {

 /**
  * \brief Output stream with a condition
  *
  * This class behaves pretty much like a standard ostream 
  * except that the printing is "under" condition
  *
  * Example:
  *
  * debug_stream rep( std::cerr, [&mc](){ return mc.config_id() %100 == 0;}
  *
  */
 class debug_stream {
  std::ostream* out;
  std::function<bool()> condition;

  public:
  debug_stream(std::ostream* out_, std::function<bool()> condition) : out(out_), condition(condition) {}
  debug_stream(std::ostream* out_) : out(out_) {}

  template <class T> debug_stream& operator<<(T const& x) {
#ifdef TRIQS_DEBUG
   if (condition && condition()) (*out) << x;
#endif
   return *this;
  }

  // this is the type of std::cout
  typedef std::basic_ostream<char, std::char_traits<char>> CoutType;

  // this is the function signature of std::endl
  typedef CoutType& (*StandardEndLine)(CoutType&);

  // define an operator<< to take in std::endl
  debug_stream& operator<<(StandardEndLine manip) {
   // call the function, but we cannot return it's value
#ifdef TRIQS_DEBUG
   if (condition && condition()) manip(*out);
#endif
   return *this;
  }
 };
}
}
