// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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

#ifndef TRIQS_ARRAYS_EXCEPTIONS_H
#define TRIQS_ARRAYS_EXCEPTIONS_H

#include <triqs/utility/stack_trace.hpp>
#include <exception>
#include <string>
#include <sstream>

namespace triqs {
  namespace arrays {

    class key_error : public std::exception {
      std::string acc;

      public:
      key_error() throw() : std::exception() {}
      virtual ~key_error() throw() {}
      template <typename T> key_error &operator<<(T const &x) {
        std::stringstream f;
        f << acc << x;
        acc = f.str();
        return *this;
      }
      virtual const char *what() const throw() { return acc.c_str(); }
    };

  } // namespace arrays
} // namespace triqs

#define TRIQS_ARRAYS_KEY_ERROR                                                                                                                       \
  throw triqs::arrays::key_error() << "triqs::arrays key_error at " << __FILE__ << " : " << __LINE__ << "\n\n Trace is :\n\n"                        \
                                   << triqs::utility::stack_trace() << "\n"

#endif
