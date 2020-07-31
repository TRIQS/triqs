// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

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
      std::ostream *out;
      std::function<bool()> condition;

      public:
      debug_stream(std::ostream *out_, std::function<bool()> condition) : out(out_), condition(condition) {}
      debug_stream(std::ostream *out_) : out(out_) {}

      template <class T> debug_stream &operator<<(T const &x) {
#ifdef TRIQS_DEBUG
        if (condition && condition()) (*out) << x;
#endif
        return *this;
      }

      // this is the type of std::cout
      typedef std::basic_ostream<char, std::char_traits<char>> CoutType;

      // this is the function signature of std::endl
      typedef CoutType &(*StandardEndLine)(CoutType &);

      // define an operator<< to take in std::endl
      debug_stream &operator<<(StandardEndLine manip) {
        // call the function, but we cannot return it's value
#ifdef TRIQS_DEBUG
        if (condition && condition()) manip(*out);
#endif
        return *this;
      }
    };
  } // namespace utility
} // namespace triqs
